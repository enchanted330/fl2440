/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ev_cli.c
 *    Description:  This file shows how to use libevent to write a client program, 
 *                  this sample program will connect to remote host server and send
 *                  data to it.
 *                 
 *        Version:  1.0.0(02/10/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/10/2014 10:20:13 AM"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>
#include <libgen.h>
#include <errno.h>


#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>

#include "ssl_api.h"
#include "version.h"

//#define DATA_LEN             4096
#define DATA_LEN             1024

static int                       g_ssl_mode = SSL_NONE;

static void sock_eventcb(struct bufferevent *bev, short events, void *user_data);
static void sock_readcb(struct bufferevent *bev, void *ctx);

static void usage(const char *progname)
{
    banner(progname);

    printf("Usage: %s [OPTION]...\n", progname); 
    printf(" %s is a network client program which used to send data to server.\n", progname);

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -H[Host    ]  Connect server hostname and port, format as IP_ADDR:PORT\n");
    printf(" -S[SSL     ]  Enable SSL, <1> for 1-way SSL authentication and <2> for 2-way SSL authentication\n");
    printf(" -s[string  ]  The string data will be sent to server\n");
    printf(" -c[char    ]  %d bytes of this character will be sent to server", DATA_LEN);
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    printf("Example: %s -H 9001 -S 1\n", progname);
    printf("Example: %s -H 9001 -s \"Hello World\"\n", progname);
    printf("Example: %s -H 127.0.0.1:9001 -c z\n", progname);

    return;
}

int main(int argc, char **argv)
{
    int                       opt = 0;
    int                       rv = 0;
    const char                *progname = NULL;
    char                      *host_port = NULL;
    int                       sock_len = 0;
    struct sockaddr_storage   connect_addr;
    struct event_base         *base = NULL;
    struct bufferevent        *bev = NULL;
    char                      data[DATA_LEN+1];
    char                      *send_string = "Hello World!";

    struct option long_options[] = 
    { 
        {"Host", required_argument, NULL, 'H'},
        {"SSL", required_argument, NULL, 'S'},
        {"string", required_argument, NULL, 's'},
        {"character", required_argument, NULL, 'c'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };


    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "H:S:s:c:vh", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            case 'S':
                g_ssl_mode = atoi(optarg);
                g_ssl_mode = g_ssl_mode>SSL_1WAY ? SSL_2WAY : SSL_1WAY;
                break;

            case 'H':
                host_port = optarg;
                break;

            case 's':
                send_string = optarg;
                break;

            case 'c':
                data[DATA_LEN] = '\0';
                memset(data, optarg[0], DATA_LEN);
                send_string = data;
                break;

            case 'v':
                banner(progname);
                return 0; 
            
            case 'h':
                usage(progname);
                return 0; 
            
            default:
                break;
        }
    }

    if(!host_port)
    {
        usage(progname);
        return -1;
    }

    memset(&connect_addr, 0, sizeof(connect_addr));
    sock_len = sizeof(connect_addr);
    if( evutil_parse_sockaddr_port(host_port, (struct sockaddr*)&connect_addr, &sock_len)<0 )
    {
        int p = atoi(host_port);
        struct sockaddr_in *sin = (struct sockaddr_in*)&connect_addr;
        
        if (p < 1 || p > 65535)
        {
            printf("ERROR: Connect port invalid\n");
            rv = -1;
            return rv;
        }

        printf("Connect to [127.0.0.1:%d]\n", p);
        sin->sin_port = htons(p);
        sin->sin_addr.s_addr = htonl(0x7f000001L); /* 127.0.0.1  */
        sin->sin_family = AF_INET;
        sock_len = sizeof(struct sockaddr_in);
    }

    if(g_ssl_mode != SSL_NONE)
    {
        printf("Initial %s SSL contex.\n", g_ssl_mode==SSL_1WAY?"1-way":"2-way");
        init_ssl();
    }

    base = event_base_new();
    if (!base)
    {
        printf("event_base_new() failure: %s\n", strerror(errno));
        rv = -2;
        goto cleanup;
    }

    if(SSL_NONE == g_ssl_mode) 
    {
        /*  arg1:event_base arg2:fd(-1: Create socket in future)  arg3:option */
        bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    }
    else
    {
        SSL *ssl = NULL;

        if(g_ssl_mode == SSL_1WAY)
            ssl = create_ssl(MODE_SSL_CLIENT, CA_FILE, NULL, NULL, NULL);
        else
            ssl = create_ssl(MODE_SSL_CLIENT|MODE_VERIFY_PEER, CA_FILE, CLI_CRT_FILE, CLI_KEY_FILE, NULL);

        if( !ssl )
        {
            printf("Create connect SSL %s peer certificate check failed.\n", g_ssl_mode==SSL_1WAY?"without":"with"); 
        } 
        printf("Create connect SSL %s peer certificate check okay.\n", g_ssl_mode==SSL_1WAY?"without":"with"); 

        bev = bufferevent_openssl_socket_new(base, -1, ssl,
                BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
    }


    if (!bev) 
    {
        printf("Error: Constructing connect bufferevent failure!\n");
        rv = -3;
        goto cleanup;
    }

    /*  arg1: bufferevent arg2: readcb arg3:writecb arg4: cbarg */
    bufferevent_setcb(bev, sock_readcb, NULL, sock_eventcb, (void *)send_string);
    if( 0 != bufferevent_enable(bev, EV_READ|EV_WRITE) )
    {
        printf("bufferevent_enable failure\n");
        rv = -4;
        goto cleanup;
    }


    bufferevent_socket_connect(bev, (struct sockaddr *)&connect_addr, sock_len);

    /* Goes into event base dispatch loop */
    event_base_dispatch(base); 


    /* Once come here, event base dispatch loop exit */
    event_base_free(base); 
    return 0;

cleanup:
    if(bev)
        bufferevent_free(bev);

    if(base)
        event_base_free(base); 
    
    return rv; 
}

static void sock_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_CONNECTED)
    {
        char        *send_string = (char *)user_data;

        printf("Connect to remote server okay.\n");
        printf("=>Send %u bytes: %s\n", strlen(send_string), send_string);
        bufferevent_write(bev, send_string, strlen(send_string));
    }
    else if (events & BEV_EVENT_EOF) 
    { 
        printf("Remote server close the connection.\n");
        bufferevent_free(bev);
    } 
    else if (events & BEV_EVENT_ERROR) 
    {
        printf("Got an error on the connection: %s\n", strerror(errno));
        bufferevent_free(bev);
    } 

    return;
}


static void sock_readcb(struct bufferevent *bev, void *user_data)
{
    int               n;
    char              buf[1024];
    //char              *send_string = (char *)user_data;
    struct evbuffer   *input = bufferevent_get_input(bev); 
    
    memset(buf, 0, sizeof(buf));
    while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0)
    {
        printf("<=Recv %d bytes: %s\n", n, buf);
    } 
    
    sleep(1); 
    
    //printf("=>Send %u bytes: %s\n", strlen(send_string), send_string);
    //bufferevent_write(bev, send_string, strlen(send_string));
}


