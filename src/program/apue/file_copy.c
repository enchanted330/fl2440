/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  file_copy.c
 *    Description:  This file used to implement a file copy command, which used to 
 *                  introduce the linux file I/O program.
 *                 
 *        Version:  1.0.0(10/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/12/2012 07:21:32 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SIZE               512

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char  *src, *dst, *buf[BUF_SIZE];
    int   fd_src = -1;
    int   fd_dst = -1;
    int   bytes = -1;
    int   rv = 0;
    struct stat  file_status;

    if(3 != argc) 
    { 
        printf("Usage: %s [SRC] [DST].\n", basename(argv[0])); 
        return -1;
    }

    src = argv[1];
    dst = argv[2];

    printf("Copy \"%s\" to\" %s.\n", src, dst);

    /* Open the source file */
    if((fd_src=open(src, O_RDONLY)) < 0)
    {
        printf("Open file \"%s\" failure: %s\n", src, strerror(errno));
        return -2;
    }

    /* Check the file status  */
    if( -1 == fstat(fd_src, &file_status) )
    {
        printf("fstat on file \"%s\" failure: %s\n", src, strerror(errno));
        rv = -2;
        goto CleanUp;
    }
            
    /* Check the file type  */
    if( S_IFREG != (file_status.st_mode & S_IFMT) )
    {
        printf("File \"%s\" is not a regular file.\n", src);
        rv = -3;
        goto CleanUp;
    }

    printf("File \"%s\" size: %lld bytes.\n", src, (long long)file_status.st_size);


    /* Open the destination file, if it's not exist then create */
    if((fd_dst=open(dst, O_RDWR|O_CREAT|O_TRUNC, 0644)) < 0)
    {
        printf("Open file \"%s\" failure: %s\n", dst, strerror(errno));
        rv = -4;
        goto CleanUp;
    }
    
    /* Read the data from the source file and write to the destination file  */
    while( ((bytes=read(fd_src, buf, sizeof(buf))) > 0) )
    {
        if(write(fd_dst, buf, bytes) != bytes)
        {
            printf("Write data to destination file failure: %s\n", strerror(errno));
            rv = -5;
            goto CleanUp;
        }
    }

    //unlink("version.h");   /* Remove this file  */
    //rename(dst, "version.h"); /* Rename the file name */

CleanUp:
    if(fd_src > 0)
        close(fd_src);

    if(fd_dst > 0)
        close(fd_dst);

    return 0;
} /* ----- End of main() ----- */


