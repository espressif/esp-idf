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

#include "ping/ping.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"

#if PING_USE_SOCKETS
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "esp_task.h"
#include "ping/ping_sock.h"
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

#define PING_TIME_DIFF_MS(_end, _start)  ((uint32_t)(((_end).tv_sec - (_start).tv_sec) * 1000 + (_end.tv_usec - _start.tv_usec)/1000))
#define PING_TIME_DIFF_SEC(_end, _start) ((uint32_t)((_end).tv_sec - (_start).tv_sec))

#if PING_USE_SOCKETS
/* ping handle */
static esp_ping_handle_t ping = NULL;
#else
static struct raw_pcb *ping_pcb;
static u16_t ping_seq_num;
static struct timeval ping_time;

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

/**
 *
 * Re-implement ping_init and ping_deinit with the APIs from ping_sock.h for back-ward compatibility sake.
 * It's highly recommended that users should use the new APIs from ping_sock.h.
 * ToDo: ping.h and esp_ping.h are deprecated now and should be removed in idf v5.x.
 *
 */

static void
on_ping_success(esp_ping_handle_t hdl, void *args)
{
  uint32_t elapsed_time, recv_len;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
  esp_ping_result(PING_RES_OK, recv_len, elapsed_time);
}

static void
on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
  uint32_t elapsed_time, recv_len;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
  esp_ping_result(PING_RES_TIMEOUT, recv_len, elapsed_time);
}

static void
on_ping_end(esp_ping_handle_t hdl, void *args)
{
  esp_ping_result(PING_RES_FINISH, 0, 0);
  esp_ping_delete_session(hdl);
}

int
ping_init(void)
{
#if PING_USE_SOCKETS
  uint32_t tos = 0;
  uint32_t ping_timeout = PING_RCV_TIMEO;
  uint32_t ping_delay = PING_DELAY;
  uint32_t ping_count_max = 3;
  uint32_t interface = 0;
  ip_addr_t ipaddr;

  esp_ping_get_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count_max, sizeof(ping_count_max));
  esp_ping_get_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(ping_timeout));
  esp_ping_get_target(PING_TARGET_DELAY_TIME, &ping_delay, sizeof(ping_delay));
  esp_ping_get_target(PING_TARGET_IP_ADDRESS, &ipaddr, sizeof(ip_addr_t));
  esp_ping_get_target(PING_TARGET_IP_TOS, &tos, sizeof(tos));
  esp_ping_get_target(PING_TARGET_IF_INDEX, &interface, sizeof(interface));

  esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
  config.count = ping_count_max;
  config.timeout_ms = ping_timeout;
  config.interval_ms = ping_delay;
  config.target_addr = ipaddr;
  config.tos = tos;
  config.interface = interface;

  esp_ping_callbacks_t cbs = {
    .on_ping_end = on_ping_end,
    .on_ping_success = on_ping_success,
    .on_ping_timeout = on_ping_timeout,
  };

  esp_ping_new_session(&config, &cbs, &ping);
  esp_ping_start(ping);
#else /* PING_USE_SOCKETS */
  ping_raw_init();
#endif /* PING_USE_SOCKETS */
  return ERR_OK;
}

void
ping_deinit(void)
{
  esp_ping_stop(ping);
  esp_ping_delete_session(ping);
}

#endif /* LWIP_IPV4 && LWIP_RAW */
