/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sys/poll.h"
#include "lwip/sockets.h"
#include "netdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_ESP_NETIF_TCPIP_LWIP_ORIG) || defined(CONFIG_ESP_NETIF_TCPIP_LWIP)

// lwIP's socket API is mostly implemented as macros,
// but ASIO uses the same symbols in different namespaces, where macros wouldn't work.
// Here we have to undefined the symbols for ASIO build and declare as standard functions
#undef getaddrinfo
#undef gethostbyname_r
#undef gethostbyname
#undef freeaddrinfo
#undef accept
#undef bind
#undef shutdown
#undef getpeername
#undef getsockname
#undef setsockopt
#undef setsockopt
#undef getsockopt
#undef closesocket
#undef connect
#undef listen
#undef recvmsg
#undef recv
#undef recvfrom
#undef sendmsg
#undef send
#undef sendto
#undef socket
#undef inet_ntop
#undef inet_pton
#undef poll
#undef select

static inline int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop)
{ return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop); }
static inline struct hostent *gethostbyname(const char *name)
{ return lwip_gethostbyname(name); }
static inline void freeaddrinfo(struct addrinfo *ai)
{ lwip_freeaddrinfo(ai); }
static inline int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
{ return lwip_getaddrinfo(nodename, servname, hints, res); }
static inline int accept(int s,struct sockaddr *addr,socklen_t *addrlen)
{ return lwip_accept(s,addr,addrlen); }
static inline int bind(int s,const struct sockaddr *name, socklen_t namelen)
{ return lwip_bind(s,name,namelen); }
static inline int shutdown(int s,int how)
{ return lwip_shutdown(s,how); }
static inline int getpeername(int s,struct sockaddr *name,socklen_t *namelen)
{ return lwip_getpeername(s,name,namelen); }
static inline int getsockname(int s,struct sockaddr *name,socklen_t *namelen)
{ return lwip_getsockname(s,name,namelen); }
static inline int setsockopt(int s,int level,int optname,const void *opval,socklen_t optlen)
{ return lwip_setsockopt(s,level,optname,opval,optlen); }
static inline int getsockopt(int s,int level,int optname,void *opval,socklen_t *optlen)
{ return lwip_getsockopt(s,level,optname,opval,optlen); }
static inline int closesocket(int s)
{ return lwip_close(s); }
static inline int connect(int s,const struct sockaddr *name,socklen_t namelen)
{ return lwip_connect(s,name,namelen); }
static inline int listen(int s,int backlog)
{ return lwip_listen(s,backlog); }
static inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{ return lwip_recvmsg(sockfd, msg, flags); }
static inline ssize_t recv(int s,void *mem,size_t len,int flags)
{ return lwip_recv(s,mem,len,flags); }
static inline ssize_t recvfrom(int s,void *mem,size_t len,int flags,struct sockaddr *from,socklen_t *fromlen)
{ return lwip_recvfrom(s,mem,len,flags,from,fromlen); }
static inline ssize_t send(int s,const void *dataptr,size_t size,int flags)
{ return lwip_send(s,dataptr,size,flags); }
static inline ssize_t sendmsg(int s,const struct msghdr *message,int flags)
{ return lwip_sendmsg(s,message,flags); }
static inline ssize_t sendto(int s,const void *dataptr,size_t size,int flags,const struct sockaddr *to,socklen_t tolen)
{ return lwip_sendto(s,dataptr,size,flags,to,tolen); }
static inline int socket(int domain,int type,int protocol)
{ return lwip_socket(domain,type,protocol); }
static inline const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{ return lwip_inet_ntop(af, src, dst, size);  }
static inline int inet_pton(int af, const char *src, void *dst)
{ return lwip_inet_pton(af, src, dst); }
#endif // CONFIG_ESP_NETIF_TCPIP_LWIP_ORIG || CONFIG_ESP_NETIF_TCPIP_LWIP

#ifdef __cplusplus
}
#endif
