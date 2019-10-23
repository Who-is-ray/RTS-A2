/*
 * Queue.c
 *
 *  Created on: Sep 24, 2019
 *      Author: Ray
 */

#include "Queue.h"
#include "Uart.h"

extern int UART_STATUS;

Queue InQ;
Queue OutQ;

void Queue_Init()
{
    InQ.Head=0;
    InQ.Tail=0;
    OutQ.Head=0;
    OutQ.Tail=0;
}

int EnQueue(QueueType t, Source s, char v)
{
    int head;
    switch (t)
    {
        case INPUT:
        {
            head = InQ.Head;
            if(((head+1) & QSM1) != InQ.Tail) // if not full
            {
                (InQ.queue[head]).value = v;
                InQ.queue[head].source = s;
                InQ.Head=(head+1)&QSM1;
                return TRUE;
            }
            break;
        }
        case OUTPUT:
        {
            head = OutQ.Head;
            if(((head+1) & QSM1) != OutQ.Tail)  // if not full
            {
                UART0_IntDisable(UART_INT_TX); // disable UART transmit interrupt
                if(UART_STATUS == BUSY) // if uart is busy
                {
                    // add to queue
                    OutQ.queue[OutQ.Head].value = v;
                    OutQ.Head=(head+1)&QSM1;
                }
                else // uart not busy
                {
                    // directly output, set to busy
                    UART_STATUS = BUSY;
                    UART0_DR_R = v;
                }
                UART0_IntEnable(UART_INT_TX); // enable UART transmit interrupt
                return TRUE;
            }
            break;
        }
    }
    return FALSE;
}

int DeQueue(QueueType t, Source* s, char* v)
{
    int tail;
    switch (t)
    {
        case INPUT:
        {
            tail = InQ.Tail;
            if(InQ.Head!=tail)  // if not empty, dequeue
            {
                *s=InQ.queue[tail].source;
                *v=InQ.queue[tail].value;
                InQ.Tail = (tail+1)&QSM1;    // Update tail
            }
            else    // if empty
                return FALSE;

            break;
        }

        case OUTPUT:
        {
            tail = OutQ.Tail;
            if(OutQ.Head!=tail)  // if not empty, dequeue
            {
                *v=OutQ.queue[tail].value;
                OutQ.Tail = (tail+1)&QSM1;    // Update tail
            }
            else    // if empty
                return FALSE;

            break;
        }
    }
    return TRUE;
}
