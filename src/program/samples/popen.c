/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  popen.c
 *    Description:  
 *                 
 *        Version:  1.0.0(07/13/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/13/2012 02:46:18 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    char           cmd[64];
    char           buf[512];
    FILE           *fp;

    if(argc != 2)
    {
        printf("%s Ipaddr\n", argv[0]); 
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "/bin/netstat -ant | grep %s", argv[1]);
    printf("popen() excute: %s\n", cmd);

    if(NULL == (fp=popen(cmd, "r")) )
    {
          perror("poen"); 
          return 0;
    }

    if(fgets(buf, 512, fp) == NULL)
         printf("Connection idle\n");
    else
         printf("Connection Busy\n");

    pclose(fp);
    return 0;
}

