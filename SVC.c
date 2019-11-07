/*
 * SVC.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KernelCall.h"
#include "PKCall.h"
#include "Message.h"
#include "Process.h"
#include "QueueFuncs.h"

extern void SysTickInit();

extern PCB* RUNNING;
extern PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE];
extern int PENDSV_ON;
extern Mailbox MAILBOXLIST[MAILBOXLIST_SIZE];
extern Mailbox* AVAILABLE_MAILBOX;

volatile int FirstSVCall = FALSE;

// function to block running process and switch to next process
// if is terminate, free the memory
void BlockRunningProcess(int isTerminate)
{
	PCB* terminated = RUNNING;
	if (RUNNING == RUNNING->Next) // the only process in the queue
		RUNNING = CheckLowerPriorityProcess(RUNNING->Priority); // chech lowere priority queue
	else
		RUNNING = RUNNING->Next;

	set_PSP((unsigned long)(RUNNING->PSP));

	Dequeue(terminated, (QueueItem**) & (PRIORITY_LIST[terminated->Priority]));

	if (isTerminate)
	{
		free(terminated->StackTop); // free the stack
		free(terminated);	// free the pcb
	}
}

void SVCall(void)
{
/* Supervisor call (trap) entry point
 * Using MSP - trapping process either MSP or PSP (specified in LR)
 * Source is specified in LR: F1 (MSP) or FD (PSP)
 * Save r4-r11 on trapping process stack (MSP or PSP)
 * Restore r4-r11 from trapping process stack to CPU
 * SVCHandler is called with r0 equal to MSP or PSP to access any arguments
 */

/* Save LR for return via MSP or PSP */
__asm("     PUSH    {LR}");

/* Trapping source: MSP or PSP? */
__asm("     TST     LR,#4");    /* Bit #3 (0100b) indicates MSP (0) or PSP (1) */
__asm("     BNE     RtnViaPSP");

/* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
__asm("     PUSH    {r4-r11}");
__asm("     MRS r0,msp");
__asm("     BL  SVCHandler");   /* r0 is MSP */
__asm("     POP {r4-r11}");
__asm("     POP     {PC}");

/* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
__asm("RtnViaPSP:");
__asm("     mrs     r0,psp");
__asm("     stmdb   r0!,{r4-r11}"); /* Store multiple, decrement before */
__asm("     msr psp,r0");
__asm("     BL  SVCHandler");   /* r0 Is PSP */

/* Restore r4..r11 from trapping process stack  */
__asm("     mrs     r0,psp");
__asm("     ldmia   r0!,{r4-r11}"); /* Load multiple, increment after */
__asm("     msr psp,r0");
__asm("     POP     {PC}");

}

void SVCHandler(Stack *argptr)
{
    /*
     * Supervisor call handler
     * Handle startup of initial process
     * Handle all other SVCs such as getid, terminate, etc.
     * Assumes first call is from startup code
     * Argptr points to (i.e., has the value of) either:
       - the top of the MSP stack (startup initial process)
       - the top of the PSP stack (all subsequent calls)
     * Argptr points to the full stack consisting of both hardware and software
       register pushes (i.e., R0..xPSR and R4..R10); this is defined in type
       stack_frame
     * Argptr is actually R0 -- setup in SVCall(), above.
     * Since this has been called as a trap (Cortex exception), the code is in
       Handler mode and uses the MSP
     */
    struct KCallArgs *kcaptr;

    if (FirstSVCall)
    {
        /*
         * Force a return using PSP
         * This will be the first process to run, so the eight "soft pulled" registers
           (R4..R11) must be ignored otherwise PSP will be pointing to the wrong
           location; the PSP should be pointing to the registers R0..xPSR, which will
           be "hard pulled"by the BX LR instruction.
         * To do this, it is necessary to ensure that the PSP points to (i.e., has) the
           address of R0; at this moment, it points to R4.
         * Since there are eight registers (R4..R11) to skip, the value of the sp
           should be increased by 8 * sizeof(unsigned int).
         * sp is increased because the stack runs from low to high memory.
        */
        set_PSP((unsigned long)(RUNNING -> PSP) + (8 * sizeof(unsigned long)));

		FirstSVCall = FALSE;

        /* Start SysTick */
		SysTickInit();
        /*
         - Change the current LR to indicate return to Thread mode using the PSP
         - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
         - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
        */
        __asm(" movw    LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
        __asm(" movt    LR,#0xFFFF");  /* Upper 16 only */
        __asm(" bx  LR");          /* Force return to PSP */
    }
    else /* Subsequent SVCs */
    {
        /*
         * kcaptr points to the arguments associated with this kernel call
         * argptr is the value of the PSP (passed in R0 and pointing to the TOS)
         * the TOS is the complete stack_frame (R4-R10, R0-xPSR)
         * in this example, R7 contains the address of the structure supplied by
            the process - the structure is assumed to hold the arguments to the
            kernel function.
         * to get the address and store it in kcaptr, it is simply a matter of
           assigning the value of R7 (arptr -> r7) to kcaptr
         */

        kcaptr = (struct KCallArgs *) argptr -> R7;
		RUNNING->PSP = (Stack*)get_PSP();
        switch(kcaptr -> Code)
        {
        case GETID:
		{
			kcaptr->RtnValue = RUNNING->PID;
			break;
		}
		case TERMINATE:
		{
			BlockRunningProcess(TRUE);

			break;
		}
		case NICE:
		{
			int old_priority = RUNNING->Priority;
			int new_priority = kcaptr->Arg1;

			Dequeue(RUNNING, (QueueItem**)&(PRIORITY_LIST[RUNNING->Priority])); // Dequeue from old priority queue
			RUNNING->Priority = new_priority; // assign new priority
			Enqueue(RUNNING, (QueueItem**)&(PRIORITY_LIST[RUNNING->Priority])); // Enqueue to new priority queue

			if (new_priority<old_priority)
			{
				RUNNING = CheckLowerPriorityProcess(old_priority);
				set_PSP((unsigned long)(RUNNING->PSP));
			}

			kcaptr->RtnValue = TRUE;

			break;
		}
		case BIND:
		{
			int mailbox_to_bind = kcaptr->Arg1;

			if (mailbox_to_bind < MAILBOXLIST_SIZE) // if mailbox number is legal
			{
				if (mailbox_to_bind == ANYMAILBOX) // bind any mailbox
				{
					if (AVAILABLE_MAILBOX == NULL) // no available mailbox
						kcaptr->RtnValue = ERROR;
					else
					{
						Mailbox* mbx = AVAILABLE_MAILBOX;
						Dequeue(mbx, (QueueItem**)&AVAILABLE_MAILBOX); // dequeue from available list
						mbx->Owner = RUNNING; // assign new owner
						Enqueue(mbx, (QueueItem**)&(RUNNING->Mailbox_Head)); // enqueue owner's mailbox queue

						kcaptr->RtnValue = mbx->ID; // return bound mailbox id
					}
				}
				else
				{
					if (MAILBOXLIST[mailbox_to_bind].Owner == NULL) // if mailbox is available
					{
						Mailbox* mbx = &MAILBOXLIST[mailbox_to_bind];
						Dequeue(mbx, (QueueItem**)&AVAILABLE_MAILBOX); // dequque from available list
						mbx->Owner = RUNNING; // assign new owner
						Enqueue(mbx, (QueueItem**)&(RUNNING->Mailbox_Head)); // enqueue owner's mailbox queue

						kcaptr->RtnValue = mbx->ID; // return bound mailbox id
					}
					else
						kcaptr->RtnValue = ERROR;
				}
			}
			else
				kcaptr->RtnValue = ERROR;
			break;
		}
		case UNBIND:
		{
			int mailbox_to_unbind = kcaptr->Arg1;
			Mailbox* mbx = &MAILBOXLIST[mailbox_to_unbind];
			if (mbx->Owner == RUNNING) // if RUNNING process own this mailbox, then unbind
			{
				mbx->Owner = NULL;
				Dequeue(mbx, (QueueItem**)&(RUNNING->Mailbox_Head));
				EnqueueMbxToAvailable(mbx,&AVAILABLE_MAILBOX);
			}
			else
				kcaptr->RtnValue = ERROR;
			break;
		}
		case RECEIVE:
		{
			RecvMsgArgs* args = (RecvMsgArgs*)kcaptr->Arg1; // get the argument

			if (MAILBOXLIST[args->Recver].Owner == RUNNING) // if receiver is valid
			{
				Message* to_recv = MAILBOXLIST[args->Recver].First_Message; // get the first message
				if (to_recv == NULL) // if no message waiting, block
				{
					// Save waiting message info
					PCB* recver = MAILBOXLIST[args->Recver].Owner;
					recver->Mailbox_Wait = args->Recver;
					recver->Msg_Wait = args;

					// Block running
					BlockRunningProcess(FALSE);
				}
				else // has message, copy to process stack
				{
					if (MAILBOXLIST[args->Recver].First_Message == MAILBOXLIST[args->Recver].Last_Message) // if last message in the queue
					{
						// clear the first and last flag
						MAILBOXLIST[args->Recver].First_Message = NULL;
						MAILBOXLIST[args->Recver].Last_Message = NULL;
					}
					else
						MAILBOXLIST[args->Recver].First_Message = to_recv->Next; // assign new head

					// Read message
					int copy_size = *args->Size < to_recv->Size ? *args->Size : to_recv->Size; // get smaller size
					*args->Sender = to_recv->Sender; // assign sender's mailbox number
					memcpy(to_recv->Message_Addr, args->Msg_addr, copy_size); // copy bytes

					free(to_recv->Message_Addr);
					free(to_recv); // release message
					*args->Size = copy_size;
				}
			}
			else
				*args->Size = INVALID_RECVER;

			break;
		}
		case SEND:
		{
			SendMsgArgs* args = (SendMsgArgs*)kcaptr->Arg1; // get the argument

			if (MAILBOXLIST[args->Sender].Owner == RUNNING) // if sender is valid
			{
				PCB* recver = MAILBOXLIST[args->Recver].Owner;
				if (recver != NULL) // if receiver is valid
				{
					if (recver->Mailbox_Wait == args->Recver && recver->Msg_Wait != NULL) // if receiver is blocked and waiting for this mailbox
					{
						RecvMsgArgs* receiver_waiting = recver->Msg_Wait;
						int copy_size = *args->Size < *receiver_waiting->Size ? *args->Size : *receiver_waiting->Size; // get smaller size

						// Copy msg to receiver
						memcpy(receiver_waiting->Msg_addr, args->Msg_addr, copy_size); // copy message
						*args->Size = copy_size; // update size

						// Unblock process
						Enqueue(recver, (QueueItem**)& (PRIORITY_LIST[recver->Priority])); // add back to process queue
					}
					else // if is not waiting on this mailbox
					{
						// Add message to mailbox
						Message* msg = malloc(sizeof(Message)); // create message struct
						msg->Message_Addr = malloc(*args->Size); // allocate message memory
						memcpy(msg->Message_Addr, args->Msg_addr, *args->Size); // copy from process stack to mailbox
						msg->Size = *args->Size; // store the size
						msg->Sender = args->Sender; // store the sender's mailbox number
						msg->Next = NULL; // to add to the end of message list

						Message* mbx_last_msg = MAILBOXLIST[args->Recver].Last_Message;
						if (mbx_last_msg == NULL) // if no message in mailbox
						{
							MAILBOXLIST[args->Recver].First_Message = msg; // update mailbox's first message pointer
							mbx_last_msg = msg; // update mailbox's last message pointer
						}
						else
						{
							mbx_last_msg->Next = msg; // add to end on list
							mbx_last_msg = msg; // update tail of list
						}
					}
				}
				else
					*args->Size = INVALID_RECVER;
			}
			else
				*args->Size = INVALID_SENDER;

			break;
		}

        default:
            kcaptr -> RtnValue = ERROR;
        }
    }
}

void PendSV_Handler()
{
    PENDSV_ON = TRUE;

	//RUNNING->PSP = (Stack*)(get_PSP()-NUM_OF_SW_PUSH_REG*sizeof(unsigned long));

	/* Save running process */
	save_registers(); /* Save active CPU registers in PCB */

	RUNNING->PSP = (Stack*)get_PSP();
	// get next running process
	RUNNING = RUNNING->Next;

	// Update PSP value
	set_PSP((unsigned long)(RUNNING->PSP));

	restore_registers(); /* Restore process��s registers */

	PENDSV_ON = FALSE;
}
