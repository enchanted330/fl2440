/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  link_list.h
 *    Description:  This is linker create head file
 *
 *        Version:  1.0.0(08/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/07/2012 11:27:46 AM"
 *
 ********************************************************************************/

#ifndef __LINK_LIST_H
#define __LINK_LIST_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


struct __LINK_NODE;

typedef struct __TEST
{
   struct __LINK_NODE          *node;
   int                          data;
} TEST;

typedef struct __LINK_NODE
{
   int                     data;
   struct __LINK_NODE      *next;
   TEST                    *test;
} LINK_NODE;

LINK_NODE    *link_create(void);
unsigned char link_insert(LINK_NODE *head, int pos, int data);
unsigned char link_del(LINK_NODE *head, int pos);
void link_revers(LINK_NODE *head);
void link_destroy(LINK_NODE *head);
void link_traverse(LINK_NODE *head);

#endif
