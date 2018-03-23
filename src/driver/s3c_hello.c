/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  s3c_hello.c
 *    Description:  This file is just to demonstrat how to write a Linux module.
 *                 
 *        Version:  1.0.0(07/26/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/26/2012 10:04:50 PM"
 *                 
 ********************************************************************************/


#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define MOD_AUTHOR                "GuoWenxue <guowenxue@gmail.com QQ:281143292>"
#define MOD_DESC                  "It's just a Linux kernel module sample!"


static int __init hello_init(void)
{
    printk("Welcome to Lingyun Embedded System Trainning!\n");
    printk("Official Website:  http://mcuos.com/thread-7178-1-1.html\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk("Wow! It's very good and I have learn much from it, thanks!\n");
    return ;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESC);
MODULE_LICENSE("GPL");

