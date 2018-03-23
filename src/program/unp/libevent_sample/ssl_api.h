/********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ssl_api.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(02/03/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/03/2014 09:51:28 PM"
 *                 
 ********************************************************************************/

#ifndef  _ssl_api_H_
#define  _ssl_api_H_

#include <openssl/ssl.h>
#include <openssl/err.h>

#define CERT_PAH                  "./certs/"
#define CA_FILE                   CERT_PAH"ca_crt.pem"
#define CLI_CRT_FILE              CERT_PAH"client_crt.pem"
#define CLI_KEY_FILE              CERT_PAH"client_key.pem"
#define SRV_CRT_FILE              CERT_PAH"server_crt.pem"
#define SRV_KEY_FILE              CERT_PAH"server_key.pem"

#define SSL_NONE                  0
#define SSL_1WAY                  1
#define SSL_2WAY                  2

#define MODE_SSL_SERVER           (1<<0)
#define MODE_SSL_CLIENT           (0<<0)
#define MODE_VERIFY_PEER          (1<<1)
#define MODE_VERIFY_NONE          (0<<1)

extern void init_ssl(void);
extern int check_cert_type(char *cert_file);
extern int check_certs(SSL * ssl, char *cn);
SSL* create_ssl(int mode, char *ca_file, char *cert_file, char *key_file, char *pwd);


#endif   /* ----- #ifndef _ssl_api_H_  ----- */


