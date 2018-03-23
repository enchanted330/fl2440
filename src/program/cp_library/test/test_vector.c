/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_vector.c
 *    Description:  This file used to test the vector library
 *                 
 *        Version:  1.0.0(08/14/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/14/2012 05:17:03 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cp_vector.h"
#include "cp_sock.h"

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{

    int i, n;
    CP_SOCK *p = NULL;

    CP_VECTOR *v = cp_vector_init(1024);

    for (i = 0; i < 10; i++)
    {
        p = cp_sock_init(NULL, 1024, 1024, 10, 30);
        cp_vector_add(v, i, p);
    }

    n = cp_vector_count(v);
    for (i = 0; i < n; i++)
    {
        p = cp_vector_get(v, i);
        printf("%d: %p\n", i, p);
    }

    p = cp_sock_init(NULL, 1024, 1024, 10, 30);
    cp_vector_add(v, 28, p);
    printf("Set 28: %p\n", p);

    for (i=0; i <cp_vector_size(v); i++)
    {
        p = cp_vector_get(v, i);
        if(p)
        {
            printf("Terminate socket %i: %p\n", i, p);
            cp_sock_term(p);
        }
    }

    cp_vector_destroy(v);

    return 0;

} /* ----- End of main() ----- */

