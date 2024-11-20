/* ARM Private Timer Driver
 * ------------------------
 *
 * ~~ Add description, etc ~~
 */

#ifndef HPS_PRIVATETIMERSSEBI_H_
#define HPS_PRIVATETIMERSSEBI_H_

#include "Util/driver_ctx.h"

// Driver context
typedef struct {
    //Header
    DrvCtx_t header;
    //Body
    volatile unsigned int * base;
    // ... 
} TimerCtx_t, *PTimerCtx_t;

// Initialise Private Timer Driver
//  - base is a pointer to the private timer module
//  - Returns Util/error Code
//  - Returns context pointer to *ctx
HpsErr_t Timer_initialise(void* base, PTimerCtx_t* pCtx);

// Check if driver initialised
//  - Returns true if driver previously initialised
bool Timer_isInitialised(PTimerCtx_t ctx);

// Sets the private timer load value
HpsErr_t Timer_SetLoad(PTimerCtx_t ctx, unsigned int load_value);

// Sets the prescalar value of the time
HpsErr_t Timer_SetPrescaler(PTimerCtx_t ctx, unsigned int timer_prescaler);

// Sets the interrupt control flag for the timer
HpsErr_t Timer_SetInterruptFlag(PTimerCtx_t ctx, bool interrupt);

// Sets the automatic reload control flag
HpsErr_t Timer_SetAutoReloadFlag(PTimerCtx_t ctx, bool autoReload);

// Enables the timer
HpsErr_t Timer_Enable(PTimerCtx_t ctx, bool enable);

// Sets the control of the HPS Private Timer
HpsErr_t Timer_SetControl(PTimerCtx_t ctx, bool enable, bool autoReload, bool interrupt, unsigned int timer_prescaler);

// Reads the current value of the private timer
HpsErr_t Timer_ReadCurrent(PTimerCtx_t ctx, unsigned int* current_val);

// Reads the timer interrupt status
HpsErr_t Timer_ReadInterruptStatus( PTimerCtx_t ctx, unsigned int* interrupt_status);

#endif /* HPS_PRIVATETIMER_H_ */
