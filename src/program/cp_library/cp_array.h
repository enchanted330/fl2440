/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_array.h
 *    Description:  This head file is for the dynaic array implement
 *
 *        Version:  1.0.0(12/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/20/2012 01:49:11 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_ARRAY_H
#define __CP_ARRAY_H

#define CP_ARRAY_FLAG_MALLOC     (1<<0)
typedef struct _CP_ARRAY
{
    unsigned char          flag;
    int                    size;
    int                    items;
    void                   **data;
} CP_ARRAY;

#define cp_array_is_full(arr)  ( (arr)->size-1==(arr)->items ? 1 :0 )
#define cp_array_is_empty(arr) ( 0==(arr)->items ? 1 : 0)
#define cp_array_count(arr) ( (arr)->items )
#define cp_array_size(arr) ( (arr)->size-1 )

#define cp_list_array_for_each(arr, i, entry)  for(i=0,entry=arr->data[i]; i<arr->size; ++i,entry=i<arr->size?arr->data[i]:NULL)


CP_ARRAY *cp_array_init(CP_ARRAY *array, int size);
void cp_array_term(CP_ARRAY *array);
int cp_array_add(CP_ARRAY *array, void *data);
void cp_array_rm_byindex(CP_ARRAY *array, int index);
int cp_array_rm_bydata(CP_ARRAY *array, void *data);
void cp_array_travel(CP_ARRAY *array);

#endif /* __CP_ARRAY_H  */
