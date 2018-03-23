/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_queue.c
 *    Description:  This file is the queue implement based on link list.
 *                 
 *        Version:  1.0.0(11/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/12/2012 01:25:26 PM"
 *                 
 ********************************************************************************/

#include "cp_queue.h"
#include "cp_common.h"

CP_QUEUE *cp_queue_init(CP_QUEUE *queue, int size)
{
    if(!queue)
    {
        queue = t_malloc(sizeof(*queue));
    }
    memset(queue, 0, sizeof(*queue));

    queue->size = size;
    queue->items = 0;
    queue->rear = queue->front = NULL;

    return queue;
}

void *cp_enqueue(CP_QUEUE *queue, void *data)
{
    CP_QNODE         *node;

    if(!queue || cp_queue_is_full(queue))
    {
        return NULL;
    }

    node = t_malloc(sizeof(*node));
    if(NULL == node)
    {
        return NULL;
    }

    node->item = data;
    node->next = NULL;

    if(cp_queue_is_empty(queue))
    {
        queue->front = node;
    }
    else
    {
        queue->rear->next = node;
    }

    queue->rear = node;
    queue->items++;

    return data;
}

void cp_travel_queue(CP_QUEUE *queue)
{
    CP_QNODE       *node;

    for(node=queue->front; node!=NULL; node=node->next)
    {
        printf("queue node[%p] save data [%p]\n", node, node->item);
    }
}

void *cp_rmqueue(CP_QUEUE *queue, void *data)
{
    CP_QNODE       *node, *tmp;

    if(!queue || cp_queue_is_empty(queue))
    {
        return NULL;
    }

    /* The first node->item is the data, we find it */
    if(queue->front->item==data)
    {
        tmp = queue->front;
        queue->front = queue->front->next;
        queue->items -= 1;
        t_free(tmp);
        return data;
    }

    for(node=queue->front; node!=NULL; node=node->next)
    {
        if(node->next->item == data)
        {
            tmp = node->next;
            queue->items -= 1;

            if(node->next != queue->rear)
                node->next = node->next->next;
            else
            {
                queue->rear = node;
                queue->rear->next = NULL; 
            }

            t_free(tmp);
            return data;
        }
    }

    return data;
}

void *cp_dequeue(CP_QUEUE *queue)
{
    CP_QNODE       *node;
    void           *item;

    if(!queue || cp_queue_is_empty(queue))
    {
        return NULL;
    }

    node = queue->front;
    queue->front = queue->front->next;

    item = node->item;
    t_free(node);

    queue->items--;

    if(queue->items == 0)
        queue->rear = NULL;

    return item;
}

void cp_queue_destroy(CP_QUEUE *queue)
{
    if(!queue)
        return ;

    while( !cp_queue_is_empty(queue) )
    {
        cp_dequeue(queue);
    }

    t_free(queue);

    return ;
}

