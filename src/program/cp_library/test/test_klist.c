/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_klist.c
 *    Description:  This file is for test kernel space double linked list.
 *                 
 *        Version:  1.0.0(11/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/12/2012 04:26:39 PM"
 *                 
 ********************************************************************************/

#include "cp_klist.h"
#include "cp_sock.h"

//#define LIST_REVERSE

void travel_list(struct list_head *head)
{
    CP_SOCK             *sock, *tsock;


    if( (sock=cp_sock_init(NULL, CP_SOCK_RCVBUF, CP_SOCK_SNDBUF, CP_SOCK_KEEPINTVL, CP_SOCK_KEEPCNT)) )
    {
        list_add_tail(&sock->rlist, head);
        printf("Add new socket %p to sock_list \n", sock);
    }

    /* Use list_for_each_entry to travel the socket list, we can not remove the node in it */
    list_for_each_entry_safe(sock, tsock, head, rlist)
    {
        printf("Travel2 socket list on socket %p\n", sock);
    }
}


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                 i;
    struct list_head    sock_list;
    CP_SOCK             *sock, *tsock;

    INIT_LIST_HEAD(&sock_list);

    for(i=0; i<10; i++)
    {
        if( (sock=cp_sock_init(NULL, CP_SOCK_RCVBUF, CP_SOCK_SNDBUF, CP_SOCK_KEEPINTVL, CP_SOCK_KEEPCNT)) )
        {
            list_add_tail(&sock->rlist, &sock_list);
            printf("Add socket %p to sock_list \n", sock);
        }
    }

    /* Use list_for_each_entry to travel the socket list, we can not remove the node in it */
    list_for_each_entry(sock, &sock_list, rlist)
    {
        printf("Travel socket list on socket %p\n", sock);
    }

    travel_list(&sock_list);

    /* Use list_for_each_entry_safe to travel the socket list and destroy the socket */
    list_for_each_entry_safe(sock, tsock, &sock_list, rlist)
    {
        list_del(&sock->rlist);
        cp_sock_term(sock);
        printf("Remove and destroy socket %p from socket list\n", sock);
    }

    return 0;
} /* ----- End of main() ----- */

