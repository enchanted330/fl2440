/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  tcp_inspect.c
 *    Description:  For in some network environment(such as firmware) will drop ping 
 *                  inspect, so we can not do ping test to diagnose the network 
 *                  connection, so I write this program to do TCP connect test.
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
#include <stdarg.h>
#include <errno.h>      /* strerror(), perror(), errno head file*/

/* Socket Program head file */
#include <sys/stat.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet define */
#include <arpa/inet.h>  /* For inet_pton() */
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>

#define MAX_BUF_SIZE                           1024
#define DEF_CONNECT_TIMEOUT                    30

#define MAJOR            1
#define MINOR            0
#define REVER            0

int                       debug = 0;

#define dbg_print(format,args...) if(debug) {printf(format, ##args);}

int network_ping_test(char *from, char *ping_ip);
int check_network_inf(char *inf);
int tcp_sock_connect(char *host, char *port, char *inf, int timeount);

void print_version(char *progname)
{
    printf("%s Version %d.%d.%d\n", progname, MAJOR, MINOR, REVER);
    printf("Copyright (C) 2012 Guo Wenxue<guowenxue@gmail.com>.\n");
    return;
}

void print_usage(char *progname)
{
    print_version(progname); 
    
    printf("Usage: %s [OPTION]... [HOST]\n", progname);
    printf(" %s is a network connection status inspect program, which support \n", progname);
    printf("   both ICMP and TCP protocol. It will return 0 means test OK and -x for failure");

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -H[host     ]  Inspect destination IP address\n");
    printf(" -I[interface]  Inspect from which ethernet card\n");
    printf(" -p[port     ]  TCP inspect server port address\n");
    printf(" -t[timeout  ]  TCP inspect connect timeout value, default %d seconds\n", DEF_CONNECT_TIMEOUT);
    printf(" -c[count    ]  TCP connect retry times, default is 1\n");
    printf(" -h[help     ]  Display this help information\n");
    printf(" -v[version  ]  Display the program version\n");

    printf("ICMP Example: %s -I eth0 -H 192.168.1.1\n", progname);
    printf("TCP  Example: %s -p 80 -H 192.168.1.1\n", progname);
    printf("TCP  Example: %s -I eth0 -p 80 -t 45 -H 192.168.1.1 -d\n", progname);

    return ;
}


/* Argc is the program linux running command arguments count, and argv is 
 * the arguments string value. All of the arguments take as string.
 */
int main(int argc, char **argv)
{
    int                       rv = 0;
    int                       opt, times = 1;
    char                      *host = NULL;
    char                      *progname = NULL;
    char                      *inf = NULL;
    char                      *port = NULL;
    unsigned char             icmp = 1;
    int                       timeout = DEF_CONNECT_TIMEOUT;

    struct option             long_options[] = 
    { 
        {"host", required_argument, NULL, 'H'},
        {"interface", required_argument, NULL, 'I'},
        {"port", required_argument, NULL, 'p'},
        {"timeout", required_argument, NULL, 't'},
        {"count", required_argument, NULL, 'c'},
        {"debug", required_argument, NULL, 'd'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "H:I:p:t:c:dvh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'I':
                inf = optarg;
                break;

            case 'H':
                host = optarg;
                break;

            case 't':
                timeout = atoi(optarg);
                break;

            case 'p':
                icmp = 0;
                port = optarg;
                break;

            case 'c':
                times = atoi(optarg);
                break;

            case 'd':
                debug = 1;
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

    if( !host ) 
    {
        print_usage(progname);
        exit(-1);
    }

    if(inf && !check_network_inf(inf) )
    {
        dbg_print("Network interface [%s] doesn't exist\n", inf);
        exit(-2);
    }

    if(icmp)
    {
        rv = network_ping_test(inf, host);
        dbg_print("ICMP inspect: ping %s from interface %s %d%% packet loss.\n", host, inf, rv);
        rv = (rv>=0&&rv<100) ? 0: 1;

        goto out;
    }

    dbg_print("TCP inspect: connect to server [%s:%s] now\n", host, port);
    while(times-- > 0) 
    {
        if(tcp_sock_connect(host, port, inf, timeout))
        {
            dbg_print("Connect[T=%d] to the server [%s:%s] failure\n", times, host, port);
            rv = -3;
        }
        else
        {
            rv = 0;
            break;
        }
    }

out:
    exit(rv);
}

void sock_bind_inf(int sock, char *inf)
{
    struct ifreq   ifr;

    if(!inf)
        return ;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, inf, IFNAMSIZ);

    dbg_print("Bind socket[%d] to interface [%s]\n", sock, inf);
    setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
}

int sock_set_nonblock(int sock)
{
    int opts;
    /*
     * fcntl may set: 
     *
     * EACCES, EAGAIN: Operation is prohibited by locks held by other 
     *          processes. Or, operation is prohibited because the file has 
     *          been memory-mapped by another process. 
     * EBADF:   fd is not an open file descriptor, or the command was F_SETLK 
     *          or F_SETLKW and the file descriptor open mode doesn't match 
     *          with the type of lock requested.
     * EDEADLK: It was detected that the specified F_SETLKW command would 
     *          cause a deadlock.       
     * EFAULT:  lock is outside your accessible address space.
     * EINTR:   For F_SETLKW, the command was interrupted by a signal. For 
     *          F_GETLK and F_SETLK, the command was interrupted by a signal 
     *          before the lock was checked or acquired. Most likely when 
     *          locking a remote file (e.g. locking over NFS), but can 
     *          sometimes happen locally.
     * EINVAL:  For F_DUPFD, arg is negative or is greater than the maximum 
     *          allowable value. For F_SETSIG, arg is not an allowable signal 
     *          number.                 
     * EMFILE:  For F_DUPFD, the process already has the maximum number of 
     *          file descriptors open.  
     * ENOLCK:  Too many segment locks open, lock table is full, or a remote 
     *          locking protocol failed (e.g. locking over NFS).
     * EPERM:   Attempted to clear the O_APPEND flag on a file that has the 
     *          append-only attribute set.
     */
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        dbg_print("fcntl() get socket options failure: %s\n", strerror(errno));
        return -1;
    } 

    opts |= O_NONBLOCK;

    if (fcntl(sock, F_SETFL, opts) < 0)    
    {
        dbg_print("fcntl() set socket options failure: %s\n", strerror(errno));
        return -1;
    }

    dbg_print("Set socket[%d] none blocking\n", sock);
    return opts;
}

int tcp_sock_connect(char *host, char *port, char *inf, int timeout)
{
    struct addrinfo     hints, *rp;
    struct addrinfo     *res = NULL;
    struct in_addr      inaddr;
    int                 sock = -1;
    int                 rv = -1;
    struct timeval      tv;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /*   Only support IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; /*   TCP protocol */
    hints.ai_flags = AI_PASSIVE;


    /*  If $host is a valid IP address, then don't use name resolution */
    if(host && inet_aton(host, &inaddr))
    {
        hints.ai_flags |= AI_NUMERICHOST;
    }   

    /*  Obtain address(es) matching host/port */
    if( (rv=getaddrinfo(host, port, &hints, &res)) )
    {
        dbg_print("getaddrinfo() parser [%s:%s] failed: %s\n", host, port, gai_strerror(rv));
        rv = -2;
        goto cleanup;
    }   

    /*  getaddrinfo() returns a list of address structures. Try each address until connect successfully*/
    for (rp=res; rp!=NULL; rp=rp->ai_next) 
    {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sock < 0)
        {
            dbg_print("socket() create failed: %s\n", strerror(errno));
            rv = -3;
        }

        sock_bind_inf(sock, inf);
        sock_set_nonblock(sock);

        rv = connect(sock, rp->ai_addr, rp->ai_addrlen);
        if (rv == 0)
        {
            dbg_print("Connect to server [%s:%s] ok\n", host, port);
            goto cleanup;
        }
        else
        {
            if (errno == EINPROGRESS) 
            { 
                fd_set              wfds;
                FD_ZERO(&wfds);
                FD_SET(sock, &wfds);

                tv.tv_sec = timeout;
                tv.tv_usec = 0;

                rv = select(sock+1, NULL, &wfds, NULL, &tv);
                printf("Select return: rv=%d\n", rv);
                if(rv > 0)
                {
                    int           error;
                    socklen_t     len=sizeof(int);

                    getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
                    if(error==0) 
                    {
                        rv=0;
                        dbg_print("select() return ok, connect to server [%s:%s] ok\n", host, port);
                        goto cleanup;
                    }
                    else
                    {
                        rv = -4;
                        dbg_print("getsockopt return error: %s\n", strerror(errno));
                    }

                }
                else
                {
                    rv = -5;
                    dbg_print("Connect to server [%s:%s] timeout or select failure\n", host, port);
                }
            }
            else
            {
                rv = -6; 
                dbg_print("Connect to server [%s:%s] failure: %s\n", host, port, strerror(errno));
                goto cleanup;
            }
        }
    }

cleanup:
    if(res)
        freeaddrinfo(res);
    if(sock > 0)
        close(sock);

    return rv;
}


int split_string_to_value(char *str, char *fmt, ...)
{
    va_list     ap;
    
    int         *iPtr;
    long        *lPtr;
    char        *pcPtr;

    char        delim[2]={*(fmt+2), '\0'};
    char        *result;

    va_start(ap, fmt);
    result = strtok( str, delim );

    while(*fmt)
    {
        switch (*fmt++)
        {
            case 's':  /* string */
                pcPtr = va_arg(ap, char *);
                if(NULL!= result)
                {
                    if(NULL!=pcPtr)
                    {
                        strcpy(pcPtr, result);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;

            case 'd':  /*  int */
                iPtr = va_arg(ap, int *);
                if(NULL!= result)
                {
                    if(NULL!=iPtr)
                    {
                        *iPtr = atoi(result);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;

            case 'l':  /*  long */
                lPtr = va_arg(ap, long *);
                if(NULL!= result)
                {
                    if(NULL!=lPtr)
                    {
                        *lPtr = strtol(result, NULL, 10);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;


            case 'x':  /*  long hex*/
                lPtr = va_arg(ap, long *);
                if(NULL!= result)
                {
                    if(NULL!=lPtr)
                    {
                        *lPtr = strtol(result, NULL, 16);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;
        }
    }

OUT:
    va_end(ap);
    return 0;
}
    
/* Check the network interface $inf exist(return 1) or not(return 0) */
int check_network_inf(char *inf)
{
    char                 dev[64];
    struct stat          buf;

    snprintf(dev, sizeof(dev), "/sys/class/net/%s", inf);
    if( stat(dev, &buf) < 0 )
        return 0;
    else
        return 1;
}

/*  Description: Use ping do network test
 *   Input args: $from: : use which NIC(Network Interface Card: eth0, eth1) or 
 *               source IP address do ping test. if it's NULL, use default route
 *               $ping_ip:  The ping test destination IP address
 *  Output args: NONE
 * Return Value: >=0 ping test packet lost percent, <0: failure
 */
int network_ping_test(char *from, char *ping_ip)
{
    FILE                 *fp;
    char                 cmd[256];
    char                 buf[512];
    int                  lost_percent = 100;
    unsigned long        tx_packets = 0;
    unsigned long        rx_packets = 0;

    memset(cmd, 0, sizeof(cmd));
    if(from) 
        snprintf(cmd, sizeof(cmd), "ping -W 1 -c 5 -s 4 %s -I %s", ping_ip, from);
    else
        snprintf(cmd, sizeof(cmd), "ping -W 1 -c 5 -s 4 %s", ping_ip);

    dbg_print("Ping command: %s\n", cmd);

    if ( !(fp=popen(cmd, "r")) )
    {   
        return -1;
    }   
    
    while (NULL != fgets(buf, sizeof(buf), fp))
    {   
        if (strstr(buf, "transmitted"))
        {
            split_string_to_value(buf, "%l,%l,%d", &tx_packets, &rx_packets, &lost_percent);
            break;
        }
    }

    pclose(fp);
    return lost_percent;
}

