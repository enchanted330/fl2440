/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ssl_api.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(02/03/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/03/2014 09:42:47 PM"
 *                 
 ********************************************************************************/

#include "ssl_api.h"


void init_ssl(void)
{
    SSL_library_init();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

int check_cert_type(char *cert_file)
{
    return strstr(cert_file, ".der") ? SSL_FILETYPE_ASN1 : SSL_FILETYPE_PEM;
}


/* SSL preliminaries. We keep the certificate and key with the context. */
SSL* create_ssl(int mode, char *ca_file, char *cert_file, char *key_file, char *pwd)
{
    SSL_CTX               *ctx = NULL;
    SSL                   *ssl = NULL;
    int                   rv;


    printf("Create SSL with: ca:%s cert:%s key:%s pwd:%s\n", ca_file, cert_file, key_file, pwd);

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
    if(ca_file)
    {
        if( !SSL_CTX_load_verify_locations(ctx, ca_file, NULL) )
        {
            printf("Load CA certification %s failure: %s\n", ca_file, ERR_reason_error_string(ERR_get_error()) );
            goto bad;
        }
        printf("Load CA certificate %s ok\n", ca_file);
    }

    if(pwd)
    {
        SSL_CTX_set_default_passwd_cb_userdata(ctx, pwd);
    }

    if(cert_file && key_file)
    {
        rv=SSL_CTX_use_certificate_file(ctx, cert_file, check_cert_type(cert_file));
        if (rv <= 0)
        {
            printf("Load certification file %s failure: %s\n", cert_file, ERR_reason_error_string(ERR_get_error()) );
            goto bad;
        }
        printf("Load certificate file %s ok\n", cert_file);

        if (SSL_CTX_use_PrivateKey_file(ctx, key_file, check_cert_type(key_file)) <= 0)
        {
            printf("Load private key file %s failure: %s\n", key_file, ERR_reason_error_string(ERR_get_error()));
            goto bad;
        }
        printf("Load private key file %s ok\n", key_file);

        if (!SSL_CTX_check_private_key(ctx))
        {
            printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
            printf("Private key does not match the certificate public key\n");
            goto bad;
        }
        printf("Private key and certificate public key matched\n");
    }

    ssl = SSL_new(ctx);
    if(!ssl)
    {
        printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
        goto bad;
    }

    return ssl;
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
            printf("Common Name not matched: \"%s\"<=>\"%s\"\n", cn, peer_cn);
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

