/*
 * QueueFuncs.c
 *
 *  Created on: Nov 6, 2019
 *      Author: Ray
 */

#include <stdio.h>
#include "QueueFuncs.h"

void Enqueue(void* _item, QueueItem** queue_head)
{
    QueueItem* item = (QueueItem*)_item;
    if ((*queue_head) == NULL) // if queue is empty, insert to queue
    {
        *queue_head = item;
        item->Next = item;
        item->Prev = item;
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
    if ((*queue_head)->Next == NULL) // if queue is empty, insert to queue
	{
		*queue_head = NULL;
		item->Next = NULL;
		item->Prev = NULL;
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
