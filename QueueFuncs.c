/*
 *File name: Queuefuncs.c
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: this is enque and dequeue the processes in process list
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
 */
#include <stdio.h>
#include "QueueFuncs.h"

void Enqueue(void* _item, QueueItem** queue_head)
{
    QueueItem* item = (QueueItem*)_item; // get item
    if ((*queue_head) == NULL) // if queue is empty, insert to queue
    {
        *queue_head = item; // update queue head
        item->Next = item; // set next
        item->Prev = item; // set previous
    }
    else
    {
        // set new item's next and previous
        item->Next = (*queue_head)->Next;
        item->Prev = (*queue_head)->Prev;

        // change connect queue
        (*queue_head)->Next->Prev = item;
        (*queue_head)->Next = item;
    }
}

void Dequeue(void* _item, QueueItem** queue_head)
{
    QueueItem* item = (QueueItem*)_item;
    if ((*queue_head)->Next == (*queue_head)) // if has only one object, clear the queue
	{
		*queue_head = NULL; // clear head
		item->Next = NULL; // clear next
		item->Prev = NULL; // clear previous
	}
	else
	{
		if ((*queue_head) == item) // if item is the head of queue
			(*queue_head) = (*queue_head)->Next; // update queue head

		// change the queue link
		item->Next->Prev = item->Prev;
		item->Prev->Next = item->Next;

		// clear mailbox value
		item->Next = NULL;
		item->Prev = NULL;
	}
}
