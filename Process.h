/*
 * Process.h
 *
 *  Created on: Oct 31, 2019
 *      Author: Ray
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define SVC()   __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP
typedef struct PCB PCB;

void set_LR(volatile unsigned long);
unsigned long get_PSP();
void set_PSP(volatile unsigned long);
unsigned long get_MSP(void);
void set_MSP(volatile unsigned long);
unsigned long get_SP();

void EnqueueProcess(PCB* pcb);
void DequeueProcess(PCB* pcb);
PCB* FindNextProcessToRun();

void volatile save_registers();
void volatile restore_registers();

#define STACKSIZE   1024

/* Cortex default stack frame */
typedef struct
{
    /* Registers saved by s/w (explicit) */
    /* There is no actual need to reserve space for R4-R11, other than
     * for initialization purposes.  Note that r0 is the h/w top-of-stack.
     */
    unsigned long R4;
    unsigned long R5;
    unsigned long R6;
    unsigned long R7;
    unsigned long R8;
    unsigned long R9;
    unsigned long R10;
    unsigned long R11;
    /* Stacked by hardware (implicit)*/
    unsigned long R0;
    unsigned long R1;
    unsigned long R2;
    unsigned long R3;
    unsigned long R12;
    unsigned long LR;
    unsigned long PC;
    unsigned long PSR;
} Stack;

/* Process control block */

struct PCB
{
    /* Links to adjacent PCBs */
    PCB* Next;
    PCB* Prev;

    /* Stack pointer - r13 (PSP) */
    Stack* PSP;

	/* Process's ID*/
	int PID;

	/* Process's priority*/
	int Priority;
} ;

#endif /* PROCESS_H_ */
