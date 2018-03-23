/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  ioctl.c
 *    Description:  This file
 *
 *        Version:  1.0.0(11/14/2011~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/14/2011 04:22:35 PM"
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>

void usage(char *program)
{
    printf("---------------------------------------------------------------\n");
    printf("This program used to excute ioctl() system call for debug.\n");
    printf("Usage: %s [dev_name] [cmd] [arg]\n", basename(program));
    printf("Example: %s /dev/led  24609 0\n", basename(program));
    printf("---------------------------------------------------------------\n");
}

int main(int argc, char **argv)
{
    int            fd = -1;
    int            ret = -1;
    char           *dev_name = NULL;
    unsigned int   cmd = 0;
    unsigned long  arg = 0;

    if(4 != argc)
    {
       usage(argv[0]);
       return -1;
    }

    dev_name = argv[1];
    cmd = strtoul(argv[2], NULL, 10);
    arg = strtoul(argv[3], NULL, 10);



    if((fd=open(dev_name, O_RDWR, 0555)) < 0)
    {
        printf("Open \"%s\" failure: %s\n", dev_name, strerror(errno));
        return 0;
    }

    if( (ret=ioctl(fd, cmd, arg)) < 0 )
    {
        printf("ioctl \"%s\" failure: %s\n", dev_name, strerror(errno));
        return 0;
    }

    printf("ioctl \"%s\" return: %d\n", dev_name, ret);

    close(fd);

    return 0;
}
       
