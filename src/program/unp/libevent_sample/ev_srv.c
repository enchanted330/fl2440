/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ev_srv.c
 *    Description:  This file shows how to use libevent to write a server program, 
 *                  this sample program will listen on a special port and echo the
 *                  data from client back.
 *                 
 *        Version:  1.0.0(02/10/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/10/2014 10:20:13 AM"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

#ifdef DBG_MEMORY
#include "memwatch.h"
#endif


static int                       g_ssl_mode = SSL_NONE;

static void accept_client_cb(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *sa, int socklen, void *user_data);

static void usage(const char *progname)
{
    banner(progname);

    printf("Usage: %s [OPTION]...\n", progname); 
    printf(" %s is a network server program which used to echo data from client back.\n", progname);

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -L[Listen  ]  Listen host and port, format as IP_ADDR:PORT\n");
    printf(" -S[SSL     ]  Enable SSL, <1> for 1-way SSL authentication and <2> for 2-way SSL authentication \n");
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    printf("Example: %s -L 0.0.0.0:9001 -S 2\n", progname);

    return;
}

int main(int argc, char **argv)
{
    int                       opt = 0;
    const char                *progname = NULL;
    char                      *host_port = NULL;
    int                       sock_len = 0;
    struct sockaddr_storage   listen_on_addr;
    struct event_base         *base;
    struct evconnlistener     *listener;


    struct option long_options[] = 
    { 
        {"SSL", required_argument, NULL, 'S'},
        {"Listen", required_argument, NULL, 'L'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };


    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "S:L:vh", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            case 'S':
                g_ssl_mode = atoi(optarg);
                g_ssl_mode = g_ssl_mode>SSL_1WAY ? SSL_2WAY : SSL_1WAY;  
                break;

            case 'L':
                host_port = optarg;
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

#ifdef DBG_MEMORY 
    /*  Collect stats on a line number basis */
    mwStatistics( 2 );
#endif

    memset(&listen_on_addr, 0, sizeof(listen_on_addr));
    sock_len = sizeof(listen_on_addr);
    if( evutil_parse_sockaddr_port(host_port, (struct sockaddr*)&listen_on_addr, &sock_len)<0 )
    {
        int p = atoi(host_port);
        struct sockaddr_in *sin = (struct sockaddr_in*)&listen_on_addr;
        
        if (p < 1 || p > 65535)
        {
            printf("ERROR: Listen port invalid\n");
            return -1;
        }

        printf("Listen on port [%d] for all the IP address\n", p);
        sin->sin_port = htons(p);
        sin->sin_addr.s_addr = htonl(INADDR_ANY);
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
        perror("event_base_new()");
        return 1;
    }


    listener = evconnlistener_new_bind(base, accept_client_cb, NULL, 
            LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE, 
            -1, (struct sockaddr*)&listen_on_addr, sock_len);
    if(!listener)
    {
        perror("Couldn't create listener");
        event_base_free(base);
        return 2;
    }


    /* Goes into event base dispatch loop */
    event_base_dispatch(base); 

    /* Once come here, event base dispatch loop exit */
    evconnlistener_free(listener);
    event_base_free(base); 
    
    return 0; 
}

static void eventcb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_EOF) 
    { 
        printf("Remote client close the connection.\n");
        bufferevent_free(bev);
    } 
    else if (events & BEV_EVENT_ERROR) 
    {
        printf("Got an error on the connection: %s\n", strerror(errno));
        bufferevent_free(bev);
    } 
    
    return;
}


static void readcb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer  *input = bufferevent_get_input(bev);
#if 1
    struct evbuffer  *output = bufferevent_get_output(bev); 
    size_t       len ;

    len = evbuffer_get_length(input);
    printf("We got %lu bytes data\n", (unsigned long)len);

    evbuffer_remove_buffer(input, output, len);

    /* Copy all the data from input buffer to output buffer  */
    //evbuffer_add_buffer(output, input);

#else
    {
        size_t       len = evbuffer_get_length(input);
        char         *data;

        if( NULL != (data=calloc(len+1, 1)) )
        {
            evbuffer_copyout(input, data, len);
        }

        printf("We got %lu bytes data: %s\n", (unsigned long)len, data);
    }
#endif
}


static void accept_client_cb(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *sa, int socklen, void *user_data)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev;

    if(g_ssl_mode == SSL_NONE)
    {
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    }
    else
    {
        SSL *ssl = NULL;

        if(g_ssl_mode == SSL_1WAY)
            ssl = create_ssl(MODE_SSL_SERVER, CA_FILE, SRV_CRT_FILE, SRV_KEY_FILE, NULL);
        else
            ssl = create_ssl(MODE_SSL_SERVER|MODE_VERIFY_PEER, CA_FILE, SRV_CRT_FILE, SRV_KEY_FILE, NULL);

        if( !ssl )
        {
            printf("Create listen SSL %s peer certificate check failed.\n", g_ssl_mode==SSL_1WAY?"without":"with"); 
        } 
        printf("Create listen SSL %s peer certificate check okay.\n", g_ssl_mode==SSL_1WAY?"without":"with"); 

        bev = bufferevent_openssl_socket_new(base, fd, ssl,
                BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
    }

    if (!bev) 
    {
        printf("Error constructing bufferevent!");
        event_base_loopbreak(base);
        return;
    }
    
    bufferevent_setcb(bev, readcb, NULL, eventcb, NULL);
    bufferevent_enable(bev, EV_WRITE|EV_READ);

    return;
}


