/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_array.c
 *    Description:  This file is a dynamic array implement
 *                 
 *        Version:  1.0.0(12/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/20/2012 01:48:27 PM"
 *                 
 ********************************************************************************/

#include <stdlib.h>
#include "cp_array.h"
#include "cp_common.h"
#include "cp_logger.h"

CP_ARRAY *cp_array_init(CP_ARRAY *array, int size)
{
    int        i;

    if( !array )
    {
        if( !(array=t_malloc(sizeof(*array))) )
        {
            return NULL;
        }
        else
        {
            memset(array, 0, sizeof(*array));
            array->flag |= CP_ARRAY_FLAG_MALLOC; 
        }
    }
    else
    {
        /* array is a variable,so clear it */
        memset(array, 0, sizeof(*array));
    }

    array->size = size;
    if( !array->data && !(array->data=(void **)malloc(array->size*sizeof(void *))) )
    {
        cp_array_term(array);
        return NULL;
    }

    for(i=0; i<array->size; i++)
    {
        array->data[i] = NULL;
    }

    return array;
}

void cp_array_term(CP_ARRAY *array)
{
    if(!array)
        return; 

    if(array->data)
        t_free(array->data);

    if(array->flag&CP_ARRAY_FLAG_MALLOC)
    {
        t_free(array);
    }

    return ;
}


int cp_array_add(CP_ARRAY *array, void *data)
{
    int              i;
    void             *entry;

    if(!array || !data)
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    /* array already full */
    if(array->items >= array->size)
    {
        log_err("array is full,can not add data [%p]\n", data);
        return -2;
    }
    
    /* We don't start the array from 0 but 1 */
    cp_list_array_for_each(array, i, entry)
    {
        if( !entry )
        {
            log_dbg("Array[%d] add data[%p] ok\n", i, data);
            array->data[i] = data; 
            array->items ++;
            break;
        }
    }

    return i;
}

void cp_array_rm_byindex(CP_ARRAY *array, int index)
{
    /* We don't start the array from 0 but 1 */
    if(!array || index<0)
    {
        log_err("Invalude input arguments\n");
        return;
    }

    if(array->data[index])
    { 
        array->items --;
        array->data[index] = NULL;
    }
}

int cp_array_rm_bydata(CP_ARRAY *array, void *data)
{
    int              i, rv = -3;
    void             *entry;

    if(!array || !data)
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    if(array->items <= 0)
    {
        log_err("array is empty,can not remove data [%p]\n", data);
        return -2;
    }

    /* We don't start the array from 0 but 1 */
    cp_list_array_for_each(array, i, entry)
    {
        if( entry == data )
        {
            array->items --;
            array->data[i] = NULL;
            rv = 0;
            break;
        }
    }

    return rv;
}

void cp_array_travel(CP_ARRAY *array)
{
    int              i;
    void             *data;

    /* We don't start the array from 0 but 1 */
    cp_list_array_for_each(array, i, data)
    {
        log_dbg("array data[%d] save data [%p]\n", i, data);
    }

    return ;
}


