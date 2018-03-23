/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  pointer.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/20/2012 01:43:19 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void get_memory(char **p, int num);

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char *ptr = NULL;

    get_memory(&ptr, 100);

    strcpy(ptr, "hello");

    return 0;
} /* ----- End of main() ----- */

void get_memory(char **p, int num)
{
    *p = (char *)malloc(sizeof(char)*num);
}

char *get_memory2(int num)
{
    char *p;
    p = (char *)malloc(sizeof(char)*num);
    return p;
}

