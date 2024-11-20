/*
 * spaceInvadeParams.h
 *
 *  Created on: Apr 11, 2024
 *      Author: sebichesh
 * 		Contributors: TomBirkbeck01, el23alyt, el23vm
 *      Description: This file contains all the global parameters required for the space invaders game
 *      and provides a central location where parameters used by many functions can be changed.
 */

#ifndef SPACEINVADEPARAMS_H_
#define SPACEINVADEPARAMS_H_

#define ENEMY_COLS 6 // Number enemies in a single row
#define ENEMY_ROWS 4 // Number of rows of enemies
#define ENEMY_LASER_WIDTH 2 // Width of enemy laser
#define ENEMY_LASER_LENGTH 10 // Height of enemy laser
#define ENEMY_LASER_DY 5 // Amount laser moves in y direction every update
#define ENEMY_COL_SPACING 9 // Number pixels between two icons in a column
#define ENEMY_ROW_SPACING 9 // Number pixels between two icons in row
#define ENEMY_ICON_WIDTH 11 // width in pixels of the enemy icon
#define ENEMY_ICON_LENGTH 8 // Length in pixels of the enemy icon
#define ALIVE 1 // value representing something is alive
#define DEAD 0 // value representing something is dead
#define SCREEN_WIDTH 240 // Width of LT24 display
#define SCREEN_LENGTH 320 // Length of LT24 display
#define NUM_DIFFICULTY_LEVELS 3 // Number of difficulty levels user will be able to choose
#define ENEMY_NEW_SHOT_DELAY 674999997 // Time between firing a new shot from an enemy
#define BARRIER_COLS 5 // max number of barriers
#define BARRIER_ICON_WIDTH 25 // Width of barrier icon
#define BARRIER_ICON_LENGTH 25 // Length of barrier icon
#define BARRIER_COL_SPACING 19 // Spacing between barriers
#define PLAYER_ICON_WIDTH 11 // Width of the player icon in pixels
#define PLAYER_ICON_LENGTH 7 // Length of the player icon in pixels
#define SHOT_COUNT 3 // Max number shots player can fire at once
#define SHOT_LENGTH 8 // Length of the player shot in pixels
#define SHOT_WIDTH 2 // Width of the player shot in pixels
#define PLAYER_X_DEFAULT 120
#define PLAYER_Y_DEFAULT 300
#define EXPLOSION_ICON_WIDTH 25 // Width of explosion icon
#define EXPLOSION_ICON_LENGTH 30 // Length of explosion icon
#endif /* SPACEINVADEPARAMS_H_ */
