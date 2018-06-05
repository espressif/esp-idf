/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_API_MSG_H
#define LWIP_HDR_API_MSG_H

#include "lwip/opt.h"

#if LWIP_NETCONN || LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */
/* Note: Netconn API is always available when sockets are enabled -
 * sockets are implemented on top of them */

#include <stddef.h> /* for size_t */

#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_MPU_COMPATIBLE
#define API_MSG_M_DEF(m)      m
#define API_MSG_M_DEF_C(t, m) t m
#if LWIP_NETCONN_SEM_PER_THREAD
#define API_MSG_M_DEF_SEM(m)  *m
#else
#define API_MSG_M_DEF_SEM(m)  API_MSG_M_DEF(m)
#endif
#else /* LWIP_MPU_COMPATIBLE */
#define API_MSG_M_DEF(m)      *m
#define API_MSG_M_DEF_C(t, m) const t * m
#define API_MSG_M_DEF_SEM(m)  API_MSG_M_DEF(m)
#endif /* LWIP_MPU_COMPATIBLE */

/* For the netconn API, these values are use as a bitmask! */
#define NETCONN_SHUT_RD   1
#define NETCONN_SHUT_WR   2
#define NETCONN_SHUT_RDWR (NETCONN_SHUT_RD | NETCONN_SHUT_WR)

/* IP addresses and port numbers are expected to be in
 * the same byte order as in the corresponding pcb.
 */
/** This struct includes everything that is necessary to execute a function
    for a netconn in another thread context (mainly used to process netconns
    in the tcpip_thread context to be thread safe). */
struct api_msg_msg {
  /** The netconn which to process - always needed: it includes the semaphore
      which is used to block the application thread until the function finished. */
  struct netconn *conn;
  /** The return value of the function executed in tcpip_thread. */
  err_t err;
  /** Depending on the executed function, one of these union members is used */
  union {
    /** used for lwip_netconn_do_send */
    struct netbuf *b;
    /** used for lwip_netconn_do_newconn */
    struct {
      u8_t proto;
    } n;
    /** used for lwip_netconn_do_bind and lwip_netconn_do_connect */
    struct {
      API_MSG_M_DEF_C(ip_addr_t, ipaddr);
      u16_t port;
    } bc;
    /** used for lwip_netconn_do_getaddr */
    struct {
      ip_addr_t API_MSG_M_DEF(ipaddr);
      u16_t API_MSG_M_DEF(port);
      u8_t local;
    } ad;
    /** used for lwip_netconn_do_write */
    struct {
      const void *dataptr;
      size_t len;
      u8_t apiflags;
#if LWIP_SO_SNDTIMEO
      u32_t time_started;
#endif /* LWIP_SO_SNDTIMEO */
    } w;
    /** used for lwip_netconn_do_recv */
    struct {
      u32_t len;
    } r;
#if LWIP_TCP
    /** used for lwip_netconn_do_close (/shutdown) */
    struct {
      u8_t shut;
#if LWIP_SO_SNDTIMEO || LWIP_SO_LINGER
      u32_t time_started;
#else /* LWIP_SO_SNDTIMEO || LWIP_SO_LINGER */
      u8_t polls_left;
#endif /* LWIP_SO_SNDTIMEO || LWIP_SO_LINGER */
    } sd;
#endif /* LWIP_TCP */
#if LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD)
    /** used for lwip_netconn_do_join_leave_group */
    struct {
      API_MSG_M_DEF_C(ip_addr_t, multiaddr);
      API_MSG_M_DEF_C(ip_addr_t, netif_addr);
      enum netconn_igmp join_or_leave;
    } jl;
#endif /* LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD) */
#if TCP_LISTEN_BACKLOG
    struct {
      u8_t backlog;
    } lb;
#endif /* TCP_LISTEN_BACKLOG */
  } msg;
#if LWIP_NETCONN_SEM_PER_THREAD
  sys_sem_t* op_completed_sem;
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
};

#if LWIP_NETCONN_SEM_PER_THREAD
#define LWIP_API_MSG_SEM(msg)          ((msg)->op_completed_sem)
#else /* LWIP_NETCONN_SEM_PER_THREAD */
#define LWIP_API_MSG_SEM(msg)          (&(msg)->conn->op_completed)
#endif /* LWIP_NETCONN_SEM_PER_THREAD */


/** This struct contains a function to execute in another thread context and
    a struct api_msg_msg that serves as an argument for this function.
    This is passed to tcpip_apimsg to execute functions in tcpip_thread context. */
struct api_msg {
  /** function to execute in tcpip_thread context */
  void (* function)(void *msg);
  /** arguments for this function */
  struct api_msg_msg msg;
};

#if LWIP_DNS
/** As lwip_netconn_do_gethostbyname requires more arguments but doesn't require a netconn,
    it has its own struct (to avoid struct api_msg getting bigger than necessary).
    lwip_netconn_do_gethostbyname must be called using tcpip_callback instead of tcpip_apimsg
    (see netconn_gethostbyname). */
struct dns_api_msg {
  /** Hostname to query or dotted IP address string */
#if LWIP_MPU_COMPATIBLE
  char name[DNS_MAX_NAME_LENGTH];
#else /* LWIP_MPU_COMPATIBLE */
  const char *name;
#endif /* LWIP_MPU_COMPATIBLE */
  /** The resolved address is stored here */
  ip_addr_t API_MSG_M_DEF(addr);
#if LWIP_IPV4 && LWIP_IPV6
  /** Type of resolve call */
  u8_t dns_addrtype;
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  /** This semaphore is posted when the name is resolved, the application thread
      should wait on it. */
  sys_sem_t API_MSG_M_DEF_SEM(sem);
  /** Errors are given back here */
  err_t API_MSG_M_DEF(err);
};
#endif /* LWIP_DNS */

#if LWIP_NETCONN_SEM_PER_THREAD
#if ESP_THREAD_SAFE
#define LWIP_NETCONN_THREAD_SEM_GET() sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE() sys_thread_sem_deinit()
#endif
#endif

#if LWIP_TCPIP_CORE_LOCKING
#ifdef LWIP_DEBUG
#define TCIP_APIMSG_SET_ERR(m, e) (m)->msg.err = e  /* catch functions that don't set err */
#else
#define TCIP_APIMSG_SET_ERR(m, e)
#endif
#if LWIP_NETCONN_SEM_PER_THREAD
#define TCPIP_APIMSG_SET_SEM(m) ((m)->msg.op_completed_sem = LWIP_NETCONN_THREAD_SEM_GET())
#else
#define TCPIP_APIMSG_SET_SEM(m)
#endif
#define TCPIP_APIMSG_NOERR(m,f) do { \
  TCIP_APIMSG_SET_ERR(m, ERR_VAL); \
  TCPIP_APIMSG_SET_SEM(m); \
  LOCK_TCPIP_CORE(); \
  f(&((m)->msg)); \
  UNLOCK_TCPIP_CORE(); \
} while(0)
#define TCPIP_APIMSG(m,f,e)   do { \
  TCPIP_APIMSG_NOERR(m,f); \
  (e) = (m)->msg.err; \
} while(0)
#define TCPIP_APIMSG_ACK(m)   NETCONN_SET_SAFE_ERR((m)->conn, (m)->err)
#else /* LWIP_TCPIP_CORE_LOCKING */
#define TCPIP_APIMSG_NOERR(m,f) do { (m)->function = f; tcpip_apimsg(m); } while(0)
#define TCPIP_APIMSG(m,f,e)   do { (m)->function = f; (e) = tcpip_apimsg(m); } while(0)
#define TCPIP_APIMSG_ACK(m)   do { NETCONN_SET_SAFE_ERR((m)->conn, (m)->err); sys_sem_signal(LWIP_API_MSG_SEM(m)); } while(0)

#endif /* LWIP_TCPIP_CORE_LOCKING */

void lwip_netconn_do_newconn         (void *m);
void lwip_netconn_do_delconn         (void *m);
void lwip_netconn_do_bind            (void *m);
void lwip_netconn_do_connect         (void *m);
void lwip_netconn_do_disconnect      (void *m);
void lwip_netconn_do_listen          (void *m);
void lwip_netconn_do_send            (void *m);
void lwip_netconn_do_recv            (void *m);
void lwip_netconn_do_write           (void *m);
void lwip_netconn_do_getaddr         (void *m);
void lwip_netconn_do_close           (void *m);
void lwip_netconn_do_shutdown        (void *m);
#if LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD)
void lwip_netconn_do_join_leave_group(void *m);
#endif /* LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD) */

#if LWIP_DNS
void lwip_netconn_do_gethostbyname(void *arg);
#endif /* LWIP_DNS */

struct netconn* netconn_alloc(enum netconn_type t, netconn_callback callback);
void netconn_free(struct netconn *conn);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETCONN || LWIP_SOCKET */

#endif /* LWIP_HDR_API_MSG_H */
