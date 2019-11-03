/*
 * Message.c
 *
 *  Created on: 2019?11?2?
 *      Author: Victor
 *Purpose: these functions used for support IPC
 */
#include "Message.h"
#include "Process.h"
#define max_Mailbox 10
struct mailbox_list mbx[max_Mailbox];
struct __message*head=NULL;
struct __message*current,*prev;
void Initialize_Mailbox(void){
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
