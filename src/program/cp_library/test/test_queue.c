/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_queue.c
 *    Description:  This file used to test the queue library
 *                 
 *        Version:  1.0.0(08/14/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/14/2012 05:17:03 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cp_queue.h"
#include "cp_sock.h"

#define MAX_ITEMS  10

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int i;

    CP_SOCK *sock = NULL;
    CP_SOCK *tmp = NULL;

    CP_QUEUE *queue = NULL;
    
    queue = cp_queue_init(NULL, MAX_ITEMS);

    //for (i=0; i<MAX_ITEMS; i++)
    for (i=0; i<MAX_ITEMS+10; i++)
    //for (i=0; i<MAX_ITEMS-3; i++)
    {
        sock = cp_sock_init(NULL, 1024, 1024, 10, 30);
        if( NULL==cp_enqueue(queue, sock) )
        {
            cp_sock_term(sock);
        }
        else
        {
            if(i==3)
            {
                tmp = sock;
            }
            printf("enqueue sock %p\n", sock);
        }
    }


    printf("queue usage %d/%d \n", cp_queue_count(queue), cp_queue_size(queue));

    cp_travel_queue(queue);
    if( cp_rmqueue(queue, tmp) )
    {
        printf("remove and terminate sock [%p] from queue\n", tmp);
        cp_sock_term(tmp);
    }
    cp_travel_queue(queue);

    while(!cp_queue_is_empty(queue))
    {
        if( (sock=cp_dequeue(queue)) )
        {
            printf("Terminate socket: %p\n", sock);
            cp_sock_term(sock);
        }
    }

    cp_queue_destroy(queue);

    return 0;

} /* ----- End of main() ----- */

