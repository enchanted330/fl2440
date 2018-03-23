/*********************************************************************************
 *      Copyright:  (C) 2016 Guo Wenxue<guowenxue@aliyun.com>
 *                  All rights reserved.
 *
 *       Filename:  kbd_device.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/26/2016)
 *         Author:  Guo Wenxue <guowenxue@aliyun.com>
 *      ChangeLog:  1, Release initial version on "07/26/2016 05:01:25 PM"
 *                 
 ********************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <mach/hardware.h>
#include <asm/gpio.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include "kbd_driver.h"

static s3c_kbd_info_t  s3c_kbd_gpios[] = {
    [0] = {
        .code = KEY_1,
        .nIRQ = IRQ_EINT0,
        .gpio = S3C2410_GPF(0),
        .setting = S3C2410_GPF0_EINT0,
    },
    [1] = {
        .code = KEY_2,
        .nIRQ = IRQ_EINT2,
        .gpio = S3C2410_GPF(2),
        .setting = S3C2410_GPF2_EINT2,
    },
    [2] = {
        .code = KEY_3,
        .nIRQ = IRQ_EINT3,
        .gpio = S3C2410_GPF(3),
        .setting = S3C2410_GPF3_EINT3,
    },
    [3] = {
        .code = KEY_4,
        .nIRQ = IRQ_EINT4,
        .gpio = S3C2410_GPF(4),
        .setting = S3C2410_GPF4_EINT4,
    },
};

/* keyboard platform device private data */
static s3c_kbd_platform_data_t s3c_kbd_data = {
    .keys = s3c_kbd_gpios,
    .nkeys = ARRAY_SIZE(s3c_kbd_gpios),
};

static void platform_kbd_release(struct device * dev)
{
        return;
}

static struct platform_device s3c_keyboard_device = {
    .name    = "s3c_kbd",
    .id      = 1,
    .dev     =
    {
        .platform_data = &s3c_kbd_data,
        .release = platform_kbd_release,
    },
};


static int __init s3c_keyboard_dev_init(void)
{
    int            rv;

    rv = platform_device_register(&s3c_keyboard_device);
    if(rv)
    {
        printk("S3C keyboard platform device register failure\n");
        return rv;
    }

    printk("S3C keyboard platform device register ok\n");
    return 0;
}

static void __exit s3c_keyboard_dev_exit(void)
{
    printk("S3C keyboard device exit\n");

    platform_device_unregister(&s3c_keyboard_device);
    return ;
}

module_init(s3c_keyboard_dev_init);
module_exit(s3c_keyboard_dev_exit);

MODULE_DESCRIPTION("FL2440 board keyboard input driver platform_device");
MODULE_AUTHOR("Guo Wenxue<guowenxue@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:FL2440 keyboard device");

