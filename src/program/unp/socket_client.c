/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.c
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
#include <getopt.h>
#include <libgen.h>

/* Socket Program head file */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet define */

#include <errno.h>      /* strerror(), perror(), errno head file*/

#include <arpa/inet.h>  /* For inet_pton() */

#define MAX_BUF_SIZE                           1024

#define MAJOR            1
#define MINOR            0
#define REVER            0

void print_version(char *progname)
{
    printf("%s Version %d.%d.%d\n", progname, MAJOR, MINOR, REVER);
    printf("Copyright (C) 2012 Guo Wenxue<guowenxue@gmail.com>.\n");
    return;
}

void print_usage(char *progname)
{
    print_version(progname); 
    
    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is a socket client test program, which used to send a file/string\n", progname);
    printf(" for sometimes to a socket server.\n");

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -i[ipaddr  ]  Socket server IP address\n");
    printf(" -p[port    ]  Socket server port address\n");
    printf(" -f[file    ]  Need to send file\n");
    printf(" -s[string  ]  Need to send strings\n");
    printf(" -c[count   ]  Send file/string count\n");
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    return ;
}


/* Argc is the program linux running command arguments count, and argv is 
 * the arguments string value. All of the arguments take as string.
 */
int main(int argc, char **argv)
{
    int                       sockfd, len, retval;
    int                       opt, send_cnt;
    char                      recv_buf[MAX_BUF_SIZE];
    char                      *server_ip = NULL;
    char                      *send_file = NULL;
    char                      *send_string = NULL;
    char                      *progname = NULL;
    unsigned short            server_port = 0;
    struct sockaddr_in        servaddr;

    struct option             long_options[] = 
    { 
        {"ipaddr", required_argument, NULL, 'i'},
        {"port", required_argument, NULL, 'p'},
        {"file", required_argument, NULL, 'f'},
        {"string", required_argument, NULL, 's'},
        {"count", required_argument, NULL, 'c'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "i:p:f:s:c:vh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'p':
                server_port = atoi(optarg);
                break;

            case 'i':
                server_ip = optarg;
                break;

            case 'f':
                send_file = optarg;
                break;

            case 's':
                send_string = optarg;
                break;

            case 'c':
                send_cnt = atoi(optarg);
                break;

            case 'v':
                print_version(progname); /*  Defined in version.h */
                return EXIT_SUCCESS;

            case 'h':  /*  Get help information */
                print_usage(progname);
                return EXIT_SUCCESS;

            default:
                break;
        }
    }

    if(NULL==server_ip || 0==server_port) 
    {
        print_usage(progname);
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
    servaddr.sin_port = htons(server_port); /*Server port*/ 

    /* argv[1] we take as Server IP address, it's the second arguments in running command  */
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) 
    {
        printf("Use inet_pton() to set the Server IP address failure.\n");
        retval = -2;
        goto CleanUp;
    }

    /* Now call connect() function to connect to the server, we can use linux command "man connect" 
     * to see this function manual */
    printf("Connect to Server [%s:%d]\n", server_ip, server_port);
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("Connect to the server [%s:%d] failure\n", server_ip, server_port);
        retval = -3;
        goto CleanUp;
    }

    /* Once we connect to the server successfully, we can receive the data from the socket socekt */
    while ( (len = read(sockfd, recv_buf, MAX_BUF_SIZE)) > 0) 
    {
        recv_buf[len] = 0; /*  null terminate */ 
        printf("Get Time from server %s:   %s\n", server_ip, recv_buf);
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

