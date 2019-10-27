/*
 *  TCP/IP or UDP/IP networking functions
 *  modified for LWIP support on ESP32
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2015 Angus Gratton
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_NET_C)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free      free
#define mbedtls_time      time
#define mbedtls_time_t    time_t
#endif

#include "mbedtls/net_sockets.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

/*
 * Prepare for using the sockets interface
 */
static int net_prepare( void )
{
    return ( 0 );
}

/*
 * Initialize a context
 */
void mbedtls_net_init( mbedtls_net_context *ctx )
{
    ctx->fd = -1;
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

    if ( ( ret = net_prepare() ) != 0 ) {
        return ( ret );
    }

    /* Do name resolution with both IPv6 and IPv4 */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if ( getaddrinfo( host, port, &hints, &addr_list ) != 0 ) {
        return ( MBEDTLS_ERR_NET_UNKNOWN_HOST );
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        int fd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );

        if ( fd < 0 ) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        if ( connect( fd, cur->ai_addr, cur->ai_addrlen ) == 0 ) {
            ctx->fd = fd; // connected!
            ret = 0;
            break;
        }

        close( fd );
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    freeaddrinfo( addr_list );

    return ( ret );
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind( mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto )
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;
    struct sockaddr_in *serv_addr = NULL;
#if SO_REUSE
    int n = 1;
#endif

    if ( ( ret = net_prepare() ) != 0 ) {
        return ( ret );
    }

    /* Bind to IPv6 and/or IPv4, but only in the desired protocol */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if ( getaddrinfo( bind_ip, port, &hints, &addr_list ) != 0 ) {
        return ( MBEDTLS_ERR_NET_UNKNOWN_HOST );
    }

    /* Try the sockaddrs until a binding succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        int fd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
        if ( fd < 0 ) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        /*SO_REUSEADDR option dafault is disable in source code(lwip)*/
#if SO_REUSE
        if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
                         (const char *) &n, sizeof( n ) ) != 0 ) {
            close( fd );
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }
#endif
        /*bind interface dafault don't process the addr is 0xffffffff for TCP Protocol*/
        serv_addr = (struct sockaddr_in *)cur->ai_addr;
        serv_addr->sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        if ( bind( fd, (struct sockaddr *)serv_addr, cur->ai_addrlen ) != 0 ) {
            close( fd );
            ret = MBEDTLS_ERR_NET_BIND_FAILED;
            continue;
        }

        /* Listen only makes sense for TCP */
        if ( proto == MBEDTLS_NET_PROTO_TCP ) {
            if ( listen( fd, MBEDTLS_NET_LISTEN_BACKLOG ) != 0 ) {
                close( fd );
                ret = MBEDTLS_ERR_NET_LISTEN_FAILED;
                continue;
            }
        }

        /* I we ever get there, it's a success */
        ctx->fd = fd;
        ret = 0;
        break;
    }

    freeaddrinfo( addr_list );

    return ( ret );

}

/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
static int net_would_block( const mbedtls_net_context *ctx )
{
    int error = errno;

    switch ( errno = error ) {
#if defined EAGAIN
    case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
        return ( 1 );
    }
    return ( 0 );
}

/*
 * Accept a connection from a remote client
 */
int mbedtls_net_accept( mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len )
{
    int ret;
    int type;

    struct sockaddr_in client_addr;

    socklen_t n = (socklen_t) sizeof( client_addr );
    socklen_t type_len = (socklen_t) sizeof( type );

    /* Is this a TCP or UDP socket? */
    if ( getsockopt( bind_ctx->fd, SOL_SOCKET, SO_TYPE,
                     (void *) &type, (socklen_t *) &type_len ) != 0 ||
            ( type != SOCK_STREAM && type != SOCK_DGRAM ) ) {
        return ( MBEDTLS_ERR_NET_ACCEPT_FAILED );
    }

    if ( type == SOCK_STREAM ) {
        /* TCP: actual accept() */
        ret = client_ctx->fd = (int) accept( bind_ctx->fd,
                                             (struct sockaddr *) &client_addr, &n );
    } else {
        /* UDP: wait for a message, but keep it in the queue */
        char buf[1] = { 0 };

        ret = recvfrom( bind_ctx->fd, buf, sizeof( buf ), MSG_PEEK,
                        (struct sockaddr *) &client_addr, &n );

    }

    if ( ret < 0 ) {
        if ( net_would_block( bind_ctx ) != 0 ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }

        return ( MBEDTLS_ERR_NET_ACCEPT_FAILED );
    }

    /* UDP: hijack the listening socket to communicate with the client,
     * then bind a new socket to accept new connections */
    if ( type != SOCK_STREAM ) {
        struct sockaddr_in local_addr;
        int one = 1;

        if ( connect( bind_ctx->fd, (struct sockaddr *) &client_addr, n ) != 0 ) {
            return ( MBEDTLS_ERR_NET_ACCEPT_FAILED );
        }

        client_ctx->fd = bind_ctx->fd;
        bind_ctx->fd   = -1; /* In case we exit early */

        n = sizeof( struct sockaddr_in );
        if ( getsockname( client_ctx->fd,
                          (struct sockaddr *) &local_addr, &n ) != 0 ||
                ( bind_ctx->fd = (int) socket( AF_INET,
                                               SOCK_DGRAM, IPPROTO_UDP ) ) < 0 ||
                setsockopt( bind_ctx->fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &one, sizeof( one ) ) != 0 ) {
            return ( MBEDTLS_ERR_NET_SOCKET_FAILED );
        }

        if ( bind( bind_ctx->fd, (struct sockaddr *) &local_addr, n ) != 0 ) {
            return ( MBEDTLS_ERR_NET_BIND_FAILED );
        }
    }

    if ( client_ip != NULL ) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *) &client_addr;
        *ip_len = sizeof( addr4->sin_addr.s_addr );

        if ( buf_size < *ip_len ) {
            return ( MBEDTLS_ERR_NET_BUFFER_TOO_SMALL );
        }

        memcpy( client_ip, &addr4->sin_addr.s_addr, *ip_len );
    }

    return ( 0 );
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    return ( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) & ~O_NONBLOCK ) );
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    return ( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) | O_NONBLOCK ) );
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
    struct timeval tv;
    tv.tv_sec  = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    select( 0, NULL, NULL, NULL, &tv );
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) read( fd, buf, len );

    if ( ret < 0 ) {
        if ( net_would_block( ctx ) != 0 ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }

        if ( errno == EPIPE || errno == ECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }

        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }

        return ( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return ( ret );
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                              uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if ( ret == 0 ) {
        return ( MBEDTLS_ERR_SSL_TIMEOUT );
    }

    if ( ret < 0 ) {
        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }

        return ( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return ( mbedtls_net_recv( ctx, buf, len ) );
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) write( fd, buf, len );

    if ( ret < 0 ) {
        if ( net_would_block( ctx ) != 0 ) {
            return ( MBEDTLS_ERR_SSL_WANT_WRITE );
        }

        if ( errno == EPIPE || errno == ECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }

        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_WRITE );
        }

        return ( MBEDTLS_ERR_NET_SEND_FAILED );
    }

    return ( ret );
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free( mbedtls_net_context *ctx )
{
    if ( ctx->fd == -1 ) {
        return;
    }

    shutdown( ctx->fd, 2 );
    close( ctx->fd );

    ctx->fd = -1;
}

#endif /* MBEDTLS_NET_C */
