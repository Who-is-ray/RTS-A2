/*
 *File name: Queuefuncs.h
 *School: Dalhousie University
 *Department: Electrical and computer engineering
 *Course: Real time system
 *Professor: Larry Hughes
 *Purpose: this is not for queue.c. this header file is support for process' queue, the queuefuncs.c
 *Author:   Ray     Su  B00634512
            Victor  Gao B00677182
 *Last day modified: Nov.06
 */
#ifndef QUEUEFUNCS_H_
#define QUEUEFUNCS_H_

typedef struct __QueueItem QueueItem;

// Queue Item Struct, only take first two elements, which is Next and Prev pointer
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
