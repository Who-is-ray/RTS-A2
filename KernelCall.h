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

// Enum for kernal call action code
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

// Kernal call argument struct
struct KCallArgs
{
	KernelCallCode Code;
    int RtnValue;
    int Arg1;
    int Arg2;
};

// Initialization Kernal
void KernelInitialization();

// Get ID Kernal call
int KC_GetID();

// Termination kernal call
void Terminate();

// Nice kernal call
int Nice(int new_priority);

// Save address to R7
void AssignR7(volatile unsigned long data);

#endif /* KERNELCALL_H_ */
