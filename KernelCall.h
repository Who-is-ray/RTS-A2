/*
 *File name: KernelCall.h
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: support for kernel call
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
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
