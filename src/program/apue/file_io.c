/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  file_io.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/28/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "09/28/2013 09:54:56 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char   *file_name = NULL;
    int    fd = -1;
    int    i = 0;
    int    len = 0;
    char   buf[1024];

    for(i=0; i<argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    if(argc != 2)
    {
        printf("Usage: %s file_name\n", argv[0]);
        return -1;
    }

    file_name = argv[1];

    if( (fd=open(file_name, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
    {
        perror("Open file failure");
        printf("Create file \"%s\" failure: %d->%s\n", file_name, errno, strerror(errno));
        return 0;
    }

    while( 1 )
    {
        memset(buf, 0, sizeof(buf));
        if( (len=read(STDIN_FILENO, buf, sizeof(buf))) > 0)
        {
            int   ssize;
            while( len > 0)
            {
                if( (ssize=write(fd, buf, len)) < 0 )
                {
                    perror("Open file failure"); 
                    printf("Create file \"%s\" failure: %d->%s\n", file_name, errno, strerror(errno));
                    break;
                }

                len -= ssize;
            }
        } /* if( (len=read(STDIN_FILENO, buf, sizeof(buf))) > 0) */

        if( !strncasecmp(buf, "term", 4) )  
            break;

    } /* while( 1 )  */

    close(fd);

    return 0;
} /* ----- End of main() ----- */

