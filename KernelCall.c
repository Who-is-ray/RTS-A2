/*
 * KernelCall.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray, Victor
 */

#include <stdio.h>
#include "KernelCall.h"
#include "Process.h"
#include "Uart.h"

#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000 // pend SV priority
#define PRIORITY_MAX 5 // highest priority

extern PCB* RUNNING; // Current running process
extern int PENDSV_ON;

// Initialization Kernal
void KernelInitialization()
{
	/* Initialize UART */
	UART0_Init();           // Initialize UART0
	InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
	//UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts

	PENDSV_ON = FALSE;

	// set PendSV priority
	NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY;
}

// Save address to R7
void AssignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}

// Get ID Kernal call
int GetID()
{
    volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */
	args.Code = GETID;

    /* Assign address if getidarg to R7 */
    AssignR7((unsigned long) &args);

    // Call Kernel
	SVC();

    return args.RtnValue;
}

// Termination kernal call
void Terminate()
{
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */
	args.Code = TERMINATE;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// No need to update PSP value here, because we will not return to this process again

	SVC();
}

// Nice kernal call
int Nice(int new_priority)
{
	if (new_priority > 0 && new_priority <= PRIORITY_MAX)
	{
		volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */
		args.Code = NICE;
		args.Arg1 = new_priority;

		/* Assign address if getidarg to R7 */
		AssignR7((unsigned long)&args);

		// Call Kernel
		SVC();

		return args.RtnValue;

	}
	else
		return FALSE;
}
