/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_array.c
 *    Description:  This file used to test the array library
 *                 
 *        Version:  1.0.0(08/14/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/14/2012 05:17:03 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cp_array.h"
#include "cp_sock.h"
#include "cp_logger.h"

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
    CP_ARRAY *array = NULL;
    
    if (!cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_MAX, LOG_ROLLBACK_NONE) || cp_log_open())
        return 0;

    if( !(array=cp_array_init(NULL, MAX_ITEMS)) )
        return -1;

    for (i=0; i<MAX_ITEMS; i++)
    {
        sock = cp_sock_init(NULL, 1024, 1024, 10, 30);
        if( cp_array_add(array, sock)<0 )
        {
            cp_sock_term(sock);
        }
        else
        {
            if(i==3)
            {
                tmp = sock;
            }
        }
    }

    printf("Array usage %d/%d \n", cp_array_count(array), cp_array_size(array));

    cp_array_travel(array);

    if( !cp_array_rm_bydata(array, tmp) )
    {
        printf("remove and terminate sock [%p] from array\n", tmp);
        cp_sock_term(tmp);
    }
    cp_array_travel(array);

    cp_list_array_for_each(array, i, sock)
    {
        cp_sock_term(sock);
    }

    cp_array_term(array);
    cp_log_term();

    return 0;

} /* ----- End of main() ----- */

