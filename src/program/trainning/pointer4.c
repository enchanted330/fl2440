/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  pointer4.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/21/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/21/2012 09:29:02 AM"
 *                 
 ********************************************************************************/


void clear_buffer(char *buffer, int size);

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char *ptr = malloc(100);


    clear_buffer(ptr);

    return 0;
} /* ----- End of main() ----- */


void clear_buffer(char *buffer, int size)
{
    memset(buffer, 0, size);
    free(buffer);
}
