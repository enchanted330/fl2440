/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  date_time_server.c
 *    Description:  This is a sample socket server demo program.
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

/* strerror(), perror(), errno head file*/
#include <errno.h>
#include <time.h>


#define DAYTIME_SERV_PORT                      6000
#define MAX_BUF_SIZE                           1024
#define LISTENQ                                1024 /* 2nd argument to listen () */

/* Argc is the program linux running command arguments count, and argv is 
 * the arguments string value. All of the arguments take as string.
 * For example:
 * If we run this program as "./socket_server 13", there are two 
 * arguments, so argc=2, argv[0] is the first argument, which is string  value
 * "./socket_server" and argv[1] is the second argment, which is string  value
 * "13".
 */
int main(int argc, char **argv)
{
    int                       listenfd, connfd;
    char                      send_buf[MAX_BUF_SIZE];
    struct sockaddr_in        servaddr;
    time_t                    ticks;
    int                       serv_port;

    if(2 == argc)
    {
        /*If there is a argument, then we take it as the server port number, and it's a
         *string value, so we use atoi() function to convert it to int type.*/
        serv_port = atoi(argv[1]); 
    }
    else
    {
        serv_port = DAYTIME_SERV_PORT;
        printf("Another Usage: %s [ServerPort]\n", argv[0]);
    }

    /*
     * Open an IPV4(AF_INET) TCP(SOCK_STREAM) Socket File Description(listenfd), UDP socket 
     * should use SOCK_DGRAM,We can use linux command "man socket" to see this function manual
     */
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /* strerror() is the 1st way to display the failure reason, argument 
         * errno is a globle variable defined in <errno.h>, we can use linux command 
         * "man strerror" to see this function manual*/
        printf("Use socket() to create a TCP socket failure: %s\n", strerror(errno));
        return -1;
    }

    /* Now we set the Server Information, include IPV4 or IPV6, Listen IP address and Port */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;   /* Set it as IPV4 protocal */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Listen all the local IP address */
    servaddr.sin_port = htons(serv_port); /*  daytime server port */

    /*
     * When a socket is created with socket(2), it exists in a name space (address family) but 
     * has no address assigned to it. bind() assigns the address specified to by addr to the 
     * socket referred to by the file descriptor listenfd. We can use Linux command "man 2 bind" 
     * to see this function manual.
     */
    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
        goto CleanUp;
    }

    /*
     * listen() marks the socket referred to by listenfd as a passive socket, that is, as a socket 
     * that will be used to accept incoming connection requests using accept(2). We can use Linux
     * command "man listen" to see this function manual.
     */
    if(listen(listenfd, LISTENQ) < 0)
    {
        printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
        goto CleanUp;
    }
    printf("%s server start to listen port %d\n", argv[0],serv_port);


    for ( ; ; ) 
    {
        /*
         * The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET).
         * It extracts the first connection request on the queue of pending connections for the listening 
         * socket linstenfd, creates a new connected socket(connfd), and returns a new file descriptor referring 
         * to that socket. The newly created socket is not in the listening state. The original socket 
         * listenfd is unaffected by this call.
         */

        if( (connfd=accept(listenfd, (struct sockaddr *)NULL, NULL)) > 0)
        {
#if 1
            ticks = time(NULL);  /* Get current system time */
            snprintf(send_buf, sizeof(send_buf), "%.24s\r\n", ctime(&ticks));
            if(write(connfd, send_buf, strlen(send_buf)) < 0)
            { 
                printf("Write current time to client failure: %s\n", strerror(errno));
            }
            close(connfd);
#else
            printf("connect client\n");
#endif
        }
    }

CleanUp:
    close(listenfd);  /* We must close socket File Description when program exit*/
    return 0;
}

