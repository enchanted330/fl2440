/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  const.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/20/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/20/2012 03:45:20 PM"
 *                 
 ********************************************************************************/


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int  v = 10;
    int v2 = 20;

    const int * c_ptr = &v;
    //int const * c_ptr = &v;
    int * const c_p = &v;
    const int * const c_ppr = &v;

    c_ptr = &v2;
    //*c_ptr = 15;
    v = 20;

    //c_p = &v2;
    *c_p = 15;

    //c_ppr = &v2;
    //*c_ppr = 15;


    return 0;
} /* ----- End of main() ----- */

