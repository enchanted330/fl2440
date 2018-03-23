/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  goodbye.c
 *    Description:  This file just a test source code
 *                 
 *        Version:  1.0.0(11/11/2011~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/11/2011 01:55:59 PM"
 *                 
 ********************************************************************************/


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
#include <stdio.h>
#include <libgen.h>
#include "version.h"

int main (int argc, char **argv)
{
    printf("Goodby cruel world!\n");

    banner(basename(argv[0]));
    return 0;
} /* ----- End of main() ----- */

