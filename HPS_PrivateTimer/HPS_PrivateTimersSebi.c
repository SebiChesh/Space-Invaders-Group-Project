
#include "HPS_PrivateTimersSebi.h"
#include "Util/bit_helpers.h"

//Register Offsets
#define TIMER_LOAD      (0x00/sizeof(unsigned int))
#define TIMER_VALUE     (0x04/sizeof(unsigned int))
#define TIMER_CONTROL   (0x08/sizeof(unsigned int))
#define TIMER_INTERRUPT (0x0C/sizeof(unsigned int))

//Control Bit Map
#define TIMER_ENABLE (1 << 0) // Timer enable flag
#define TIMER_AUTO_RELOAD (1 << 1) // Timer Enable Flag
#define TIMER_ICF_ENABLE (1 << 2) //Timer Interrupt control flag enable

// Cleanup function called when driver destroyed.
//  - Disables any hardware and interrupts.
//  - Free any allocated memory
static void _Timer_cleanup(PTimerCtx_t ctx) {
    if (ctx->base) {
        // Stop the timer running when driver cleared.
        ctx->base[TIMER_CONTROL] = 0;
    }
}

static void _Timer_SetLoad (PTimerCtx_t ctx, unsigned int load_value) {
	ctx->base[TIMER_LOAD] = load_value;

}

static void _Timer_SetPrescaler(PTimerCtx_t ctx, unsigned int timer_prescaler) {
	ctx->base[TIMER_CONTROL] |= (timer_prescaler << 8); // Sets the prescaler value

}

static void _Timer_SetInterruptFlag(PTimerCtx_t ctx, bool interrupt) {
    if (interrupt) {
        ctx->base[TIMER_CONTROL] |= TIMER_ICF_ENABLE; // Sets individual bit

    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_ICF_ENABLE; // Clears individual bit

    }
    
}

static void _Timer_SetAutoReloadFlag(PTimerCtx_t ctx, bool autoReload) {
    if (autoReload) {
        ctx->base[TIMER_CONTROL] |= TIMER_AUTO_RELOAD; // Sets individual bit

    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_AUTO_RELOAD; // Clears individual bit

    }
    
}

static void _Timer_Enable(PTimerCtx_t ctx, bool enable) {
    if (enable) {
        ctx->base[TIMER_CONTROL] |= TIMER_ENABLE;

    } else {
        ctx->base[TIMER_CONTROL] &= ~TIMER_ENABLE;
    }

}

// Initialise Private Timer Driver
//  - base is a pointer to the private timer module
//  - Returns Util/error Code
//  - Returns context pointer to *ctx
HpsErr_t Timer_initialise(void* base, PTimerCtx_t* pCtx) {
    //Ensure user pointers valid
    if (!base) return ERR_NULLPTR;
    if (!pointerIsAligned(base, sizeof(unsigned int))) return ERR_ALIGNMENT;
    //Allocate the driver context, validating return value.
    HpsErr_t status = DriverContextAllocateWithCleanup(pCtx, &_Timer_cleanup);
    if (IS_ERROR(status)) return status;
    //Save base address pointers
    PTimerCtx_t ctx = *pCtx;
    ctx->base = (unsigned int*)base;
    //Ensure timer initialises to disabled
    ctx->base[TIMER_CONTROL] = 0;
    
    // ... Any other initialisation "stuff" ...
    
    //Initialised
    DriverContextSetInit(ctx);
    return status;
}

// Check if driver initialised
//  - Returns true if driver previously initialised
bool Timer_isInitialised(PTimerCtx_t ctx) {
    return DriverContextCheckInit(ctx);
}

// Sets the private timer load value
HpsErr_t Timer_SetLoad(PTimerCtx_t ctx, unsigned int load_value) {
	HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

	_Timer_SetLoad(ctx, load_value);

	return ERR_SUCCESS;
}

// Sets the prescalar value of the time
HpsErr_t Timer_SetPrescaler(PTimerCtx_t ctx, unsigned int timer_prescaler) {
	HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

	_Timer_SetPrescaler(ctx, timer_prescaler);

	return ERR_SUCCESS;
}

// Sets the interrupt control flag for the timer
HpsErr_t Timer_SetInterruptFlag(PTimerCtx_t ctx, bool interrupt) {
	HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

	_Timer_SetInterruptFlag(ctx, interrupt);

	return ERR_SUCCESS;
}

// Sets the automatic reload control flag
HpsErr_t Timer_SetAutoReloadFlag(PTimerCtx_t ctx, bool autoReload) {
    HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

    _Timer_SetAutoReloadFlag(ctx, autoReload);

    return ERR_SUCCESS;
}

// Enables the timer
HpsErr_t Timer_Enable(PTimerCtx_t ctx, bool enable) {
    HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

    _Timer_Enable(ctx, enable);

    return ERR_SUCCESS;
}

// Sets the timer control
HpsErr_t Timer_SetControl(PTimerCtx_t ctx, bool enable, bool autoReload, bool interrupt, unsigned int timer_prescaler) {
	HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

    _Timer_SetPrescaler(ctx, timer_prescaler);
    _Timer_SetInterruptFlag(ctx, interrupt);
    _Timer_SetAutoReloadFlag(ctx, autoReload);
	_Timer_Enable(ctx, enable);

	return ERR_SUCCESS;
}

// Reads current value of the timer
HpsErr_t Timer_ReadCurrent(PTimerCtx_t ctx, unsigned int* currrent_val) {
	if (!currrent_val) return ERR_NULLPTR;
	HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

	*currrent_val = ctx->base[TIMER_VALUE];

	return ERR_SUCCESS;
}

// Reads the timer interrupt status
HpsErr_t Timer_ReadInterruptStatus( PTimerCtx_t ctx, unsigned int* interrupt_status) {
 	if (!interrupt_status) return ERR_NULLPTR;
    HpsErr_t status = DriverContextValidate(ctx);
	if (IS_ERROR(status)) return status;

    // Reads interrupt flag
    *interrupt_status = ctx->base[TIMER_INTERRUPT];

    // If set, clear the flag
    if(*interrupt_status) ctx->base[TIMER_INTERRUPT] = 0x1; 

    return ERR_SUCCESS;
}

