/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  pointer3.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/20/2012 02:21:40 PM"
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
    char var = 'A' ;
    char var2 = 'T';

    char *p = &var;
    char **pp = &p;


    *p = 'B';
    printf("Value: %c %c %c\n", var, *p, **pp);

    **pp = 'C';
    printf("Value: %c %c %c\n", var, *p, **pp);

    *pp = &var2;
    printf("Value: %c %c %c\n", var, *p, **pp);


    return 0;
} /* ----- End of main() ----- */

