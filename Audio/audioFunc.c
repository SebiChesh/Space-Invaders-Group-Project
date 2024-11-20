/*
 * audioFunc.c
 *
 *  Created on: 26 Apr 2024
 *      Author: Russo-BIT
        Acknowledgements: DE1SoC_WM8731 audio driver was kindly provided by T Carpenter
 */


#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_GPIO/HPS_GPIO.h"
#include "HPS_I2C/HPS_I2C.h"
#include "audioFunc.h"

// Include necessary libraries
#include <math.h>
#include <stdlib.h>

// The tone generation code from Lab Task 1 of ELEC5620M�s Unit 3.1 was adapted to develop this function and achieve the desired outcome
// This function generates a 0.1s tone for the player's laser shot sound effect
void player_shot_audio (PWM8731Ctx_t audio){

    PHPSGPIOCtx_t gpio;
    PHPSI2CCtx_t i2c;
	double phase = 0.0;
    double inc = 0.0;
    double ampl = 0.0;
    signed int audio_sample = 0;
    unsigned int sampleRate;
    unsigned int space;
    unsigned long current_sample_index = 0;

    WM8731_clearFIFO(audio, true,true);

 	WM8731_getSampleRate(audio, &sampleRate);
    inc   = 800.0 * PI / sampleRate;  // Calculate the phase increment based on desired frequency - 800Hz
    ampl  = pow(2,32);                   // Desired amplitude
    phase = 0.0;
    unsigned int totalSamples = 0.1*sampleRate; // Calculate total number of sampled based on desired duration - 0.1seconds
    unsigned int sampleCounter = 0;

    while (sampleCounter < totalSamples) {
        //Check FIFO Space before writing to the left/right channel pointers
    	unsigned int space;
    	WM8731_getFIFOSpace(audio, &space);

        //If there is space in the write FIFO for both channels:
        if (space > 0) {
            //Increment the phase
            phase = phase + inc;
            //Ensure phase is wrapped to range 0 to 2Pi (range of sin function)
            while (phase >= PI) {
                phase = phase - PI;
            }
            // Calculate next sample of the output tone.
            audio_sample = (signed int)( ampl * sin( phase ) );
            // Output tone to left and right channels.
            WM8731_writeSample(audio, audio_sample, audio_sample);
            sampleCounter++;
        }
        //Reset the watchdog.
        HPS_ResetWatchdog();
    }
}

// This function read the samples of the countdown_audio array and writes them to the respective channels
void countdown_audio(PWM8731Ctx_t audio){
    PHPSGPIOCtx_t gpio;
    PHPSI2CCtx_t i2c;

    WM8731_clearFIFO(audio, true,true);

    unsigned int totalSamples = sizeof(countdownAudio) / sizeof(countdownAudio[0]); // total number of samples in the array
    unsigned int sampleIndex = 0;

    // Loop through samples until total samples exhausted
    while (sampleIndex < totalSamples) {
        //Check FIFO Space before writing to the left/right channel pointers
    	unsigned int space;
    	WM8731_getFIFOSpace(audio, &space);

        //If there is space in the write FIFO for both channels:
        if (space > 0) {
            // Output tone to left and right channels.
            WM8731_writeSample(audio, countdownAudio[sampleIndex][0], countdownAudio[sampleIndex][1]);
            sampleIndex++;
        }
        //Reset the watchdog.
        HPS_ResetWatchdog();
    }
}

// This function read the samples of the explosion_audio array and writes them to the respective channels
void explosion_audio(PWM8731Ctx_t audio){
    PHPSGPIOCtx_t gpio;
    PHPSI2CCtx_t i2c;

    WM8731_clearFIFO(audio, true,true);

    unsigned int totalSamples = sizeof(explosionAudio) / sizeof(explosionAudio[0]); // total number of samples in the array
    unsigned int sampleIndex = 0;

    // Loop through samples until total samples exhausted
    while (sampleIndex < totalSamples) {
        //Check FIFO Space before writing to the left/right channel pointers
    	unsigned int space;
    	WM8731_getFIFOSpace(audio, &space);

        //If there is space in the write FIFO for both channels:
        if (space > 0) {
            // Output tone to left and right channels.
            WM8731_writeSample(audio, explosionAudio[sampleIndex][0], explosionAudio[sampleIndex][1]);
            sampleIndex++;
        }
        //Reset the watchdog.
        HPS_ResetWatchdog();
    }
}
