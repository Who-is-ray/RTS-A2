/*
 *File name: Queue.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: a queue that support the uart to store the message that wait to assigned to uart data register.
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.19
 */

#include <string.h>
#include "Queue.h"
#include "Uart.h"

extern int UART_STATUS;

Queue InQ;
Queue OutQ;

void Queue_Init()
{//initialize both input and output queue
    InQ.Head=0;//input queue's head
    InQ.Tail=0;//input queue's tail
    OutQ.Head=0;//output queue's head
    OutQ.Tail=0;//output queue's tail
}

int EnQueueIO(QueueType t, Source s, char v)
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

int DeQueueIO(QueueType t, Source* s, char* v)
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

/* Transmit a character*/
void TransChar(char c)
{
	while (EnQueueIO(OUTPUT, UART, c) == FALSE); // wait until it is enqueued
}

/* Output a string*/
void OutputData(char* s, int size)
{
	int i;
	for (i = 0; i < size; i++) // output each character in string
	{
		TransChar(*s);
		s++;
	}
}
