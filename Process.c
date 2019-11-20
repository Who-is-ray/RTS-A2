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
#define PROCESS_1_MBX	1
#define PROCESS_2_MBX	2
#define PROCESS_3_MBX	3

// create and initialize priority list
PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL};

// RUNNING Pcb
volatile PCB* RUNNING = NULL;

// function of idle process
void process_IDLE()
{
	while (1);
}


// Uart output process
void process_UART_OUTPUT()
{
	int msg = 0;
	int size = sizeof(msg);
	int sender;// null_sender, null_msg, null_size;
	Bind(UART_ISR_MBX);
	Bind(UART_OUTPUT_MBX);
	Send(UART_ISR_MBX, UART_ISR_MBX, &msg, &size);

	while (TRUE)
	{
		Receive(UART_OUTPUT_MBX, &sender, &msg, &size);
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
//#define TEST_TERMINATION
//#define TEST_BIND_SEND_RECEIVE
#define TEST_BLOCK_UNBLOCK

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


#ifdef TEST_TERMINATION // Testing of Nice

void process_1()
{
	unsigned int i;
	for (i = 0; i < 10000; i++)
	{
		UART0_DR_R = 'x';
	}
}

void process_2()
{
	unsigned int i;
	for (i = 0; i < 20000; i++)
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

#endif // TEST_TERMINATION

#ifdef TEST_BIND_SEND_RECEIVE

void process_1()
{
	int mbx = Bind(PROCESS_1_MBX);
	unsigned int i;
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'x';
	}
	char msg;
	int size;
	int sender;
	Receive(ANYMAILBOX, &sender, &msg, &size);
	while (TRUE)
	{
		UART0_DR_R = msg;
	}
}

void process_2()
{
	int i;
	for (i = 0; i < 2000; i++)
	{
		UART0_DR_R = 'y';
	}
	int mbx = Bind(PROCESS_2_MBX);
	char msg = 'A';
	int size = sizeof(msg);
	Send(PROCESS_1_MBX, mbx, &msg, &size);
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

#endif // TEST_BIND_SEND_RECEIVE

#ifdef TEST_BLOCK_UNBLOCK

void process_1()
{
	Bind(PROCESS_1_MBX);
	unsigned int i;
	for (i = 0; i < 200; i++)
	{
		UART0_DR_R = 'x';
	}
	char msg;
	int size = sizeof(msg);
	int sender;	
	Receive(ANYMAILBOX, &sender, &msg, &size);
	while (TRUE)
	{
		UART0_DR_R = msg;
	}
}

void process_2()
{
	int i;
	for (i = 0; i < 20000; i++)
	{
		UART0_DR_R = 'y';
	}
	int mbx = Bind(2);
	char msg = 'A';
	int size = sizeof(msg);
	Send(PROCESS_1_MBX, mbx, &msg, &size);
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

#endif // TEST_BLOCK_UNBLOCK
