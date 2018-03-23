/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  kbd.c
 *    Description:  This file used to test TK-5 numeric keyboard on ARM
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
#include <fcntl.h>
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
    __u16 code;  /* Key value  */
    __s32 value; /* 1: Pressed  0:Not pressed  2:Always Pressed */
};  
#endif

#define TRUE               1
#define FALSE              0

#define EV_RELEASED        0
#define EV_PRESSED         1
#define EV_REPEAT          2

#define MAX_CNT            20 

#define KEY_NUMLOCK        69

#define BACK_SPACE         0x7F
#define KEYMAP_BASE        0x300

#define KEY_NUMBERS        18

/* Key_map 1 is for numeric input, 0 is for functiona input */
char key_map[2][KEY_NUMBERS][10]={ 
                     {"Ins","End","Down","PgDn","Left","5","Right","Home","Up","PgUp","+","-","*","/","Enter","","Del"},
                     {"0","1","2","3","4","5","6","7","8","9","+","-","*","/","Enter","","."}
                  };

int main(int argc, char **argv)
{
    char                  *kbd_dev = NULL;
    char                  kbd_name[256] = "Unknown";
    int                   kbd_fd = -1;

    const char            *tty_dev ="/dev/tty0";
    int                   tty_fd = -1;
    int                   kbd_mode = -1;

    int                   i, rd;
    int                   key_value = 0;
    int                   size = sizeof (struct input_event);

    unsigned char         num_input = FALSE;
    struct input_event    ev[MAX_CNT]; 


    if(argc != 2)
    {
        printf("Usage: %s [DEVNAME]\n", argv[0]);
        printf("Example: %s /dev/event0\n", argv[0]);
        return 0;
    }

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");

    kbd_dev = argv[1];

    if ((kbd_fd = open(kbd_dev, O_RDONLY)) < 0)
    {
        printf("Open %s failure: %s", kbd_dev, strerror(errno));
        return -1;
    }

    if ((tty_fd = open(tty_dev, O_RDWR)) < 0)
    {
        printf("Open %s failure: %s", tty_dev, strerror(errno));
        return -1;
    }

    if (ioctl(tty_fd, KDGKBMODE, &kbd_mode)) 
    { 
        perror("KDGKBMODE");
        printf("kbd_mode: error reading keyboard mode\n");
        return -1;
    }
    printf ("Keyboard Mode (0x%0x):\n", kbd_mode);

    ioctl (kbd_fd, EVIOCGNAME (sizeof (kbd_name)), kbd_name);
    printf ("Reading data From %s (%s):\n", kbd_dev, kbd_name);


    while (1)
    {
        if ((rd = read (kbd_fd, ev, size*MAX_CNT )) < size)
        {
            printf("Reading data from kbd_fd failure: %s\n", strerror(errno));
            break;
        }

        for(i=0; i<rd/size; i++)
        {
            if(EV_LED==ev[i].type && EV_PRESSED==ev[i].value)
            {
                /*  NumLock LED turns on, it's a numeric value input */
                num_input=TRUE;
                continue;
            }

            if(EV_KEY==ev[i].type && EV_PRESSED==ev[i].value)
            {
                if(KEY_NUMLOCK != ev[i].code)
                { 
                    /* Find the key value, break out*/
                    key_value = ev[i].code; 
                    break;
                }
            }
        }
        
        if(key_value != 0)
        {
            //printf("%s input: %d\n", TRUE==num_input?"Numeric":"Not numeric", key_value );
            struct kbentry ke;

            ke.kb_table = 0;
            ke.kb_index = key_value;

            if (ioctl(tty_fd, KDGKBENT, &ke) < 0) 
            {
                perror("KDGKBENT");
            }
            else
            {
                if (BACK_SPACE == ke.kb_value) 
                {
                    printf("BackSpace\n");
                }
                else
                {
                    printf("%s\n", key_map[num_input][ke.kb_value-KEYMAP_BASE]);
                }
            }

            /* After use it, must clear it here */
            num_input = FALSE;
            key_value = 0;
        }
    }

    return 0;
}
