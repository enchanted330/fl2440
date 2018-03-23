/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  date_time_client.c
 *    Description:  This is a sample socket client demo program.
 *                 
 *        Version:  1.0.0(06/08/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "06/08/2012 02:50:51 PM"
 *                 
 ********************************************************************************/

/* Some Unix Program Standard head file  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Socket Program head file */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet define */

#include <errno.h>      /* strerror(), perror(), errno head file*/

#include <arpa/inet.h>  /* For inet_pton() */

#define DAYTIME_SERV_PORT                      8888
#define MAX_BUF_SIZE                           1024

/* Argc is the program linux running command arguments count, and argv is 
 * the arguments string value. All of the arguments take as string.
 * For example:
 * If we run this program as "./socket_client 192.168.3.15", there are two 
 * arguments, so argc=2, argv[0] is the first argument, which is string  value
 * "./socket_client" and argv[1] is the second argment, which is string  value
 * "1921.68.3.15".
 */
int main(int argc, char **argv)
{
    int                       sockfd, len, retval;
    char                      recv_buf[MAX_BUF_SIZE];
    struct sockaddr_in        servaddr;

    /* Check the arguments count, if No serverip address give as an argument, 
     * then program exit */
    if(2 != argc) 
    {
        printf("usage: %s <ServerIPaddress>\n", argv[0]);
        printf("Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>\n");
        return -1;
    }

    /* Open an IPV4(AF_INET) TCP(SOCK_STREAM) Socket File Description, UDP socket should 
     * use SOCK_DGRAM,We can use linux command "man socket" to see this function manual
     */
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /* strerror() is the 1st way to display the failure reason, argument 
         * errno is a globle variable defined in <errno.h>, we can use linux command 
         * "man strerror" to see this function manual*/
        printf("Use socket() to create a TCP socket failure: %s\n", strerror(errno));
        return -1;
    }

    /* Now we set the Server Information, include IPV4 or IPV6, Server Port, Server IP address  */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;   /* Set it as IPV4 protocal */
    servaddr.sin_port = htons(DAYTIME_SERV_PORT); /* Daytime server port*/ 

    /* argv[1] we take as Server IP address, it's the second arguments in running command  */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) 
    {
        printf("Use inet_pton() to set the Server IP address failure.\n");
        retval = -2;
        goto CleanUp;
    }

    /* Now call connect() function to connect to the server, we can use linux command "man connect" 
     * to see this function manual */
    printf("Connect to Server [%s:%d]\n", argv[1], DAYTIME_SERV_PORT);
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("Connect to the server [%s:%d] failure\n", argv[1], DAYTIME_SERV_PORT);
        retval = -3;
        goto CleanUp;
    }

    /* Once we connect to the server successfully, we can receive the data from the socket socekt */
    while ( (len = read(sockfd, recv_buf, MAX_BUF_SIZE)) > 0) 
    {
        recv_buf[len] = 0; /*  null terminate */ 
        printf("Get Time from server %s:   %s\n", argv[1], recv_buf);
    }

    if( len < 0 )
    {
        /* perror() ss the 2nd way to display the failure reason */
        perror("Read() from socket failure");  
        retval = -4;
        goto CleanUp;
    }

CleanUp:
    close(sockfd);  /* We must close socket File Description when program exit*/
    return 0;
}

