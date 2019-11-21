/*
 * Message.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray, Victor
 */

#include <stdio.h>
#include <stdlib.h>
#include "Message.h"
#include "QueueFuncs.h"

#ifndef MESSAGE_C_
#define MESSAGE_C_

Mailbox MAILBOXLIST[MAILBOXLIST_SIZE]; // Mailbox list holds all mailboxes
Mailbox* AVAILABLE_MAILBOX = NULL; // The head of available mailbox

// Enqueue mailbox to available mailboxes
void EnqueueMbxToAvailable(Mailbox* mbx, Mailbox** queue_head)
{
	Enqueue(mbx, (QueueItem**)queue_head); // Enqueue mailbox to available mailboxes

	// clear the owner
	mbx->Owner = NULL;

	// clear last message pointer
	mbx->Last_Message = NULL;

	// release all message
	while (mbx->First_Message != NULL)
	{
		Message* next = mbx->First_Message->Next; // get the first msg
		free(mbx->First_Message); // free msg
		mbx->First_Message = next; // update head
	}
}

// Initialization Mailbox list
void MailboxListIntialization()
{
	int i;
	for (i = 0; i < MAILBOXLIST_SIZE; i++) // Set mailbox one by one
	{
		MAILBOXLIST[i].ID = i; // set mailbox ID
		EnqueueMbxToAvailable(&MAILBOXLIST[i],&AVAILABLE_MAILBOX); // Add to Available mailbox list
	}
}

#endif /* MESSAGE_C_ */
