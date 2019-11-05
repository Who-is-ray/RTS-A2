/*
 * KernelCall.h
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray
 */

#ifndef KERNELCALL_H_
#define KERNELCALL_H_

typedef enum 
{
	GETID, 
	NICE, 
	TERMINATE,
	SEND,
	RECEIVE,
	BIND,
	UNBIND
}KernelCallCode;

struct KCallArgs
{
	KernelCallCode Code;
    int RtnValue;
    int Arg1;
    int Arg2;
};

void KernelInitialization();
int KC_GetID();
void Terminate();
int Nice(int new_priority);
void AssignR7(volatile unsigned long data);

#endif /* KERNELCALL_H_ */
