/*
 * Message.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include "Message.h"

#ifndef MESSAGE_C_
#define MESSAGE_C_

#define MAILBOXLIST_SIZE	32

Mailbox MAILBOXLIST[MAILBOXLIST_SIZE];
Mailbox* AVAILABLE_MAILBOX = NULL;

void IntializeMailboxList()
{
	int i;
	for (i = 0; i < MAILBOXLIST_SIZE; i++)
	{
		MAILBOXLIST[i].First_Message = NULL;
		MAILBOXLIST[i].Last_Message = NULL;
		MAILBOXLIST[i].Owner = NULL;

		// Add to Available maibox list
		EnqueueToAvailiableMbx(&MAILBOXLIST[i]);
	}
}

void EnqueueToAvailableMbx(Mailbox* Mbx)
{
	if (AVAILABLE_MAILBOX == NULL) // if has no item in the queue
		AVAILABLE_MAILBOX = Mbx;
	else // insert after the first item
	{
		Mbx->Prev = AVAILABLE_MAILBOX;
		Mbx->Next = AVAILABLE_MAILBOX->Next;
		AVAILABLE_MAILBOX->Next->Prev = Mbx;
		AVAILABLE_MAILBOX->Next = Mbx;
	}
}

void DequeueFromAvailableMbx(Mailbox* Mbx)
{
	if (AVAILABLE_MAILBOX == Mbx)
	{
		Mbx->Next = NULL;
		Mbx->Prev = NULL;
		AVAILABLE_MAILBOX = NULL;
	}
}

#endif /* MESSAGE_C_ */
