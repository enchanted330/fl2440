/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  layout.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/21/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/21/2012 09:46:06 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int a = 0;
static int c ;
char *p1;
const int A=10;

char arr[10]={'\0'};

void main(void)
{
    int b;
    char s[] = "hello";

    char *p2;
    static int c=0;
    char *p3 = "Hello";

    char *buffer = malloc(100);
    char buff[10];

    p1=(char *)malloc(10);
    p2=(char *)malloc(20);

    strcpy(p1, "12345");

    buffer[1] = 'A';
    *(buffer+1) = 'B';

    p3[1] = 'X';

    p3 = s;
    p3[1] = 'x';

    {
        char const *ptr1 = arr;
        char *const ptr2 = arr; 
        char const *const ptr3;

        *(ptr1+1) = '3';
        ptr1[1] = '3';
        ptr1 = s;

        *(ptr2+1) = '4';
        ptr2 = s; 

        *(ptr3+1) = '4';
        ptr3 = s;
    }

    printf("%d\n", add(4));
    printf("%d\n", add(4));

    return ;
}

static int m = 3;

int add(int c)
{
    int k;
    static int t;

    t++;

    k = t+c + m; 

    return k;
}


