/*
 * main.c
 *
 *  Created on: Apr 11, 2024
 *      Author: sebichesh
 * 		Contributors: TomBirkbeck01, el23alyt, el23vm, Russo-BIT
 *      Description: Main file for running space invaders game
 *      Acknowledgements: LT24 driver was kindly provided by T Carpenter, the A9 HPS private timer was developed by sebastian cheshire during the lab classes, Seven-Seg driver was developed by Andrea Yanez during Assignment 1 with code provided by David Cowell and Harry Clegg 
 */
#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"
#include "HPS_PrivateTimer/HPS_PrivateTimersSebi.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_GPIO/HPS_GPIO.h"
#include "HPS_I2C/HPS_I2C.h"
#include "spaceInvadeFunc.h"
#include <stdlib.h>
#include "Images/barrier_health_5.h"
#include "Images/barrier_health_4.h"
#include "Images/barrier_health_3.h"
#include "Images/barrier_health_2.h"
#include "Images/barrier_health_1.h"
#include "Audio/audioFunc.h"


int main (void) {
	while(1){

		//LT24 display ctx pointer
		PLT24Ctx_t lt24;

		//Initialise the LCD Display.
		exitOnFail(
				LT24_initialise(LSC_BASE_GPIO_JP1,LSC_BASE_LT24HWDATA, &lt24), //Initialise LCD
				ERR_SUCCESS);                                                  //Exit if not successful
		HPS_ResetWatchdog();

		//Initialise the Start Screen.
		unsigned int difficulty_level = displayStartScreen(lt24);

	    // Context variables for audio CODEC, GPIO, and I2C
	    PWM8731Ctx_t audio;
	    PHPSGPIOCtx_t gpio;
	    PHPSI2CCtx_t i2c;


	    // Initialising drivers for audio output
	    exitOnFail(HPS_GPIO_initialise(LSC_BASE_ARM_GPIO, ARM_GPIO_DIR, ARM_GPIO_I2C_GENERAL_MUX, 0, &gpio), ERR_SUCCESS);
	    exitOnFail(HPS_I2C_initialise(LSC_BASE_I2C_GENERAL, I2C_SPEED_STANDARD, &i2c), ERR_SUCCESS);
	    exitOnFail(WM8731_initialise(LSC_BASE_AUDIOCODEC, i2c, &audio), ERR_SUCCESS);

		//explosion_audio(audio);

		//A9 Timer Variables
		bool enable = true;
		bool autoReload = true;
		bool interrupt = false;
		unsigned int timer_prescaler = 0;
		unsigned int private_timer_current_value;
		unsigned int private_timer_interrupt_status;
		PTimerCtx_t timerCtx;
		//Initialising PV timer
		exitOnFail(Timer_initialise(LSC_BASE_PRIV_TIM, &timerCtx), ERR_SUCCESS);
		exitOnFail(Timer_SetLoad(timerCtx, 0xFFFFFFFF), ERR_SUCCESS); //Set Timer Controller Load Value
		exitOnFail(Timer_SetControl(timerCtx, enable, autoReload, interrupt, timer_prescaler), ERR_SUCCESS); // Sets the timer control;

		//Event Scheduler variables
	    unsigned int eventSchedulerTimings[4] = {224999999/20, 224999999, 224999999/2, 224999999/40}; // stores the timing delays for updating the enemy shot, enemy position, player shot cooldown and player shot position
		unsigned int eventSchedulerLastTriggered[4]; //stores when the events were last triggered 
		unsigned int enemySpeedDivider = 1; // variable for speeding up enemies after every wave

		//Initialising and populating enemy info
		struct enemyInfo enemies; // initialises instance of enemyInfo structure called enemies
		initialPopEnemyInfo(&enemies); // populates the enemies structure with their initial x, y positions and sets each enemies status to alive i.e 1
		drawInvaders(&enemies, lt24); // draws invaders

		//Initalising enemyShotInfo
		struct enemyShotInfo enemyShots;
		enemyShots.lastTimeShot = ENEMY_NEW_SHOT_DELAY + 4;
		enemyShots.status = 0;

		//Initialising player
		struct playerInfo player; // Initialise and populate the playerInfo struct, player will start at (120,300)
		player.xPos = PLAYER_X_DEFAULT;
		player.yPos = PLAYER_Y_DEFAULT;
		player.status = 5;
		// Difficulty selected
		unsigned int difficulty = 2;

		//Initialising barrierInfo
		struct barrierInfo barriers;
	    // Initialize barriers with default settings
	    initialPopBarrierInfo(&barriers);
	    // Adjust barrier settings based on the selected difficulty level
	    selectDifficulty(&barriers, difficulty_level);
	    drawBarriers(&barriers, lt24);

	    // Play countdown audio before player is drawn to prepare player for gameplay
		countdown_audio(audio);

		//Initialising playerShotInfo
		struct playerShotInfo playerShot; // Initialise and populate the playerShot struct
		for (int i=0;i<SHOT_COUNT;i++){
			playerShot.xPos[i]=0;
			playerShot.yPos[i]=0;
			playerShot.status[i]=0;
		}

		int shootCooldown = 0;

		//gameOver variable
		bool gameOver = false;

		// Variables for updateEnemyInfo
		bool moveDown = false;
		int dx= ENEMY_ICON_LENGTH;

		// Red LEDs base address
		volatile unsigned int *LEDR_ptr = (unsigned int *) LSC_BASE_RED_LEDS;
		*LEDR_ptr = 0X1F;   // Set initial value of LEDs

		//Score variable
		unsigned int score = 0;

		//Initialising controlInfo
		struct controlInfo buttons;

		//Physical button memory base addresses
		volatile unsigned int *ADC_chan0_ptr = ((unsigned int *)0xFF204000);
		volatile unsigned int *ADC_chan1_ptr = ((unsigned int *)0xFF204004);  // Declare analog input pin locations
		volatile unsigned int *ADC_chan2_ptr = ((unsigned int *)0xFF204008);
		volatile unsigned int *ADC_chan4_ptr = ((unsigned int *)0xFF204010);
		*ADC_chan1_ptr = 1;														// Driving a 1 on channel 1 forces auto-update of values

		volatile unsigned char *key_ptr = ((unsigned char*)0xFF200050);    // Pointer to the 4 push buttons
		volatile unsigned char *slide_ptr = ((unsigned char*)0xFF200040);  // Pointer to the slide switches
		volatile unsigned char *switch_ptr =  ((unsigned char*)0xFF200053);  // Pointer to the slide switches


		int flag = 1;
		while (flag) {


			exitOnFail(Timer_ReadCurrent(timerCtx, &private_timer_current_value), ERR_SUCCESS);

			getButtons(&buttons,ADC_chan0_ptr,ADC_chan2_ptr,ADC_chan4_ptr,key_ptr,slide_ptr);

			// checks if enough time has passed since the enemy shot position was last updated
			if (eventSchedulerLastTriggered[0] - private_timer_current_value >= eventSchedulerTimings[0]) {

				eventSchedulerLastTriggered[0] = private_timer_current_value; // stores time when enemy shot was last updated
				updateEnemyShot(&enemies, &enemyShots, lt24, timerCtx, audio, &private_timer_current_value, difficulty, &barriers, &player, &gameOver);
			};

			exitOnFail(Timer_ReadCurrent(timerCtx, &private_timer_current_value), ERR_SUCCESS);

			// checks if enough time has passed for the movement of the enemies
			if (eventSchedulerLastTriggered[1] - private_timer_current_value >= (eventSchedulerTimings[1]/enemySpeedDivider)) { // enemySpeedDivider allows to increase the enemies speed, every time all enemies die

				eventSchedulerLastTriggered[1] = private_timer_current_value;
				drawScore (score); // Outputs score in the HEX displays (0-2)
				drawLives (LEDR_ptr, &player); // Turns the LEDs according to the lives (1-5)
				updateEnemyInfo(&enemies, lt24, &barriers, &dx, &moveDown, &gameOver); // Updates the movement of the enemies
				allEnemiesDead(&enemies, lt24, &enemySpeedDivider, &dx, &moveDown); // When all enemies die, draws a new set of enemies at the top left screen and updates the enemySpeedDivider
				drawInvaders(&enemies, lt24); // Draws enemies

			}

			if (eventSchedulerLastTriggered[2] - private_timer_current_value >= eventSchedulerTimings[2]){
				eventSchedulerLastTriggered[2] = private_timer_current_value;
				shootCooldown = 1;   // Only allow the player to shoot another shot if the cooldown has been reached (reload time)
			}

			if (eventSchedulerLastTriggered[3] - private_timer_current_value >= eventSchedulerTimings[3]){
				updateShot(&playerShot,&player,&enemies, &buttons,lt24,&shootCooldown,&score,&barriers);  	// Create new shots and update existing ones
				updatePlayer(&player,&buttons,lt24,&enemyShots);											// Update the player location based on button inputs
				shotAudio(&buttons, audio, &shootCooldown, slide_ptr);
				eventSchedulerLastTriggered[3] = private_timer_current_value;
			}
			if(gameOver)
			{
				while (!(*key_ptr)){
					volatile unsigned char *switch_ptr =  ((unsigned char*)0xFF200051);  // Pointer to the slide switches
					drawLives (LEDR_ptr, &player);
					displayGameOverScreen(lt24);
					HPS_ResetWatchdog(); //Just reset the watchdog.
				}

				flag = 0;

			}

			HPS_ResetWatchdog(); //Just reset the watchdog.
		}

		HPS_ResetWatchdog();

	}
}
