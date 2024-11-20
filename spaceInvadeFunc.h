/*
 * spaceInvadeFunc.h
 *
 *  Created on: Apr 11, 2024
 *      Author: sebichesh
 * 		Contributors: TomBirkbeck01, el23alyt, el23vm, Russo-BIT
 */
#include "spaceInvadeParams.h"
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"

#ifndef SPACEINVADEFUNC_H_
#define SPACEINVADEFUNC_H_

struct enemyInfo {
	unsigned int xPos[ENEMY_ROWS][ENEMY_COLS];
	unsigned int yPos[ENEMY_ROWS][ENEMY_COLS];
	unsigned int status[ENEMY_ROWS][ENEMY_COLS];
};

struct enemyShotInfo {
	unsigned int xPos;
	unsigned int yPos;
	unsigned int status;
	unsigned int lastTimeShot;
	unsigned int lowestEnemyCol;
	unsigned int LowestEnemyRow;
};

struct barrierInfo {
	unsigned int xPos[BARRIER_COLS];
	unsigned int yPos[BARRIER_COLS];
	unsigned int status[BARRIER_COLS];
};

struct playerInfo {
	unsigned int xPos;
	unsigned int yPos;
	unsigned int status;
};

struct controlInfo{
	int xVal;
	unsigned int shootVal;
};

struct playerShotInfo {
	unsigned int xPos[SHOT_COUNT];
	unsigned int yPos[SHOT_COUNT];
	unsigned int status[SHOT_COUNT];
};

void initialPopEnemyInfo(struct enemyInfo * enemies);

void exitOnFail(signed int status, signed int successStatus);

void drawCuboid(unsigned int x0, unsigned int y0, unsigned int width, unsigned int length, PLT24Ctx_t lt24, unsigned short colour);

void drawInvaders(struct enemyInfo * enemies, PLT24Ctx_t lt24);

void drawInvader(int x, int y, unsigned short colour, PLT24Ctx_t lt24);

void drawExplosion(int x, int y, unsigned short colour, PLT24Ctx_t lt24);

void updateEnemyShot(struct enemyInfo * enemies, struct enemyShotInfo * enemyShots, PLT24Ctx_t lt24, PTimerCtx_t timerCtx, PWM8731Ctx_t audio, unsigned int* private_timer_current_value, unsigned int difficulty, struct barrierInfo * barriers, struct playerInfo * player, bool * gameOver);

bool getLowestEnemyAlive(struct enemyInfo * enemies, struct enemyShotInfo * enemyShots);

void initialPopBarrierInfo(struct barrierInfo * barriers);

void selectDifficulty(struct barrierInfo * barriers, int difficulty_level);

void drawBarriers(struct barrierInfo * barriers, PLT24Ctx_t lt24);

bool isCollided (unsigned int x0, unsigned int y0, unsigned int width0, unsigned int length0, unsigned int x1, unsigned int y1, unsigned int width1, unsigned int length1);

void updateEnemyInfo(struct enemyInfo * enemies, PLT24Ctx_t lt24, struct barrierInfo * barriers, int * dx, bool * moveDown, bool * gameOver);

void allEnemiesDead(struct enemyInfo * enemies, PLT24Ctx_t lt24, unsigned int * enemySpeedDivider, int * dx, bool * moveDown);

void drawScore (unsigned int score);

void drawLives (volatile unsigned int *LEDR_ptr, struct playerInfo * player);

int displayStartScreen(PLT24Ctx_t lt24);

void displayGameOverScreen(PLT24Ctx_t lt24);

void drawShot(int x, int y, unsigned short colour, PLT24Ctx_t lt24);

void updateShot(struct playerShotInfo * playerShot,struct playerInfo * player,struct enemyInfo *enemies, struct controlInfo * buttons,PLT24Ctx_t lt24,int *shootCooldown, unsigned int *score, struct barrierInfo *barriers);

void drawPlayer(int x, int y, unsigned short colour, PLT24Ctx_t lt24);

void updatePlayer(struct playerInfo * player, struct controlInfo * buttons ,PLT24Ctx_t lt24,struct enemyShotInfo * enemyShots);

void getButtons(struct controlInfo * buttons,volatile unsigned int *ADC_chan0_ptr,volatile unsigned int *ADC_chan2_ptr,volatile unsigned int *ADC_chan4_ptr, volatile unsigned char *key_ptr, volatile unsigned char *slide_ptr);

void shotAudio(struct controlInfo * buttons,PWM8731Ctx_t audio,int *shootCooldown, volatile unsigned char *slide_ptr);

void explosion_Audio(PWM8731Ctx_t audio, struct enemyShotInfo * enemyShots, struct playerInfo * player);

#endif /* SPACEINVADEFUNC_H_ */
