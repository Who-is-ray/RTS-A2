/**
 *File name: Main.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: This is main function of the code, it calls the functions in specific sequence.
 *Author:	Ray		Su	B00634512
			Victor	Gao	B00677182
 *Last day modified: Oct.23
 */

#include <stdio.h>
#include "Uart.h"
#include "Queue.h"
#include "Message.h"
#include "KernelCall.h"
#include "Process.h"

extern int FirstSVCall;

void main (void)
 {
	/* Initialization Uart, Systick and Queues. Enable interrupts*/
	UART0_Init(); // initialize Uart
	Queue_Init(); // initialize queue
	MailboxListIntialization(); // Initialize mailbox list
	KernelInitialization(); // Initialize Kernel
	enable();    // Enable Master (CPU) Interrupts
	Initialize_Process(); // Initialize all processes

	// Force to go thread mode
	FirstSVCall = TRUE;
	SVC();
}
