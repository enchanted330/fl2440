/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  func_pointer.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/21/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/21/2012 10:09:34 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>

int test(int count, char *string)
{
    int i;

    for(i=0; i<count; i++)
        printf("%s\n", string);

    return 0;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int (*p)(int, char *) = test;

    p(3, "Hello");

    return 0;
} /* ----- End of main() ----- */


