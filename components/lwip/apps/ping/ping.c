/**
 * @file
 * Ping sender module
 *
 */

/*
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
 */

/** 
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 *
 */

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "ping.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"

#if PING_USE_SOCKETS
#include "lwip/sockets.h"
#include "lwip/inet.h"
#endif /* PING_USE_SOCKETS */

#ifdef ESP_PING
#include "esp_ping.h"
#include "lwip/ip_addr.h"
#endif
/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_OFF
#endif

/** ping target - should be an "ip4_addr_t" */
#ifndef PING_TARGET
#define PING_TARGET   (netif_default ? *netif_ip4_gw(netif_default) : (*IP4_ADDR_ANY))
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

/* ping variables */
static u16_t ping_seq_num;
static struct timeval ping_time;
#if ESP_PING
static sys_sem_t ping_sem = NULL;
static bool ping_init_flag = false;
#endif
#if !PING_USE_SOCKETS
static struct raw_pcb *ping_pcb;
#endif /* PING_USE_SOCKETS */

#define PING_TIME_DIFF_MS(_end, _start)  ((uint32_t)(((_end).tv_sec - (_start).tv_sec) * 1000 + (_end.tv_usec - _start.tv_usec)/1000))
#define PING_TIME_DIFF_SEC(_end, _start) ((uint32_t)((_end).tv_sec - (_start).tv_sec))

/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id     = PING_ID;
  iecho->seqno  = htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < data_len; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  iecho->chksum = inet_chksum(iecho, len);
}

#if PING_USE_SOCKETS

/* Ping using the socket ip */
static err_t
ping_send(int s, ip_addr_t *addr)
{
  int err;
  struct icmp_echo_hdr *iecho;
  struct sockaddr_in to;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
  LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);
  LWIP_ASSERT("ping: expect IPv4 address", !IP_IS_V6(addr));

  iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
  if (!iecho) {
    return ERR_MEM;
  }

  ping_prepare_echo(iecho, (u16_t)ping_size);

  to.sin_len = sizeof(to);
  to.sin_family = AF_INET;
  inet_addr_from_ipaddr(&to.sin_addr, ip_2_ip4(addr));

  err = sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

  mem_free(iecho);

  return (err ? ERR_OK : ERR_VAL);
}

static void
ping_recv(int s)
{
  char buf[64];
  int len;
  struct sockaddr_in from;
  struct ip_hdr *iphdr;
  struct icmp_echo_hdr *iecho;
  int fromlen = sizeof(from);
  struct timeval now;

  while((len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
    if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) {
      if (from.sin_family != AF_INET) {
        /* Ping is not IPv4 */ 
        LWIP_DEBUGF( PING_DEBUG, ("ping: invalid sin_family\n"));
      } else {
        ip4_addr_t fromaddr;
        inet_addr_to_ipaddr(&fromaddr, &from.sin_addr);
        iphdr = (struct ip_hdr *)buf;
        iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
 
        LWIP_DEBUGF( PING_DEBUG, ("ping: recv seq=%d ", ntohs(iecho->seqno)));
        ip4_addr_debug_print(PING_DEBUG, &fromaddr);
        gettimeofday(&now, NULL);
        LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\n", PING_TIME_DIFF_MS(now, ping_time)));

        if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
          /* do some ping result processing */
#ifdef ESP_PING
          esp_ping_result((ICMPH_TYPE(iecho) == ICMP_ER), len, PING_TIME_DIFF_MS(now, ping_time));
#else
          PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
#endif
          return;
        } else {
          LWIP_DEBUGF( PING_DEBUG, ("ping: drop\n"));
        }
      }
    }
    fromlen = sizeof(from);
  }

  gettimeofday(&now, NULL);
  if (len == 0) {
    LWIP_DEBUGF( PING_DEBUG, ("ping: recv - %"U32_F" ms - timeout\n", PING_TIME_DIFF_MS(now, ping_time)));
  }

  /* do some ping result processing */
#ifdef ESP_PING
  esp_ping_result(0, len, PING_TIME_DIFF_MS(now, ping_time));
#else
  PING_RESULT(0);
#endif
}

static void
ping_thread(void *arg)
{
  uint32_t ping_timeout = PING_RCV_TIMEO;
  uint32_t ping_delay = PING_DELAY;
  ip_addr_t ping_target;
  int ret;
  int s;

#ifdef ESP_PING
  uint32_t ping_count_cur = 0;
  uint32_t ping_count_max = 3;
  ip4_addr_t ipaddr;
  int lev;

  esp_ping_get_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count_max, sizeof(ping_count_max));
  esp_ping_get_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(ping_timeout));
  esp_ping_get_target(PING_TARGET_DELAY_TIME, &ping_delay, sizeof(ping_delay));
  esp_ping_get_target(PING_TARGET_IP_ADDRESS, &ipaddr.addr, sizeof(uint32_t));
  ip_addr_copy_from_ip4(ping_target, ipaddr);
#else
  ip_addr_copy_from_ip4(ping_target, PING_TARGET);
#endif

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
  int timeout = ping_timeout;
#else
  struct timeval timeout;
  timeout.tv_sec = ping_timeout/1000;
  timeout.tv_usec = (ping_timeout%1000)*1000;
#endif

  LWIP_UNUSED_ARG(arg);

  if ((s = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
    goto _exit_new_socket_failed;
  }

  ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  LWIP_ASSERT("setting receive timeout failed", ret == 0);
  LWIP_UNUSED_ARG(ret);

  while (1) {
#ifdef ESP_PING
    if (ping_count_cur++ >= ping_count_max) {
      goto _exit;
    }

    if (ping_init_flag == false) {
      goto _exit;
    }
#endif    
    if (ping_send(s, &ping_target) == ERR_OK) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send seq=%d ", ping_seq_num));
      ip_addr_debug_print(PING_DEBUG, &ping_target);
      LWIP_DEBUGF( PING_DEBUG, ("\n"));

      gettimeofday(&ping_time, NULL);
      ping_recv(s);
    } else {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      ip_addr_debug_print(PING_DEBUG, &ping_target);
      LWIP_DEBUGF( PING_DEBUG, (" - error\n"));
    }
    sys_msleep(ping_delay);
  }

#ifdef ESP_PING
_exit:
  close(s);

_exit_new_socket_failed:
  esp_ping_result(PING_RES_FINISH, 0, 0);
  SYS_ARCH_PROTECT(lev);
  if (ping_init_flag) { /* Ping closed by this thread */
    LWIP_DEBUGF( PING_DEBUG, ("ping: closed by self "));
    if (ping_sem) {
      sys_sem_free(&ping_sem);
    }
    ping_sem = NULL;
    ping_init_flag = false;
    SYS_ARCH_UNPROTECT(lev);
  } else { /* Ping closed by task calls ping_deinit */
    LWIP_DEBUGF( PING_DEBUG, ("ping: closed by other"));
    SYS_ARCH_UNPROTECT(lev);
    if (ping_sem) {
      sys_sem_signal(&ping_sem);
    }
  }
  vTaskDelete(NULL);
#endif
}

#else /* PING_USE_SOCKETS */

/* Ping using the raw ip */
static u8_t
ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
  struct icmp_echo_hdr *iecho;
  struct timeval now;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_ASSERT("p != NULL", p != NULL);

  if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
      pbuf_header(p, -PBUF_IP_HLEN) == 0) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
      ip_addr_debug_print(PING_DEBUG, addr);
      gettimeofday(&now, NULL);
      LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\n", PING_TIME_DIFF_MS(now, ping_time)));

      /* do some ping result processing */
      PING_RESULT(1);
      pbuf_free(p);
      return 1; /* eat the packet */
    }
    /* not eaten, restore original packet */
    pbuf_header(p, PBUF_IP_HLEN);
  }

  return 0; /* don't eat the packet */
}

static void
ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
  struct pbuf *p;
  struct icmp_echo_hdr *iecho;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

  LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
  ip_addr_debug_print(PING_DEBUG, addr);
  LWIP_DEBUGF( PING_DEBUG, ("\n"));
  LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    return;
  }
  if ((p->len == p->tot_len) && (p->next == NULL)) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    ping_prepare_echo(iecho, (u16_t)ping_size);

    raw_sendto(raw, p, addr);
    ping_time = system_get_time();
  }
  pbuf_free(p);
}

static void
ping_timeout(void *arg)
{
  struct raw_pcb *pcb = (struct raw_pcb*)arg;
  ip_addr_t ping_target;

  LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

  ip_addr_copy_from_ip4(ping_target, PING_TARGET);
  ping_send(pcb, &ping_target);
  sys_timeout(PING_DELAY, ping_timeout, pcb);
}

static void
ping_raw_init(void)
{
  ping_pcb = raw_new(IP_PROTO_ICMP);
  LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

  raw_recv(ping_pcb, ping_recv, NULL);
  raw_bind(ping_pcb, IP_ADDR_ANY);
  sys_timeout(PING_DELAY, ping_timeout, ping_pcb);
}

void
ping_send_now(void)
{
  ip_addr_t ping_target;
  LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
  ip_addr_copy_from_ip4(ping_target, PING_TARGET);
  ping_send(ping_pcb, &ping_target);
}

#endif /* PING_USE_SOCKETS */

int
ping_init(void)
{
  int ret;
  int lev;

  SYS_ARCH_PROTECT(lev);
  if (ping_init_flag) {
    SYS_ARCH_UNPROTECT(lev);
    /* Currently we only support one ping, call ping_deinit to kill the running ping before start new ping */
    return ERR_INPROGRESS;
  }
  ret = sys_sem_new(&ping_sem, 0);
  if (ERR_OK != ret) {
    SYS_ARCH_UNPROTECT(lev);
    return ERR_MEM;
  }
  ping_init_flag = true;
  SYS_ARCH_UNPROTECT(lev);
#if PING_USE_SOCKETS
  sys_thread_new("ping_thread", ping_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
#else /* PING_USE_SOCKETS */
  ping_raw_init();
#endif /* PING_USE_SOCKETS */
  return ERR_OK;
}

void
ping_deinit(void)
{
  int lev;

  SYS_ARCH_PROTECT(lev);
  if (ping_init_flag == false) {
    SYS_ARCH_UNPROTECT(lev);
    return;
  }

  ping_init_flag = false;
  SYS_ARCH_UNPROTECT(lev);
  if (ping_sem) {
    sys_sem_wait(&ping_sem);

    SYS_ARCH_PROTECT(lev);
    sys_sem_free(&ping_sem);
    ping_sem = NULL;
    SYS_ARCH_UNPROTECT(lev);
  }
}

#endif /* LWIP_IPV4 && LWIP_RAW */
