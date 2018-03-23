/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  epoll_server.c
 *    Description:  This file describ how to use epoll() to program
 *                 
 *        Version:  1.0.0(06/19/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "06/19/2012 11:30:48 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/resource.h>

#include "list.h"

#define MAX_CLIENTS          1024
#define LISTENQ              MAX_CLIENTS+1
#define EPOLL_TIMEOUT        500

#define BUF_SIZE             1024

#define DEBUG_PRINT 
#ifdef  DEBUG_PRINT
#define dbg_print(format,args...) printf(format, ##args)
#else
#define dbg_print(format,args...) do{} while(0);
#endif

#define YES                  1
#define NO                   0

typedef struct __SOCKET_INFO
{
    int                      sockfd;
    char                     buf[BUF_SIZE]; /* Data buffer */
    struct sockaddr_in       raddr;         /* Remote address */
    struct list_head         list; 
} SOCKET_INFO;

LIST_HEAD                    (socket_head);

unsigned char                g_stop = 0;
int                          epfd;
struct epoll_event           evts[LISTENQ]; /* One for listen socket */  

#define MODE_HEX             0x01
#define MODE_CHAR            0x02
#define DEF_COLUMN           64
static inline void print_buffer(const char *prefix, char *buf, int len, int column, int mode)
{
    int         i;

    if(prefix)
        dbg_print("%s", prefix);

    for(i=0; i<len; i++)
    {
        if( (0!=column) && !(i%(column)) && i!=0)
            dbg_print("\n");

        if(MODE_HEX == mode)
            dbg_print("%02x ", (unsigned char)buf[i]);
        else
            dbg_print("%c", buf[i]);
    }

    dbg_print("\n");
}


int set_nonblocking(int sockfd)
{
    int           opts;
    
    if( (opts=fcntl(sockfd, F_GETFL)) < 0)
    {
        dbg_print("Set Nonblocking F_GETFL error: %s\n", strerror(errno));
        return -1;
    }
    
    opts = opts|O_NONBLOCK;
    if(fcntl(sockfd, F_SETFL, opts)<0)
    {
        dbg_print("Set Nonblocking F_SETFL error: %s\n", strerror(errno));
        return -2;
    }

    return 0;
}

/* ****************************************************************************
 * FunctionName: signal_handler
 * Description : The signal catch handler, will set result in g_proc_signal.
 * Inputs      : signo: The received signal number
 * Output      : g_proc_signal
 * Return      : NONE
 * *****************************************************************************/
void signal_handler(int signo)
{
    switch(signo)
    {
        case SIGINT:
        case SIGTERM:
            dbg_print("Program pid[%d] receive SIGINT/SIGTERM signal, exit program now\n", getpid());
            g_stop = 1;
            break;

        case SIGSEGV:
            dbg_print("Program pid[%d] receive SIGSEGV signal, exit program now\n", getpid());
            g_stop = 1;
            break;

        case SIGPIPE:
            dbg_print("Program pid[%d] receive SIGPIPE signal, exit program now\n", getpid());
            g_stop = 1;
            break;

        default:
            dbg_print("Program pid[%d] receive signal[%d].\n", getpid(), signo);
            break;
    }

    return ;
}


/* ****************************************************************************
 * FunctionName: install_signal
 * Description : Install process signal handler 
 * Inputs      : NONE
 * Output      : NONE
 * Return      : NONE
 * *****************************************************************************/
void install_signal(void)
{       
    struct sigaction sigact, sigign;
    dbg_print("Install signal action.\n");
    
    /* Initialize the catch signal structure. */ 
    sigemptyset(&sigact.sa_mask); 
    sigact.sa_flags = 0;
    sigact.sa_handler = signal_handler;
        
    /* Setup the ignore signal. */
    sigemptyset(&sigign.sa_mask);
    sigign.sa_flags = 0;
    sigign.sa_handler = SIG_IGN;
                                 
    sigaction(SIGTERM, &sigact, 0); /* catch terminate signal "kill" command */
    sigaction(SIGINT,  &sigact, 0); /* catch interrupt signal CTRL+C */
    //sigaction(SIGSEGV, &sigact, 0); /* catch segmentation faults  */ 
    sigaction(SIGCHLD, &sigact, 0); /* catch child process return */
    sigaction(SIGPIPE, &sigact, 0); /* catch broken pipe */
    //sigaction(SIGUSR2, &sigact, 0); /* catch USER signal */ 
 
    return;
}

int network_init(void)
{
    struct  rlimit           rlt;

    /* Set the maximum open file description for per process */
    rlt.rlim_max = rlt.rlim_cur = 1024;  
    if( setrlimit( RLIMIT_NOFILE, &rlt ) == -1 )   
    {
        printf("Setrlimit Error : %s\n", strerror(errno));  
        exit( EXIT_FAILURE ); 
    }

    return 0;
}

static SOCKET_INFO *fd_add(int fd, int events) 
{
	struct epoll_event      ev;
	SOCKET_INFO             *socket = NULL;
    socklen_t               sock_len = sizeof(struct sockaddr_in); 

    if( ! (socket = (SOCKET_INFO *)malloc(sizeof(SOCKET_INFO))) )
    {
        printf("Malloc error, can not server for socket [%d].\n", fd);
        return NULL;
    }
    memset(socket, 0, sizeof(SOCKET_INFO));

	socket->sockfd = fd;
    getpeername(socket->sockfd, (struct sockaddr *)&(socket->raddr), &sock_len);

    list_add(&(socket->list), &socket_head);

    dbg_print("Add new event: socket[%d] from [%s:%d] %p\n", socket->sockfd,
            inet_ntoa(socket->raddr.sin_addr), ntohs(socket->raddr.sin_port), socket);

	ev.data.ptr = socket;
	ev.events = events | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	return socket;
}

static void fd_mod(SOCKET_INFO *socket, int events) 
{
	struct epoll_event ev;

	ev.data.ptr = socket;
	ev.events = events | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_MOD, socket->sockfd, &ev);
}


void term_socket(SOCKET_INFO *socket)
{ 
    dbg_print("Terminate socket[%d] from [%s:%d] %p\n", socket->sockfd,
            inet_ntoa(socket->raddr.sin_addr), ntohs(socket->raddr.sin_port), socket);

    epoll_ctl(epfd, EPOLL_CTL_DEL, socket->sockfd, NULL); 

    close(socket->sockfd);
    socket->sockfd = -1;

    list_del(&socket->list);
    free(socket);
}

void term_all_socket(void)
{
    SOCKET_INFO              *socket, *entry;
    
    list_for_each_entry_safe(socket, entry, &socket_head, list)
    {
        term_socket(socket);
    }

    close(epfd);
}

int main( int argc, char ** argv )  
{ 
    int                      listen_fd, conn_fd;
    int                      i, port, nfds;
    int                      opt = 1;
    struct sockaddr_in       serv_addr;  

    int                      size;
    SOCKET_INFO              *client;

    if(argc != 2)
    {
        printf("Usage: %s [port]\n", argv[0]);
        exit( EXIT_FAILURE ); 
    }
    port = atoi(argv[1]);

    /* Initialize network */
    network_init();

    /* Install catch signal */
    install_signal();

    /* Create server listen socket and set nonblock */
    if( (listen_fd=socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0)) < 0 )
    {
        printf("Create server socket Error : %s\n", strerror(errno));  
        return -1;
    }
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    dbg_print("Create server listen socket fd=%d\n", listen_fd);  


    /* Register epoll event for server listen socket */
    if( (epfd=epoll_create1(0)) < 0 )
    {
        printf("create epoll Error: %s\n", strerror(errno));
        goto CleanUp;
    }

    /* Bind and listen server socket */
    bzero(&serv_addr, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    serv_addr.sin_port = htons(port);  

    if( bind( listen_fd, ( struct sockaddr *)&serv_addr, sizeof( struct sockaddr ) ) == -1 )
    { 
        printf("Bind socket Error : %s\n", strerror(errno));  
        goto CleanUp;
    }
    dbg_print("Server Bind successfully!\n");

    if( listen( listen_fd, LISTENQ ) == -1 ) 
    {
        printf("Listen socket Error : %s\n", strerror(errno));  
        goto CleanUp;
    }
    dbg_print("Server Listen successfully!\n");

    fd_add(listen_fd, EPOLLIN);

    while( !g_stop )
    {
        nfds=epoll_wait(epfd, evts, LISTENQ, EPOLL_TIMEOUT);

        for(i=0; i<nfds; ++i)
        {
            client = (SOCKET_INFO *)evts[i].data.ptr;
            if ( NULL==client || client->sockfd < 0)
            {
                printf("Find a disconnected socket.\n");
                continue;
            }

            /* New client connect to server */
            if(client->sockfd == listen_fd) 
            {
                struct sockaddr_in       client_addr;
                socklen_t                sock_len = sizeof(struct sockaddr_in); 

                if((conn_fd=accept(listen_fd,(struct sockaddr *)&client_addr, &sock_len)) < 0)
                {
                    printf("Accept new client Error : %s\n", strerror(errno));  
                    continue;
                }
                set_nonblocking(conn_fd);

                if( NULL == fd_add(conn_fd, EPOLLIN) )
                {
                    close(conn_fd);
                    continue;
                }

            }
            /* Data incoming from connected client */
            else if(evts[i].events & EPOLLIN) 
            {
                memset(client->buf, 0, BUF_SIZE);
                if ( (size = read(client->sockfd, client->buf, BUF_SIZE)) < 0) 
                {
                    if (errno == ECONNRESET) 
                    {
                        printf("Receive ECONNRESET and terminate sockfd [%d]\n", client->sockfd);
                        term_socket(client);
                    }
                    else
                    {
                        printf("Read data from socket[%d] Error : %s\n", client->sockfd, strerror(errno));
                    }
                    continue;
                }
                /* Data incoming but can not read, it's failure */
                else if(size == 0) 
                {
                    printf("Receive disconnect signal and terminate sockfd [%d]\n", client->sockfd);
                    term_socket(client);
                    continue;
                }

                dbg_print("Receive [%u] bytes data from socket[%d]\n", size, client->sockfd);
                print_buffer("<<==|", client->buf, size, DEF_COLUMN, MODE_HEX );

                /* Modify EPOLL event to send data */
                fd_mod(client, EPOLLOUT);
            }
            /* Data need send out to connected client */
            else if( evts[i].events & EPOLLOUT )
            {
                const char          *ptr;
                int                 len, left;

                dbg_print("Write [%u] bytes data to socket[%d]\n", size, client->sockfd);
                print_buffer("==>>|", client->buf, size, DEF_COLUMN, MODE_HEX );

                left = size;
                ptr = client->buf;
                while(left > 0)
                {
                   if( (len=write(client->sockfd, ptr, left)) <= 0 )
                   {
                       if(len<0 && EINTR==errno)
                       {
                           len = 0;
                       }
                       else
                       {
                           printf("Write to client failure and terminate sockfd [%d].\n", client->sockfd);
                           term_socket(client);
                           goto CONTINUE;
                       }
                   }

                   left -= len;
                   ptr += len;
                }

                /* Modify EPOLL event to recv data */
                fd_mod(client, EPOLLIN);
CONTINUE: 
                ;
            }
        } /*  i<nfds */
    } /* while(1)  */


CleanUp:
    dbg_print("Exit program and do cleanup now.\n");
    term_all_socket();

    return 0;
}
