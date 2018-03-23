/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue <guowenxue@gmail.com> 
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file used to do ioctl() on common device or communicate 
 *                  with serial port/TTY device.
 *                 
 *        Version:  1.0.0(10/18/2011~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/18/2011 10:08:05 AM"
 *                 
 ********************************************************************************/
#include <getopt.h>
#include <libgen.h>
#include <sys/ioctl.h>

#include "cp_comport.h"
#include "cp_common.h"
#include "version.h"

unsigned char g_ucProcToken = 0x01;
unsigned char g_ucCtrlZ;

void print_version(char *name);
void usage(char *name);
int do_ioctl(char *dev_name, int cmd, int arg);
void signal_handler(int i_sig);

int main(int argc, char **argv)
{
    int opt = 0;
    int retval = 0;
    int recv_size = 0;
    int i;
    char *dev_name = NULL;
    int baudrate = 115200;
    char *settings = "8N1N";
    char buf[512];
    unsigned char disp_mode = 0x00;

    struct sigaction sigact;

    struct option long_options[] = {
        {"device", required_argument, NULL, 'd'},
        {"baudrate", required_argument, NULL, 'b'},
        {"settings", required_argument, NULL, 's'},
        {"ioctl", required_argument, NULL, 'i'},
        {"hex", no_argument, NULL, 'x'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "d:b:s:ivh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
          case 'd':
              dev_name = optarg;
              break;
          case 'b':
              baudrate = atoi(optarg);
              break;
          case 's':            /* Default settings as 8N1N */
              settings = optarg;
              break;
          case 'i':
              if (5 != argc)
              {
                  usage(argv[0]);
              }
              else
              {
                  do_ioctl(argv[2], atoi(argv[3]), atoi(argv[4]));
              }
              return 0;
          case 'x':            /* Display receive data as Hex mode */
              disp_mode = 0x01;
              break;
          case 'v':            /* version */
              print_version(argv[0]);
              return 0;
          case 'h':            /* help */
              usage(argv[0]);
              return 0;
          default:
              break;
        }                       /* end of "switch(opt)" */
    }

    if (argc < 2)
    {
        usage(argv[0]);
        return 0;
    }

    COM_PORT *comport = NULL;
    if (NULL == (comport = comport_init(dev_name, baudrate, settings)))
    {
        printf("Comport initialize failure.\n");
        return -1;
    }

    if ( (retval=comport_open(comport)) < 0)
    {
        printf("Failed to open %s with baudrate %d, %s. RetCode [%d]\n", dev_name, baudrate,
               settings, retval);
        return -1;
    }

    nonblock();

    /* Process level signal handler */
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = signal_handler;

    sigaction(SIGTERM, &sigact, NULL);  /* catch terminate signal */
    sigaction(SIGINT, &sigact, NULL);   /* catch interrupt signal */
    sigaction(SIGSEGV, &sigact, NULL);  /* catch segmentation faults */
    sigaction(SIGTSTP, &sigact, NULL);  /* catch ctrl+Z */
    sigaction(SIGSTOP, &sigact, NULL);  /* catch ctrl+Z */

    while (0x01 == g_ucProcToken)
    {
        recv_size = comport_recv(comport, buf, sizeof(buf) - 1, 10);
        if (recv_size > 0)
        {
            for (i = 0; i < recv_size; i++)
            {
                if (0 == disp_mode)
                    printf("%c", buf[i]);
                else
                    printf("%02X ", buf[i]);
            }
            fflush(stdout);
        }
        if (0 != kbhit())
        {
            retval = fgetc(stdin);

            if (0x0A == retval)
            {
                buf[0] = 0x0D; /* 13 == 0x0D */
            }
            else
            {
                buf[0] = retval;
            }

            comport_send(comport, buf, 1);
        }
        else if (0x00 != g_ucCtrlZ)
        {
            g_ucCtrlZ = 0x00;
            buf[0] = 0x1A;
            comport_send(comport, buf, 1);
        }
    }

    comport_term(comport);
    return 0;
}                               /* ----- End of main() ----- */

void print_version(char *name)
{
    char *progname = NULL;
    char *ptr = NULL;

    ptr = strdup(name);
    progname = basename(ptr);

    printf("%s version: %d.%d.%d Build %04d on %s\n", progname, MAJOR, MINOR, REVER, SVNVER, DATE);
    printf("Copyright (C) 2010 guowenxue <guowenxue@gmail.com>\n");

    t_free(ptr);
    return;
}

void usage(char *name)
{
    char *progname = NULL;
    char *ptr = NULL;

    ptr = strdup(name);
    progname = basename(ptr);
    printf("Usage1: comport -d <device> [-b <baudrate>][-s <settings>] [-x]\n");
    printf("Usage2: comport [-i <driver port> <cmd> <arg>][--help][--version]\n");
    printf(" -d[device  ]  device name\n");
    printf(" -b[baudrate]  device baudrate (115200, 57600, 19200, 9600), default is 115200\n");
    printf(" -s[settings]  device settings as like 8N1N(default setting)\n");
    printf("                 - data bits: 8, 7\n");
    printf("                 - parity: N=None, O=Odd, E=Even, S=Space\n");
    printf("                 - stop bits: 1, 0\n");
    printf("                 - flow control: N=None, H=Hardware, S=Software, B=Both\n");
    printf(" -x[hex     ]  display received data in hex format\n");
    printf(" -i[ioctl   ]  ioctl system call (cmd & arg only support int)\n");
    printf(" -v[version ]  Display the program version\n");
    printf(" -h[help    ]  Display this help information\n");

    print_version(progname);

    t_free(ptr);

    return;
}

int do_ioctl(char *dev_name, int cmd, int arg)
{
    int fd = -1;
    int retval = -1;
    if (((fd = open(dev_name, O_RDWR)) < 0))
    {
        printf("Open device \"%s\" failure: %s\n", dev_name, strerror(errno));
        return -1;
    }

    retval = ioctl(fd, cmd, arg);
    printf("ioctl (%s, %d, %d) returned %d\n", dev_name, cmd, arg, retval);

    close(fd);
    return retval;
}

void signal_handler(int i_sig)
{
    if (SIGTERM == i_sig || SIGINT == i_sig)
    {
        g_ucProcToken = 0x00;
    }
    else if (20 == i_sig)
    {
        g_ucCtrlZ = 0x01;
    }
}


