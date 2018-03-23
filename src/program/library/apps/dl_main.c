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
#include <dlfcn.h>

#define LIB_VEND1 "../vendor1/libmodul1.so"
#define LIB_VEND2 "../vendor2/libmodul2.so"


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    void *pHandle1 = NULL; 
    void *pHandle2 = NULL; 
    void (*pFunc)(); 

    pHandle1 = dlopen(LIB_VEND1, RTLD_NOW); 
    if(!pHandle1)
    { 
        printf("Can't find %s \n", LIB_VEND1); 
        exit(1); 
    }  

    pHandle2 = dlopen(LIB_VEND2, RTLD_NOW); 
    if(!pHandle2)
    { 
        printf("Can't find %s \n", LIB_VEND2); 
        exit(1); 
    }  

    pFunc = (void (*)())dlsym(pHandle1, "module_init"); 
    if(pFunc) 
        pFunc(); 
    else 
        printf("Can't find function module_init\n");  
    
    pFunc = (void (*)())dlsym(pHandle2, "func1"); 
    if(pFunc) 
        pFunc(); 
    else 
        printf("Can't find function func1\n");  
 

    pFunc = (void (*)())dlsym(pHandle1, "module_exit"); 
    if(pFunc) 
        pFunc(); 
    else 
        printf("Can't find function module_exit\n");  
    


cleanup:
    if(pHandle1)
        dlclose(pHandle1);  

    if(pHandle2)
        dlclose(pHandle2);  

    return 0;
} /* ----- End of main() ----- */


