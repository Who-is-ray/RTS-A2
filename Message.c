/*
 * Message.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include <stdlib.h>
#include "Message.h"
#include "QueueFuncs.h"

#ifndef MESSAGE_C_
#define MESSAGE_C_

Mailbox MAILBOXLIST[MAILBOXLIST_SIZE];
Mailbox* AVAILABLE_MAILBOX = NULL;

void EnqueueMbxToAvailable(Mailbox* mbx, Mailbox** queue_head)
{
	Enqueue(mbx, (QueueItem**)queue_head);

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

void MailboxListIntialization()
{
	int i;
	for (i = 0; i < MAILBOXLIST_SIZE; i++)
	{
		MAILBOXLIST[i].ID = i;
		EnqueueMbxToAvailable(&MAILBOXLIST[i],&AVAILABLE_MAILBOX); // Add to Available maibox list
	}
}

#endif /* MESSAGE_C_ */
