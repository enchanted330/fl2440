/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  ssl_common.c
 *    Description:  This file 
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

#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "ssl_common.h"

int check_cert_type(char *cert)
{
    return strstr(cert, ".der") ? SSL_FILETYPE_ASN1 : SSL_FILETYPE_PEM;
}


/* SSL preliminaries. We keep the certificate and key with the context. */
SSL_CTX* init_ssl_context(int mode, char *ca, char *cert, char *key, char *pwd)
{
    SSL_CTX               *ctx;
    int                   rv;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    /* Use SSL V2,V3 method  */
    if( mode & MODE_SSL_SERVER )
    {
        ctx = SSL_CTX_new (SSLv23_server_method());
    }
    else
    {
        ctx = SSL_CTX_new (SSLv23_client_method());
    }
    if(!ctx) 
    {
        printf("SSL_CTX_new failure: %s\n", ERR_reason_error_string(ERR_get_error()) );
        return NULL;
    }

    if( mode & MODE_VERIFY_PEER )
    {
        /* Request to verify peer certification  */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    }

    if(ca)
    {
        if( !SSL_CTX_load_verify_locations(ctx, ca, NULL) )
        {
            printf("Load CA certification %s failure: %s\n", ca, ERR_reason_error_string(ERR_get_error()) );
            goto bad;
        }
        printf("Load CA certificate %s ok\n", ca);
    }

    if(pwd)
    {
        SSL_CTX_set_default_passwd_cb_userdata(ctx, pwd);
    }

    if(cert && key)
    { 
        rv=SSL_CTX_use_certificate_file(ctx, cert, check_cert_type(cert));
        if (rv <= 0) 
        {
            printf("Load certification file %s failure: %s\n", cert, ERR_reason_error_string(ERR_get_error()) );
            goto bad;
        } 
        printf("Load certificate file %s ok\n", cert);
        
        if (SSL_CTX_use_PrivateKey_file(ctx, key, check_cert_type(key)) <= 0) 
        { 
            printf("Load private key file %s failure: %s\n", key, ERR_reason_error_string(ERR_get_error()));
            goto bad;
        } 
        printf("Load private key file %s ok\n", key);
        
        if (!SSL_CTX_check_private_key(ctx)) 
        {
            printf("%s\n", ERR_error_string(ERR_get_error(), NULL)); 
            printf("Private key does not match the certificate public key\n");
            goto bad;
        } 
        printf("Private key and certificate public key matched\n");
    }

    return ctx;

bad:
    if(ctx)
        SSL_CTX_free (ctx);
    return NULL;
}


int check_certs(SSL * ssl, char *cn)
{
    X509        *cert;
    char        *line; 
    SSL_CTX     *ctx = ssl->ctx;
    int         rv = 0;
    
    printf("SSL connection using %s encryption\n", SSL_get_cipher(ssl));

    if( !(ctx->verify_mode&SSL_VERIFY_PEER) )
    {
        printf("Configure don't verify peer certificate\n");
        return 0;
    }

    if(SSL_get_verify_result(ssl)!=X509_V_OK)
    {
        printf("SSL_get_verify_result check failure: %s\n", ERR_reason_error_string(ERR_get_error()) );
        return -1;
    }

    cert = SSL_get_peer_certificate(ssl);
    if(!cert)
    {
        printf ("Peer does not have certificate.\n");
        rv = -1;
        goto out;
    }
    
    printf("Peer Certificate file information:\n"); 
    
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    if(line)
    {
        printf("Subject: %s\n", line);
        free(line);
    } 
    
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    if(line)
    {
        printf("Issuer: %s\n", line);
        free(line);
    }

    if(cn)
    {
        char        peer_cn[256];    /* Peer Common Name */
        memset(peer_cn, 0, sizeof(peer_cn));
        X509_NAME_get_text_by_NID (X509_get_subject_name(cert), NID_commonName, peer_cn, sizeof(peer_cn));
        if(strcmp(cn, peer_cn))
        {
            printf("Common Name matched: \"%s\"<=>\"%s\"\n", cn, peer_cn);
            rv = -2;
            goto out;
        }
        printf("Common Name matched: \"%s\"\n", peer_cn);
    }

out:
    if(cert)
        X509_free(cert);

    return rv;
}


