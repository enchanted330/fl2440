/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  linker.c
 *    Description:  It create a linker and revers it. This is the main file.
 *                 
 *        Version:  1.0.0(08/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/07/2012 11:27:46 AM"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include "link_list.h"

int main(int argc, char **argv)
{
      unsigned char       retVal;
      int                 i;
      LINK_NODE           *head = NULL;


      if( NULL == (head=link_create()) )
              return -1;

      for(i=1; i<5; i++)
      {
           retVal = link_insert(head, i, i);
           if( retVal) 
           {
                 printf("Insert node[%d] failure, retVal = 0x%02x\n", i, retVal); 
                 break;
           }
      }
      link_traverse(head);

      link_revers(head);
      printf("After revers...........................................................\n");
      link_traverse(head);

      printf("Destroy the linker now.\n");
      link_destroy(head);

      return 0;
}

