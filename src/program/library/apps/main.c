/*********************************************************************************
 *      Copyright:  (C) 2013 fulinux<fulinux@sina.com> 
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(04/27/2013~)
 *         Author:  fulinux <fulinux@sina.com>
 *      ChangeLog:  1, Release initial version on "04/27/2013 01:49:43 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>

#include "module1.h"
#include "module2.h"

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    module_init();

    func1();

    module_exit();

    return 0;
} /* ----- End of main() ----- */


