/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  list.c
 *    Description:  This file used to demonstrate how to use the linux kernel list
 *                 
 *        Version:  1.0.0(06/25/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "06/25/2012 10:27:09 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define BUF_SIZE             1024
#define MAX_CLIENT           1024

typedef struct __SOCKET_INFO
{
    unsigned char            active;
    int                      sockfd;
    int                      events;
    char                     buf[BUF_SIZE]; /*  Data buffer  */
    struct list_head         list;          /*  Linux list head  */
} SOCKET_INFO;

int main(int argc, char **argv)
{
    int                     i;
    SOCKET_INFO             *socket;
    struct list_head        *pos;
    LIST_HEAD               (socket_head);

    for(i=0; i<MAX_CLIENT; i++)
    {
        if( ! (socket = (SOCKET_INFO *)malloc(sizeof(SOCKET_INFO))) )
        {
            printf("Malloc failure.\n");
            goto CleanUp;
        }

        memset(socket, 0, sizeof(SOCKET_INFO));
        socket->sockfd = i+1;
        printf("Add socket: %d\n", socket->sockfd);
        list_add(&(socket->list), &socket_head);
    }
    printf("\n");

    list_for_each(pos, &socket_head)
    {
        socket = list_entry(pos, SOCKET_INFO, list);
        printf("list_for_each list socket: %d\n", socket->sockfd);
    }
    printf("\n");

    list_for_each_entry(socket, &socket_head, list)
    {
        printf("list_for_each_entry socket: %d\n", socket->sockfd);
    }
    printf("\n");

CleanUp:
#if 0
    {
        struct list_head        *ptr;
        list_for_each_entry_reverse(socket, &socket_head, list)
        {
            ptr = socket->list.next;
            list_del(&socket->list);
            printf("Delete socket: %d\n", socket->sockfd);
            free(socket);

            socket = list_entry(ptr, SOCKET_INFO, list);
        }
#else
    {
        SOCKET_INFO            *entry;
        list_for_each_entry_safe(socket, entry, &socket_head, list)
        {
            printf("Delete socket: %d\n", socket->sockfd);
            list_del(&socket->list);
            free(socket);
        }
        printf("\n");
    }
#endif
    
    return 0;
}
