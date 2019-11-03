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
#include "Uart.h"

#define PRIORITY_LIST_SIZE  6
#define PSR_INITIAL_VAL		0x01000000

// create and initialize priority list
PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL};

// RUNNING Pcb
volatile PCB* RUNNING = NULL;

//*********To Remove**********//
#include "Queue.h"
#include "Uart.h"
#include "Systick.h"
#include <string.h>

#define LINE_FEED               10
#define ENTER                   13

/* Transmit a character*/
void TransChar(char c)
{
    while(EnQueue(OUTPUT, UART, c)==FALSE); // wait until it is enqueued
}

/* Output a string*/
void OutputString(const char* s)
{
    int i;
    for(i = 0; i <= strlen(s); i++) // output each character in string
        TransChar(s[i]);
}

/* Move cursor to new line*/
void OutputNewLine()
{
    TransChar(ENTER);
    TransChar(LINE_FEED);
}

//*********End To Remove**********//

void process_1()
{
	int i;
	for (i=0; i < 200; i++)
	{
	    UART0_DR_R = 'x';
	}
	i = 0;
}

void process_2()
{
	while (1)
	{
	    UART0_DR_R = 'y';
	}
}

void process_3()
{
	while (1)
	{
	    UART0_DR_R = 'z';
	}
}

// function of idle process
void process_IDLE()
{
    while(1)
    {
        //OutputString("Idle");
        //OutputNewLine();
    }
}

int reg_process(void (*func_name)(), int pid, int priority)
{
    Stack* stack = (Stack*)malloc(STACKSIZE); // Allocate memory for stack
    stack->PSR = PSR_INITIAL_VAL; // Assign PSR initial value
    stack->PC = (unsigned long)func_name; // Assign process's function to PC
    stack->LR = (unsigned long)Terminate; // Assign terminate function to LR
    stack->R0 = NULL;
    stack->R1 = NULL;
    stack->R2 = NULL;
    stack->R3 = NULL;
    stack->R4 = NULL;
    stack->R5 = NULL;
    stack->R6 = NULL;
    stack->R7 = NULL;
    stack->R8 = NULL;
    stack->R9 = NULL;
    stack->R10 = NULL;
    stack->R11 = NULL;
    stack->R12 = NULL;

	PCB* pcb = (PCB*)malloc(sizeof(PCB)); // Allocate memory for pcb
	pcb->PID = pid; // Assign ID
	pcb->Priority = priority; // Assign priority
	pcb->PSP = stack;
	EnqueueProcess(pcb); // Add to queue

	if ((RUNNING == NULL) || (priority > RUNNING->Priority)) 
		// if is the first process or has higher priority, assign it to RUNNING
		RUNNING = pcb;

	return TRUE;
}

// Initialize all processes and force switch to thread mode
void Initialize_Process()
{
	reg_process(process_IDLE, 0, 0); // register idle process
	reg_process(process_1, 1, 4); // register process 1
	reg_process(process_2, 2, 4); // register process 1
	reg_process(process_3, 3, 4); // register process 1

}

// Enqueue process to queue
void EnqueueProcess(PCB* pcb)
{
	if (PRIORITY_LIST[pcb->Priority] == NULL) // no process in the queue
	{
		PRIORITY_LIST[pcb->Priority] = pcb;
		pcb->Next = pcb;
		pcb->Prev = pcb;
	}
	else // add to existing queue
	{
		pcb->Prev = PRIORITY_LIST[pcb->Priority];
		pcb->Next = PRIORITY_LIST[pcb->Priority]->Next;
		PRIORITY_LIST[pcb->Priority]->Next->Prev = pcb;
		PRIORITY_LIST[pcb->Priority]->Next = pcb;
	}
}

// Dequeue process from queue
void DequeueProcess(PCB* pcb)
{
	if (pcb->Next == pcb) // the only process in this queue
		PRIORITY_LIST[pcb->Priority] = NULL;
	else // not the only process in the queue
	{
		if (PRIORITY_LIST[pcb->Priority] == pcb) // if is the head of queue
			PRIORITY_LIST[pcb->Priority] = pcb->Next; // update queue
		pcb->Prev->Next = pcb->Next;
		pcb->Next->Prev = pcb->Prev;
	}
}

// return pcb pointer of next process to run, next process can not be itself
PCB* CheckLowerPriorityProcess()
{
	int i;
	for (i = RUNNING->Priority - 1; i >= 0; i--) // check lower priority
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
