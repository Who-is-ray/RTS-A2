/*
 * KernelCall.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include "KernelCall.h"
#include "Process.h"
#include "Uart.h"

#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000

extern PCB* RUNNING;
extern int PENDSV_ON;

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

void AssignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}

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

void Terminate()
{
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */
	args.Code = TERMINATE;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// No need to update PSP value here, because we will not return to this process again

	SVC();
}

int Nice(int new_priority)
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
