/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/07/2012 10:24:50 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char         buf[64] = "Hello World!";

    printf("buf: %s\n", buf);
    memmove(&buf[0], &buf[6], strlen(buf)-6);
    printf("buf2: %s\n", buf);

    return 0;
} /* ----- End of main() ----- */

