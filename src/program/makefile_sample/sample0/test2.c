/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test2.c
 *    Description:  This file is test function, which just display something.
 *                 
 *        Version:  1.0.0(10/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/12/2012 06:00:30 PM"
 *                 
 ********************************************************************************/
#include <stdio.h>

int test2(void)
{
    printf("[%s:%d FUNC:%s] Goodbye World!\n", __FILE__, __LINE__, __FUNCTION__);
    return 0;
}
