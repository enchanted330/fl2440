/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_ini.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/18/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/18/2012 10:54:09 AM"
 *                 
 ********************************************************************************/

#include "cp_iniparser.h"


#define INI_CONF   "sample.ini"

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    dictionary   *ini;
    FILE         *fp;

    ini=iniparser_load(INI_CONF);

    fp=fopen(INI_CONF, "w+");

    iniparser_set(ini, "section1:key1", "30");


    iniparser_dump(ini, stderr);

    iniparser_dump_ini(ini, fp);


    iniparser_freedict(ini);


    return 0;
} /* ----- End of main() ----- */


