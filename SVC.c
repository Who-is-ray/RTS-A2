/*
 *File name: SVC.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: includes all the features that service routine call has.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.19
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

extern PCB* RUNNING; // RUNNING process
extern PCB* PRIORITY_LIST[PRIORITY_LIST_SIZE]; // priority list stores all processes
extern int PENDSV_ON; // pend sv flag
extern Mailbox MAILBOXLIST[MAILBOXLIST_SIZE]; // mailbox list stores all mailboxes
extern Mailbox* AVAILABLE_MAILBOX; // head of available mailboxes

volatile int FirstSVCall = FALSE; // first sv call flag
int UNBLOCK_PRIORITY = 0; //A global variable of the priority of unblocked process

// function to block running process and switch to next process
// if is terminate, free the memory
void BlockRunningProcess(RecvMsgArgs* args)
{
	if (args != NULL) // if not for termination
	{
		// Save waiting message info
		RUNNING->Mailbox_Wait = args->Recver;
		RUNNING->Msg_Wait = args;
	}

	PCB* terminated = RUNNING;
	if (RUNNING == RUNNING->Next) // the only process in the queue
	{
		PRIORITY_LIST[RUNNING->Priority] = NULL; // clear the head
		RUNNING = CheckLowerPriorityProcess(RUNNING->Priority); // chech lowere priority queue
	}
	else
		RUNNING = RUNNING->Next; // update RUNNING

	set_PSP((unsigned long)(RUNNING->PSP));

	Dequeue(terminated, (QueueItem**) & (PRIORITY_LIST[terminated->Priority]));

	if (args == NULL) // if is termination only
	{
		free(terminated->StackTop); // free the stack
		free(terminated);	// free the pcb
	}
}

// Read message from specific mailbox
void ReceiveMsgFromMailbox(RecvMsgArgs* args, Mailbox* mbx)
{
	Message* to_recv = mbx->First_Message; // get the first message

	if (to_recv == mbx->Last_Message) // if last message in the queue
	{
		// clear the first and last flag
	    mbx->First_Message = NULL;
	    mbx->Last_Message = NULL;
	}
	else
	    mbx->First_Message = to_recv->Next; // assign new head

	// Read message
	int copy_size = *args->Size < to_recv->Size ? *args->Size : to_recv->Size; // get smaller size
	*args->Sender = to_recv->Sender; // assign sender's mailbox number
	memcpy(args->Msg_addr, to_recv->Message_Addr, copy_size); // copy bytes

	free(to_recv->Message_Addr);
	free(to_recv); // release message
	*args->Size = copy_size;
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
			kcaptr->RtnValue = RUNNING->PID; // return id
			break;
		}
		case TERMINATE:
		{
			BlockRunningProcess(NULL); // terminate RUNNING

			break;
		}
		case NICE:
		{
			int old_priority = RUNNING->Priority;
			int new_priority = kcaptr->Arg1;

			Dequeue(RUNNING, (QueueItem**)&(PRIORITY_LIST[RUNNING->Priority])); // Dequeue from old priority queue
			RUNNING->Priority = new_priority; // assign new priority
			Enqueue(RUNNING, (QueueItem**)&(PRIORITY_LIST[RUNNING->Priority])); // Enqueue to new priority queue

			if (new_priority<old_priority) // if nice to lower priority queue
			{
				RUNNING = CheckLowerPriorityProcess(old_priority); // update RUNNING
				PRIORITY_LIST[RUNNING->Priority] = RUNNING; // update head of process queue

				set_PSP((unsigned long)(RUNNING->PSP)); // run new process
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
			if (mailbox_to_unbind>=0 && mailbox_to_unbind <MAILBOXLIST_SIZE) // if mailbox number is valid
			{
				Mailbox* mbx = &MAILBOXLIST[mailbox_to_unbind];
				if (mbx->Owner == RUNNING) // if RUNNING process own this mailbox, then unbind
				{
					mbx->Owner = NULL; // clear owner
					Dequeue(mbx, (QueueItem**)&(RUNNING->Mailbox_Head)); // remove from process's mailbox list
					EnqueueMbxToAvailable(mbx,&AVAILABLE_MAILBOX); // add to available mailboxes
				}
				else
					kcaptr->RtnValue = ERROR;
			}
			else
				kcaptr->RtnValue = ERROR;
			break;
		}
		case RECEIVE:
		{
			RecvMsgArgs* args = (RecvMsgArgs*)kcaptr->Arg1; // get the argument

			if (args->Recver == ANYMAILBOX) // if receive from any mailbox
			{
				Mailbox* head = RUNNING->Mailbox_Head;
				if (head != NULL) // if bound to at least one mailbox
				{
					Mailbox* mbx = head;
					while (TRUE) // search each mailbox
					{
						if (mbx->First_Message != NULL) // if have message in mailbox
						{
							ReceiveMsgFromMailbox(args, mbx);
							break;
						}
						
						mbx = mbx->Next;
						if (mbx == head) // if searched all mailboxes, no message
						{
						    // Block running
						    BlockRunningProcess(args);
							break;
						}
					}
				}
				else
					*args->Size = INVALID_RECVER;
			}
			else // receive from specific mailbox
			{
				if (MAILBOXLIST[args->Recver].Owner == RUNNING) // if receiver is valid
				{
					if (MAILBOXLIST[args->Recver].First_Message == NULL) // if no message waiting, block
					{
						// Block running
						BlockRunningProcess(args);
					}
					else // has message, copy to process stack
						ReceiveMsgFromMailbox(args, &MAILBOXLIST[args->Recver]);
				}
				else
					*args->Size = INVALID_RECVER;
			}
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
					if ((recver->Mailbox_Wait == args->Recver || recver->Mailbox_Wait == ANYMAILBOX) && recver->Msg_Wait != NULL) // if receiver is blocked and waiting for this mailbox
					{
						RecvMsgArgs* receiver_waiting = recver->Msg_Wait;
						int copy_size = *args->Size < *receiver_waiting->Size ? *args->Size : *receiver_waiting->Size; // get smaller size

						// Copy msg to receiver
						memcpy(receiver_waiting->Msg_addr, args->Msg_addr, copy_size); // copy message
						*args->Size = copy_size; // update size

						// Unblock process
						Enqueue(recver, (QueueItem**)& (PRIORITY_LIST[recver->Priority])); // add back to process queue
						recver->Mailbox_Wait = NULL;
						recver->Msg_Wait = NULL;
						UNBLOCK_PRIORITY = recver->Priority;
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
							MAILBOXLIST[args->Recver].Last_Message = msg; // update mailbox's last message pointer
						}
						else
						{
							mbx_last_msg->Next = msg; // add to end on list
							MAILBOXLIST[args->Recver].Last_Message = msg; // update tail of list
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

	/* Save running process */
	save_registers(); /* Save active CPU registers in PCB */
	RUNNING->PSP = (Stack*)get_PSP(); // update PSP

	PRIORITY_LIST[RUNNING->Priority] = RUNNING->Next; // Update the head of queue

	// get next running process
	if (UNBLOCK_PRIORITY > RUNNING->Priority) // if unblocked higher priority process
		RUNNING = PRIORITY_LIST[UNBLOCK_PRIORITY]; // run unblocked process
	else
		RUNNING = RUNNING->Next; // run next

	// Update PSP value
	set_PSP((unsigned long)(RUNNING->PSP));

	restore_registers(); /* Restore process¡¯s registers */

	PENDSV_ON = FALSE;
}
