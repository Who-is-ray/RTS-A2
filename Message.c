/*
 * Message.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include <stdlib.h>
#include "Message.h"

#ifndef MESSAGE_C_
#define MESSAGE_C_

Mailbox MAILBOXLIST[MAILBOXLIST_SIZE];
Mailbox* AVAILABLE_MAILBOX = NULL;

void EnqueueMbxToQueue(Mailbox* mbx, Mailbox** queue_head)
{
	if (*queue_head == NULL) // if has no item in the queue
	{
	    *queue_head = mbx;
		mbx->Next = mbx;
		mbx->Prev = mbx;
	}
	else // insert after the first item
	{
		mbx->Prev = *queue_head;
		mbx->Next = (*queue_head)->Next;
		(*queue_head)->Next->Prev = mbx;
		(*queue_head)->Next = mbx;
	}

	// clear the owner
	mbx->Owner = NULL;

	// clear last message pointer
	mbx->Last_Message = NULL;

	// release all message
	while (mbx->First_Message != NULL)
	{
		Message* next = mbx->First_Message->Next;
		free(mbx->First_Message);
		mbx->First_Message = next;
	}
}

void DequeueMbxFromQueue(Mailbox* mbx, Mailbox** queue_head)
{
	if ((*queue_head)->Next == NULL) // if is the only mailbox in the queue
	{
		mbx->Next = NULL;
		mbx->Prev = NULL;
		(*queue_head) = NULL;
	}
	else // remove from the queue
	{
	    if((*queue_head) == mbx) // if is the head of queue
			(*queue_head) = (*queue_head)->Next; // update the head

		// change the queue link
		mbx->Next->Prev = mbx->Prev;
		mbx->Prev->Next = mbx->Next;
		
		// clear mailbox value
		mbx->Next = NULL;
		mbx->Prev = NULL;
	}
}

void MailboxListIntialization()
{
	int i;
	for (i = 0; i < MAILBOXLIST_SIZE; i++)
	    EnqueueMbxToQueue(&MAILBOXLIST[i],&AVAILABLE_MAILBOX); // Add to Available maibox list
}

#endif /* MESSAGE_C_ */
