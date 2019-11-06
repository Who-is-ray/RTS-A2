/*
 * PKCall.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#ifndef PKCALL_H_
#define PKCALL_H_

void Send(int recver, int sender, void* msg, int* size);
void Receive(int recver, int* sender, void* msg, int* size);
int Bind(int mbx);
int Unbind(int mbx);

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
	int* Size;
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
	int* Size;
} RecvMsgArgs;

#endif /* PKCALL_H_ */
