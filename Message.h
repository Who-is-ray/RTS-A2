/*
 * Message.h
 *
 *  Created on: 2019?11?2?
 *      Author: Victor
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
typedef struct __message{//contain 4 elements
    unsigned long message_addr;//??not sure about this
    unsigned int size;
    //pointer, point to mailbox which send from.
    mbx* sender_mbx;
    //pointer, point to next message
    msg* next_msg;
} msg;
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
extern void Initialize_Mailbox(void);

#endif /* MESSAGE_H_ */
