/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  array.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/20/2012 03:10:14 PM"
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
    int arr[3][4] = {{0,1,2,3},{4,5,6,7}, {8,9,10,11}};

    printf("array: %d\n", **arr);
    printf("array+1: %d\n", **(arr+1));
    printf("array[1]: %d\n", *(arr[1]));
    printf("array[1]+1: %d\n", *(arr[1]+1));

    return 0;
} /* ----- End of main() ----- */

