/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/08/2011~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/08/2011 04:25:37 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <libgen.h>
#include <version.h>


void print_version (char *name);
void usage(char *name);
extern int test(void);

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                   opt = 0;  /* used by getopt_long*/
    struct option long_options[] =
    { 
        {"foreground", no_argument,       NULL, 'f'},
        {"debug",      required_argument, NULL, 'd'},
        {"version",    no_argument,       NULL, 'v'},
        {"help",       no_argument,       NULL, 'h'},
        {NULL,         0,                 NULL,  0 }
    };

    while ((opt = getopt_long(argc, argv, "fd:vh", long_options, NULL)) != -1)
    {
        switch(opt)
        {
           case 'f':
                printf("Running program on front.\n");
                break;
           case 'd':       /* debug*/
                printf("Running program on debug mode.\n");
                break;
           case 'v':        /* version*/
                banner(basename(argv[0]));
                return   EXIT_SUCCESS;
           case 'h':         /* help*/
                usage(argv[0]); 
                return 0;
           default:
                break;
        }/* end of "switch(opt)"*/
    }

    test();

    banner(basename(argv[0]));
    return 0;
} /* ----- End of main() ----- */



void  usage(char *name)
{
 char             *progname = NULL;
 char             *ptr = NULL;

 ptr = strdup(name);
 progname = (char *)basename(ptr);

 printf("Usage: %s [OPTION]...\n", progname);
 printf(" %s is a daemon program running as a deamon program on an embedded device.\n", progname);

 printf("Mandatory arguments to long options are mandatory for short options too:\n");
 printf("  -f, --foreground       Run this program on foreground.\n");
 printf("  -d, --debug=[1,2,3,4]  Display debug messages, log level specified by the argument:\n");
 printf("                          0[Disable] 1[Critcal] 2[Error] 3[debug] 4[Info]\n");
 printf("  -v, --version          Display the program version number\n");
 printf("  -h, --help             Display this usage information\n");

 banner(progname);

 free(ptr);

 exit(0);
}
