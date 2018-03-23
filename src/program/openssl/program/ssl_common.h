/********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ssl_common.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(04/12/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "04/12/2013 12:14:30 PM"
 *                 
 ********************************************************************************/

#ifndef __SSL_COMMON_H
#define __SSL_COMMON_H

#define MAXBUF                    1024
#define MAX_CLIENT                1024

#define MODE_SSL_SERVER           (1<<0)
#define MODE_SSL_CLIENT           (0<<0)

#define MODE_VERIFY_PEER          (1<<1)
#define MODE_VERIFY_NONE          (0<<1)

#define DEF_SSL_PORT              9000
#define CERT_ROOT                 "../cert/"

#define CA_CERT                   CERT_ROOT"/certs/ca_crt.pem"
#if 0
#define SERVER_CERT               CERT_ROOT"/certs/server_crt.der"
#define CLIENT_CERT               CERT_ROOT"/certs/client_crt.der"
#define CLIENT_KEY                CERT_ROOT"/cert/private/client_key.der"
#define SERVER_KEY                CERT_ROOT"/private/server_key.der"
#else
#define SERVER_CERT               CERT_ROOT"/certs/server_crt.pem"
#define CLIENT_CERT               CERT_ROOT"/certs/client_crt.pem"
#define CLIENT_KEY                CERT_ROOT"/private/client_key.pem"
#define SERVER_KEY                CERT_ROOT"/private/server_key.pem"
#endif


SSL_CTX* init_ssl_context(int mode, char *ca, char *cert, char *key, char *pwd);
int check_certs(SSL * ssl, char *cn);

#endif

