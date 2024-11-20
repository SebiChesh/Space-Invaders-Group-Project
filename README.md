## Project Description
This repository contains the files for implementing a version of the classic arcade game: Space Invaders on a DE1SoC board. The game is written entierely in C and was developed using ARM development studios. This was done for the group assessment for ELEC5620M at the University Of Leeds. The code is split into a main file, function and parameter file. The main file handles gameflow logic and event scheduling, whilst the function file contains functions that handle gameplay logic and a header file containing global parameters to improve the readability and simplifify the modification of code. The repository is split as follows: Firstly a table containg a list of all coding files in the repository together with a short description. Followed by a Description of each function used in the code.

| Location | File | Purpose |
| ---  | --- | --- |
| `fullBuild\HPS_PrivateTimer\HPS_PrivateTimersSebi.c`  | `HPS_PrivateTimersSebi.c` | Source file for the HPS Private timer driver developed by Sebastian Cheshire during lab classes|
| `fullBuild\HPS_PrivateTimer\HPS_PrivateTimersSebi.h`  | `HPS_PrivateTimersSebi.h` | Header file for the HPS Private timer driver developed by Sebastian Cheshire during lab classes |
| `fullBuild\DE1SoC_SevenSeg\DE1SoC_SevenSeg.c`  | `DE1SoC_SevenSeg.c` | Source file containing functions to drive the seven segment displays developed by Andre Yanez during lab classes |
| `fullBuild\DE1SoC_SevenSeg\DE1SoC_SevenSeg.h`  | `DE1SoC_SevenSeg.h` | Header file containing functions to drive the seven segment displays developed by Andre Yanez during lab classes |
| `fullBuild\Images`  | `N/A` | This folder contains the .c and .h files which store the RGB565 image arrays for all images used for the game |
| `fullBuild\main.c`  | `main.c` | Main file for running gameplay flow and event scheduling |
| `fullBuild\spaceInvadeFunc.c`  | `spaceInvadeFunc.c` | Source file containing all functions used to implement our version of Space Invaders |
| `fullBuild\spaceInvadeFunc.h`  | `spaceInvadeFunc.h` | Header file containing all functions used to implement our version of Space Invaders |
| `fullBuild\spaceInvadeParams.h`  | `spaceInvadeParams.h` | Header file contains all the global parameters required for the space invaders game and provides a central location where parameters used by many functions can be changed |
| `fullBuild\Audio`  | `N/A` | This folder contains all files associated with the implementation of audio playback |
| `fullBuild\Audio\Sounds`  | `N/A` | This folder contains the .c and .h files which store the sample arrays for the countdown and explosion audio |
| `fullBuild\Audio\audioFunc.c`  | `audioFunc.c` | Source file containing all functions used to implement the generation and playback of audio samples |
| `fullBuild\Audio\audioFunc.h`  | `audioFunc.h` | Header file containing all functions used to implement the generation and playback of audio samples |
| `fullBuild\Audio\Audio2c.m`  | `Audio2c.m` | MATLAB file containing the function used to convert .wav and .mp3 files to sample arrays in .c file |




## Detailed File/Function Description
### Main.c
This file is the primary entry point of the Space Invader game, containing the game initialization, game mechanics, hardware interaction, and user interface. Within the main game loop, it manages player movement, enemy behaviour, collision detection, and rendering graphics on the LCD display. It allows players to choose difficulty levels at the start screen and restart the game when they reach the game over screen. Timers and event schedulers are utilized to regulate game events and ensure smooth gameplay progression. The file interfaces with hardware peripherals using appropriate driver functions.

### spaceInvadeFunc.c/.h
These files contain all functions used to implement our version of Space Invaders. They include functions for game logic, collision detectors and drawing images to the LT24 display. The functions are as follows: 

#### drawCuboid
Draws a coloured rectangle with hex colour code "colour" of size "width" x "length" whose top left corner is at "x0" and "y0".

#### initialPopEnemyInfo
A helper function to populate an instance of the enemy info struct using parameters such as number of enemy rows / cols and enemy spacing.

#### drawInvaders
Loops through all enemies present in the game logic and if alive draws them at their respective locations by calling drawInvader.

#### drawInvader
Draws an invader icon using a 2D array containing the pixel art information for the icon.

#### updateEnemyShot
Updates and draws the position of the enemy shot that is fired and checks if it has collided with anything and updates the game accordingly

#### getLowestEnemyAlive
Gets the x and y position of a randomly chosen alive enemy in the lowest row with an alive enemy in it and uses this to set the x y position of the enemyshot so that it fires from the randomly chosen enemy

#### initialPopBarrierInfo
Initially populates the enemies with their initial positions

#### selectDifficulty
Adjusts barrier settings based on the selected difficulty level within the game

#### drawBarriers
Draws Each barrier icon based on its health as seen below.

![image](https://github.com/leeds-embedded-systems/ELEC5620M-Mini-Project-Group-24/assets/67597947/8db9907d-5cb6-4001-8071-07758900f468)

#### isCollided
Takes the top left x y position of two objects as well as their lengths and widths and returns true if they overlap and false if not.

#### updateEnemyInfo
Updates the movement of the enemies, enemies start at the top left of the screen and move right, when they reach the end of the right screen, they move down once to return to the left.

#### allEnemiesDead
Checks if any enemies are alive. If no enemies are left alive then it spawns a new set of enemies at the top of the screen at a faster speed.

#### drawScore
To print the score in the external seven-segment displays.

#### drawLives
To light the LEDS according to lives.

#### displayStartScreen
Presents the game's initial screen with customizable difficulty levels and intuitive navigation prompts

#### displayGameOverScreen
Displays "Game Over" message, guiding the player to return to the start screen for restarting the game by pressing a button.

#### drawShot
Simple function to draw a shot shapt at a given XY based on the shot size parameters

#### updateShot
Accesses and modifys the playerShot struct, if a new shot is fired it initialises the shot and sets it's status to alive. Any alive shot is updated to it's new position and redrawn such that it moves upwards along the screen. All shot collision is handled here; colliding with a barrier will reduce the barrier health by 1 and kill the shot, colliding with an enemy will kill both the enemy and the shot. Shots are also deleted if they have moved upwards off the top of the screen. A cooldown flag is accessed to limit the players firing rate.

#### drawPlayer
Draws the player icon using a 2D array containing the pixel art information for the icon.

#### updatePlayer
Reads the buttons statuses and moves the player icon if the user is trying to move. There are some conditions to stop the player from exiting off the sides of the screen. drawPlayer is called to redraw the icon in its new position.

#### getbuttons
Utilises the memory mapped peripherals to access the hardware I/O on the DE1, by defualt will use the 4 pushbuttons on the board to update the buttonVals struct, but switching SW0 on the board will change the control to the analog joystick conneced via the ADC.

#### shotAudio
Plays generated shot sound when player shoots and audio is turned on via switch 2.

### spaceInvadeParams.h
This file contains all the global parameters required for the space invaders game and provides a central location where parameters used by many functions can be changed. This was used instad of assigning variables or instead having gameplay paramters hard coded into each function, for readility and ease code changes if these variables changed in the future.

| Parameter Name | Value | Description |
| ---  | --- | --- |
| ENEMY_COLS  | 6 | Number enemies in a single row |
| ENEMY_ROWS  | 4 | Number of rows of enemies |
| ENEMY_LASER_WIDTH  | 2 | Width of enemy laser |
| ENEMY_LASER_LENGTH | 10 | Height of enemy laser |
| ENEMY_LASER_DY | 5 | Amount laser moves in y direction every update |
| ENEMY_COL_SPACING | 9 | Number pixels between two icons in a column |
| ENEMY_ROW_SPACING | 9 | Number pixels between two icons in row |
| ENEMY_ICON_WIDTH | 11 | Width in pixels of the enemy icon |
| ENEMY_ICON_LENGTH | 8 | Length in pixels of the enemy icon |
| ALIVE | 1 | value representing something is alive |
| DEAD | 0 | value representing something is dead |
| SCREEN_WIDTH | 240 | Width of LT24 display |
| SCREEN_LENGTH | 320 | Length of LT24 display |
| NUM_DIFFICULTY_LEVELS | 3 | Number of difficulty levels user will be able to choose |
| ENEMY_NEW_SHOT_DELAY | 674999997 | Time between firing a new shot from an enemy |
| BARRIER_COLS | 5 | max number of barriers |
| BARRIER_ICON_WIDTH | 5 | Width of barrier icon |
| BARRIER_ICON_LENGTH | 25 | Length of barrier icon |
| BARRIER_COL_SPACING | 19 | Spacing between barriers |
| PLAYER_ICON_WIDTH | 11 | Width of the player icon in pixels |
| PLAYER_ICON_LENGTH | 7 | Length of the player icon in pixels |
| SHOT_COUNT | 3 | Max number shots player can fire at once |
| SHOT_LENGTH | 8 | Length of the player shot in pixels |
| SHOT_WIDTH | 2 | Width of the player shot in pixels |
| PLAYER_X_DEFAULT | 120 | default x location on the screen for the player icon |
| PLAYER_Y_DEFAULT | 300 | default y location on the screen for the player icon |

### audioFunc.c/.h
This file contains all functions used to implement the generation and playback of audio samples. The functions are as follows: 

#### player_shot_audio
This function uses a phase accumulator to generate a tone of 0.1 seconds for the player shot audio and writes samples from the phase to the FIFO. It is an adaptation of the tone generation code from Lab Task 1 of ELEC5620M's Unit 3.1.

#### countdown_audio
This function writes the countdown audio samples to the FIFO.

#### explosion_audio
This function writes the explosion audio samples to the FIFO.

### Audio2c.m
This file contains the MATLAB function used to convert mono or stereo audio files from .mp3 or .wav format to a sample of arrays. It converts the samples to 32-bit signed integers.

## Game Instructions




https://github.com/leeds-embedded-systems/ELEC5620M-Mini-Project-Group-24/assets/67597947/90cf30a2-d81d-412e-89a9-0a6878ed3ecc





