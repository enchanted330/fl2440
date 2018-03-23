/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  event_button.c
 *    Description:  This file used to test GPIO button driver builtin Linux kernel on ARM board
 *                 
 *        Version:  1.0.0(07/13/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/13/2012 02:46:18 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

#if 0 /* Just for comment here, Reference to linux-3.3/include/linux/input.h */
struct input_event 
{
    struct timeval time;
    __u16 type;  /* 0x00:EV_SYN 0x01:EV_KEY 0x04:EV_MSC 0x11:EV_LED*/
    __u16 code;  /* key value, which key */
    __s32 value; /* 1: Pressed  0:Not pressed  2:Always Pressed */
};  
#endif

#define TRUE               1
#define FALSE              0

#define EV_RELEASED        0
#define EV_PRESSED         1
#define EV_REPEAT          2

#define BUTTON_CNT         5

#define MODE_POLL          0x01
#define MODE_NORMAL        0x02

void usage(char *name);
void display_button_event(struct input_event *ev, int cnt);

int main(int argc, char **argv)
{
    char                  *kbd_dev = NULL;
    char                  kbd_name[256] = "Unknown";
    int                   kbd_fd = -1;

    int                   rv, opt;
    int                   mode = MODE_NORMAL;
    int                   size = sizeof (struct input_event);

    struct input_event    ev[BUTTON_CNT]; 

    struct option long_options[] = {
        {"device", required_argument, NULL, 'd'},
        {"poll", no_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "d:ph", long_options, NULL)) != -1)
    { 
        switch (opt)
        {
            case 'd':
                kbd_dev = optarg;
                break;

            case 'p':
                mode = MODE_POLL;
                break;

            case 'h':
                usage(argv[0]);
                return 0;

            default:
                break;
        }
    }

    if(NULL == kbd_dev)
    {
        usage(argv[0]);
        return -1;
    }

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");


    if ((kbd_fd = open(kbd_dev, O_RDONLY)) < 0)
    {
        printf("Open %s failure: %s", kbd_dev, strerror(errno));
        return -1;
    }

    ioctl (kbd_fd, EVIOCGNAME (sizeof (kbd_name)), kbd_name);
    printf ("Monitor input device %s (%s) event with %s mode:\n", kbd_dev, kbd_name, MODE_POLL==mode?"poll":"infilit loop");


#if 0 /* Not implement in the Linux GPIO button driver */
    unsigned char key_b[BUTTON_CNT/8 + 1];
    memset(key_b, 0, sizeof(key_b));
    if(ioctl(kbd_fd, EVIOCGKEY(sizeof(key_b)), key_b) < 0)
    {
        printf("EVIOCGKEY ioctl get error: %s\n", strerror(errno));
        return -1;
    }
#endif

#if 0 /* Not implement in the Linux GPIO button driver */
    /* rep[0]表示在按键重复出现之前 delay的时间,rep[1]表示按键重复出现的时间间隔。 */
    int rep[2] ={2500, 1000} ;
    if(ioctl(kbd_fd, EVIOCSREP, rep) < 0)
    {
        printf("EVIOCSREP ioctl get error: %s\n", strerror(errno));
        return -1;
    }

    if(ioctl(kbd_fd, EVIOCGREP, rep) < 0)
    {
        printf("EVIOCGKEY ioctl get error: %s\n", strerror(errno));
        return -1;
    }
    else
    {
        printf("repeate speed: [0]= %d, [1] = %d/n", rep[0], rep[1]);
    }
#endif

    while (1)
    {
        if(MODE_POLL==mode)
        {
            fd_set rds;
            FD_ZERO(&rds);
            FD_SET(kbd_fd, &rds);

            rv = select(kbd_fd + 1, &rds, NULL, NULL, NULL);
            if (rv < 0)
            {
                printf("Select() system call failure: %s\n", strerror(errno));
                goto CleanUp;
            }
            else if (FD_ISSET(kbd_fd, &rds))
            { 
                if ((rv = read (kbd_fd, ev, size*BUTTON_CNT )) < size)
                {
                    printf("Reading data from kbd_fd failure: %s\n", strerror(errno));
                    break;
                }
                else
                {
                    display_button_event(ev, rv/size);
                }
            }
        }
        else
        {
            if ((rv = read (kbd_fd, ev, size*BUTTON_CNT )) < size)
            {
                printf("Reading data from kbd_fd failure: %s\n", strerror(errno));
                break;
            }
            else
            {
                display_button_event(ev, rv/size);
            }
        }
    }

CleanUp:
    close(kbd_fd);

    return 0;
}

void usage(char *name)
{
    char *progname = NULL;
    char *ptr = NULL;

    ptr = strdup(name);
    progname = basename(ptr);

    printf("Usage: %s [-p] -d <device>\n", progname);
    printf(" -d[device  ] button device name\n");
    printf(" -p[poll    ] Use poll mode, or default use infinit loop.\n");
    printf(" -h[help    ] Display this help information\n"); 

    free(ptr);

    return;
}

void display_button_event(struct input_event *ev, int cnt)
{
    int i;
    struct timeval        pressed_time, duration_time;

    for(i=0; i<cnt; i++)
    {
        //printf("type:%d code:%d value:%d\n", ev[i].type, ev[i].code, ev[i].value);
        if(EV_KEY==ev[i].type && EV_PRESSED==ev[i].value)
        {
            if(BTN_1 == ev[i].code)
            {
                pressed_time = ev[i].time;
                printf("S1 button key[%d] pressed time: %ld.%ld\n", ev[i].code, pressed_time.tv_sec, pressed_time.tv_usec);
            }
            else if(BTN_2 == ev[i].code)
            {
                pressed_time = ev[i].time;
                printf("S2 button key[%d] pressed time: %ld.%ld\n", ev[i].code, pressed_time.tv_sec, pressed_time.tv_usec);
            }
            else if(BTN_3 == ev[i].code)
            {
                pressed_time = ev[i].time;
                printf("S3 button key[%d] pressed time: %ld.%ld\n", ev[i].code, pressed_time.tv_sec, pressed_time.tv_usec);
            }
            else if(BTN_4 == ev[i].code)
            {
                pressed_time = ev[i].time;
                printf("S4 button key[%d] pressed time: %ld.%ld\n", ev[i].code, pressed_time.tv_sec, pressed_time.tv_usec);
            }
            else
            {
                pressed_time = ev[i].time;
                printf("button key[%d]  pressed time: %ld.%ld\n", ev[i].code, pressed_time.tv_sec, pressed_time.tv_usec);
            }
        }
        if(EV_KEY==ev[i].type && EV_RELEASED==ev[i].value)
        {
            if(BTN_1 == ev[i].code)
            {
                timersub(&ev[i].time, &pressed_time, &duration_time);
                printf("S1 button key[%d] released time: %ld.%ld\n", ev[i].code, ev[i].time.tv_sec, ev[i].time.tv_usec);
                printf("S1 button key[%d] duration time: %ld.%ld\n", ev[i].code, duration_time.tv_sec, duration_time.tv_usec);
            }
            else if(BTN_2 == ev[i].code)
            {
                timersub(&ev[i].time, &pressed_time, &duration_time);
                printf("S2 button key[%d] released time: %ld.%ld\n", ev[i].code, ev[i].time.tv_sec, ev[i].time.tv_usec);
                printf("S2 button key[%d] duration time: %ld.%ld\n", ev[i].code, duration_time.tv_sec, duration_time.tv_usec);
            }
            else if(BTN_3 == ev[i].code)
            {
                timersub(&ev[i].time, &pressed_time, &duration_time);
                printf("S3 button key[%d] released time: %ld.%ld\n", ev[i].code, ev[i].time.tv_sec, ev[i].time.tv_usec);
                printf("S3 button key[%d] duration time: %ld.%ld\n", ev[i].code, duration_time.tv_sec, duration_time.tv_usec);
            }
            else if(BTN_4 == ev[i].code)
            {
                timersub(&ev[i].time, &pressed_time, &duration_time);
                printf("S4 button key[%d] released time: %ld.%ld\n", ev[i].code, ev[i].time.tv_sec, ev[i].time.tv_usec);
                printf("S4 button key[%d] duration time: %ld.%ld\n", ev[i].code, duration_time.tv_sec, duration_time.tv_usec);
            }
            else
            {
                timersub(&ev[i].time, &pressed_time, &duration_time);
                printf("button key[%d] released time: %ld.%ld\n", ev[i].code, ev[i].time.tv_sec, ev[i].time.tv_usec);
                printf("button key[%d] duration time: %ld.%ld\n", ev[i].code, duration_time.tv_sec, duration_time.tv_usec);
            }
        }
    } /*  for(i=0; i<cnt; i++) */
}
