/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  drv_adc.c
 *    Description:  
 *                 
 *        Version:  1.0.0(04/01/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "04/01/2012 04:33:22 PM"
 *       Warnning:  Please not enable the S3C2440 common ADC driver in linux kernel 
 *                  when test this driver.
 *                 
 ********************************************************************************/

#include <plat/regs-adc.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/clk.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/miscdevice.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

/* 定义了一个用来保存经过虚拟映射后的内存地址*/
static void __iomem *adc_base;

/* 保存从平台时钟队列中获取ADC的时钟*/
static struct clk *adc_clk;

int dev_id = 1;

/* 申明并初始化一个信号量ADC_LOCK，对ADC资源进行互斥访问*/
DEFINE_MUTEX(ADC_LOCK);

/* 设备名称*/
#define DEVICE_NAME    "adc"

/* 定义并初始化一个等待队列adc_waitq，对ADC资源进行阻塞访问*/
static DECLARE_WAIT_QUEUE_HEAD(adc_waitq);

/* 用于标识AD转换后的数据是否可以读取，0表示不可读取*/
static volatile int ev_adc = 0;

/* 用于保存读取的AD转换后的值，该值在ADC中断中读取*/
static int adc_data;


/* 设置ADC控制寄存器，开启AD转换*/
static void start_adc(void)
{
    unsigned int tmp;

    tmp = (1 << 14) | (255 << 6) | (0 << 3);    /*  0 1 00000011 000 0 0 0 */
    writel(tmp, adc_base + S3C2410_ADCCON); /* AD预分频器使能、模拟输入通道设为AIN0 */

    tmp = readl(adc_base + S3C2410_ADCCON);
    tmp = tmp | (1 << 0);       /*  0 1 00000011 000 0 0 1 */
    writel(tmp, adc_base + S3C2410_ADCCON); /* AD转换开始 */
}


/* ADC中断服务程序，该服务程序主要是从ADC数据寄存器中读取AD转换后的值*/
static irqreturn_t adc_irq(int irq, void *dev_id)
{
    /* 保证了应用程序读取一次这里就读取AD转换的值一次，
     * 避免应用程序读取一次后发生多次中断多次读取AD转换值*/
    if (!ev_adc)
    {
        /* 读取AD转换后的值保存到全局变量adc_data中，S3C2410_ADCDAT0定义在regs-adc.h中，
         * 这里为什么要与上一个0x3ff，很简单，因为AD转换后的数据是保存在ADCDAT0的第0-9位，
         * 所以与上0x3ff(即：1111111111)后就得到第0-9位的数据，多余的位就都为0*/
        adc_data = readl(adc_base + S3C2410_ADCDAT0) & 0x3ff;

        /* 将可读标识为1，并唤醒等待队列 */
        ev_adc = 1;
        wake_up_interruptible(&adc_waitq);
    }

    return IRQ_HANDLED;
}


/* ADC设备驱动的打开接口函数*/
static int adc_open(struct inode *inode, struct file *file)
{
    return 0;
}

/* ADC设备驱动的读接口函数*/
static ssize_t adc_read(struct file *filp, char *buffer, size_t count, loff_t * ppos)
{
    /* 试着获取信号量(即：加锁) */
    mutex_lock(&ADC_LOCK);

    if (!ev_adc)  /* 表示还没有AD转换后的数据，不可读取 */
    {
        if (filp->f_flags & O_NONBLOCK)
        {
            /* 应用程序若采用非阻塞方式读取则返回错误 */
            return -EAGAIN;
        }
        else  /* 以阻塞方式进行读取 */
        {
            /* 设置ADC控制寄存器，开启AD转换 */
            start_adc();

            /* 使等待队列进入睡眠 */
            wait_event_interruptible(adc_waitq, ev_adc);
        }
    }

    /* 能到这里就表示已有AD转换后的数据，则标识清0，给下一次读做判断用 */
    ev_adc = 0;

    /* 将读取到的AD转换后的值发往到上层应用程序 */
    copy_to_user(buffer, (char *)&adc_data, sizeof(adc_data));

    /* 释放获取的信号量(即：解锁) */
    mutex_unlock(&ADC_LOCK);

    return sizeof(adc_data);
}

/* ADC设备驱动的关闭接口函数*/
static int adc_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* 字符设备的相关操作实现*/
static struct file_operations adc_fops = {
    .owner = THIS_MODULE,
    .open = adc_open,
    .read = adc_read,
    .release = adc_release,
};

/* misc设备结构体实现*/
static struct miscdevice adc_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,    /* 次设备号，定义在miscdevice.h中，为255 */
    .name = DEVICE_NAME,        /* 设备名称 */
    .fops = &adc_fops,          /* 对ADC设备文件操作 */
};

static int __init adc_init(void)
{
    int ret;

    /* 申请ADC中断服务，这里使用的是共享中断:IRQF_SHARED,为什么要使用共享中断，因为在触摸屏驱动中
     * 也使用了这个中断号。中断服务程序为:adc_irq在下面实现，IRQ_ADC是ADC的中断号，这里注意：
     * 申请中断函数的最后一个参数一定不能为NULL，否则中断申请会失败，如果中断服务程序中用不到这个
     * 参数，就随便给个值就好了，我这里就给个1*/
    ret = request_irq(IRQ_ADC, adc_irq, IRQF_SHARED, DEVICE_NAME, &dev_id);
    if (ret)
    {
        /* 错误处理 */
        printk(KERN_ERR "IRQ%d error %d\n", IRQ_ADC, ret);
        return -EINVAL;
    }

    /* 将ADC的IO端口占用的这段IO空间映射到内存的虚拟地址，ioremap定义在io.h中。
     * 注意：IO空间要映射后才能使用，以后对虚拟地址的操作就是对IO空间的操作,
     * S3C2410_PA_ADC是ADC控制器的基地址，定义在mach-s3c2410/include/mach/map.h中，0x20是虚拟地址长度大小*/
    adc_base = ioremap(S3C2410_PA_ADC, 0x20);
    if (adc_base == NULL)
    {
        /* 错误处理 */
        printk(KERN_ERR "Failed to remap register block\n");
        ret = -EINVAL;
        goto err_nomap;
    }

    /* 从平台时钟队列中获取ADC的时钟，这里为什么要取得这个时钟，因为ADC的转换频率跟时钟有关。
     *     系统的一些时钟定义在arch/arm/plat-s3c24xx/s3c2410-clock.c中*/
    adc_clk = clk_get(NULL, "adc");
    if (!adc_clk)
    {
        /* 错误处理 */
        printk(KERN_ERR "failed to find adc clock source\n");
        goto err_irq;
        return -ENOENT;
    }

    /* 时钟获取后要使能后才可以使用，clk_enable定义在arch/arm/plat-s3c/clock.c中 */
    clk_enable(adc_clk);

    /* 把看ADC注册成为misc设备，misc_register定义在miscdevice.h中
     *      adc_miscdev结构体定义及内部接口函数在第②步中讲,MISC_DYNAMIC_MINOR是次设备号，定义在miscdevice.h中*/
    ret = misc_register(&adc_miscdev);
    if (ret)
    {
        /* 错误处理 */
        printk(KERN_ERR "cannot register miscdev on minor=%d (%d)\n", MISC_DYNAMIC_MINOR, ret);
        goto err_noclk;
    }

    printk(DEVICE_NAME " initialized!\n");

    return 0;

//以下是上面错误处理的跳转点
  err_nomap:
    iounmap(adc_base);

  err_noclk:
    clk_disable(adc_clk);
    clk_put(adc_clk);

  err_irq:
    free_irq(IRQ_ADC, &dev_id); /* 释放中断 */

    return ret;
}

static void __exit adc_exit(void)
{
    free_irq(IRQ_ADC, &dev_id); /* 释放中断 */
    iounmap(adc_base);          /* 释放虚拟地址映射空间 */

    if (adc_clk)                /* 屏蔽和销毁时钟 */
    {
        clk_disable(adc_clk);
        clk_put(adc_clk);
        adc_clk = NULL;
    }

    misc_deregister(&adc_miscdev);  /* 注销misc设备 */
}

module_init(adc_init);
module_exit(adc_exit);
MODULE_LICENSE("GPL");
