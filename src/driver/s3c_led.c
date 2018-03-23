/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  s3c_led.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/26/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/26/2012 10:03:40 PM"
 *                 
 ********************************************************************************/

#include <linux/module.h>   /* Every Linux kernel module must include this head */
#include <linux/init.h>     /* Every Linux kernel module must include this head */
#include <linux/kernel.h>   /* printk() */
#include <linux/fs.h>       /* struct fops */
#include <linux/errno.h>    /* error codes */
#include <linux/cdev.h>     /* cdev_alloc()  */
#include <asm/io.h>         /* ioremap()  */
#include <linux/ioport.h>   /* request_mem_region() */

#include <asm/ioctl.h>      /* Linux kernel space head file for macro _IO() to generate ioctl command  */
#ifndef __KERNEL__
#include <sys/ioctl.h>      /* User space head file for macro _IO() to generate ioctl command */
#endif
//#include <linux/printk.h> /* Define log level KERN_DEBUG, no need include here */


#define DRV_AUTHOR                "Guo Wenxue <guowenxue@gmail.com>"
#define DRV_DESC                  "S3C24XX LED driver"

#define DEV_NAME                  "led"
#define LED_NUM                   4

/* Set the LED dev major number */
//#define LED_MAJOR                 79
#ifndef LED_MAJOR
#define LED_MAJOR                 0
#endif

#define DRV_MAJOR_VER             1
#define DRV_MINOR_VER             0
#define DRV_REVER_VER             0

#define DISABLE                   0
#define ENABLE                    1

#define GPIO_INPUT                0x00
#define GPIO_OUTPUT               0x01


#define PLATDRV_MAGIC             0x60
#define LED_OFF                   _IO (PLATDRV_MAGIC, 0x18)
#define LED_ON                    _IO (PLATDRV_MAGIC, 0x19)

#define S3C_GPB_BASE              0x56000010
#define GPBCON_OFFSET             0
#define GPBDAT_OFFSET             4
#define GPBUP_OFFSET              8
#define S3C_GPB_LEN               0x10        /* 0x56000010~0x56000020  */

int led[LED_NUM] = {5,6,8,10};  /* Four LEDs use GPB5,GPB6,GPB8,GPB10 */

static void __iomem *s3c_gpb_membase;


#define s3c_gpio_write(val, reg) __raw_writel((val), (reg)+s3c_gpb_membase)
#define s3c_gpio_read(reg)       __raw_readl((reg)+s3c_gpb_membase)


int dev_count = ARRAY_SIZE(led);
int dev_major = LED_MAJOR;
int dev_minor = 0;
int debug = DISABLE;

static struct cdev      *led_cdev;

static int s3c_hw_init(void)
{
    int          i;
    volatile unsigned long  gpb_con, gpb_dat, gpb_up;

    if(!request_mem_region(S3C_GPB_BASE, S3C_GPB_LEN, "s3c2440 led"))
    {
        return -EBUSY;
    }

    if( !(s3c_gpb_membase=ioremap(S3C_GPB_BASE, S3C_GPB_LEN)) )
    {
        release_mem_region(S3C_GPB_BASE, S3C_GPB_LEN);
        return -ENOMEM;
    }

    for(i=0; i<dev_count; i++)
    {
        /* Set GPBCON register, set correspond GPIO port as input or output mode  */
        gpb_con = s3c_gpio_read(GPBCON_OFFSET);
        gpb_con &= ~(0x3<<(2*led[i]));   /* Clear the currespond LED GPIO configure register */
        gpb_con |= GPIO_OUTPUT<<(2*led[i]); /* Set the currespond LED GPIO as output mode */
        s3c_gpio_write(gpb_con, GPBCON_OFFSET);

        /* Set GPBUP register, set correspond GPIO port pull up resister as enable or disable  */
        gpb_up = s3c_gpio_read(GPBUP_OFFSET);
        //gpb_up &= ~(0x1<<led[i]); /* Enable pull up resister */
        gpb_up |= (0x1<<led[i]);  /* Disable pull up resister */
        s3c_gpio_write(gpb_up, GPBUP_OFFSET);

        /* Set GPBDAT register, set correspond GPIO port power level as high level or low level */
        gpb_dat = s3c_gpio_read(GPBDAT_OFFSET);
        //gpb_dat &= ~(0x1<<led[i]); /* This port set to low level, then turn LED on */
        gpb_dat |= (0x1<<led[i]);  /* This port set to high level, then turn LED off */
        s3c_gpio_write(gpb_dat, GPBDAT_OFFSET);
    }

    return 0;
}


static void turn_led(int which, unsigned int cmd)
{
    volatile unsigned long  gpb_dat;

    gpb_dat = s3c_gpio_read(GPBDAT_OFFSET);

    if(LED_ON == cmd)
    {
        gpb_dat &= ~(0x1<<led[which]); /*  Turn LED On */
    }
    else if(LED_OFF == cmd)
    {
        gpb_dat |= (0x1<<led[which]);  /*  Turn LED off */
    }

    s3c_gpio_write(gpb_dat, GPBDAT_OFFSET);
}

static void s3c_hw_term(void)
{
    int                     i;
    volatile unsigned long  gpb_dat;

    for(i=0; i<dev_count; i++)
    {
        gpb_dat = s3c_gpio_read(GPBDAT_OFFSET);
        gpb_dat |= (0x1<<led[i]);  /* Turn LED off */
        s3c_gpio_write(gpb_dat, GPBDAT_OFFSET);
    }

    release_mem_region(S3C_GPB_BASE, S3C_GPB_LEN);
    iounmap(s3c_gpb_membase);
}


static int led_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);

    file->private_data = (void *)minor;

    printk(KERN_DEBUG "/dev/led%d opened.\n", minor);
    return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "/dev/led%d closed.\n", iminor(inode));

    return 0;
}

static void print_help(void)
{
    printk("Follow is the ioctl() commands for %s driver:\n", DEV_NAME);
    //printk("Enable Driver debug command: %u\n", SET_DRV_DEBUG);
    printk("Turn LED on command  : %u\n", LED_ON);
    printk("Turn LED off command : %u\n", LED_OFF);

    return;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int which = (int)file->private_data;

    switch (cmd)
    {
        case LED_ON:

            turn_led(which, LED_ON);
            break;

        case LED_OFF:
            turn_led(which, LED_OFF);
            break;
    
        default:
            printk(KERN_ERR "%s driver don't support ioctl command=%d\n", DEV_NAME, cmd);
            print_help();
            break;
    }

    return 0;
}


static struct file_operations led_fops = 
{
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .unlocked_ioctl = led_ioctl,
};

static int __init s3c_led_init(void)
{
    int                    result;
    dev_t                  devno;

    if( 0 != s3c_hw_init() )
    {
        printk(KERN_ERR "s3c2440 LED hardware initialize failure.\n");
        return -ENODEV;
    }

    /*  Alloc the device for driver */
    if (0 != dev_major) /*  Static */
    {
        devno = MKDEV(dev_major, 0);
        result = register_chrdev_region (devno, dev_count, DEV_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&devno, dev_minor, dev_count, DEV_NAME);
        dev_major = MAJOR(devno);
    }

    /*  Alloc for device major failure */
    if (result < 0)
    {
        printk(KERN_ERR "S3C %s driver can't use major %d\n", DEV_NAME, dev_major);
        return -ENODEV;
    } 
    printk(KERN_DEBUG "S3C %s driver use major %d\n", DEV_NAME, dev_major);

    if(NULL == (led_cdev=cdev_alloc()) )
    {
        printk(KERN_ERR "S3C %s driver can't alloc for the cdev.\n", DEV_NAME);
        unregister_chrdev_region(devno, dev_count);
        return -ENOMEM;
    }
    
    led_cdev->owner = THIS_MODULE;
    cdev_init(led_cdev, &led_fops);

    result = cdev_add(led_cdev, devno, dev_count);
    if (0 != result)
    {   
        printk(KERN_INFO "S3C %s driver can't reigster cdev: result=%d\n", DEV_NAME, result); 
        goto ERROR;
    }

            
    printk(KERN_ERR "S3C %s driver[major=%d] version %d.%d.%d installed successfully!\n", 
            DEV_NAME, dev_major, DRV_MAJOR_VER, DRV_MINOR_VER,DRV_REVER_VER);
    return 0;


ERROR:
    printk(KERN_ERR "S3C %s driver installed failure.\n", DEV_NAME);
    cdev_del(led_cdev);
    unregister_chrdev_region(devno, dev_count);
    return result;
}

static void __exit s3c_led_exit(void)
{
    dev_t devno = MKDEV(dev_major, dev_minor);

    s3c_hw_term();

    cdev_del(led_cdev);
    unregister_chrdev_region(devno, dev_count);

    printk(KERN_ERR "S3C %s driver version %d.%d.%d removed!\n", 
            DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER,DRV_REVER_VER);

    return ;
}



/* These two functions defined in <linux/init.h> */
module_init(s3c_led_init);
module_exit(s3c_led_exit);

module_param(debug, int, S_IRUGO);
module_param(dev_major, int, S_IRUGO);

MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL");
