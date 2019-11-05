/*
 * Message.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#define MAILBOXLIST_SIZE	32

typedef struct __Message Message;
typedef struct __Mailbox Mailbox;

void MailboxListIntialization();
void EnqueueMbxToQueue(Mailbox* mbx, Mailbox** queue_head);
void DequeueMbxFromQueue(Mailbox* mbx, Mailbox** queue_head);

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
	// Links to adjacent Mailboxs when mailbox in available list
	Mailbox* Next;
	Mailbox* Prev;

	// Pointer of the first message in the mailbox
	Message* First_Message;

	// Pointer of the last message in the mailbox
	Message* Last_Message;

	// Owner of the mailbox, PCB pointer
	void* Owner;
};

#endif /* MESSAGE_H_ */
