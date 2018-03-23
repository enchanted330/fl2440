/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_queue.h
 *    Description:  This head file is for the queue implement based on link list.
 *
 *        Version:  1.0.0(11/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/12/2012 01:26:34 PM"
 *                 
 ********************************************************************************/
#ifndef __CP_QUEUE
#define __CP_QUEUE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef  struct _CP_QNODE
{
    void               *item;
    struct _CP_QNODE   *next;
} CP_QNODE;


typedef struct _CP_QUEUE
{
    CP_QNODE           *front;
    CP_QNODE           *rear;
    int                items;
    int                size;
} CP_QUEUE;

#define cp_queue_is_full(pq)  ( (pq)->size==(pq)->items ? 1 :0 )
#define cp_queue_is_empty(pq) ( 0==(pq)->items ? 1 : 0)
#define cp_queue_count(pq) ( (pq)->items )
#define cp_queue_size(pq) ( (pq)->size )

CP_QUEUE *cp_queue_init(CP_QUEUE *queue, int size);
void *cp_enqueue(CP_QUEUE *queue, void *data);
void cp_travel_queue(CP_QUEUE *queue);
void *cp_rmqueue(CP_QUEUE *queue, void *data);
void *cp_dequeue(CP_QUEUE *queue);
void cp_queue_destroy(CP_QUEUE *queue);
#define cp_queue_destroy_clear(queue) {cp_queue_destroy(queue); queue=NULL;}

#endif
