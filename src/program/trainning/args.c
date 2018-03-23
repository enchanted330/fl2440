/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  args.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/27/2012 11:22:19 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int  i;

    printf("argc=%d\n", argc);

    for(i=1; i<argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
        printf("argv[%d][0]: %c\n", i, argv[i][0]);
    }

    return 0;
} /* ----- End of main() ----- */

