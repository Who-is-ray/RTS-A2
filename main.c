/**
 * Real Time System Assignment 2
 *
 * This program is a
 *
 * Author: Ray Su B00634512
 * Fall 2019
 */

//*********To Remove**********//
#include "Queue.h"
#include "Systick.h"
//*********To Remove**********//

#include <stdio.h>
#include "Process.h"
#include "KernelCall.h"

extern int FirstSVCall;

void main (void)
 {
	/* Initialization Uart, Systick and Queues. Enable interrupts*/
	// To update
	Queue_Init();           // Initialize Queues
	KernelInitialization(); // Initialize Kernel
	enable();    // Enable Master (CPU) Interrupts
	Initialize_Process(); // Initialize all processes

	// Force to go thread mode
	FirstSVCall = TRUE;
	SVC();
}
