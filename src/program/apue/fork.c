/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  fork.c
 *    Description:  This file used to to create a process and do something.
 *                 
 *        Version:  1.0.0(10/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/12/2012 08:13:19 PM"
 *                 
 ********************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define COUNT   3

void child_prcoess_running(void)
{
    int    i;

    for(i=1; i<COUNT; i++)
    {
        printf("--Child process pid[%d] running [%d].\n", getpid(), i);
        sleep(1);
    }

    return;
}

void parent_prcoess_running(void)
{
    int    i;

    for(i=1; i<COUNT; i++)
    {
        printf("++Parent process pid[%d] running [%d].\n", getpid(), i);
        sleep(1);
    }

    return;
}



/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    pid_t  pid = -1;

    printf("Parent process pid[%d] start running now.\n", getpid());

    if( (pid=fork()) < 0)
    {
        printf("fork() create child process failure: %s\n", strerror(errno));
        return -1;
    }
    else if( 0 == pid)
    {
        child_prcoess_running();
        //return 0;
    }
    else if(pid > 0)
    {
        printf("Create child process pid[%d].\n", pid);
        parent_prcoess_running();
    }

    printf("====Process pid[%d] exit now.\n", getpid());

    return 0;
} /* ----- End of main() ----- */


