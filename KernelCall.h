/*
 * KernelCall.h
 *
 *  Created on: Nov 1, 2019
 *      Author: Ray
 */

#ifndef KERNELCALL_H_
#define KERNELCALL_H_

enum KernelCallCode {GETID, NICE, TERMINATE,SEND,RECEIVE,BIND};

struct KCallArgs
{
    int Code;
    int RtnValue;
    int Arg1;
    int Arg2;
};

void KernelInitialization();
int KC_GetID();
void Terminate();
int Nice(int new_priority);

#endif /* KERNELCALL_H_ */
