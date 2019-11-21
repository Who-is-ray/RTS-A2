/*
 *File name: Message.h
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: support for the message.c
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
 */
#ifndef MESSAGE_H_
#define MESSAGE_H_

#define MAILBOXLIST_SIZE	32 // size of mailbox list

typedef struct __Message Message;
typedef struct __Mailbox Mailbox;

// Enqueue mailbox to available mailboxes
void MailboxListIntialization();

// Initialization Mailbox list
void EnqueueMbxToAvailable(Mailbox* mbx, Mailbox** queue_head);

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

	// Mailbox ID
	int ID;
};

#endif /* MESSAGE_H_ */
