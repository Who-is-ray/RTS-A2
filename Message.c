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

void EnqueueToAvailableMbx(Mailbox* Mbx)
{
	if (AVAILABLE_MAILBOX == NULL) // if has no item in the queue
	{
		AVAILABLE_MAILBOX = Mbx;
		Mbx->Next = Mbx;
		Mbx->Prev = Mbx;
	}
	else // insert after the first item
	{
		Mbx->Prev = AVAILABLE_MAILBOX;
		Mbx->Next = AVAILABLE_MAILBOX->Next;
		AVAILABLE_MAILBOX->Next->Prev = Mbx;
		AVAILABLE_MAILBOX->Next = Mbx;
	}

	// clear the owner
	Mbx->Owner = NULL;

	// clear last message pointer
	Mbx->Last_Message = NULL;

	// release all message
	while (Mbx->First_Message != NULL)
	{
		Message* next = Mbx->First_Message->Next;
		free(Mbx->First_Message);
		Mbx->First_Message = next;
	}
}

void DequeueFromAvailableMbx(Mailbox* Mbx)
{
	if (AVAILABLE_MAILBOX->Next == NULL) // if is the only mailbox in the queue
	{
		Mbx->Next = NULL;
		Mbx->Prev = NULL;
		AVAILABLE_MAILBOX = NULL;
	}
	else // remove from the queue
	{
	    if(AVAILABLE_MAILBOX == Mbx) // if is the head of queue
	        AVAILABLE_MAILBOX = AVAILABLE_MAILBOX->Next; // update the head

		// change the queue link
		Mbx->Next->Prev = Mbx->Prev;
		Mbx->Prev->Next = Mbx->Next;
		
		// clear mailbox value
		Mbx->Next = NULL;
		Mbx->Prev = NULL;
	}
}

void MailboxListIntialization()
{
	int i;
	for (i = 0; i < MAILBOXLIST_SIZE; i++)
	    EnqueueToAvailableMbx(&MAILBOXLIST[i]); // Add to Available maibox list
}

#endif /* MESSAGE_C_ */
