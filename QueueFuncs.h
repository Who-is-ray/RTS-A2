/*
 * QueueFuncs.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Ray
 */

#ifndef QUEUEFUNCS_H_
#define QUEUEFUNCS_H_

typedef struct __QueueItem QueueItem;

struct __QueueItem
{
    QueueItem* Next;
    QueueItem* Prev;
};

// Enqueue item to a queue. Always insert to after the queue head
void Enqueue(void* _item, QueueItem** queue_head);

// Dequeue item from a queue.
void Dequeue(void* _item, QueueItem** queue_head);

#endif /* QUEUEFUNCS_H_ */
