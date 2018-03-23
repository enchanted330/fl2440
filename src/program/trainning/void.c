/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  void.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/27/2012 10:38:38 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _student
{
    char *name;
    int  sn;
} student, *pstudent;

void print_student(void *data);

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    student st[4] = 
    {
        {.name="guowenxue", .sn=10},
        {.name="liuyong", .sn=11},
        {.name="kanglu", .sn=12},
        {.name=NULL, .sn=0},
    };

    printf("name:%s SN:%d\n", st[0].name, st[0].sn);

    print_student((void *)st);
    return 0;
} /* ----- End of main() ----- */


void print_student(void *data)
{
    pstudent pst = (student *)data;

    for( ; NULL!=pst->name; pst++)
        printf("name:%s SN:%d\n", pst->name, pst->sn);
}
