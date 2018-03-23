/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  hello.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/16/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "03/16/2013 10:50:26 AM"
 *                 
 ********************************************************************************/

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static __init int hello_init(void)
{
    printk(KERN_ALERT "hello world!\n");
    return 0;
}

static __exit void hello_exit(void)
{
    printk(KERN_ALERT "goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);


