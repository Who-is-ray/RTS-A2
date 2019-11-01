/*
 * Process.c
 *
 *  Created on: Oct 31, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include <stdlib.h>
#include "process.h"

#define PRIORITY_LIST_SIZE  6

volatile PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE];


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

//*********To Remove**********//


void Initialize_Process()
{
    Stack* IDLE_stack = (Stack*)malloc(sizeof(Stack));
    PCB* IDLE_PCB = (PCB*)malloc(sizeof(PCB));
	IDLE_PCB->PSP = IDLE_stack;
}

void Process_IDLE()
{
    while(1)
    {
        OutputString("Idle");
        OutputNewLine();
    }
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
