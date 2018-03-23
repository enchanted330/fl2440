/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  fstream.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/27/2012 11:52:56 AM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILE_PATH   "file_test.txt"

int main(void)
{
    FILE *fp;

    char buf[64] = {"I love China !\n"};
    char *ptr2 = buf;

    /* Create a file and write some data to it  */
    fp = fopen(FILE_PATH,"w");
    if(!fp)
        return ;

    fwrite(buf, sizeof(char), 64, fp);
    fclose(fp);

    /* Open a file and read the content */
    fp = fopen(FILE_PATH,"r");
    if(fp == NULL)
        return ;

    memset(ptr2, 0, 64);
    fread(ptr2,1,64, fp);

    printf("%s----------\n",ptr2);

    return 0;
}

