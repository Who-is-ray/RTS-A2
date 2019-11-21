/*
 *File name: Process.h
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: include all the process.c needs.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
 */
#ifndef PROCESS_H_
#define PROCESS_H_

#define TRUE    1
#define FALSE   0
#define ERROR	-1
#define PRIVATE static

#define SVC()   __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP
#define NUM_OF_SW_PUSH_REG  8
#define PRIORITY_LIST_SIZE  6
#define ANYMAILBOX -1 // Mailbox number for bind any available mailbox

typedef struct __PCB PCB;

void set_LR(volatile unsigned long); // set CPU LR
unsigned long get_PSP(); //get CPU PSP
void set_PSP(volatile unsigned long); // set CPU PSP
unsigned long get_MSP(void); //get CPU MSP
void set_MSP(volatile unsigned long); //set CPU MSP
unsigned long get_SP(); // get CPU SP

// return pcb pointer of the first process below input priority
PCB* CheckLowerPriorityProcess(int priority);
void Initialize_Process();

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
struct __PCB
{
    /* Links to adjacent PCBs */
    PCB* Next;
    PCB* Prev;

	/* Process's ID*/
	int PID;

	/* Process's priority*/
	int Priority;

    /* Stack pointer - r13 (PSP) */
    Stack* PSP;

	/* Pointer to the top of reserved stack, for release memeory using*/
    void* StackTop;

	/* The head of mailbox, Mailbox pointer*/
	void* Mailbox_Head;

	/* Waiting mailbox number for unblock*/
	int Mailbox_Wait;

	/* Waiting message info, RecvMsgArgs pointer*/
	void* Msg_Wait;
} ;

#endif /* PROCESS_H_ */
