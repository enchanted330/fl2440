/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_string.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/27/2012 01:28:39 PM"
 *                 
 ********************************************************************************/

#include <cp_string.h>


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int            i;
    cp_string      *rcv;
    cp_string      *snd;

    rcv = cp_string_create_empty(64);
    snd = cp_string_create_empty(64);

    printf("=======================\n");
    printf("Test cp_string_copy \n");
    printf("=======================\n");
    cp_string_clear_data(rcv);
    cp_string_cstrcpy(rcv, "Hello world!");
    printf("Receive buffer data:\n");
    cp_string_dump(rcv);

    for(i=0; i<20; i++) 
    {
        if( cp_string_copy(snd, rcv) > 0)
        {
            printf("[%d] Send buffer data:\n", i);
            cp_string_dump(snd);
        }
    }

    printf("\n=======================\n");
    printf("Test cp_string_move \n");
    printf("=======================\n");

    cp_string_clear_data(snd);
    cp_string_clear_data(rcv);
    for(i=0; i<20; i++) 
    {
        cp_string_cstrcpy(rcv, "Hello world!");
        //printf("Set new receive buffer data [%d] bytes:\n", cp_string_len(rcv));
        cp_string_dump(rcv);

        if( cp_string_move(snd, rcv) > 0)
        {
            printf("[%d] Send buffer [%d] bytes data:\n", i, cp_string_len(snd));
            cp_string_dump(snd);
            
            printf("[%d] receive buffer [%d] bytes data:\n", i, cp_string_len(rcv));
            cp_string_dump(rcv);
        }
    }

    cp_string_destroy(rcv);
    cp_string_destroy(snd);


    return 0;
} /* ----- End of main() ----- */

