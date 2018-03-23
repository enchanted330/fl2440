/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_vector.h
 *    Description:  This head file is for dynamic array
 *
 *        Version:  1.0.0(11/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/12/2012 11:20:53 AM"
 *                 
 ********************************************************************************/

#ifndef __CP_VECTOR
#define __CP_VECTOR

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct _CP_VECTOR
{
    int           size;
    int           used;
    void          **mem;
} CP_VECTOR;

#define cp_vector_count(v)  ((v) ? (v)->used : 0)
#define cp_vector_size(v)  ((v) ? (v)->size : 0)

CP_VECTOR *cp_vector_init(int size);
void *cp_vector_add(CP_VECTOR *vector, int index, void *item);
void *cp_vector_del(CP_VECTOR *vector, int index);
void *cp_vector_get(CP_VECTOR *vector, int index);
void cp_vector_destroy(CP_VECTOR *vector);

#endif


