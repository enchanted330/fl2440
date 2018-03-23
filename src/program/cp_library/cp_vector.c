/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_vector.c
 *    Description:  This file is for a dynamic array
 *                 
 *        Version:  1.0.0(11/12/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/12/2012 11:20:30 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cp_vector.h"
#include "cp_common.h"

CP_VECTOR *cp_vector_init(int size)
{
    CP_VECTOR  *vector = NULL;
    if(size < 0)
        return NULL;

    vector = (CP_VECTOR *)t_malloc(sizeof(CP_VECTOR));
    if(!vector)
    {
        return NULL;
    }

    vector->mem = t_malloc(size*sizeof(void *));
    if(!vector->mem)
    {
        t_free(vector);
        return NULL;
    }

    memset(vector->mem, 0, size);
    vector->size = size;
    vector->used = 0;

    return vector;
}

void *cp_vector_add(CP_VECTOR *vector, int index, void *item)
{
    if(index<0 || index>vector->size)
        return NULL;

    vector->mem[index]=item;
    vector->used++;

    return item;
}

void *cp_vector_del(CP_VECTOR *vector, int index)
{
    void    *save;

    if(index<0 ||index>vector->size)
        return NULL;

    save = vector->mem[index];

    vector->mem[index]=NULL;
    vector->used--;

    return save;
}

void *cp_vector_get(CP_VECTOR *vector, int index)
{
    if(index<0 ||index>vector->size)
        return NULL;

    return vector->mem[index];
}

void cp_vector_destroy(CP_VECTOR *vector)
{
    if(!vector)
       return;

    if(vector->mem)
    {
        t_free(vector->mem);
    }

    t_free(vector);
}



