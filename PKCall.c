/*
 *File name: PKCall.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: pkcall provide the features that used by message system, includes send bind unbind and receive.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
 */

#include <stdlib.h>
#include "PKCall.h"
#include "KernelCall.h"
#include "Process.h"

extern PCB* RUNNING; // RUNNING process

int PKCall(KernelCallCode code, int pkmsg)
{
	/*
	Process-kernel call function.  Supplies code and kernel message to the
	kernel is a kernel request.  Returns the result (return code) to the caller.
	*/
	volatile struct KCallArgs arg;
	/* Pass code and pkmsg to kernel in arglist structure */
	arg.Code = code;
	arg.Arg1 = pkmsg;
	/* R7 = address of arglist structure */
	AssignR7((unsigned long)&arg);

	/* Call kernel */
	SVC();
	/* Return result of request to caller */
	return arg.RtnValue;
}

// Send message
int Send(int recver, int sender, void* msg, int* size)
{//send message from a process to a process, it takes the sender receiver message and size.
	SendMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };//format the send arguments.
	PKCall(SEND, (int)&arg);//call pkcall to do it in kernel space.
	return *arg.Size;
}

// Receive message
int Receive(int recver, int* sender, void* msg, int* size)
{//similar to send
	RecvMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };
	PKCall(RECEIVE, (int)&arg);
	return *arg.Size;
}

// bind mailbox
int Bind(int mbx)
{//bind is to allocate the mailbox to a process when process requesting a new mailbox
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */

	// pass code and mailbox number to bind to kernel
	args.Code = BIND;
	args.Arg1 = mbx;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// Call Kernel
	SVC();

	return args.RtnValue;//returns the mailbox number that allocates.
}

// Unbind mailbox
int Unbind(int mbx)
{//deallocate the mailbox(free the mailbox) when a process requesting to unbind the mailbox, or when terminates
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */

	// pass code and mailbox number to bind to kernel
	args.Code = UNBIND;
	args.Arg1 = mbx;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// Call Kernel
	SVC();

	return args.RtnValue;//returns the mailbox number that deallocated.
}
