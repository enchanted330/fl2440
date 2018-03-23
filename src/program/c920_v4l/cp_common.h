/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_common.h
 *    Description:  This head file is for some common definition
 *
 *        Version:  1.0.0(11/13/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/13/2012 01:48:01 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_COMMON_H
#define __CP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({   \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

//#define MEM_LEAK_CHECK

static inline void *_t_malloc(size_t size
#ifdef MEM_LEAK_CHECK
, const char *file, unsigned int line
#endif
        )
{
    void *ptr;
    
    if ((ptr = malloc (size)))
        memset (ptr, 0, size);
#ifdef MEM_LEAK_CHECK
    printf ("MALLOC,0x%p @%s:%u\n", ptr, file, line);
#endif
    return ptr;
}

static inline void _t_free(void *ptr
#ifdef MEM_LEAK_CHECK
, const char *file, unsigned int line
#endif
        )
{
#ifdef MEM_LEAK_CHECK
    printf ("FREE,0x%p @%s:%u\n", ptr, file, line);
#endif
    free(ptr);
}

#ifdef MEM_LEAK_CHECK
#define t_free(p)    if(p){ _t_free(p, __FILE__, __LINE__); p=NULL; }
#define t_malloc(s)  _t_malloc(s, __FILE__, __LINE__)
#else
#define t_free(p)    if(p){ _t_free(p); p=NULL; }
#define t_malloc(s)  _t_malloc(s)
#endif

#endif

