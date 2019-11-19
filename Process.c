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
#define UART_OUTPUT_MBX	30
#define UART_ISR_MBX	31
#define	PID_1		1
#define PID_2		2
#define PID_3		3
#define PID_IDLE	0
#define PID_UART	4

// create and initialize priority list
PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL};

// RUNNING Pcb
volatile PCB* RUNNING = NULL;

void process_1()
{
    Bind(6);
    Bind(3);
    Bind(12);
	char msg_x = 'x';
	int size_x = sizeof(msg_x);
	int mbx = Bind(11);
    while(1)
	{
		Send(UART_OUTPUT_MBX, mbx, &msg_x, &size_x);
	}
}

void process_2()
{
	char msg_y = 'y';
	int size_y = sizeof(msg_y);
	int mbx = Bind(2);
    int i;
    for (i=0; i < 2000; i++)
    {
		Send(UART_OUTPUT_MBX, mbx, &msg_y, &size_y);
    }	
	int msg1 = 123;
	int msg2 = 789;
	int size = sizeof(msg1);
    Send(11, mbx, &msg1, &size);
    Send(11, mbx, &msg2, &size);
	while (1)
	{
		Send(UART_OUTPUT_MBX, mbx, &msg_y, &size_y);
	}
}

void process_3()
{
	char msg_z = 'z';
	int size_z = sizeof(msg_z);
	int mbx = Bind(-1);
	while (1)
	{
		Send(UART_OUTPUT_MBX, mbx, &msg_z, &size_z);
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


// Uart output process
void process_UART_OUTPUT()
{
	int msg = 0;
	int size = sizeof(msg);
	int sender, null_sender, null_msg, null_size;
	Bind(UART_ISR_MBX);
	Bind(UART_OUTPUT_MBX);
	Send(UART_ISR_MBX, UART_ISR_MBX, &msg, &size);

	while (TRUE)
	{
		Receive(UART_OUTPUT_MBX, &sender, &msg, &size);
		Receive(UART_ISR_MBX, &null_sender, &null_msg, &null_size);
		UART0_DR_R = msg;
	}
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

// Initialize all processes and force switch to thread mode
void Initialize_Process()
{
	reg_process(process_IDLE, PID_IDLE, 0); // register idle process
	reg_process(process_1, PID_1, 4); // register process 1
	reg_process(process_2, PID_2, 4); // register process 1
	reg_process(process_3, PID_3, 4); // register process 1
	reg_process(process_UART_OUTPUT, PID_UART, 5); // register UART process, 
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
