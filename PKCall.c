/*
 * PKCall.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Ray
 */

#include <stdlib.h>
#include "PKCall.h"
#include "KernelCall.h"
#include "Process.h"

extern PCB* RUNNING;

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

	// Update PSP value
	set_PSP((unsigned long)(RUNNING->PSP));

	/* Call kernel */
	SVC();
	/* Return result of request to caller */
	return arg.RtnValue;
}

void Send(int recver, int sender, void* msg, int* size)
{
	SendMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };
	PKCall(SEND, (int)&arg);
}

void Receive(int recver, int* sender, void* msg, int* size)
{
	RecvMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };
	PKCall(RECEIVE, (int)&arg);
}

int Bind(int mbx)
{
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */

	// pass code and mailbox number to bind to kernel
	args.Code = BIND;
	args.Arg1 = mbx;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// Update PSP value
	set_PSP((unsigned long)(RUNNING->PSP));

	// Call Kernel
	SVC();

	return args.RtnValue;
}
