/*
 * PKCall.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#ifndef PKCALL_H_
#define PKCALL_H_

// Structure of send 
typedef struct
{
	// Mailbox number of receiver
	int Recver;

	// Mailbox number of sender
	int Sender;

	// Address of message to send
	void* Msg_addr;

	// Size of message, number of bytes
	int Size;
} SendMsgArgs;

// Structure of send 
typedef struct
{
	// Mailbox number of receiver
	int Recver;

	// Mailbox number of sender
	int* Sender;

	// Address of message to send
	void* Msg_addr;

	// Size of message, number of bytes
	int Size;
} RecvMsgArgs;

#endif /* PKCALL_H_ */
