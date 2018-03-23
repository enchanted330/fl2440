/*********************************************************************************
 *      Copyright:  (C) 2016 Guo Wenxue<guowenxue@aliyun.com>
 *                  All rights reserved.
 *
 *       Filename:  kbd_driver.c
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
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <mach/hardware.h>
#include <asm/gpio.h>
#include <asm/irq.h>
#include <linux/slab.h>
#include <mach/regs-gpio.h>

#include "kbd_driver.h"

/* 1HZ=100*jiffies 1*jiffies=10ms => 1HZ=100*10ms = 1s */
#define CANCEL_DITHERING_DELAY          (HZ/50)   /* Remove button push down dithering timer delay 20ms  */ 

typedef struct s3c_kbd_s 
{
    struct timer_list           *timers; /* every key get a cancel dithering timer */
    struct input_dev            *input_dev;
    s3c_kbd_platform_data_t     *pdata;
} s3c_kbd_t;  /*---  end of struct s3c_kbd_s  ---*/


s3c_kbd_t                       *s3c_kbd = NULL;

static irqreturn_t s3c_kbd_intterupt(int irq, void *dev_id) 
{
    int                         i;
    int                         found = 0;
    struct platform_device      *pdev =  dev_id;
    s3c_kbd_t                   *s3c_kbd = NULL;
    
    s3c_kbd = platform_get_drvdata(pdev);

    for(i=0; i<s3c_kbd->pdata->nkeys; i++)
    {
        if(irq == s3c_kbd->pdata->keys[i].nIRQ)
        {
            found = 1;
            break;
        }
    }

    if(!found) /*  An ERROR interrupt */
        return IRQ_NONE;

    mod_timer(&s3c_kbd->timers[i], jiffies+CANCEL_DITHERING_DELAY); 
    return IRQ_HANDLED;  
}

static void  cancel_dithering_timer_handler(unsigned long data)
{
    int                      which =(int)data;
    unsigned int             pinval;  

    pinval = s3c2410_gpio_getpin(s3c_kbd->pdata->keys[which].gpio);

    if( pinval ) 
    {
        //printk("s3c_kbd key[%d] code[%d] released\n", which, s3c_kbd->pdata->keys[which].code);
        input_event(s3c_kbd->input_dev, EV_KEY, s3c_kbd->pdata->keys[which].code, 0);  
    }
    else
    {
        //printk("s3c_kbd key[%d] code[%d] pressed\n", which, s3c_kbd->pdata->keys[which].code);
        input_event(s3c_kbd->input_dev, EV_KEY, s3c_kbd->pdata->keys[which].code, 1);  
    }
    
    input_sync(s3c_kbd->input_dev);
}

static int s3c_kbd_probe(struct platform_device *pdev)
{
    int                         i = 0;
    int                         rv = -ENOMEM;
    struct input_dev            *input_dev = NULL;
    s3c_kbd_platform_data_t     *pdata = pdev->dev.platform_data;

    /* malloc s3c_kbd struct */
    s3c_kbd = kmalloc(sizeof(s3c_kbd_t), GFP_KERNEL);
    if( !s3c_kbd )
    {
        printk("error: s3c_kbd_probe kmalloc() for s3c_kbd failure\n");
        goto fail;
    }
    memset(s3c_kbd, 0, sizeof(s3c_kbd_t));

    /* malloc cancel dithering timer for every key */
    s3c_kbd->timers = (struct timer_list *) kmalloc(pdata->nkeys*sizeof(struct timer_list), GFP_KERNEL);
    if( !s3c_kbd->timers )
    {
        printk("error: s3c_kbd_probe kmalloc() for s3c_kbd timers failure\n");
        goto fail;
    }
    memset(s3c_kbd->timers, 0, pdata->nkeys*sizeof(struct timer_list));

    /* malloc input_dev for keyboard */
    input_dev=input_allocate_device();
    if( !input_dev )
    {
        printk("error: s3c_kbd_probe input_allocate_device() failure\n");
        goto fail;
    }

    /* setup input_dev  */
    input_dev->name = pdev->name;
    input_dev->dev.parent = &pdev->dev;
    input_dev->id.bustype = BUS_HOST;
    input_dev->id.vendor = 0x0001;
    input_dev->id.product = 0x0001;
    input_dev->id.version = 0x0100;

    set_bit(EV_KEY,input_dev->evbit);  
    set_bit(EV_REP,input_dev->evbit); 

    /* Initialize all the keys and interrupt */
    for(i=0; i<pdata->nkeys; i++)
    {
        set_bit(pdata->keys[i].code, input_dev->keybit);
        s3c2410_gpio_cfgpin(pdata->keys[i].gpio, pdata->keys[i].setting);
        irq_set_irq_type(pdata->keys[i].nIRQ, IRQ_TYPE_EDGE_BOTH);
        
        rv = request_irq(pdata->keys[i].nIRQ, s3c_kbd_intterupt, IRQF_DISABLED, pdev->name, pdev);
        if( rv )
        {
            printk("error: request IRQ[%d] for key<%d> failure\n", pdata->keys[i].nIRQ, i);
            rv = -EBUSY;
            goto fail;
        }
        
        //printk("s3c_kbd request IRQ[%d] for key<%d> ok\n", pdata->keys[i].nIRQ, i);

         /* Initialize all the keys cancel dithering timer */
        setup_timer(&s3c_kbd->timers[i], cancel_dithering_timer_handler, i);
    }

    /* register input device */
    rv = input_register_device(input_dev);
    if( rv )
    {
        printk("error: s3c_kbd_probe input_register_device error!\n");
        goto fail;
    }

    /* set s3c_kbd as private data in pdev */
    s3c_kbd->input_dev = input_dev;
    s3c_kbd->pdata = pdata;
    platform_set_drvdata(pdev, s3c_kbd);

    printk("s3c_kbd_probe ok\n");
    return 0;

fail:
    while(i--)
    {
        disable_irq(pdata->keys[i].nIRQ);
        free_irq(pdata->keys[i].nIRQ, pdev);
        del_timer( &s3c_kbd->timers[i] );
    }

    if(input_dev)
    {
        input_free_device(input_dev);
    }

    if(s3c_kbd && s3c_kbd->timers)
    {
        kfree(s3c_kbd->timers);
    }

    if(s3c_kbd)
    {
        kfree(s3c_kbd);
    }
    printk("s3c_kbd_probe failed\n");

    return -ENODEV;
}

static int s3c_kbd_remove(struct platform_device *pdev)
{
    int                         i = 0;
    s3c_kbd_t                   *s3c_kbd = platform_get_drvdata(pdev);

    for(i=0; i<s3c_kbd->pdata->nkeys; i++)
    {
        del_timer( &s3c_kbd->timers[i] );
        disable_irq(s3c_kbd->pdata->keys[i].nIRQ);
        free_irq(s3c_kbd->pdata->keys[i].nIRQ, pdev);
    }

    input_unregister_device(s3c_kbd->input_dev);

    kfree(s3c_kbd->timers);
    kfree(s3c_kbd);

    printk("s3c_kbd_remove ok\n");

    return 0;
}

static struct platform_driver s3c_keyboard_driver = {
     .probe      = s3c_kbd_probe,
     .remove     = s3c_kbd_remove,
     .driver     = {
         .name       = "s3c_kbd",
         .owner      = THIS_MODULE,
     },
};

static int __init s3c_keyboard_drv_init(void)
{
    int            rv;

    rv = platform_driver_register(&s3c_keyboard_driver);
    if(rv)
    {
        printk("s3c keyboard platform driver register failure\n");
        return rv;
    }

    printk("s3c keyboard platform driver register ok\n");
    return 0;
}

static void __exit s3c_keyboard_drv_exit(void)
{
    printk("s3c keyboard driver exit\n");

    platform_driver_unregister(&s3c_keyboard_driver);
    return ;
}

module_init(s3c_keyboard_drv_init);
module_exit(s3c_keyboard_drv_exit);

MODULE_DESCRIPTION("FL2440 board keyboard input driver platform_driver");
MODULE_AUTHOR("Guo Wenxue<guowenxue@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:FL2440 keyboard driver");

