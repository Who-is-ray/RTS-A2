/*
 * KernelCall.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include "KernelCall.h"
#include "Process.h"

void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}

int GetID()
{
    volatile struct KCallArgs getidarg; /* Volatile to actually reserve space on stack */
    getidarg.Code = GETID;

    /* Assign address if getidarg to R7 */
    assignR7((unsigned long) &getidarg);

	SVC();

    return getidarg.RtnValue;

}

void Terminate()
{
	volatile struct KCallArgs getidarg; /* Volatile to actually reserve space on stack */
	getidarg.Code = TERMINATE;

	/* Assign address if getidarg to R7 */
	assignR7((unsigned long)&getidarg);

	SVC();
}
