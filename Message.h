/*
 * Message.h
 *
 *  Created on: 2019?11?2?
 *      Author: Victor
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
typedef struct __message{//contain 4 elements
    void* message_addr;//??not sure about this
    unsigned int size;
    //pointer, point to mailbox which send from.
    unsigned sender_mbx;
    //pointer, point to next message
    struct __message* next_msg;
} message;
typedef struct mailbox_list{//contain 5 elements
    //mailbox list for several mbx belong to same process
    struct mailbox_list*Next;
    struct mailbox_list*Prev;
    //pointer, point to pcb
    struct __PCB* pcb_ptr;
    //pointer, point to the message
    struct __message* msg_ptr;
    //pointer, point to the last message in one of the mailbox
    struct __message* tail_msg;
} ;
typedef struct Send_Recv_args{//5 arguments, used for both send and recv
    unsigned int dst_mbx;//the receiver box
    unsigned int from_mbx;//the sender box
    void * msg;//the address of the actual msg or data
    unsigned int size;//the size of the msg
    int rtncode;//the corresponding return code
};
typedef struct krequest{
    int code;
    int rtnvalue;
    void*pkmsg;//address(32bit value) of process message
};
extern void Initialize_Mailbox(void);
extern int p_send(unsigned int dst,unsigned int from,void*msg,unsigned int size);
extern int p_recv(unsigned int dst,unsigned int from,void*msg,unsigned int size);
extern void pkcall(int code,void*pmsg);
#endif /* MESSAGE_H_ */
