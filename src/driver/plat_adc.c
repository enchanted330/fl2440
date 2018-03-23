/*********************************************************************************
 *      Copyright:  (C) 2011 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  s3c_adc.c
 *    Description:  This is the common adc driver runs on S3C24XX.
 *                 
 *        Version:  1.0.0(04/01/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "04/01/2012 11:39:10 AM"
 *                 
 ********************************************************************************/
#include "s3c_driver.h"
#include <plat/adc.h>
#include <plat/devs.h>

#define DRV_AUTHOR                "Guo Wenxue <guowenxue@gmail.com>"
#define DRV_DESC                  "S3C24XX adc driver"

/* Driver version*/
#define DRV_MAJOR_VER             1
#define DRV_MINOR_VER             0
#define DRV_REVER_VER             0

#define DEV_NAME                  DEV_ADC_NAME

//#define DEV_MAJOR                 DEV_ADC_MAJOR
#ifndef DEV_MAJOR
#define DEV_MAJOR                 0 /*  dynamic major by default */ 
#endif

#define TIMER_TIMEOUT             40

static int debug = DISABLE;
static int dev_major = DEV_MAJOR;
static int dev_minor = 0;


/* ============================ Platform Device part ===============================*/

struct adc_dev                                     
{                                                         
    struct s3c_adc_client *padc_client; 
    char channel; 
    struct cdev                     cdev; 
    struct class                    *dev_class;
}adc_device;

static struct resource s3c_adcdev_resource[] = {
    [0] = { 
        .start = S3C24XX_PA_ADC, 
        .end   = S3C24XX_PA_ADC + S3C24XX_SZ_ADC - 1, 
        .flags = IORESOURCE_MEM, 
    },
};

static void platform_adc_release(struct device * dev)
{
        return;
}

struct platform_device s3c_adc_device = { 
    .name             = "s3c_adc", 
    .id               = -1, 
    .dev              =
    {
        .parent   = &s3c_device_adc.dev, 
        .release  = platform_adc_release,
    },
    .num_resources    = ARRAY_SIZE(s3c_adcdev_resource), 
    .resource         = s3c_adcdev_resource,
};


/* ===================== adc device driver part ===========================*/

static int adc_open(struct inode *inode, struct file *file)
{ 
    return 0;
}


static int adc_release(struct inode *inode, struct file *file)
{ 
    return 0;
}

static int adc_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{ 
    int  adc_value; 
    
    printk("ADC read now.\n");
    adc_value = s3c_adc_read(adc_device.padc_client, adc_device.channel); 
    copy_to_user(buf, &adc_value, sizeof(adc_value)); 
    printk("ADC read OK.\n");
    return sizeof(adc_value);
}

static void print_adc_help(void)
{
    printk("Follow is the ioctl() command for adc driver:\n");
    printk("Enable Driver debug command: %u\n", SET_DRV_DEBUG);
    printk("Get Driver verion  command : %u\n", GET_DRV_VER);
    printk("Set ADC channel command : %u\n", ADC_SET_CHANNEL);
}

/* compatible with kernel version >=2.6.38*/
static long adc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{ 
    switch (cmd)
    {
        case SET_DRV_DEBUG:
            dbg_print("%s driver debug now.\n", DISABLE == arg ? "Disable" : "Enable");
            debug = (0==arg) ? DISABLE : ENABLE;
            break;

        case GET_DRV_VER:
            print_version(DRV_VERSION);
            return DRV_VERSION;

        case ADC_SET_CHANNEL:
            printk("ADC set Channel %lu\n", arg);
            adc_device.channel = arg;
            break;

        default: 
            dbg_print("%s driver don't support ioctl command=%d\n", DEV_NAME, cmd); 
            print_adc_help();
            return -EINVAL;

    }
    return 0;
}


static struct file_operations adc_fops = { 
    .owner = THIS_MODULE, 
    .open = adc_open, 
    .read = adc_read, 
    .release = adc_release, 
    .unlocked_ioctl = adc_ioctl, /* compatible with kernel version >=2.6.38*/
};

static int s3c_adc_probe(struct platform_device *dev)
{
    int result = 0;
    dev_t devno;

    adc_device.padc_client = s3c_adc_register(dev,NULL,NULL,0); 
    adc_device.channel=0;

    /* Alloc the device for driver */
    if (0 != dev_major) 
    { 
        devno = MKDEV(dev_major, dev_minor); 
        result = register_chrdev_region(devno, 1, DEV_NAME); 
    } 
    else 
    { 
        result = alloc_chrdev_region(&devno, dev_minor, 1, DEV_NAME); 
        dev_major = MAJOR(devno); 
    }

    /* Alloc for device major failure */ 
    if (result < 0) 
    { 
        printk("%s driver can't get major %d\n", DEV_NAME, dev_major); 
        return result; 
    }

    /* Initialize button structure and register cdev*/
    memset(&adc_device, 0, sizeof(adc_device));
    cdev_init (&(adc_device.cdev), &adc_fops);
    adc_device.cdev.owner  = THIS_MODULE;

    result = cdev_add (&(adc_device.cdev), devno , 1); 
    if (result) 
    { 
        printk (KERN_NOTICE "error %d add %s device", result, DEV_NAME); 
        goto ERROR; 
    } 
    
    adc_device.dev_class = class_create(THIS_MODULE, DEV_NAME); 
    if(IS_ERR(adc_device.dev_class)) 
    { 
        printk("%s driver create class failture\n",DEV_NAME); 
        result =  -ENOMEM; 
        goto ERROR; 
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)     
    device_create(adc_device.dev_class, NULL, devno, NULL, DEV_NAME);
#else
    device_create (adc_device.dev_class, NULL, devno, DEV_NAME);
#endif

    printk("S3C %s driver version %d.%d.%d initiliazed.\n", DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER, DRV_REVER_VER); 

    return 0;
               

ERROR: 
    printk("S3C %s driver version %d.%d.%d install failure.\n", DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER, DRV_REVER_VER); 
    cdev_del(&(adc_device.cdev)); 

    unregister_chrdev_region(devno, 1); 
    return result;

}

static int s3c_adc_remove(struct platform_device *dev)
{
    dev_t devno = MKDEV(dev_major, dev_minor);

    s3c_adc_release(adc_device.padc_client);

    cdev_del(&(adc_device.cdev)); 
    device_destroy(adc_device.dev_class, devno); 
    class_destroy(adc_device.dev_class); 
    
    unregister_chrdev_region(devno, 1); 
    printk("S3C %s driver removed\n", DEV_NAME);

    return 0;
}


static struct platform_driver s3c_adc_driver = { 
    .probe      = s3c_adc_probe, 
    .remove     = s3c_adc_remove, 
    .driver     = { 
        .name       = "s3c_adc", 
        .owner      = THIS_MODULE, 
    },
};


static int __init s3c_adc_init(void)
{
   int       ret = 0;

   ret = platform_device_register(&s3c_adc_device);
   if(ret)
   {
        printk(KERN_ERR "%s:%d: Can't register platform device %d\n", __FUNCTION__,__LINE__, ret); 
        goto fail_reg_plat_dev;
   }
   dbg_print("Regist S3C adc Platform Device successfully.\n");

   ret = platform_driver_register(&s3c_adc_driver);
   if(ret)
   {
        printk(KERN_ERR "%s:%d: Can't register platform driver %d\n", __FUNCTION__,__LINE__, ret); 
        goto fail_reg_plat_drv;
   }
   dbg_print("Regist S3C adc Platform Driver successfully.\n");

   return 0;

fail_reg_plat_drv:
   platform_driver_unregister(&s3c_adc_driver);
fail_reg_plat_dev:
   return ret;
}


static void s3c_adc_exit(void)
{
    dbg_print("%s():%d remove adc platform drvier\n", __FUNCTION__,__LINE__);
    platform_driver_unregister(&s3c_adc_driver);
    dbg_print("%s():%d remove adc platform device\n", __FUNCTION__,__LINE__);
    platform_device_unregister(&s3c_adc_device);
}

module_init(s3c_adc_init);
module_exit(s3c_adc_exit);

module_param(debug, int, S_IRUGO);
module_param(dev_major, int, S_IRUGO);
module_param(dev_minor, int, S_IRUGO);

MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:S3C24XX_adc");


