/*
 * spaceInvadeFunc.c
 *
 *  Created on: Apr 11, 2024
 *      Author: sebichesh
 * 		Contributors: TomBirkbeck01, el23alyt, el23vm, Russo-BIT
 *      Description: Contains all the essential gameplay functions for the space invaders game
 */
#include "HPS_PrivateTimer/HPS_PrivateTimersSebi.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"
#include "spaceInvadeFunc.h"
#include "spaceInvadeParams.h"
#include "Images/barrier_health_5.h"
#include "Images/barrier_health_4.h"
#include "Images/barrier_health_3.h"
#include "Images/barrier_health_2.h"
#include "Images/barrier_health_1.h"
#include "Images/game_over_screen.h"
#include "Images/start_screen.h"
#include "Images/easy.h"
#include "Images/normal.h"
#include "Images/hard.h"
#include "Images/black.h"
#include "Images/explosion.h"
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"
#include "HPS_GPIO/HPS_GPIO.h"
#include "HPS_I2C/HPS_I2C.h"
#include "Util/macros.h"
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include "Audio/audioFunc.h"

void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
}

// draws a coloured rectangle with hex colour code "colour" of size "width" x "length" whose top left corner is at "x0" and "y0"
void drawCuboid(unsigned int x0, unsigned int y0, unsigned int width, unsigned int length, PLT24Ctx_t lt24, unsigned short colour) {
    for (unsigned int j = y0; j < y0 + length && j < SCREEN_LENGTH; ++j) {
        for (unsigned int i = x0; i < x0 + width && i < SCREEN_WIDTH; ++i) {

            exitOnFail(LT24_drawPixel(lt24,colour,i,j), ERR_SUCCESS); // draws a coloured pixel at (i, j)

        }
    }
}

// Initially populates the enemy scruct with their initial positions
void initialPopEnemyInfo(struct enemyInfo * enemies) {

	for (int i=0;i<ENEMY_ROWS;i++){
		for (int j=0;j<ENEMY_COLS;j++){

			enemies->xPos[i][j] = ((ENEMY_ICON_WIDTH + ENEMY_COL_SPACING)*j + 10); // sets x position
			enemies->yPos[i][j] = ((ENEMY_ICON_WIDTH + ENEMY_ROW_SPACING)*i + 10); // sets y position
			enemies->status[i][j] = ALIVE; // sets enemy alive

		}
	}

	HPS_ResetWatchdog(); //Just reset the watchdog.
}

// Recursively calls drawInvader to populate the screen with the enemies, if that enemy is alive
void drawInvaders(struct enemyInfo * enemies, PLT24Ctx_t lt24){
	for (int x=0;x<ENEMY_ROWS;x++){
		for (int y=0;y<ENEMY_COLS;y++){
			if (enemies->status[x][y]){   // If the enemy is alive we draw it
				drawInvader(enemies->xPos[x][y],enemies->yPos[x][y],LT24_WHITE,lt24);

				HPS_ResetWatchdog(); //Just reset the watchdog.
			}
		}
	}
}

// Loops through a 2D array containing the icon of an invader, drawing a pixel on the screen if it is in the icon
void drawInvader(int x, int y, unsigned short colour, PLT24Ctx_t lt24){
	int invader[8][11] =
		{
		{0,0,1,0,0,0,0,0,1,0,0},         //Enemy icon pixel data (squint and you can see the invader)
		{0,0,0,1,0,0,0,1,0,0,0},
		{0,0,1,1,1,1,1,1,1,0,0},
		{0,1,1,0,1,1,1,0,1,1,0},
		{1,1,1,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,1,1,0,1},
		{1,0,1,0,0,0,0,0,1,0,1},
		{0,0,0,1,1,0,1,1,0,0,0}};

	for (int i=0; i < 8; i++){
		for (int j=0; j < 11;j++){
			if (invader[i][j]){
				LT24_drawPixel(lt24,colour,x+j,y+i);  // Draw a pixel of the specified colour if it is in the icon
			}

		}
	}
}

// Updates and draws the position of the enemy shot that is fired and checks if it has collided with anything and updates the game accordingly
void updateEnemyShot(struct enemyInfo * enemies, struct enemyShotInfo * enemyShots, PLT24Ctx_t lt24, PTimerCtx_t timerCtx, PWM8731Ctx_t audio, unsigned int* private_timer_current_value, unsigned int difficulty, struct barrierInfo * barriers, struct playerInfo * player, bool * gameOver) {

	bool barrierCollision = false;
	bool playerCollision = false;
	bool endOfScreenCollision = false;

	// Checks if there is a enemylaser has been fired, if yes then does collision checks and updates laser accordingly, if not then fires enemy laser from a random alive enemy in the lowest row
	if (enemyShots->status) {

		// Checks if enemy shot if updated will collided with the end of the screen, if so sets endOfScreenCollision to true
		if (enemyShots->yPos >= (SCREEN_LENGTH-ENEMY_LASER_LENGTH-6)) { // checks if shot at end of screen

			endOfScreenCollision = true;

		// Checks if enemy laser shot collides with player and if so sets player Collision high
		} else if (isCollided(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, player->xPos, player->yPos, PLAYER_ICON_WIDTH, PLAYER_ICON_LENGTH)) {
			
			playerCollision = true;

		// Checks if enemy laser has collided with an alive barrier and if so sets barrierCollision high
		} else {

			// Checking if laser has collided with a barrier if yes it reduces health of barrier sets enemy shot status low and barrierCollision to true
			for (int i=0;i<BARRIER_COLS;i++) {

				if (barriers->status[i]) {

					barrierCollision = isCollided(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, barriers->xPos[i], barriers->yPos[i], BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH);
					
					if (barrierCollision) {

						barriers->status[i] -= 1;
						enemyShots->status = 0;
						break;

					}
				}
			}
		}
			
			// checks if barrierCollision has occured and if so draws black at the shots current position and redraws the barriers 
			if (barrierCollision) {

				drawCuboid(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, lt24,(unsigned short) 0x0000); // clears the shot
				drawBarriers(barriers, lt24); // redraws the barriers

			// Checks if playerCollision has occured and if so draws black over shot current position, sets it status to 0. It also checks if player has enough lives
			} else if (playerCollision) {
				drawCuboid(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, lt24,(unsigned short) 0x0000); // clears the shot
				enemyShots->status = 0;

				// checks if player has 0 lives left and if so sets gameOver flag to true, else it reduces player lives by 1
				if (player->status <= 1) {

					player->status -= 1;
					exitOnFail(LT24_copyFrameBuffer(lt24,explosion,player->xPos,player->yPos-EXPLOSION_ICON_LENGTH+PLAYER_ICON_LENGTH,EXPLOSION_ICON_WIDTH, EXPLOSION_ICON_LENGTH),ERR_SUCCESS); // draws explosion at player position
					explosion_audio(audio); // plays explosion audio
					*gameOver = true;


				} else {
					
					player->status -= 1; // reduce the health of the player by 1
					
				}

			// Checks if enemy shot hits end of screen flag is high and if so draws black over the shot and sets its status low
			} else if (endOfScreenCollision){

				drawCuboid(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, lt24,(unsigned short) 0x0000); // clears the shot
				enemyShots->status = 0;
			
			// If no collision has occured then the shot position is updated and its trail removed
			} else {

				enemyShots->yPos = enemyShots->yPos + ENEMY_LASER_DY; // moves shot by dy down screen
				drawCuboid(enemyShots->xPos, enemyShots->yPos-ENEMY_LASER_DY, ENEMY_LASER_WIDTH, ENEMY_LASER_DY, lt24,(unsigned short) 0x0000); // removes trail of shot
				drawCuboid(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, lt24,(unsigned short) 0xF800); // draws enemy shot at its new position

			}
		

	// no enemy shot is currently in the air so it checks if enough time has passed between firing the last shot and if so a shot is fired from a random enemy that is alive in the lowest row
	} else if (enemyShots->lastTimeShot - *private_timer_current_value >= ENEMY_NEW_SHOT_DELAY) {

		// checks if there is an enemy alive in the lowest row and if so fires a shot from its position
		if (getLowestEnemyAlive(enemies, enemyShots)) {

			enemyShots->status = 1;
			exitOnFail(Timer_ReadCurrent(timerCtx, private_timer_current_value), ERR_SUCCESS); 
			enemyShots->lastTimeShot = *private_timer_current_value; // stores time when shot is fired
			drawCuboid(enemyShots->xPos, enemyShots->yPos, ENEMY_LASER_WIDTH, ENEMY_LASER_LENGTH, lt24,(unsigned short) 0xF800); // draws shot fired from enemy position

		}
	}
}

// gets the x and y position of a randomly chosen alive enemy in the lowest row with an alive enemy in it and uses this to set the x y position of the enemyshot so that it fires from the randomly chosen enemy
bool getLowestEnemyAlive(struct enemyInfo * enemies, struct enemyShotInfo * enemyShots) {
	
	bool targetFound = false; // flag to that goes high if an enemy has been found alive in a row
	unsigned int aliveJPos[ENEMY_COLS]; // array that will be popualted with the column index locations of alive enemies in a row
	unsigned int rowAlive; // Stores the row where the enemies are alive
	unsigned int numAlive; // stores the number of alive enemies left in a row

	// Stores the index position of all alive enemies in the lowest row with alive enemies.
	for (signed int i = ENEMY_ROWS-1; i>=0; i--) {

		numAlive = 0;

		for (unsigned int j = 0; j<ENEMY_COLS; j++) {
			
			//checks if the current enemy checked in loop is alive, if so then stores the j index position of the alive enemy in the aliveJPos array and increases the counter for the number of enemies alive in the row
			if (enemies->status[i][j]) {

				aliveJPos[numAlive] = j; // Stores the column index positon of the alive enemy
				numAlive++;
				targetFound = true;

			}
		}
		
		// checks if an alive enemy has been found in the current row and if so stores its row index position
		if (targetFound) {

			rowAlive = i; // stores the row index position of the alive enemies
			break;

		}
	}

	// Checks if any enemies are alive and if so it sets the x and y position for the next shot to be from a randomly selected enemy alive.
	if (numAlive) {

		unsigned int index = rand() % numAlive; // gets row index position of a random enemy that is still alive in the lowest row with enemies alive
		enemyShots->xPos = enemies->xPos[rowAlive][aliveJPos[index]] + ENEMY_ICON_WIDTH/2; // setting x position of enemy laser so that it is shooting from the random lowest alive enemy
		enemyShots->yPos = enemies->yPos[rowAlive][aliveJPos[index]] + ENEMY_ICON_LENGTH + 2; // setting y position of enemy laser so that it is shooting from the random lowest alive enemy

	}

	return targetFound; // returns true if an enemy is alive else false
}

// Initially populates the enemies with their initial positions
void initialPopBarrierInfo(struct barrierInfo * barriers) {

	for (int i = 0; i < BARRIER_COLS; i++) {

		barriers->xPos[i] = ((BARRIER_ICON_WIDTH + BARRIER_COL_SPACING) * i + 19);
		barriers->yPos[i] = SCREEN_LENGTH - 50;
		barriers->status[i] = 5;

	}

}

// Adjust barrier settings based on the selected difficulty level
	void selectDifficulty(struct barrierInfo * barriers, int difficulty_level) {

	    switch (difficulty_level) {
	        case 0:
	            // Set to default values- All barriers
	            break;

	        case 1:
	            for (int i = 0; i < BARRIER_COLS; i++) {
	                barriers->xPos[i] = ((BARRIER_ICON_WIDTH + BARRIER_COL_SPACING) * i + 19);
	                barriers->yPos[i] = SCREEN_LENGTH - 50;
	                if (i % 2 == 0) {
	                    barriers->status[i] = 5;
	                } else {
	                    barriers->status[i] = 0;
	                }
	            }//Three barriers
	            break;

	        case 2:
	            for (int i = 0; i < BARRIER_COLS; i++) {
	                barriers->xPos[i] = ((BARRIER_ICON_WIDTH + BARRIER_COL_SPACING) * i + 19);
	                barriers->yPos[i] = SCREEN_LENGTH - 50;
	                barriers->status[i] = 0; // No barriers
	            }
	            break;

	        default:
	            break;
	    }
	}


// Draws Each barrier icon based on its health
void drawBarriers(struct barrierInfo * barriers, PLT24Ctx_t lt24) {

	for (int i=0;i<BARRIER_COLS;i++) {

		switch (barriers->status[i]) {
			case 5:
			
				exitOnFail(LT24_copyFrameBuffer(lt24,barrier_health_5,barriers->xPos[i],barriers->yPos[i],BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH),ERR_SUCCESS);
			
				break;
			case 4:

				exitOnFail(LT24_copyFrameBuffer(lt24,barrier_health_4,barriers->xPos[i],barriers->yPos[i],BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH),ERR_SUCCESS);

				break;
			case 3:

				exitOnFail(LT24_copyFrameBuffer(lt24,barrier_health_3,barriers->xPos[i],barriers->yPos[i],BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH),ERR_SUCCESS);

				break;
			case 2:

				exitOnFail(LT24_copyFrameBuffer(lt24,barrier_health_2,barriers->xPos[i],barriers->yPos[i],BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH),ERR_SUCCESS);

				break;
			case 1:

				exitOnFail(LT24_copyFrameBuffer(lt24,barrier_health_1,barriers->xPos[i],barriers->yPos[i],BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH),ERR_SUCCESS);

				break;
			
			default:

				drawCuboid(barriers->xPos[i], barriers->yPos[i], BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH, lt24,(unsigned short) 0x0000);
		}
	}
}

// takes the top left x y position of two objects as well as their lengths and widths and returns true if they overlap and false if not
bool isCollided (unsigned int x0, unsigned int y0, unsigned int width0, unsigned int length0, unsigned int x1, unsigned int y1, unsigned int width1, unsigned int length1) {

	if (((x0 >= x1) && (x0 < (x1 + width1))) || (((x0 + width0 -1) >= x1) && ((x0 + width0 - 1) < (x1 + width1))) || ((x0<x1) && (x0+width0 > x1+width1)) || ((x1<x0) && (x1+width1 > x0+width0))) {
		if (((y0 >= y1) && (y0 < (y1 + length1))) || (((y0 + length0 -1) >= y1) && ((y0 + length0 - 1) < (y1 + length1)))  || ((y0<y1) && (y0+length0-1 > y1+length1)) || ((y1<y0) && (y1+length1-1 > y0+length0)) ) {

			return true;

		} else { return false; }
	} else { return false; }
}


// Updates the movement of the enemies, enemies start at the top left of the screen and move right, when they arrive to the end of the right screen, they move down once to return to the left.
void updateEnemyInfo(struct enemyInfo * enemies, PLT24Ctx_t lt24, struct barrierInfo * barriers, int * dx, bool * moveDown, bool * gameOver) {
	bool movedDown = false; // For down movement, flag that checks if the enemies have moved down one time
	bool barrierCollision = false;

	// For every enemy
	for (int i=ENEMY_ROWS-1;i>=0;i--){
		for (int j=ENEMY_COLS-1;j>=0;j--){

			// Check if they are alive for the movement
			if (enemies->status[i][j] == ALIVE){

				// Movement of the enemies
				if (* moveDown == false){
					enemies->xPos[i][j] += *dx; // Moves  right or left with a distance dx
					drawCuboid(enemies->xPos[i][j] - *dx, enemies->yPos[i][j], ENEMY_ICON_WIDTH, ENEMY_ICON_LENGTH, lt24, LT24_BLACK); // Removes track of enemies previous location
				}
				else {
					enemies->yPos[i][j] += ENEMY_ROW_SPACING; // Moves down with a distance ENEMY_ROW_SPACING
					drawCuboid(enemies->xPos[i][j], enemies->yPos[i][j] - ENEMY_ROW_SPACING, ENEMY_ICON_WIDTH, ENEMY_ICON_LENGTH, lt24, LT24_BLACK); // Removes track of enemies previous location
				}

				// Checks for collisions with all barriers
				for (int l=0;l<BARRIER_COLS;l++) {

					if (barriers->status[l]) {

						barrierCollision = isCollided(enemies->xPos[i][j], enemies->yPos[i][j], ENEMY_ICON_WIDTH, ENEMY_ICON_LENGTH, barriers->xPos[l], barriers->yPos[l], BARRIER_ICON_WIDTH, BARRIER_ICON_LENGTH);

						if (barrierCollision) {
							// If collision occurred eliminate the barrier and the enemy
							barriers->status[l] = 0;
							enemies->status[i][j] = DEAD;
							drawBarriers(barriers, lt24);

							}
						}
					}
			}
			// Game Over when reaching the end of the screen at the bottom
			if (enemies->yPos[i][j] >= SCREEN_LENGTH-20) {
					*gameOver = true;
			}
		}
	}

	// Flag that switches to check if the enemies have moved down once
	if (* moveDown) {

		*moveDown = false;
		movedDown = true;

	}

	// Checks the enemy at the left-most position
	int colAliveLeft=0;
	int rowAliveLeft=0;
	for (int j=0;j<ENEMY_COLS;j++) {
		for (int i=0;i<ENEMY_ROWS;i++) {
			if (enemies->status[i][j]){
				rowAliveLeft= i; // sets the variable rowAliveLeft with the enemy that is alive
				break;
			}
		}
		if (enemies->status[rowAliveLeft][j]){
			colAliveLeft= j; // sets the variable colAliveLeft with the enemy that is alive
			break;
		}
	}

	// Checks the enemy at the right-most position
	int colAliveRight=ENEMY_COLS-1;
	int rowAliveRight=ENEMY_ROWS-1;
	for (int j=ENEMY_COLS-1;j>=0;j--) {
		for (int i=0;i<ENEMY_ROWS;i++) {
			if (enemies->status[i][j]){
				rowAliveRight= i; // sets the variable rowAliveRight with the enemy that is alive
				break;
			}
		}
		if (enemies->status[rowAliveRight][j]){
			colAliveRight= j; // sets the variable colAliveRight with the enemy that is alive
			break;
		}
	}

	// If the enemies are moving right and reached the right end of the screen
	if (enemies->xPos[rowAliveRight][colAliveRight] >= SCREEN_WIDTH-20 && !movedDown) {
		*dx = -10; // The direction has to change to the left
		*moveDown = true; // Now it is time to move down one time
	}
	// If the enemies are moving left and reached the left end of the screen
	else if ((enemies->xPos[rowAliveLeft][colAliveLeft] <= 11 || enemies->xPos[rowAliveLeft][colAliveLeft] >= SCREEN_WIDTH) && !movedDown) {
		*dx = 10; // The direction has to change to the right
		*moveDown = true; // Now it is time to move down one time
	}
}

// Checks if any enemies are alive
void allEnemiesDead(struct enemyInfo * enemies, PLT24Ctx_t lt24, unsigned int * enemySpeedDivider, int * dx, bool * moveDown){

	bool allDead = true;
	// For every enemy
	for (int i=ENEMY_ROWS-1;i>=0;i--) {
		for (int j=ENEMY_COLS-1;j>=0;j--) {

			if (enemies->status[i][j]) {

				allDead = false;

			} //sets allDead to false if an alive enemy is found
		}
	}

	// If no enemies are left alive then it spawns a new set of enemies at the top of the screen
	if (allDead) {
		for (int i=ENEMY_ROWS-1;i>=0;i--){
			for (int j=ENEMY_COLS-1;j>=0;j--){

				enemies->status[i][j] = ALIVE;

			}
		}

		*enemySpeedDivider += 2; // Increases the enemies speed as well
		initialPopEnemyInfo(enemies); // Redraws the enemies at the top left of the screen
		* moveDown = false; // Resets the variable moveDown in case they have just moved down
		* dx = 10; // Resets the variable dx in case they were moving left, they have to start moving right.

	}

	HPS_ResetWatchdog(); //Just reset the watchdog.
}

// To print the score in the HEX
void drawScore (unsigned int score){

	// If score is higher than 99, use the second display to have scores up to 999
	if (score>99){
		DE1SoC_SevenSeg_SetSingle(2,score/100);
		DE1SoC_SevenSeg_SetDoubleDec(0, score%100);
	}
	else{
		DE1SoC_SevenSeg_SetSingle(2,0);
		DE1SoC_SevenSeg_SetDoubleDec(0, score); // Sets the score up to 99 in first display
	}

}

// To light the LEDS according to lives
void drawLives (volatile unsigned int *LEDR_ptr, struct playerInfo * player){
	  switch (player->status) {
			case 5: *LEDR_ptr=0x1F; break;
			case 4: *LEDR_ptr=0xF; break;
			case 3: *LEDR_ptr=0X7; break;
			case 2: *LEDR_ptr=0x3; break;
			case 1: *LEDR_ptr=0x1; break;
			case 0: *LEDR_ptr= 0; break;
	  }
}

    volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;
	unsigned int key_last_state = 0;
// Function to detect and return the currently pressed keys.
// This function has been taken from lab notes in Unit 1 on Minerva.
unsigned int getPressedKeys() {

    unsigned int key_current_state = *key_ptr;
    unsigned int keys_pressed = (~key_current_state) & (key_last_state);
    key_last_state = key_current_state;
    return keys_pressed;
}


/*
 * Display the start screen and prompt the user to select a difficulty level.
 * Returns the selected difficulty level.
 */
int displayStartScreen(PLT24Ctx_t lt24) {
    // Display the start screen
    exitOnFail(
        LT24_copyFrameBuffer(lt24, start_screen, 0, 0, 240, 320),
        ERR_SUCCESS);

    // Define difficulty levels
    enum { EASY, NORMAL, HARD };
    unsigned int difficulty_level = EASY; // Initialize to EASY by default

    unsigned int keys_pressed;
    do {
        // Check for pressed keys to adjust the difficulty level
        keys_pressed = getPressedKeys();
        if (keys_pressed & 0x8) {
            difficulty_level = (difficulty_level + 1) % 3; // Cycle through difficulty levels
        } else if (keys_pressed & 0x4) {
            difficulty_level = (difficulty_level - 1 + 3) % 3; // Cycle through difficulty levels
        }

        // Display the corresponding difficulty level image
        switch (difficulty_level) {
            case EASY:
                exitOnFail(
                    LT24_copyFrameBuffer(lt24, easy, 0, 0, 240, 320),
                    ERR_SUCCESS);
                break;
            case NORMAL:
                exitOnFail(
                    LT24_copyFrameBuffer(lt24, normal, 0, 0, 240, 320),
                    ERR_SUCCESS);
                break;
            case HARD:
                exitOnFail(
                    LT24_copyFrameBuffer(lt24, hard, 0, 0, 240, 320),
                    ERR_SUCCESS);
                break;
        }

        // Reset the watchdog timer
        HPS_ResetWatchdog();

    } while (!(keys_pressed & 0x1)); // Continue looping until the confirm key is pressed

    // Display a black background screen
    exitOnFail(
        LT24_copyFrameBuffer(lt24, black, 0, 0, 240, 320),
        ERR_SUCCESS);

    return difficulty_level; // Return the selected difficulty level
}


// Display the game over screen
void displayGameOverScreen(PLT24Ctx_t lt24) {
    unsigned int keys_pressed;
    exitOnFail(
        LT24_copyFrameBuffer(lt24, game_over_screen, 0, 0, 240, 320),
        ERR_SUCCESS);
}

// Simple function for drawing a shot based on shot_length and shot_height
void drawShot(int x, int y, unsigned short colour, PLT24Ctx_t lt24){
	for (int i=0;i<SHOT_WIDTH;i++){
		for (int j=0;j<SHOT_LENGTH;j++){
			LT24_drawPixel(lt24,colour,x+i,y+j);
		}
	}
}

/* This function is responsible for creating the player shots and moving the player shots across the screen.
 * There are multiple stages to this operation, the button status and shot cooldown must first be checked to
 * decide whether or not a new shot should be created. Then the amount of shots currently active must be calculated.
 * Each alive shot is then updated such that it moves upwards on the screen. We also perform all collision detection here.
 */
void updateShot(struct playerShotInfo * playerShot,struct playerInfo * player,struct enemyInfo *enemies, struct controlInfo * buttons,PLT24Ctx_t lt24,int *shootCooldown, unsigned int *score, struct barrierInfo * barriers){


	char playerShoots = buttons->shootVal;				// Read the shoot button value
	if (*shootCooldown == 0){							// Don't shoot if we haven't reached the cool down yet
		playerShoots = 0;
	}

	unsigned int currAlive = 0;							// Calculate how many shots are currently alive
	for (int i=0;i<SHOT_COUNT;i++){						// Clear any dead ones from the screen
		if (playerShot->status[i] == ALIVE){
			currAlive=i+1;
		}
		else drawShot(playerShot->xPos[i],playerShot->yPos[i],LT24_BLACK,lt24);
	}



	for (int i=0;i<=currAlive;i++){
		// For every active shot check the following IFs:

		// If we have maxxed out the shot count we don't need to do any more checks
		if (i==SHOT_COUNT) break;


		// If we shoot and the last shot has died:
		// 		Create a new shot on the player location
		// 		Enable this new shot
		// 		Start the shot cool down so player can't spam
		if (playerShoots & *shootCooldown & (playerShot->status[i]==DEAD)){
			playerShot->xPos[i] = player->xPos+PLAYER_ICON_WIDTH/2;
			playerShot->yPos[i] = player->yPos-SHOT_LENGTH;
			playerShot->status[i] = ALIVE;
			*shootCooldown=0;
			playerShoots=0;
		}


		// If the current shot is alive:
		// 		Update it's position and re-draw it
		if (playerShot->status[i]==ALIVE){
			drawShot(playerShot->xPos[i],playerShot->yPos[i],LT24_BLACK,lt24);
			playerShot->yPos[i] = playerShot->yPos[i]-3;
			drawShot(playerShot->xPos[i],playerShot->yPos[i],LT24_RED,lt24);

		}

		// If the current shot has gone off the screen:
		//		Set it's status to DEAD
		if (playerShot->yPos[i] <= 0 || playerShot->yPos[i]>=SCREEN_LENGTH){
			playerShot->status[i]=DEAD;
		}


		// For every enemy in the game, if the shot has collided with any of them:
		// 		Set the enemy to DEAD and the shot to DEAD
		for (int j=0; j < ENEMY_ROWS; j++){
			for (int k=0; k < ENEMY_COLS;k++){
				if (isCollided(enemies->xPos[j][k],enemies->yPos[j][k],ENEMY_ICON_WIDTH,ENEMY_ICON_LENGTH,playerShot->xPos[i],playerShot->yPos[i],SHOT_WIDTH,SHOT_LENGTH) & playerShot->status[i] & enemies->status[j][k]){
					enemies->status[j][k]=DEAD;
					drawCuboid(enemies->xPos[j][k], enemies->yPos[j][k], ENEMY_ICON_WIDTH, ENEMY_ICON_LENGTH, lt24, LT24_BLACK);
					drawShot(playerShot->xPos[i],playerShot->yPos[i],LT24_BLACK,lt24);
					playerShot->status[i]=DEAD;
					playerShot->xPos[i]=0;
					playerShot->yPos[i]=0;
					*score = *score +1;
				}
				HPS_ResetWatchdog();  //Reset the watchdog
			}
		}

		// For every barrier in the game, if the shot has collided with any of them:
		// 		Reduce the barrier health by 1, and set the shot to DEAD
		for (int j=0; j< BARRIER_COLS;j++){
			if (isCollided(barriers->xPos[j], barriers->yPos[j],BARRIER_ICON_WIDTH,BARRIER_ICON_LENGTH,playerShot->xPos[i],playerShot->yPos[i],SHOT_WIDTH,SHOT_LENGTH) & playerShot->status[i] & !(barriers->status[j]==0)){
				barriers->status[j]=barriers->status[j] -1;
				drawShot(playerShot->xPos[i],playerShot->yPos[i],LT24_BLACK,lt24);
				playerShot->status[i]=DEAD;
				playerShot->xPos[i]=0;
				playerShot->yPos[i]=0;
				drawBarriers(barriers,lt24);

			}
		}
	}
}

// Loops through a 2D array containing the icon of the player, drawing a pixel on the screen if it is in the icon
void drawPlayer(int x, int y, unsigned short colour, PLT24Ctx_t lt24){
	int player[7][11]={
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,1,1,0,0,0,0},
		{0,0,0,0,1,1,1,0,0,0,0},
		{0,1,1,1,1,1,1,1,1,1,0},
		{1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1}};
	for (int i=0; i < 7; i++){
		for (int j=0; j < 11;j++){
			if (player[i][j]){
				LT24_drawPixel(lt24,colour,x+j,y+i);
			}

		}
	}
}

// Update the position of the player based on the current buttons pressed. If the player is moving off the edge of screen we block this
void updatePlayer(struct playerInfo * player, struct controlInfo * buttons ,PLT24Ctx_t lt24, struct enemyShotInfo * enemyShots){

	if (buttons->xVal != 0) {    // Nonzero value means moving in positive X or negative X direction
		drawPlayer(player->xPos,player->yPos,LT24_BLACK, lt24);
		player->xPos=player->xPos + buttons->xVal;
	}
	if (player->xPos <= 3){      // Going off the left edge of the screen is blocked
		player->xPos=3;
	}
	if (player->xPos >= SCREEN_WIDTH - PLAYER_ICON_WIDTH-1 ){  // Going off the right edge of the screen is blocked
		player->xPos=SCREEN_WIDTH - PLAYER_ICON_WIDTH-1;
	}

    drawPlayer(player->xPos,player->yPos,LT24_WHITE, lt24);  // Draw the player in their new position
}

// Takes pointers to all memory mapped control peripherals used by the game and assigns their values to the button values struct
// Depending on weather or not the SW0 is pressed we select between analogue ADC input or push button controls.
void getButtons(struct controlInfo * buttons,volatile unsigned int *ADC_chan0_ptr,volatile unsigned int *ADC_chan2_ptr,volatile unsigned int *ADC_chan4_ptr, volatile unsigned char *key_ptr, volatile unsigned char *slide_ptr){

	if (*slide_ptr & 0x1){    // If slider is high then use ADC inputs

		// The dX is a function of the ADC analogue value
		buttons->xVal = 2-(round(((*ADC_chan2_ptr & 0xFFF)*4)/4095));
		// If the analogue stick is depressed we set the shoot command value
		buttons->shootVal = !((*ADC_chan4_ptr & 0xFFF));

    }
    else {              // Else directly access the push buttons

    	char playerLeft = *key_ptr & 0x8; 	//1000
    	char playerRight = *key_ptr & 0x4; 	//0100

    	if (playerLeft){
    		buttons->xVal = -2;
    	}
    	else if (playerRight){
    		buttons->xVal = 2;
    	}
    	else buttons->xVal = 0;

    	// Set a shoot command value high if rightmost button is presed
    	buttons->shootVal = *key_ptr & 0x1; //0001
    }


}

void shotAudio(struct controlInfo * buttons,PWM8731Ctx_t audio,int *shootCooldown, volatile unsigned char *slide_ptr){
	char playerShoots = buttons->shootVal;				// Read the shoot button value
	// If fire button is pressed, shooting is allowed and SW2 is high, play shot audio
    if (playerShoots && *shootCooldown == 0 && (*slide_ptr & 0x4)){
        player_shot_audio(audio); // play generated shot sound
    }
}
