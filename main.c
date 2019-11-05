/**
 * Real Time System Assignment 2
 *
 * This program is a
 *
 * Author:	Ray		Su	B00634512
			Victor	Gao	B00
 * Fall 2019
 */

#include <stdio.h>
#include "Message.h"
#include "KernelCall.h"
#include "Process.h"

extern int FirstSVCall;

void main (void)
 {
	/* Initialization Uart, Systick and Queues. Enable interrupts*/
	// To update
	MailboxListIntialization(); // Initialize mailbox list
	KernelInitialization(); // Initialize Kernel
	enable();    // Enable Master (CPU) Interrupts
	Initialize_Process(); // Initialize all processes

	// Force to go thread mode
	FirstSVCall = TRUE;
	SVC();
}
