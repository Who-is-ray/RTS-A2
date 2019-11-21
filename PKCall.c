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
#include "Uart.h"

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

	/* Call kernel */
	SVC();
	/* Return result of request to caller */
	return arg.RtnValue;
}

int Send(int recver, int sender, void* msg, int* size)
{
    if(recver == UART_OUTPUT_MBX)
    {
        UART0_DR_R = *((int*)(msg));
        return TRUE;
    }
    else
    {
        SendMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };
        PKCall(SEND, (int)&arg);

        return *arg.Size;
    }
}

int Receive(int recver, int* sender, void* msg, int* size)
{
	RecvMsgArgs arg = { .Recver = recver, .Sender = sender, .Msg_addr = msg, .Size = size };
	PKCall(RECEIVE, (int)&arg);
	return *arg.Size;
}

int Bind(int mbx)
{
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */

	// pass code and mailbox number to bind to kernel
	args.Code = BIND;
	args.Arg1 = mbx;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// Call Kernel
	SVC();

	return args.RtnValue;
}

int Unbind(int mbx)
{
	volatile struct KCallArgs args; /* Volatile to actually reserve space on stack */

	// pass code and mailbox number to bind to kernel
	args.Code = UNBIND;
	args.Arg1 = mbx;

	/* Assign address if getidarg to R7 */
	AssignR7((unsigned long)&args);

	// Call Kernel
	SVC();

	return args.RtnValue;
}
