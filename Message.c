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
	Mailbox* head = *queue_head;
	if (head == NULL) // if has no item in the queue
	{
		head = mbx;
		mbx->Next = mbx;
		mbx->Prev = mbx;
	}
	else // insert after the first item
	{
		mbx->Prev = AVAILABLE_MAILBOX;
		mbx->Next = AVAILABLE_MAILBOX->Next;
		AVAILABLE_MAILBOX->Next->Prev = mbx;
		AVAILABLE_MAILBOX->Next = mbx;
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
	Mailbox* head = *queue_head;
	if (head->Next == NULL) // if is the only mailbox in the queue
	{
		mbx->Next = NULL;
		mbx->Prev = NULL;
		head = NULL;
	}
	else // remove from the queue
	{
	    if(head == mbx) // if is the head of queue
			head = head->Next; // update the head

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
