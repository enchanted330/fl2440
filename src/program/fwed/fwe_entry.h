/********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  fwe_entry.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(01/13/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/13/2013 02:53:41 PM"
 *                 
 ********************************************************************************/

#ifndef __FWE_ENTRY_H_
#define __FWE_ENTRY_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

#include "version.h"

#define FWE_ENTRY_NAME "fwed"

typedef int (APPLET_MAIN)(int argc, char **argv);

typedef struct __FWE_APPLET
{
    char             *name; 
    APPLET_MAIN      *main;
} FWE_APPLET;



#endif /* end of __FWE_ENTRY_H_  */
