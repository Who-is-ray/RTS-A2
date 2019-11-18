/*
 * Process.c
 *
 *  Created on: Oct 31, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "KernelCall.h"
#include "PKCall.h"
#include "Uart.h"
#include "QueueFuncs.h"

#define PSR_INITIAL_VAL		0x01000000
#define INITIAL_STACK_TOP_OFFSET    960
// create and initialize priority list
PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL};

// RUNNING Pcb
volatile PCB* RUNNING = NULL;

// function of idle process
void process_IDLE()
{
    while(1)
    {
        //OutputString("Idle");
        //OutputNewLine();
    }
}


// Uart output process
void process_UART_OUTPUT()
{

}

int reg_process(void (*func_name)(), int pid, int priority)
{
	PCB* pcb = (PCB*)malloc(sizeof(PCB)); // Allocate memory for pcb
	pcb->PID = pid; // Assign ID
	pcb->Priority = priority; // Assign priority
	pcb->StackTop = malloc(STACKSIZE);
	pcb->Msg_Wait = NULL; // clear message wait
	pcb->Mailbox_Head = NULL; // clear mailbox head

	// Stack should grow from bottom
	pcb->PSP = (Stack*)((unsigned long)pcb->StackTop + INITIAL_STACK_TOP_OFFSET);

	pcb->PSP->PSR = PSR_INITIAL_VAL; // Assign PSR initial value
	pcb->PSP->PC = (unsigned long)func_name; // Assign process's function to PC
	pcb->PSP->LR = (unsigned long)Terminate; // Assign terminate function to LR

	// Assign all other registers to null
	pcb->PSP->R0 = NULL;
	pcb->PSP->R1 = NULL;
	pcb->PSP->R2 = NULL;
	pcb->PSP->R3 = NULL;
	pcb->PSP->R4 = NULL;
	pcb->PSP->R5 = NULL;
	pcb->PSP->R6 = NULL;
	pcb->PSP->R7 = NULL;
	pcb->PSP->R8 = NULL;
	pcb->PSP->R9 = NULL;
	pcb->PSP->R10 = NULL;
	pcb->PSP->R11 = NULL;
	pcb->PSP->R12 = NULL;

	Enqueue(pcb, (QueueItem**)&(PRIORITY_LIST[pcb->Priority])); // Add to queue

	if ((RUNNING == NULL) || (priority > RUNNING->Priority)) 
		// if is the first process or has higher priority, assign it to RUNNING
		RUNNING = pcb;

	return TRUE;
}

// return pcb pointer of the first process below input priority
PCB* CheckLowerPriorityProcess(int priority)
{
	int i;
	for (i = priority - 1; i >= 0; i--) // check lower priority
	{
		if (PRIORITY_LIST[i] != NULL)
			return PRIORITY_LIST[i];
	}
	return NULL; // return null if no process left, includes idle process
}

unsigned long get_PSP(void)
{
	/* Returns contents of PSP (current process stack */
	__asm(" mrs     r0, psp");
	__asm(" bx  lr");
	return 0;   /***** Not executed -- shuts compiler up */
			/***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
	/* Returns contents of MSP (main stack) */
	__asm(" mrs     r0, msp");
	__asm(" bx  lr");
	return 0;
}

void set_PSP(volatile unsigned long ProcessStack)
{
	/* set PSP to ProcessStack */
	__asm(" msr psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
	/* Set MSP to MainStack */
	__asm(" msr msp, r0");
}

void volatile save_registers()
{
	/* Save r4..r11 on process stack */
	__asm(" mrs     r0,psp");
	/* Store multiple, decrement before; '!' - update R0 after each store */
	__asm(" stmdb   r0!,{r4-r11}");
	__asm(" msr psp,r0");
}

void volatile restore_registers()
{
	/* Restore r4..r11 from stack to CPU */
	__asm(" mrs r0,psp");
	/* Load multiple, increment after; '!' - update R0 */
	__asm(" ldmia   r0!,{r4-r11}");
	__asm(" msr psp,r0");
}

unsigned long get_SP()
{
	/**** Leading space required -- for label ****/
	__asm("     mov     r0,SP");
	__asm(" bx  lr");
	return 0;
}

/// Testing
//#define TEST_NICE

#ifdef TEST_NICE // Testing of Nice

void process_1()
{
	unsigned int i;
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'x';
	}
	Nice(5);
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'x';
	}
	Nice(4);
	while (TRUE)
	{
		UART0_DR_R = 'x';
	}
}

void process_2()
{
	while (TRUE)
	{
		UART0_DR_R = 'y';
	}
}

void process_3()
{
	while (TRUE)
	{
		UART0_DR_R = 'z';
	}
}

// Initialize all processes and force switch to thread mode
void Initialize_Process()
{
	reg_process(process_IDLE, 0, 0); // register idle process
	reg_process(process_1, 1, 3); // register process 1
	reg_process(process_2, 2, 3); // register process 1
	reg_process(process_3, 3, 3); // register process 1
}

#endif // TEST_NICE


#ifdef TEST_MESSAGE // Testing of Nice

void process_1()
{
	Bind(6);
	Bind(3);
	Bind(12);
	int mbx = Bind(11);
	unsigned int i;
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'x';
	}
	Nice(5);
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'x';
	}
	int msg;
	int size;
	int sender;
	Receive(-1, &sender, &msg, &size);
	Receive(-1, &sender, &msg, &size);
	Nice(1);
}

void process_2()
{
	int i;
	for (i = 0; i < 2000; i++)
	{
		UART0_DR_R = 'y';
	}
	int mbx = Bind(2);
	int msg1 = 123;
	int msg2 = 789;
	int size = sizeof(msg1);
	Send(11, mbx, &msg1, &size);
	Send(11, mbx, &msg2, &size);
	while (TRUE)
	{
		UART0_DR_R = 'y';
	}
}

void process_3()
{
	while (TRUE)
	{
		UART0_DR_R = 'z';
	}
}

// Initialize all processes and force switch to thread mode
void Initialize_Process()
{
	reg_process(process_IDLE, 0, 0); // register idle process
	reg_process(process_1, 1, 3); // register process 1
	reg_process(process_2, 2, 3); // register process 1
	reg_process(process_3, 3, 3); // register process 1
}

#endif // TEST_NICE