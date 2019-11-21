/*
 * PKCall.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray, Victor
 */

#ifndef PKCALL_H_
#define PKCALL_H_

// Error code
typedef enum
{
	INVALID_SENDER = -2,// invalid sender	(-2)
	INVALID_RECVER		// invalid receiver (-1)
} MsgRtnCode; // send message error code

// Send message
int Send(int recver, int sender, void* msg, int* size);

// Receive message
int Receive(int recver, int* sender, void* msg, int* size);

// Bind mailbox
int Bind(int mbx);

// Unbind mailbox
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
