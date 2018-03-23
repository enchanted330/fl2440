/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  ssl_server.c
 *    Description:  This file is the SSL server program for test
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
    printf(" %s is a SSL server test program running on the device\n", progname);
    
    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf(" -p[port    ]  Listen port, default [%d]\n", DEF_SSL_PORT);
    printf(" -V[Verify  ]  Verify peer certificate\n");
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n"); 
    
    printf("\n");
    banner(progname);
    return ;
}

int cp_sock_set_reuseaddr(int sockfd)
{
    int opt = 1;
    int len = sizeof (int);

    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len))
    {
        printf("Set socket[%d] option SO_REUSEADDR failed:%s\n", sockfd, strerror(errno));
        return -1;
    } 
    
    printf("Set socket[%d] option SO_REUSEADDR ok\n", sockfd); 
    return 0;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                    sockfd, new_fd = -1;
    int                    len;
    socklen_t              addrlen;
    struct sockaddr_in     srv_addr, cli_addr;  
    char                   buf[MAXBUF];
    SSL_CTX                *ctx;
    int                    port = DEF_SSL_PORT;
    int                    opt;
    int                    verify_peer = 0;
    const char             *progname=NULL;

    struct option long_options[] = {
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"Verify", no_argument, NULL, 'V'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    while ((opt = getopt_long(argc, argv, "vhVp:", long_options, NULL)) != -1)
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

            case 'p':
                port = atoi(optarg);
                break;
        }
    
    }

    if(verify_peer)
        ctx = init_ssl_context(MODE_SSL_SERVER|MODE_VERIFY_PEER, CA_CERT, SERVER_CERT, SERVER_KEY, NULL);
    else 
        ctx = init_ssl_context(MODE_SSL_SERVER, CA_CERT, SERVER_CERT, SERVER_KEY, NULL);

    if( !ctx )
    {
        printf("Initialise SSL contex %s peer certificate check failure.\n", verify_peer?"with":"without");
        return 0;
    }
    else
    {
        printf("Initialise SSL contex %s peer certificate check successfully.\n", verify_peer?"with":"without");
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        printf("Create socket failure: %s\n", strerror(errno));
        exit(1);
    } 
    printf("Create socket[%d] successfully\n", sockfd);

    bzero(&srv_addr, sizeof(srv_addr)); 
    srv_addr.sin_family = PF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY; 

    if(cp_sock_set_reuseaddr(sockfd))
    {
        close(sockfd);
        return -2;
    }
    
    if (bind(sockfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr)) == -1) 
    {
        printf("Bind socket[%d] failure: %s\n", sockfd, strerror(errno));
        exit(1);
    } 
    printf("Bind socket[%d] successfully\n", sockfd);

    if (listen(sockfd, MAX_CLIENT) == -1) 
    { 
        printf("Listen socket[%d] on port [%d] failure: %s\n", sockfd, port, strerror(errno));
        exit(1);
    }
    printf("Listen socket[%d] on port [%d] successfully\n", sockfd, port);

    while(1)
    {
        SSL *ssl = NULL;

        addrlen = sizeof(struct sockaddr);
        
        if ((new_fd = accept(sockfd, (struct sockaddr *) &cli_addr, &addrlen)) < 0) 
        {
            printf("Accept socket[%d] failure: %s\n", sockfd, strerror(errno));
            exit(errno);
        }

        printf("Accept client connection from %s, port %d, socket %d\n", 
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), new_fd); 

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, new_fd);
        if (SSL_accept(ssl) == -1) 
        {
            printf("SSL accept failure: %s\n", ERR_reason_error_string(ERR_get_error()));
            goto finish;
        }

        if(check_certs(ssl, NULL))
        {
            printf("Check peer certificate failure\n");
            goto finish;
        }

        bzero(buf, MAXBUF);
        snprintf(buf, sizeof(buf), "Hello Client [%s:%d]", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        len = SSL_write(ssl, buf, strlen(buf)); 
        if (len <= 0) 
        {
            printf ("Send Message \"%s\" failure: %s\n", buf, ERR_reason_error_string(ERR_get_error()) );
            goto finish;
        } 
        else
        {
            printf("Send %d bytes message: \"%s\"\n", len, buf);
        }

        while(1)
        {
            bzero(buf, MAXBUF);
            len = SSL_read(ssl, buf, MAXBUF);
            if (len > 0) 
            {
                printf("Receive %d bytes message: \"%s\"\n", len, buf);
                len = SSL_write(ssl, buf, len);
                if(len <= 0)
                    break;
            }
            else if(len == 0)
            {
                printf("Remote client disconnect\n");
                break;
            }
            else
            {
                printf ("Receive data from cleint failure: %s\n", ERR_reason_error_string(ERR_get_error()));
                break;
            }
        }

finish:
        if(ssl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }

        if(new_fd > 0)
        {
            close(new_fd);
            new_fd = -1;
        }
    }

    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
} /* ----- End of main() ----- */

