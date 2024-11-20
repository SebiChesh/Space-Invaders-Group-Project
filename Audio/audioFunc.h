/*
 * audioFunc.h
 *
 *  Created on: 26 Apr 2024
 *      Author: Russo-BIT
 */

#ifndef AUDIO_AUDIOFUNC_H_
#define AUDIO_AUDIOFUNC_H_

#define PI 3.14159265358979323846

void player_shot_audio (PWM8731Ctx_t audio);
void countdown_audio(PWM8731Ctx_t audio);
void explosion_audio(PWM8731Ctx_t audio);

extern const signed int explosionAudio[133279][2];
extern const signed int countdownAudio[169968][2];

#endif /* AUDIO_AUDIOFUNC_H_ */
