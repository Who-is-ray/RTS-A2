/*
 * Message.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#include "Process.h"

#ifndef MESSAGE_H_
#define MESSAGE_H_

typedef struct __Message Message;
typedef struct __Mailbox Mailbox;

// stucture of Message
struct __Message
{
	// Address of message
	void* Message_Addr;

	// Size of message
	int Size;

	// Sender's mailbox number
	int Sender;

	// Pointer of next message in the mailbox
	Message* Next;
};

// Structure of Mailbox
struct __Mailbox
{
	// Links to adjacent Mailboxs which bound to the same process
	Mailbox* Next;
	Mailbox* Prev;

	// Pointer of the first message in the mailbox
	Message* First_Message;

	// Pointer of the last message in the mailbox
	Message* Last_Message;

	// Owner of the mailbox
	PCB* Owner;
};

#endif /* MESSAGE_H_ */
