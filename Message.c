/*
 * Message.c
 *
 *  Created on: 2019?11?2?
 *      Author: Victor
 *Purpose: these functions used for support IPC
 */
#include "Message.h"
#include "Process.h"
#include"KernelCall.h"
#include<stdio.h>
#define max_Mailbox 10
/*the position of time sever and uart output are fixed
 * does uart input need a mailbox, too?*/
#define Time_sever_mailbox 9
#define Uart_output_mailbox 8
struct mailbox_list mbx[max_Mailbox];
struct __message*message_head=NULL;
struct __message*message_curr,*message_prev;
extern void assignR7(volatile unsigned long data);
void pkcall(int code,void*pkmsg){

    if (code == send)
    {
        /*process-kernel call function. Supplies code and kernel message to the
         * kernel is a kernel request. Has no return*/
        volatile struct KCallArgs arg;
        arg.Code = send;
        arg.Arg1 = (int)pkmsg;

        /*R7=address of arglist structure*/
        assignR7((unsigned long)&arglist);
        /*call kernel*/
        SVC();
    }
}

void Initialize_Mailbox(void){
    /*initialize the mailboxes in mailbox list.
     * there are 10 mailboxes, mbx[7,8,9] are
     * reserved for uart in, uart out and time server*/
    int i=0;
    for(i=0;i<max_Mailbox;i++){
        /*before mailbox belong to same process
         * their next, prev and pcb_ptr are all
         * set to null
         */
        mbx[i].Next=NULL;
        mbx[i].Prev=NULL;
        mbx[i].pcb_ptr=NULL;
    }
}
int p_send(unsigned int dst,unsigned int from,void*msg,unsigned int size){
    struct Send_Recv_args send_args;
    send_args.dst_mbx=dst;
    send_args.from_mbx=from;
    send_args.msg=msg;
    send_args.size=size;
    pkcall(SEND,&send_args);
    return send_args.rtncode;
}
int p_recv(unsigned int dst,unsigned int from,void*msg,unsigned int size){
    struct Send_Recv_args recv_args;
    recv_args.dst_mbx=dst;
        recv_args.from_mbx=from;
        recv_args.msg=msg;
        recv_args.size=size;
        pkcall(SEND,&recv_args);
        return recv_args.rtncode;
}
