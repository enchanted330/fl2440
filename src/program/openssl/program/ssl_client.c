/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  ssl_client.c
 *    Description:  This file is the SSL client test program
 *                 
 *        Version:  1.0.0(02/04/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *
 *      ChangeLog:  1, Release initial version on "02/04/2013 01:13:54 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl_common.h"
#include "version.h"

static void print_usage(const char *progname)
{
    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is a SSL client test program running on the device\n", progname);

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf(" -i[ipaddr  ]  Remote server IP address, default 127.0.0.1\n");
    printf(" -p[port    ]  Connect port, default [%d]\n", DEF_SSL_PORT);
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    printf("\n");
    banner(progname);
    return ;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                    sockfd, len;
    struct sockaddr_in     sock_addr;
    char                   buffer[MAXBUF];
    SSL_CTX                *ctx;
    SSL                    *ssl;
    char                   *server = "127.0.0.1";
    int                    port = DEF_SSL_PORT;
    int                    opt;
    int                    verify_peer = 0;
    const char             *progname=NULL;


    struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
        {"ipaddr", required_argument, NULL, 'i'},
        {"Verify", no_argument, NULL, 'V'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    while ((opt = getopt_long(argc, argv, "vhVp:i:", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            case 'v':
                banner(progname); /*   Defined in version.h */
                return 0;

            case 'h':
                print_usage(progname); /*   Defined in version.h */
                return 0;

            case 'V':
                verify_peer = 1;
                break;

            case 'i':
                server = optarg;
                break;

            case 'p':
                port = atoi(optarg);
                break;
        }

    }
    
    if(verify_peer)
    {
        ctx = init_ssl_context(MODE_SSL_CLIENT|MODE_VERIFY_PEER, CA_CERT, CLIENT_CERT, CLIENT_KEY, NULL);
        //ctx = init_ssl_context(MODE_SSL_CLIENT|MODE_VERIFY_PEER, CA_CERT, NULL, NULL, NULL);
    }
    else
    {
        ctx = init_ssl_context(MODE_SSL_CLIENT, NULL, NULL, NULL, NULL);
    }

    if( !ctx )
    {
        printf("Initialise SSL contex %s peer certificate check failure.\n", verify_peer?"with":"without");
        return 0;
    }
    printf("Initialise SSL contex %s peer certificate check successfully.\n", verify_peer?"with":"without");

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        printf("Create socket failure: %s\n", strerror(errno));
        exit(1);
    } 
    printf("Create socket[%d] successfully\n", sockfd);

    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    if (inet_aton(server, (struct in_addr *) &sock_addr.sin_addr.s_addr) == 0)
    {
        printf("inet_aotn convert failure: %s\n", strerror(errno));
        exit(errno);
    }

    if (connect(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) != 0)
    {
        printf("Connect socket[%d] failure: %s\n", sockfd, strerror(errno));
        exit(errno);
    }
    printf("Connect socket[%d] to [%s:%d] successfully\n", sockfd, server, port);

    /* Start SSL Negotiation... */
    ssl = SSL_new(ctx);
    if(!ssl)
    {
        printf("%s\n", ERR_error_string(ERR_get_error(), NULL)); 
        goto finish;
    }
    SSL_set_fd(ssl, sockfd); 
    if (SSL_connect(ssl) != 1)
    {
        printf("SSL connect failure: %s\n", ERR_reason_error_string(ERR_get_error()));
        goto finish;
    }
    printf("SSL_connect socket[%d] to [%s:%d] successfully\n", sockfd, server, port);


    if(check_certs(ssl, NULL))
    {
        printf("Check peer certificate failure\n");
        goto finish;
    }

    bzero(buffer, MAXBUF);
    len = SSL_read(ssl, buffer, MAXBUF);
    if (len > 0)
    {
        printf("Receive %d bytes message: \"%s\"\n", len, buffer);
    }
    else
    {
        printf("Receive message from server failure [len=%d]: %s\n", len, ERR_reason_error_string(ERR_get_error()));
        goto finish;
    }

    bzero(buffer, MAXBUF);
    snprintf(buffer, sizeof(buffer), "Hello Server [%s:%d]!", server, port);
    len = SSL_write(ssl, buffer, strlen(buffer));
    if (len < 0)
    {
        printf("Send message to server failure [len=%d]: %s\n", len, ERR_reason_error_string(ERR_get_error()));
    }
    else
    {
        printf("Send %d bytes message: \"%s\"\n", len, buffer);
    }

    bzero(buffer, MAXBUF);
    len = SSL_read(ssl, buffer, MAXBUF);
    if (len > 0)
    {
        printf("Receive %d bytes message: \"%s\"\n", len, buffer);
    }
    else
    {
        printf("Receive message from server failure [len=%d]: %s\n", len, ERR_reason_error_string(ERR_get_error()));
        goto finish;
    }



finish:
    if(ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(sockfd);
    if(ctx)
        SSL_CTX_free(ctx);
    return 0;
} /* ----- End of main() ----- */

