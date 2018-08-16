/**
 * @file
 * Transmission Control Protocol for IP
 *
 * This file contains common functions for the TCP implementation, such as functinos
 * for manipulating the data structures and the TCP timer functions. TCP functions
 * related to input and output is found in tcp_in.c and tcp_out.c respectively.
 *
 */

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

#include "lwip/opt.h"

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/nd6.h"

#include <string.h>

#ifndef TCP_LOCAL_PORT_RANGE_START
/* From http://www.iana.org/assignments/port-numbers:
   "The Dynamic and/or Private Ports are those from 49152 through 65535" */
#define TCP_LOCAL_PORT_RANGE_START        0xc000
#define TCP_LOCAL_PORT_RANGE_END          0xffff
#define TCP_ENSURE_LOCAL_PORT_RANGE(port) ((u16_t)(((port) & ~TCP_LOCAL_PORT_RANGE_START) + TCP_LOCAL_PORT_RANGE_START))
#endif

#if LWIP_TCP_KEEPALIVE
#define TCP_KEEP_DUR(pcb)   ((pcb)->keep_cnt * (pcb)->keep_intvl)
#define TCP_KEEP_INTVL(pcb) ((pcb)->keep_intvl)
#else /* LWIP_TCP_KEEPALIVE */
#define TCP_KEEP_DUR(pcb)   TCP_MAXIDLE
#define TCP_KEEP_INTVL(pcb) TCP_KEEPINTVL_DEFAULT
#endif /* LWIP_TCP_KEEPALIVE */

const char * const tcp_state_str[] = {
  "CLOSED",
  "LISTEN",
  "SYN_SENT",
  "SYN_RCVD",
  "ESTABLISHED",
  "FIN_WAIT_1",
  "FIN_WAIT_2",
  "CLOSE_WAIT",
  "CLOSING",
  "LAST_ACK",
  "TIME_WAIT"
};


/* last local TCP port */
static s16_t tcp_port = TCP_LOCAL_PORT_RANGE_START;

/* Incremented every coarse grained timer shot (typically every 500 ms). */
u32_t tcp_ticks;

const u8_t tcp_backoff[13] = { 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7};
 /* Times per slowtmr hits */
const u8_t tcp_persist_backoff[7] = { 3, 6, 12, 24, 48, 96, 120 };


/* The TCP PCB lists. */

/** List of all TCP PCBs bound but not yet (connected || listening) */
struct tcp_pcb *tcp_bound_pcbs;
/** List of all TCP PCBs in LISTEN state */
union tcp_listen_pcbs_t tcp_listen_pcbs;
/** List of all TCP PCBs that are in a state in which
 * they accept or send data. */
struct tcp_pcb *tcp_active_pcbs;
/** List of all TCP PCBs in TIME-WAIT state */
struct tcp_pcb *tcp_tw_pcbs;

/** An array with all (non-temporary) PCB lists, mainly used for smaller code size */
struct tcp_pcb ** const tcp_pcb_lists[] = {&tcp_listen_pcbs.pcbs, &tcp_bound_pcbs,
        &tcp_active_pcbs, &tcp_tw_pcbs};

u8_t tcp_active_pcbs_changed;

/** Timer counter to handle calling slow-timer from tcp_tmr() */
static u8_t tcp_timer;
static u8_t tcp_timer_ctr;
static u16_t tcp_new_port(void);

/**
 * Initialize this module.
 */
void
tcp_init(void)
{
#if LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS && defined(LWIP_RAND)
  tcp_port = TCP_ENSURE_LOCAL_PORT_RANGE(LWIP_RAND());
#endif /* LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS && defined(LWIP_RAND) */
}

/**
 * Called periodically to dispatch TCP timers.
 */
void
tcp_tmr(void)
{
  /* Call tcp_fasttmr() every 250 ms */
  tcp_fasttmr();

  if (++tcp_timer & 1) {
    /* Call tcp_slowtmr() every 500 ms, i.e., every other timer
       tcp_tmr() is called. */
    tcp_slowtmr();
  }
}

#if LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG
/** Called when a listen pcb is closed. Iterates one pcb list and removes the
 * closed listener pcb from pcb->listener if matching.
 */
static void
tcp_remove_listener(struct tcp_pcb *list, struct tcp_pcb_listen *lpcb)
{
  struct tcp_pcb *pcb;
  for (pcb = list; pcb != NULL; pcb = pcb->next) {
    if (pcb->listener == lpcb) {
      pcb->listener = NULL;
    }
  }
}
#endif

void
tcp_set_fin_wait_1(struct tcp_pcb *pcb)
{
  pcb->state = FIN_WAIT_1;
#if ESP_LWIP
  pcb->tmr = tcp_ticks;
#endif
}

/** Called when a listen pcb is closed. Iterates all pcb lists and removes the
 * closed listener pcb from pcb->listener if matching.
 */
static void
tcp_listen_closed(struct tcp_pcb *pcb)
{
#if LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG
  size_t i;
  LWIP_ASSERT("pcb != NULL", pcb != NULL);
  LWIP_ASSERT("pcb->state == LISTEN", pcb->state == LISTEN);
  for (i = 1; i < LWIP_ARRAYSIZE(tcp_pcb_lists); i++) {
    tcp_remove_listener(*tcp_pcb_lists[i], (struct tcp_pcb_listen *)pcb);
  }
#endif
  LWIP_UNUSED_ARG(pcb);
}

#if TCP_LISTEN_BACKLOG
/** @ingroup tcp_raw
 * Delay accepting a connection in respect to the listen backlog:
 * the number of outstanding connections is increased until
 * tcp_backlog_accepted() is called.
 *
 * ATTENTION: the caller is responsible for calling tcp_backlog_accepted()
 * or else the backlog feature will get out of sync!
 *
 * @param pcb the connection pcb which is not fully accepted yet
 */
void
tcp_backlog_delayed(struct tcp_pcb *pcb)
{
  LWIP_ASSERT("pcb != NULL", pcb != NULL);
  if ((pcb->flags & TF_BACKLOGPEND) == 0) { 
    if (pcb->listener != NULL) {
      pcb->listener->accepts_pending++;
      LWIP_ASSERT("accepts_pending != 0", pcb->listener->accepts_pending != 0);
      pcb->flags |= TF_BACKLOGPEND;
    }    
  }
}

/** @ingroup tcp_raw
 * A delayed-accept a connection is accepted (or closed/aborted): decreases
 * the number of outstanding connections after calling tcp_backlog_delayed().
 *
 * ATTENTION: the caller is responsible for calling tcp_backlog_accepted()
 * or else the backlog feature will get out of sync!
 *
 * @param pcb the connection pcb which is now fully accepted (or closed/aborted)
 */
void
tcp_backlog_accepted(struct tcp_pcb *pcb)
{
  LWIP_ASSERT("pcb != NULL", pcb != NULL);
  if ((pcb->flags & TF_BACKLOGPEND) != 0) {
    if (pcb->listener != NULL) {
      LWIP_ASSERT("accepts_pending != 0", pcb->listener->accepts_pending != 0);
      pcb->listener->accepts_pending--;
      pcb->flags &= ~TF_BACKLOGPEND;
    }
  }
}
#endif /* TCP_LISTEN_BACKLOG */

/**
 * Closes the TX side of a connection held by the PCB.
 * For tcp_close(), a RST is sent if the application didn't receive all data
 * (tcp_recved() not called for all data passed to recv callback).
 *
 * Listening pcbs are freed and may not be referenced any more.
 * Connection pcbs are freed if not yet connected and may not be referenced
 * any more. If a connection is established (at least SYN received or in
 * a closing state), the connection is closed, and put in a closing state.
 * The pcb is then automatically freed in tcp_slowtmr(). It is therefore
 * unsafe to reference it.
 *
 * @param pcb the tcp_pcb to close
 * @return ERR_OK if connection has been closed
 *         another err_t if closing failed and pcb is not freed
 */
static err_t
tcp_close_shutdown(struct tcp_pcb *pcb, u8_t rst_on_unacked_data)
{
  err_t err;

  if (rst_on_unacked_data && ((pcb->state == ESTABLISHED) || (pcb->state == CLOSE_WAIT))) {
    if ((pcb->refused_data != NULL) || (pcb->rcv_wnd != TCP_WND_MAX(pcb))) {
      /* Not all data received by application, send RST to tell the remote
         side about this. */
      LWIP_ASSERT("pcb->flags & TF_RXCLOSED", pcb->flags & TF_RXCLOSED);

      /* don't call tcp_abort here: we must not deallocate the pcb since
         that might not be expected when calling tcp_close */
      tcp_rst(pcb->snd_nxt, pcb->rcv_nxt, &pcb->local_ip, &pcb->remote_ip,
               pcb->local_port, pcb->remote_port);

      tcp_pcb_purge(pcb);
      TCP_RMV_ACTIVE(pcb);
      if (pcb->state == ESTABLISHED) {
        /* move to TIME_WAIT since we close actively */
        pcb->state = TIME_WAIT;
        TCP_REG(&tcp_tw_pcbs, pcb);
      } else {
        /* CLOSE_WAIT: deallocate the pcb since we already sent a RST for it */
        if (tcp_input_pcb == pcb) {
          /* prevent using a deallocated pcb: free it from tcp_input later */
          tcp_trigger_input_pcb_close();
        } else {
          memp_free(MEMP_TCP_PCB, pcb);
        }
      }
      return ERR_OK;
    }
  }

  switch (pcb->state) {
  case CLOSED:
    /* Closing a pcb in the CLOSED state might seem erroneous,
     * however, it is in this state once allocated and as yet unused
     * and the user needs some way to free it should the need arise.
     * Calling tcp_close() with a pcb that has already been closed, (i.e. twice)
     * or for a pcb that has been used and then entered the CLOSED state
     * is erroneous, but this should never happen as the pcb has in those cases
     * been freed, and so any remaining handles are bogus. */
    err = ERR_OK;
    if (pcb->local_port != 0) {
      TCP_RMV(&tcp_bound_pcbs, pcb);
    }
    memp_free(MEMP_TCP_PCB, pcb);
    pcb = NULL;
    break;
  case LISTEN:
    err = ERR_OK;
    tcp_listen_closed(pcb);
    tcp_pcb_remove(&tcp_listen_pcbs.pcbs, pcb);
    memp_free(MEMP_TCP_PCB_LISTEN, pcb);
    pcb = NULL;
    break;
  case SYN_SENT:
    err = ERR_OK;
    TCP_PCB_REMOVE_ACTIVE(pcb);
    memp_free(MEMP_TCP_PCB, pcb);
    pcb = NULL;
    MIB2_STATS_INC(mib2.tcpattemptfails);
    break;
  case SYN_RCVD:
    err = tcp_send_fin(pcb);
    if (err == ERR_OK) {
      tcp_backlog_accepted(pcb);
      MIB2_STATS_INC(mib2.tcpattemptfails);
      tcp_set_fin_wait_1(pcb);
    }
    break;
  case ESTABLISHED:
    err = tcp_send_fin(pcb);
    if (err == ERR_OK) {
      MIB2_STATS_INC(mib2.tcpestabresets);
      tcp_set_fin_wait_1(pcb);
    }
    break;
  case CLOSE_WAIT:
    err = tcp_send_fin(pcb);
    if (err == ERR_OK) {
      MIB2_STATS_INC(mib2.tcpestabresets);
      pcb->state = LAST_ACK;
    }
    break;
  default:
    /* Has already been closed, do nothing. */
    err = ERR_OK;
    pcb = NULL;
    break;
  }

  if (pcb != NULL && err == ERR_OK) {
    /* To ensure all data has been sent when tcp_close returns, we have
       to make sure tcp_output doesn't fail.
       Since we don't really have to ensure all data has been sent when tcp_close
       returns (unsent data is sent from tcp timer functions, also), we don't care
       for the return value of tcp_output for now. */
    tcp_output(pcb);
  }
  return err;
}

/**
 * Closes the connection held by the PCB.
 *
 * Listening pcbs are freed and may not be referenced any more.
 * Connection pcbs are freed if not yet connected and may not be referenced
 * any more. If a connection is established (at least SYN received or in
 * a closing state), the connection is closed, and put in a closing state.
 * The pcb is then automatically freed in tcp_slowtmr(). It is therefore
 * unsafe to reference it (unless an error is returned).
 *
 * @param pcb the tcp_pcb to close
 * @return ERR_OK if connection has been closed
 *         another err_t if closing failed and pcb is not freed
 */
err_t
tcp_close(struct tcp_pcb *pcb)
{
  LWIP_DEBUGF(TCP_DEBUG, ("tcp_close: closing in "));
  tcp_debug_print_state(pcb->state);

  if (pcb->state != LISTEN) {
    /* Set a flag not to receive any more data... */
    pcb->flags |= TF_RXCLOSED;
  }
  /* ... and close */
  return tcp_close_shutdown(pcb, 1);
}

/**
 * Causes all or part of a full-duplex connection of this PCB to be shut down.
 * This doesn't deallocate the PCB unless shutting down both sides!
 * Shutting down both sides is the same as calling tcp_close, so if it succeds,
 * the PCB should not be referenced any more.
 *
 * @param pcb PCB to shutdown
 * @param shut_rx shut down receive side if this is != 0
 * @param shut_tx shut down send side if this is != 0
 * @return ERR_OK if shutdown succeeded (or the PCB has already been shut down)
 *         another err_t on error.
 */
err_t
tcp_shutdown(struct tcp_pcb *pcb, int shut_rx, int shut_tx)
{
  if (pcb->state == LISTEN) {
    return ERR_CONN;
  }
  if (shut_rx) {
    /* shut down the receive side: set a flag not to receive any more data... */
    pcb->flags |= TF_RXCLOSED;
    if (shut_tx) {
      /* shutting down the tx AND rx side is the same as closing for the raw API */
      return tcp_close_shutdown(pcb, 1);
    }
    /* ... and free buffered data */
    if (pcb->refused_data != NULL) {
      pbuf_free(pcb->refused_data);
      pcb->refused_data = NULL;
    }
  }
  if (shut_tx) {
    /* This can't happen twice since if it succeeds, the pcb's state is changed.
       Only close in these states as the others directly deallocate the PCB */
    switch (pcb->state) {
    case SYN_RCVD:
    case ESTABLISHED:
    case CLOSE_WAIT:
      return tcp_close_shutdown(pcb, (u8_t)shut_rx);
    default:
      /* Not (yet?) connected, cannot shutdown the TX side as that would bring us
        into CLOSED state, where the PCB is deallocated. */
      return ERR_CONN;
    }
  }
  return ERR_OK;
}

/**
 * Abandons a connection and optionally sends a RST to the remote
 * host.  Deletes the local protocol control block. This is done when
 * a connection is killed because of shortage of memory.
 *
 * @param pcb the tcp_pcb to abort
 * @param reset boolean to indicate whether a reset should be sent
 */
void
tcp_abandon(struct tcp_pcb *pcb, int reset)
{
  u32_t seqno, ackno;
#if LWIP_CALLBACK_API
  tcp_err_fn errf;
#endif /* LWIP_CALLBACK_API */
  void *errf_arg;

  /* pcb->state LISTEN not allowed here */
  LWIP_ASSERT("don't call tcp_abort/tcp_abandon for listen-pcbs",
    pcb->state != LISTEN);
  /* Figure out on which TCP PCB list we are, and remove us. If we
     are in an active state, call the receive function associated with
     the PCB with a NULL argument, and send an RST to the remote end. */
  if (pcb->state == TIME_WAIT) {
    tcp_pcb_remove(&tcp_tw_pcbs, pcb);
    memp_free(MEMP_TCP_PCB, pcb);
  } else {
    int send_rst = 0;
    u16_t local_port = 0;
    seqno = pcb->snd_nxt;
    ackno = pcb->rcv_nxt;
#if LWIP_CALLBACK_API
    errf = pcb->errf;
#endif /* LWIP_CALLBACK_API */
    errf_arg = pcb->callback_arg;
    if ((pcb->state == CLOSED) && (pcb->local_port != 0)) {
      /* bound, not yet opened */
      TCP_RMV(&tcp_bound_pcbs, pcb);
    } else {
      send_rst = reset;
      local_port = pcb->local_port;
      TCP_PCB_REMOVE_ACTIVE(pcb);
    }
    if (pcb->unacked != NULL) {
      tcp_segs_free(pcb->unacked);
    }
    if (pcb->unsent != NULL) {
      tcp_segs_free(pcb->unsent);
    }
#if TCP_QUEUE_OOSEQ
    if (pcb->ooseq != NULL) {
      tcp_segs_free(pcb->ooseq);
      pcb->ooseq = NULL;
    }
#endif /* TCP_QUEUE_OOSEQ */
    tcp_backlog_accepted(pcb);
    if (send_rst) {
      LWIP_DEBUGF(TCP_RST_DEBUG, ("tcp_abandon: sending RST\n"));
      tcp_rst(seqno, ackno, &pcb->local_ip, &pcb->remote_ip, local_port, pcb->remote_port);
    }
    memp_free(MEMP_TCP_PCB, pcb);
    TCP_EVENT_ERR(errf, errf_arg, ERR_ABRT);
  }
}

/**
 * Aborts the connection by sending a RST (reset) segment to the remote
 * host. The pcb is deallocated. This function never fails.
 *
 * ATTENTION: When calling this from one of the TCP callbacks, make
 * sure you always return ERR_ABRT (and never return ERR_ABRT otherwise
 * or you will risk accessing deallocated memory or memory leaks!
 *
 * @param pcb the tcp pcb to abort
 */
void
tcp_abort(struct tcp_pcb *pcb)
{
  tcp_abandon(pcb, 1);
}

/**
 * Binds the connection to a local port number and IP address. If the
 * IP address is not given (i.e., ipaddr == NULL), the IP address of
 * the outgoing network interface is used instead.
 *
 * @param pcb the tcp_pcb to bind (no check is done whether this pcb is
 *        already bound!)
 * @param ipaddr the local ip address to bind to (use IP_ADDR_ANY to bind
 *        to any local address
 * @param port the local port to bind to
 * @return ERR_USE if the port is already in use
 *         ERR_VAL if bind failed because the PCB is not in a valid state
 *         ERR_OK if bound
 */
err_t
tcp_bind(struct tcp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port)
{
  int i;
  int max_pcb_list = NUM_TCP_PCB_LISTS;
  struct tcp_pcb *cpcb;

#if LWIP_IPV4
  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (ipaddr == NULL) {
    ipaddr = IP_ADDR_ANY;
  }
#endif /* LWIP_IPV4 */

  /* still need to check for ipaddr == NULL in IPv6 only case */
  if ((pcb == NULL) || (ipaddr == NULL) || !IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr)) {
    return ERR_VAL;
  }

  LWIP_ERROR("tcp_bind: can only bind in state CLOSED", pcb->state == CLOSED, return ERR_VAL);

#if SO_REUSE
  /* Unless the REUSEADDR flag is set,
     we have to check the pcbs in TIME-WAIT state, also.
     We do not dump TIME_WAIT pcb's; they can still be matched by incoming
     packets using both local and remote IP addresses and ports to distinguish.
   */
  if (ip_get_option(pcb, SOF_REUSEADDR)) {
    max_pcb_list = NUM_TCP_PCB_LISTS_NO_TIME_WAIT;
  }
#endif /* SO_REUSE */

  if (port == 0) {
    port = tcp_new_port();
    if (port == 0) {
      return ERR_BUF;
    }
  } else {
    /* Check if the address already is in use (on all lists) */
    for (i = 0; i < max_pcb_list; i++) {
      for (cpcb = *tcp_pcb_lists[i]; cpcb != NULL; cpcb = cpcb->next) {
        if (cpcb->local_port == port) {
#if SO_REUSE
          /* Omit checking for the same port if both pcbs have REUSEADDR set.
             For SO_REUSEADDR, the duplicate-check for a 5-tuple is done in
             tcp_connect. */
          if (!ip_get_option(pcb, SOF_REUSEADDR) ||
              !ip_get_option(cpcb, SOF_REUSEADDR))
#endif /* SO_REUSE */
          {
            /* @todo: check accept_any_ip_version */
            if ((IP_IS_V6(ipaddr) == IP_IS_V6_VAL(cpcb->local_ip)) &&
                (ip_addr_isany(&cpcb->local_ip) ||
                ip_addr_isany(ipaddr) ||
                ip_addr_cmp(&cpcb->local_ip, ipaddr))) {
              return ERR_USE;
            }
          }
        }
      }
    }
  }

  if (!ip_addr_isany(ipaddr)) {
    ip_addr_set(&pcb->local_ip, ipaddr);
  }
  pcb->local_port = port;
  TCP_REG(&tcp_bound_pcbs, pcb);
  LWIP_DEBUGF(TCP_DEBUG, ("tcp_bind: bind to port %"U16_F"\n", port));
  return ERR_OK;
}
#if LWIP_CALLBACK_API
/**
 * Default accept callback if no accept callback is specified by the user.
 */
static err_t
tcp_accept_null(void *arg, struct tcp_pcb *pcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_abort(pcb);

  return ERR_ABRT;
}
#endif /* LWIP_CALLBACK_API */

/**
 * Set the state of the connection to be LISTEN, which means that it
 * is able to accept incoming connections. The protocol control block
 * is reallocated in order to consume less memory. Setting the
 * connection to LISTEN is an irreversible process.
 *
 * @param pcb the original tcp_pcb
 * @param backlog the incoming connections queue limit
 * @return tcp_pcb used for listening, consumes less memory.
 *
 * @note The original tcp_pcb is freed. This function therefore has to be
 *       called like this:
 *             tpcb = tcp_listen(tpcb);
 */
struct tcp_pcb *
tcp_listen_with_backlog(struct tcp_pcb *pcb, u8_t backlog)
{
  struct tcp_pcb_listen *lpcb;

  LWIP_UNUSED_ARG(backlog);
  LWIP_ERROR("tcp_listen: pcb already connected", pcb->state == CLOSED, return NULL);

  /* already listening? */
  if (pcb->state == LISTEN) {
    return pcb;
  }
#if SO_REUSE
  if (ip_get_option(pcb, SOF_REUSEADDR)) {
    /* Since SOF_REUSEADDR allows reusing a local address before the pcb's usage
       is declared (listen-/connection-pcb), we have to make sure now that
       this port is only used once for every local IP. */
    for (lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb = lpcb->next) {
      if ((lpcb->local_port == pcb->local_port) &&
          ip_addr_cmp(&lpcb->local_ip, &pcb->local_ip)) {
        /* this address/port is already used */
        return NULL;
      }
    }
  }
#endif /* SO_REUSE */
  lpcb = (struct tcp_pcb_listen *)memp_malloc(MEMP_TCP_PCB_LISTEN);
  if (lpcb == NULL) {
    return NULL;
  }
  lpcb->callback_arg = pcb->callback_arg;
  lpcb->local_port = pcb->local_port;
  lpcb->state = LISTEN;
  lpcb->prio = pcb->prio;
  lpcb->so_options = pcb->so_options;
  lpcb->ttl = pcb->ttl;
  lpcb->tos = pcb->tos;
#if LWIP_IPV4 && LWIP_IPV6
  IP_SET_TYPE_VAL(lpcb->remote_ip, pcb->local_ip.type);
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  ip_addr_copy(lpcb->local_ip, pcb->local_ip);
  if (pcb->local_port != 0) {
    TCP_RMV(&tcp_bound_pcbs, pcb);
  }
  memp_free(MEMP_TCP_PCB, pcb);
#if LWIP_CALLBACK_API
  lpcb->accept = tcp_accept_null;
#endif /* LWIP_CALLBACK_API */
#if TCP_LISTEN_BACKLOG
  lpcb->accepts_pending = 0;
  tcp_backlog_set(lpcb, backlog);
#endif /* TCP_LISTEN_BACKLOG */
  TCP_REG(&tcp_listen_pcbs.pcbs, (struct tcp_pcb *)lpcb);
  return (struct tcp_pcb *)lpcb;
}

/**
 * Update the state that tracks the available window space to advertise.
 *
 * Returns how much extra window would be advertised if we sent an
 * update now.
 */
u32_t tcp_update_rcv_ann_wnd(struct tcp_pcb *pcb)
{
  u32_t new_right_edge = pcb->rcv_nxt + pcb->rcv_wnd;

  if (TCP_SEQ_GEQ(new_right_edge, pcb->rcv_ann_right_edge + LWIP_MIN((TCP_WND(pcb) / 2), pcb->mss))) {
    /* we can advertise more window */
    pcb->rcv_ann_wnd = pcb->rcv_wnd;
    return new_right_edge - pcb->rcv_ann_right_edge;
  } else {
    if (TCP_SEQ_GT(pcb->rcv_nxt, pcb->rcv_ann_right_edge)) {
      /* Can happen due to other end sending out of advertised window,
       * but within actual available (but not yet advertised) window */
      pcb->rcv_ann_wnd = 0;
    } else {
      /* keep the right edge of window constant */
      u32_t new_rcv_ann_wnd = pcb->rcv_ann_right_edge - pcb->rcv_nxt;
#if !LWIP_WND_SCALE
      LWIP_ASSERT("new_rcv_ann_wnd <= 0xffff", new_rcv_ann_wnd <= 0xffff);
#endif
      pcb->rcv_ann_wnd = (tcpwnd_size_t)new_rcv_ann_wnd;
    }
    return 0;
  }
}

/**
 * This function should be called by the application when it has
 * processed the data. The purpose is to advertise a larger window
 * when the data has been processed.
 *
 * @param pcb the tcp_pcb for which data is read
 * @param len the amount of bytes that have been read by the application
 */
void
tcp_recved(struct tcp_pcb *pcb, u16_t len)
{
  int wnd_inflation;

  /* pcb->state LISTEN not allowed here */
  LWIP_ASSERT("don't call tcp_recved for listen-pcbs",
    pcb->state != LISTEN);

  pcb->rcv_wnd += len;
  if (pcb->rcv_wnd > TCP_WND_MAX(pcb)) {
    pcb->rcv_wnd = TCP_WND_MAX(pcb);
  } else if (pcb->rcv_wnd == 0) {
    /* rcv_wnd overflowed */
    if ((pcb->state == CLOSE_WAIT) || (pcb->state == LAST_ACK)) {
      /* In passive close, we allow this, since the FIN bit is added to rcv_wnd
         by the stack itself, since it is not mandatory for an application
         to call tcp_recved() for the FIN bit, but e.g. the netconn API does so. */
      pcb->rcv_wnd = TCP_WND_MAX(pcb);
    } else {
      LWIP_ASSERT("tcp_recved: len wrapped rcv_wnd\n", 0);
    }
  }

  wnd_inflation = tcp_update_rcv_ann_wnd(pcb);

  /* If the change in the right edge of window is significant (default
   * watermark is TCP_WND(pcb)/4), then send an explicit update now.
   * Otherwise wait for a packet to be sent in the normal course of
   * events (or more window to be available later) */
  if (wnd_inflation >= TCP_WND_UPDATE_THRESHOLD(pcb)) {
    tcp_ack_now(pcb);
    tcp_output(pcb);
  }

  LWIP_DEBUGF(TCP_DEBUG, ("tcp_recved: received %"U16_F" bytes, wnd %"TCPWNDSIZE_F" (%"TCPWNDSIZE_F").\n",
         len, pcb->rcv_wnd, TCP_WND_MAX(pcb) - pcb->rcv_wnd));
}

/**
 * Allocate a new local TCP port.
 *
 * @return a new (free) local TCP port number
 */
static u16_t
tcp_new_port(void)
{
  u8_t i;
  u16_t n = 0;
  struct tcp_pcb *pcb;

again:

#if ESP_RANDOM_TCP_PORT
  tcp_port = abs(LWIP_RAND()) % (TCP_LOCAL_PORT_RANGE_END - TCP_LOCAL_PORT_RANGE_START);
  tcp_port += TCP_LOCAL_PORT_RANGE_START;
#else
  if (tcp_port++ == TCP_LOCAL_PORT_RANGE_END) {
    tcp_port = TCP_LOCAL_PORT_RANGE_START;
  }
#endif

  /* Check all PCB lists. */
  for (i = 0; i < NUM_TCP_PCB_LISTS; i++) {
    for (pcb = *tcp_pcb_lists[i]; pcb != NULL; pcb = pcb->next) {
      if (pcb->local_port == tcp_port) {
        if (++n > (TCP_LOCAL_PORT_RANGE_END - TCP_LOCAL_PORT_RANGE_START)) {
          return 0;
        }
        goto again;
      }
    }
  }
  return tcp_port;
}

/**
 * Connects to another host. The function given as the "connected"
 * argument will be called when the connection has been established.
 *
 * @param pcb the tcp_pcb used to establish the connection
 * @param ipaddr the remote ip address to connect to
 * @param port the remote tcp port to connect to
 * @param connected callback function to call when connected (on error,
                    the err calback will be called)
 * @return ERR_VAL if invalid arguments are given
 *         ERR_OK if connect request has been sent
 *         other err_t values if connect request couldn't be sent
 */
err_t
tcp_connect(struct tcp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port,
      tcp_connected_fn connected)
{
  err_t ret;
  u32_t iss;
  u16_t old_local_port;

  if ((pcb == NULL) || (ipaddr == NULL) || !IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr)) {
    return ERR_VAL;
  }

  LWIP_ERROR("tcp_connect: can only connect from state CLOSED", pcb->state == CLOSED, return ERR_ISCONN);

  LWIP_DEBUGF(TCP_DEBUG, ("tcp_connect to port %"U16_F"\n", port));
  ip_addr_set(&pcb->remote_ip, ipaddr);
  pcb->remote_port = port;

  /* check if we have a route to the remote host */
  if (ip_addr_isany(&pcb->local_ip)) {
    /* no local IP address set, yet. */
    struct netif *netif;
    const ip_addr_t *local_ip;
    ip_route_get_local_ip(&pcb->local_ip, &pcb->remote_ip, netif, local_ip);
    if ((netif == NULL) || (local_ip == NULL)) {
      /* Don't even try to send a SYN packet if we have no route
         since that will fail. */
      return ERR_RTE;
    }
    /* Use the address as local address of the pcb. */
    ip_addr_copy(pcb->local_ip, *local_ip);
  }

  old_local_port = pcb->local_port;
  if (pcb->local_port == 0) {
    pcb->local_port = tcp_new_port();
    if (pcb->local_port == 0) {
      return ERR_BUF;
    }
  } else {
#if SO_REUSE
    if (ip_get_option(pcb, SOF_REUSEADDR)) {
      /* Since SOF_REUSEADDR allows reusing a local address, we have to make sure
         now that the 5-tuple is unique. */
      struct tcp_pcb *cpcb;
      int i;
      /* Don't check listen- and bound-PCBs, check active- and TIME-WAIT PCBs. */
      for (i = 2; i < NUM_TCP_PCB_LISTS; i++) {
        for (cpcb = *tcp_pcb_lists[i]; cpcb != NULL; cpcb = cpcb->next) {
          if ((cpcb->local_port == pcb->local_port) &&
              (cpcb->remote_port == port) &&
              ip_addr_cmp(&cpcb->local_ip, &pcb->local_ip) &&
              ip_addr_cmp(&cpcb->remote_ip, ipaddr)) {
            /* linux returns EISCONN here, but ERR_USE should be OK for us */
            return ERR_USE;
          }
        }
      }
    }
#endif /* SO_REUSE */
  }

  iss = tcp_next_iss();
  pcb->rcv_nxt = 0;
  pcb->snd_nxt = iss;
  pcb->lastack = iss - 1;
  pcb->snd_lbb = iss - 1;
  /* Start with a window that does not need scaling. When window scaling is
     enabled and used, the window is enlarged when both sides agree on scaling. */
  pcb->rcv_wnd = pcb->rcv_ann_wnd = TCPWND_MIN16(TCP_WND(pcb));
  pcb->rcv_ann_right_edge = pcb->rcv_nxt;
  pcb->snd_wnd = TCP_WND(pcb);
  /* As initial send MSS, we use TCP_MSS but limit it to 536.
     The send MSS is updated when an MSS option is received. */
  pcb->mss = (TCP_MSS > 536) ? 536 : TCP_MSS;
#if TCP_CALCULATE_EFF_SEND_MSS
  pcb->mss = tcp_eff_send_mss(pcb->mss, &pcb->local_ip, &pcb->remote_ip);
#endif /* TCP_CALCULATE_EFF_SEND_MSS */
  pcb->cwnd = 1;
  pcb->ssthresh = TCP_WND(pcb);
#if LWIP_CALLBACK_API
  pcb->connected = connected;
#else /* LWIP_CALLBACK_API */
  LWIP_UNUSED_ARG(connected);
#endif /* LWIP_CALLBACK_API */

  /* Send a SYN together with the MSS option. */
  ret = tcp_enqueue_flags(pcb, TCP_SYN);
  if (ret == ERR_OK) {
    /* SYN segment was enqueued, changed the pcbs state now */
    pcb->state = SYN_SENT;
    if (old_local_port != 0) {
      TCP_RMV(&tcp_bound_pcbs, pcb);
    }
    TCP_REG_ACTIVE(pcb);
    MIB2_STATS_INC(mib2.tcpactiveopens);

    tcp_output(pcb);
  }
  return ret;
}

/**
 * Called every 500 ms and implements the retransmission timer and the timer that
 * removes PCBs that have been in TIME-WAIT for enough time. It also increments
 * various timers such as the inactivity timer in each PCB.
 *
 * Automatically called from tcp_tmr().
 */
void
tcp_slowtmr(void)
{
  struct tcp_pcb *pcb, *prev;
  tcpwnd_size_t eff_wnd;
  u8_t pcb_remove;      /* flag if a PCB should be removed */
  u8_t pcb_reset;       /* flag if a RST should be sent when removing */
  err_t err;

  err = ERR_OK;

  ++tcp_ticks;
  ++tcp_timer_ctr;

tcp_slowtmr_start:
  /* Steps through all of the active PCBs. */
  prev = NULL;
  pcb = tcp_active_pcbs;
  if (pcb == NULL) {
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: no active pcbs\n"));
  }
  while (pcb != NULL) {
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: processing active pcb\n"));
    LWIP_ASSERT("tcp_slowtmr: active pcb->state != CLOSED\n", pcb->state != CLOSED);
    LWIP_ASSERT("tcp_slowtmr: active pcb->state != LISTEN\n", pcb->state != LISTEN);
    LWIP_ASSERT("tcp_slowtmr: active pcb->state != TIME-WAIT\n", pcb->state != TIME_WAIT);
    if (pcb->last_timer == tcp_timer_ctr) {
      /* skip this pcb, we have already processed it */
      pcb = pcb->next;
      continue;
    }
    pcb->last_timer = tcp_timer_ctr;

    pcb_remove = 0;
    pcb_reset = 0;

    if (pcb->state == SYN_SENT && pcb->nrtx == TCP_SYNMAXRTX) {
      ++pcb_remove;
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: max SYN retries reached\n"));
    }
    else if (pcb->nrtx == TCP_MAXRTX) {
      ++pcb_remove;
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: max DATA retries reached\n"));
    } else {
      if (pcb->persist_backoff > 0) {
        
        /* If snd_wnd is zero, use persist timer to send 1 byte probes
         * instead of using the standard retransmission mechanism. */
        u8_t backoff_cnt = tcp_persist_backoff[pcb->persist_backoff-1];

        if (pcb->persist_cnt < backoff_cnt) {
          pcb->persist_cnt++;
        }
        if (pcb->persist_cnt >= backoff_cnt) {
          if (tcp_zero_window_probe(pcb) == ERR_OK) {
            pcb->persist_cnt = 0;
            if (pcb->persist_backoff < sizeof(tcp_persist_backoff)) {
              pcb->persist_backoff++;
            }
          }
        }
      } else {
        /* Increase the retransmission timer if it is running */
        if (pcb->rtime >= 0) {
          ++pcb->rtime;
        }

        if (pcb->unacked != NULL && pcb->rtime >= pcb->rto) {
          /* Time for a retransmission. */
          LWIP_DEBUGF(TCP_RTO_DEBUG, ("tcp_slowtmr: rtime %"S16_F
                                      " pcb->rto %"S16_F"\n",
                                      pcb->rtime, pcb->rto));

          ESP_STATS_TCP_PCB(pcb);

          /* Double retransmission time-out unless we are trying to
           * connect to somebody (i.e., we are in SYN_SENT). */
          if (pcb->state != SYN_SENT) {
              pcb->rto = ((pcb->sa >> 3) + pcb->sv) << tcp_backoff[pcb->nrtx];
          }

          /* Reset the retransmission timer. */
          pcb->rtime = 0;

          /* Reduce congestion window and ssthresh. */
          eff_wnd = LWIP_MIN(pcb->cwnd, pcb->snd_wnd);
          pcb->ssthresh = eff_wnd >> 1;
          if (pcb->ssthresh < (tcpwnd_size_t)(pcb->mss << 1)) {
            pcb->ssthresh = (pcb->mss << 1);
          }
          pcb->cwnd = pcb->mss;
          LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_slowtmr: cwnd %"TCPWNDSIZE_F
                                       " ssthresh %"TCPWNDSIZE_F"\n",
                                       pcb->cwnd, pcb->ssthresh));

          /* The following needs to be called AFTER cwnd is set to one
             mss - STJ */
          tcp_rexmit_rto(pcb);
        }
      }
    }
    /* Check if this PCB has stayed too long in FIN-WAIT-2 */
#if ESP_LWIP
    if ((pcb->state == FIN_WAIT_2) || (pcb->state == FIN_WAIT_1)) {
#else
    if (pcb->state == FIN_WAIT_2) {
#endif
      /* If this PCB is in FIN_WAIT_2 because of SHUT_WR don't let it time out. */
      if (pcb->flags & TF_RXCLOSED) {
        /* PCB was fully closed (either through close() or SHUT_RDWR):
           normal FIN-WAIT timeout handling. */
        if ((u32_t)(tcp_ticks - pcb->tmr) >
            TCP_FIN_WAIT_TIMEOUT / TCP_SLOW_INTERVAL) {
          ++pcb_remove;
          LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: removing pcb stuck in FIN-WAIT-2\n"));
        }
      }
    }

    /* Check if KEEPALIVE should be sent */
    if (ip_get_option(pcb, SOF_KEEPALIVE) &&
       ((pcb->state == ESTABLISHED) ||
        (pcb->state == CLOSE_WAIT))) {
      if ((u32_t)(tcp_ticks - pcb->tmr) >
         (pcb->keep_idle + TCP_KEEP_DUR(pcb)) / TCP_SLOW_INTERVAL)
      {
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: KEEPALIVE timeout. Aborting connection to "));
        ip_addr_debug_print(TCP_DEBUG, &pcb->remote_ip);
        LWIP_DEBUGF(TCP_DEBUG, ("\n"));

        ++pcb_remove;
        ++pcb_reset;
      } else if ((u32_t)(tcp_ticks - pcb->tmr) >
                (pcb->keep_idle + pcb->keep_cnt_sent * TCP_KEEP_INTVL(pcb))
                / TCP_SLOW_INTERVAL)
      {
        err = tcp_keepalive(pcb);
        if (err == ERR_OK) {
          pcb->keep_cnt_sent++;
        }
      }
    }

    /* If this PCB has queued out of sequence data, but has been
       inactive for too long, will drop the data (it will eventually
       be retransmitted). */
#if TCP_QUEUE_OOSEQ
    if (pcb->ooseq != NULL &&
        (u32_t)tcp_ticks - pcb->tmr >= pcb->rto * TCP_OOSEQ_TIMEOUT) {
      tcp_segs_free(pcb->ooseq);
      pcb->ooseq = NULL;
      LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_slowtmr: dropping OOSEQ queued data\n"));
    }
#endif /* TCP_QUEUE_OOSEQ */

    /* Check if this PCB has stayed too long in SYN-RCVD */
    if (pcb->state == SYN_RCVD) {
      if ((u32_t)(tcp_ticks - pcb->tmr) >
          TCP_SYN_RCVD_TIMEOUT / TCP_SLOW_INTERVAL) {
        ++pcb_remove;
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: removing pcb stuck in SYN-RCVD\n"));
      }
    }

    /* Check if this PCB has stayed too long in LAST-ACK */
    if (pcb->state == LAST_ACK) {
      if ((u32_t)(tcp_ticks - pcb->tmr) > 2 * TCP_MSL / TCP_SLOW_INTERVAL) {
        ++pcb_remove;
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: removing pcb stuck in LAST-ACK\n"));
      }
    }

    /* If the PCB should be removed, do it. */
    if (pcb_remove) {
      struct tcp_pcb *pcb2;
      tcp_err_fn err_fn;
      void *err_arg;
      tcp_pcb_purge(pcb);
      /* Remove PCB from tcp_active_pcbs list. */
      if (prev != NULL) {
        LWIP_ASSERT("tcp_slowtmr: middle tcp != tcp_active_pcbs", pcb != tcp_active_pcbs);
        prev->next = pcb->next;
      } else {
        /* This PCB was the first. */
        LWIP_ASSERT("tcp_slowtmr: first pcb == tcp_active_pcbs", tcp_active_pcbs == pcb);
        tcp_active_pcbs = pcb->next;
      }

      if (pcb_reset) {
        tcp_rst(pcb->snd_nxt, pcb->rcv_nxt, &pcb->local_ip, &pcb->remote_ip,
                 pcb->local_port, pcb->remote_port);
      }

      err_fn = pcb->errf;
      err_arg = pcb->callback_arg;
      pcb2 = pcb;
      pcb = pcb->next;
      memp_free(MEMP_TCP_PCB, pcb2);

      tcp_active_pcbs_changed = 0;
      TCP_EVENT_ERR(err_fn, err_arg, ERR_ABRT);
      if (tcp_active_pcbs_changed) {
        goto tcp_slowtmr_start;
      }
    } else {
      /* get the 'next' element now and work with 'prev' below (in case of abort) */
      prev = pcb;
      pcb = pcb->next;

      /* We check if we should poll the connection. */
      ++prev->polltmr;
      if (prev->polltmr >= prev->pollinterval) {
        prev->polltmr = 0;
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_slowtmr: polling application\n"));
        tcp_active_pcbs_changed = 0;
        TCP_EVENT_POLL(prev, err);
        if (tcp_active_pcbs_changed) {
          goto tcp_slowtmr_start;
        }
        /* if err == ERR_ABRT, 'prev' is already deallocated */
        if (err == ERR_OK) {
          tcp_output(prev);
        }
      }
    }
  }


  /* Steps through all of the TIME-WAIT PCBs. */
  prev = NULL;
  pcb = tcp_tw_pcbs;
  while (pcb != NULL) {
    LWIP_ASSERT("tcp_slowtmr: TIME-WAIT pcb->state == TIME-WAIT", pcb->state == TIME_WAIT);
    pcb_remove = 0;

    /* Check if this PCB has stayed long enough in TIME-WAIT */
    if ((u32_t)(tcp_ticks - pcb->tmr) > 2 * TCP_MSL / TCP_SLOW_INTERVAL) {
      ++pcb_remove;
    }

    /* If the PCB should be removed, do it. */
    if (pcb_remove) {
      struct tcp_pcb *pcb2;
      tcp_pcb_purge(pcb);
      /* Remove PCB from tcp_tw_pcbs list. */
      if (prev != NULL) {
        LWIP_ASSERT("tcp_slowtmr: middle tcp != tcp_tw_pcbs", pcb != tcp_tw_pcbs);
        prev->next = pcb->next;
      } else {
        /* This PCB was the first. */
        LWIP_ASSERT("tcp_slowtmr: first pcb == tcp_tw_pcbs", tcp_tw_pcbs == pcb);
        tcp_tw_pcbs = pcb->next;
      }
      pcb2 = pcb;
      pcb = pcb->next;
      memp_free(MEMP_TCP_PCB, pcb2);
    } else {
      prev = pcb;
      pcb = pcb->next;
    }
  }
}

/**
 * Is called every TCP_FAST_INTERVAL (250 ms) and process data previously
 * "refused" by upper layer (application) and sends delayed ACKs.
 *
 * Automatically called from tcp_tmr().
 */
void
tcp_fasttmr(void)
{
  struct tcp_pcb *pcb;

  ++tcp_timer_ctr;

tcp_fasttmr_start:
  pcb = tcp_active_pcbs;

  while (pcb != NULL) {
    if (pcb->last_timer != tcp_timer_ctr) {
      struct tcp_pcb *next;
      pcb->last_timer = tcp_timer_ctr;
      /* send delayed ACKs */
      if (pcb->flags & TF_ACK_DELAY) {
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_fasttmr: delayed ACK\n"));
        tcp_ack_now(pcb);
        tcp_output(pcb);
        pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
      }

      next = pcb->next;

      /* If there is data which was previously "refused" by upper layer */
      if (pcb->refused_data != NULL) {
        tcp_active_pcbs_changed = 0;
        tcp_process_refused_data(pcb);
        if (tcp_active_pcbs_changed) {
          /* application callback has changed the pcb list: restart the loop */
          goto tcp_fasttmr_start;
        }
      }
      pcb = next;
    } else {
      pcb = pcb->next;
    }
  }
}

/** Call tcp_output for all active pcbs that have TF_NAGLEMEMERR set */
void
tcp_txnow(void)
{
  struct tcp_pcb *pcb;

  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    if (pcb->flags & TF_NAGLEMEMERR) {
      tcp_output(pcb);
    }
  }
}

/** Pass pcb->refused_data to the recv callback */
err_t
tcp_process_refused_data(struct tcp_pcb *pcb)
{
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
  struct pbuf *rest;
  while (pcb->refused_data != NULL)
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
  {
    err_t err;
    u8_t refused_flags = pcb->refused_data->flags;
    /* set pcb->refused_data to NULL in case the callback frees it and then
       closes the pcb */
    struct pbuf *refused_data = pcb->refused_data;
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
    pbuf_split_64k(refused_data, &rest);
    pcb->refused_data = rest;
#else /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
    pcb->refused_data = NULL;
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
    /* Notify again application with data previously received. */
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: notify kept packet\n"));
    TCP_EVENT_RECV(pcb, refused_data, ERR_OK, err);
    if (err == ERR_OK) {
      /* did refused_data include a FIN? */
      if (refused_flags & PBUF_FLAG_TCP_FIN
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
          && (rest == NULL)
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
         ) {
        /* correct rcv_wnd as the application won't call tcp_recved()
           for the FIN's seqno */
        if (pcb->rcv_wnd != TCP_WND_MAX(pcb)) {
          pcb->rcv_wnd++;
        }
        TCP_EVENT_CLOSED(pcb, err);
        if (err == ERR_ABRT) {
          return ERR_ABRT;
        }
      }
    } else if (err == ERR_ABRT) {
      /* if err == ERR_ABRT, 'pcb' is already deallocated */
      /* Drop incoming packets because pcb is "full" (only if the incoming
         segment contains data). */
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: drop incoming packets, because pcb is \"full\"\n"));
      return ERR_ABRT;
    } else {
      /* data is still refused, pbuf is still valid (go on for ACK-only packets) */
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
      if (rest != NULL) {
        pbuf_cat(refused_data, rest);
      }
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
      pcb->refused_data = refused_data;
      return ERR_INPROGRESS;
    }
  }
  return ERR_OK;
}

/**
 * Deallocates a list of TCP segments (tcp_seg structures).
 *
 * @param seg tcp_seg list of TCP segments to free
 */
void
tcp_segs_free(struct tcp_seg *seg)
{
  while (seg != NULL) {
    struct tcp_seg *next = seg->next;
    tcp_seg_free(seg);
    seg = next;
  }
}

/**
 * Frees a TCP segment (tcp_seg structure).
 *
 * @param seg single tcp_seg to free
 */
void
tcp_seg_free(struct tcp_seg *seg)
{
  if (seg != NULL) {
    if (seg->p != NULL) {
      pbuf_free(seg->p);
      seg->p = NULL;
    }
    memp_free(MEMP_TCP_SEG, seg);
  }
}

/**
 * Sets the priority of a connection.
 *
 * @param pcb the tcp_pcb to manipulate
 * @param prio new priority
 */
void
tcp_setprio(struct tcp_pcb *pcb, u8_t prio)
{
  pcb->prio = prio;
}

#if TCP_QUEUE_OOSEQ
/**
 * Returns a copy of the given TCP segment.
 * The pbuf and data are not copied, only the pointers
 *
 * @param seg the old tcp_seg
 * @return a copy of seg
 */
struct tcp_seg *
tcp_seg_copy(struct tcp_seg *seg)
{
  struct tcp_seg *cseg;

  cseg = (struct tcp_seg *)memp_malloc(MEMP_TCP_SEG);
  if (cseg == NULL) {
    return NULL;
  }
  SMEMCPY((u8_t *)cseg, (const u8_t *)seg, sizeof(struct tcp_seg));
  pbuf_ref(cseg->p);
  return cseg;
}
#endif /* TCP_QUEUE_OOSEQ */

#if LWIP_CALLBACK_API
/**
 * Default receive callback that is called if the user didn't register
 * a recv callback for the pcb.
 */
err_t
tcp_recv_null(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  if (p != NULL) {
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
  } else if (err == ERR_OK) {
    return tcp_close(pcb);
  }
  return ERR_OK;
}
#endif /* LWIP_CALLBACK_API */

/**
 * Kills the oldest active connection that has the same or lower priority than
 * 'prio'.
 *
 * @param prio minimum priority
 */
static void
tcp_kill_prio(u8_t prio)
{
  struct tcp_pcb *pcb, *inactive;
  u32_t inactivity;
  u8_t mprio;

  mprio = LWIP_MIN(TCP_PRIO_MAX, prio);

  /* We kill the oldest active connection that has lower priority than prio. */
  inactivity = 0;
  inactive = NULL;
  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    if (pcb->prio <= mprio &&
       (u32_t)(tcp_ticks - pcb->tmr) >= inactivity) {
      inactivity = tcp_ticks - pcb->tmr;
      inactive = pcb;
      mprio = pcb->prio;
    }
  }
  if (inactive != NULL) {
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_kill_prio: killing oldest PCB %p (%"S32_F")\n",
           (void *)inactive, inactivity));
    tcp_abort(inactive);
  }
}

/**
 * Kills the oldest connection that is in specific state.
 * Called from tcp_alloc() for LAST_ACK and CLOSING if no more connections are available.
 */
static void
tcp_kill_state(enum tcp_state state)
{
  struct tcp_pcb *pcb, *inactive;
  u32_t inactivity;

  inactivity = 0;
  inactive = NULL;
  /* Go through the list of active pcbs and get the oldest pcb that is in state
     CLOSING/LAST_ACK. */
  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    if (pcb->state == state) {
      if ((u32_t)(tcp_ticks - pcb->tmr) >= inactivity) {
        inactivity = tcp_ticks - pcb->tmr;
        inactive = pcb;
      }
    }
  }
  if (inactive != NULL) {
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_kill_closing: killing oldest %s PCB %p (%"S32_F")\n",
           tcp_state_str[state], (void *)inactive, inactivity));
    /* Don't send a RST, since no data is lost. */
    tcp_abandon(inactive, 0);
  }
}

/**
 * Kills the oldest connection that is in TIME_WAIT state.
 * Called from tcp_alloc() if no more connections are available.
 */
static void
tcp_kill_timewait(void)
{
  struct tcp_pcb *pcb, *inactive;
  u32_t inactivity;

  inactivity = 0;
  inactive = NULL;
  /* Go through the list of TIME_WAIT pcbs and get the oldest pcb. */
  for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
    if ((u32_t)(tcp_ticks - pcb->tmr) >= inactivity) {
      inactivity = tcp_ticks - pcb->tmr;
      inactive = pcb;
    }
  }
  if (inactive != NULL) {
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_kill_timewait: killing oldest TIME-WAIT PCB %p (%"S32_F")\n",
           (void *)inactive, inactivity));
    tcp_abort(inactive);
  }
}

#if ESP_LWIP
typedef struct {
    u8_t time_wait;
    u8_t closing;
    u8_t fin_wait2;
    u8_t last_ack;
    u8_t fin_wait1;
    u8_t listen;
    u8_t bound;
    u8_t total;
}tcp_pcb_num_t;

void tcp_pcb_num_cal(tcp_pcb_num_t *tcp_pcb_num)
{
    struct tcp_pcb_listen *listen;
    struct tcp_pcb *pcb;

    if (!tcp_pcb_num){
        return;
    }

    memset(tcp_pcb_num, 0, sizeof(*tcp_pcb_num));
    for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
        tcp_pcb_num->total ++;
        tcp_pcb_num->time_wait ++;
    }

    for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next){
        tcp_pcb_num->total ++;
        if (pcb->state == FIN_WAIT_2){
            tcp_pcb_num->fin_wait2 ++;
        } else if (pcb->state == LAST_ACK) {
            tcp_pcb_num->last_ack ++;
        } else if (pcb->state == CLOSING) {
            tcp_pcb_num->closing ++;
        } else if (pcb->state == FIN_WAIT_1){
            tcp_pcb_num->fin_wait1 ++;
        }
    }

    for (listen = tcp_listen_pcbs.listen_pcbs; listen != NULL; listen = listen->next){
        tcp_pcb_num->total ++;
        tcp_pcb_num->listen ++;
    }

    for (pcb = tcp_bound_pcbs; pcb != NULL; pcb = pcb->next){
        tcp_pcb_num->total ++;
        tcp_pcb_num->bound ++;
    }
}
#endif


/**
 * Allocate a new tcp_pcb structure.
 *
 * @param prio priority for the new pcb
 * @return a new tcp_pcb that initially is in state CLOSED
 */
struct tcp_pcb *
tcp_alloc(u8_t prio)
{
  struct tcp_pcb *pcb;
  u32_t iss;

#if ESP_LWIP
    tcp_pcb_num_t tcp_pcb_num;

    tcp_pcb_num_cal(&tcp_pcb_num);

    if (tcp_pcb_num.total >= MEMP_NUM_TCP_PCB){
        if (tcp_pcb_num.time_wait > 0){
            tcp_kill_timewait();
        } else if (tcp_pcb_num.last_ack > 0){
            tcp_kill_state(LAST_ACK);
        } else if (tcp_pcb_num.closing > 0){
            tcp_kill_state(CLOSING);
        } else if (tcp_pcb_num.fin_wait2 > 0){
            tcp_kill_state(FIN_WAIT_2);
        } else if (tcp_pcb_num.fin_wait1 > 0){
            tcp_kill_state(FIN_WAIT_1);
        } else {
            tcp_kill_prio(prio);
        }
    }

    tcp_pcb_num_cal(&tcp_pcb_num);
    if (tcp_pcb_num.total >= MEMP_NUM_TCP_PCB){
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_alloc: no available tcp pcb %d %d %d %d %d %d %d %d\n",
           tcp_pcb_num.total, tcp_pcb_num.time_wait, tcp_pcb_num.last_ack, tcp_pcb_num.closing,
           tcp_pcb_num.fin_wait2, tcp_pcb_num.fin_wait1, tcp_pcb_num.listen, tcp_pcb_num.bound));
        return NULL;
    }

#endif

  pcb = (struct tcp_pcb *)memp_malloc(MEMP_TCP_PCB);
  if (pcb == NULL) {
    /* Try killing oldest connection in TIME-WAIT. */
    LWIP_DEBUGF(TCP_DEBUG, ("tcp_alloc: killing off oldest TIME-WAIT connection\n"));
    tcp_kill_timewait();
    /* Try to allocate a tcp_pcb again. */
    pcb = (struct tcp_pcb *)memp_malloc(MEMP_TCP_PCB);
    if (pcb == NULL) {
      /* Try killing oldest connection in LAST-ACK (these wouldn't go to TIME-WAIT). */
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_alloc: killing off oldest LAST-ACK connection\n"));
      tcp_kill_state(LAST_ACK);
      /* Try to allocate a tcp_pcb again. */
      pcb = (struct tcp_pcb *)memp_malloc(MEMP_TCP_PCB);
      if (pcb == NULL) {
        /* Try killing oldest connection in CLOSING. */
        LWIP_DEBUGF(TCP_DEBUG, ("tcp_alloc: killing off oldest CLOSING connection\n"));
        tcp_kill_state(CLOSING);
        /* Try to allocate a tcp_pcb again. */
        pcb = (struct tcp_pcb *)memp_malloc(MEMP_TCP_PCB);
        if (pcb == NULL) {
          /* Try killing active connections with lower priority than the new one. */
          LWIP_DEBUGF(TCP_DEBUG, ("tcp_alloc: killing connection with prio lower than %d\n", prio));
          tcp_kill_prio(prio);
          /* Try to allocate a tcp_pcb again. */
          pcb = (struct tcp_pcb *)memp_malloc(MEMP_TCP_PCB);
          if (pcb != NULL) {
            /* adjust err stats: memp_malloc failed multiple times before */
            MEMP_STATS_DEC(err, MEMP_TCP_PCB);
          }
        }
        if (pcb != NULL) {
          /* adjust err stats: memp_malloc failed multiple times before */
          MEMP_STATS_DEC(err, MEMP_TCP_PCB);
        }
      }
      if (pcb != NULL) {
        /* adjust err stats: memp_malloc failed multiple times before */
        MEMP_STATS_DEC(err, MEMP_TCP_PCB);
      }
    }
    if (pcb != NULL) {
      /* adjust err stats: memp_malloc failed above */
      MEMP_STATS_DEC(err, MEMP_TCP_PCB);
    }
  }
  if (pcb != NULL) {
    memset(pcb, 0, sizeof(struct tcp_pcb));

#if ESP_PER_SOC_TCP_WND
    pcb->per_soc_tcp_wnd = TCP_WND_DEFAULT;
    pcb->per_soc_tcp_snd_buf = TCP_SND_BUF_DEFAULT;
#endif

    pcb->prio = prio;
    pcb->snd_buf = TCP_SND_BUF_DEFAULT;
    pcb->snd_queuelen = 0;
    /* Start with a window that does not need scaling. When window scaling is
       enabled and used, the window is enlarged when both sides agree on scaling. */
    pcb->rcv_wnd = pcb->rcv_ann_wnd = TCPWND_MIN16(TCP_WND(pcb));
#if LWIP_WND_SCALE
    /* snd_scale and rcv_scale are zero unless both sides agree to use scaling */
    pcb->snd_scale = 0;
    pcb->rcv_scale = 0;
#endif
    pcb->tos = 0;
    pcb->ttl = TCP_TTL;
    /* As initial send MSS, we use TCP_MSS but limit it to 536.
       The send MSS is updated when an MSS option is received. */
    pcb->mss = (TCP_MSS > 536) ? 536 : TCP_MSS;
    pcb->rto = 3000 / TCP_SLOW_INTERVAL;
    pcb->sa = 0;
    pcb->sv = 3000 / TCP_SLOW_INTERVAL;
    pcb->rtime = -1;
    pcb->cwnd = 1;
    iss = tcp_next_iss();
    pcb->snd_wl2 = iss;
    pcb->snd_nxt = iss;
    pcb->lastack = iss;
    pcb->snd_lbb = iss;
    pcb->tmr = tcp_ticks;
    pcb->last_timer = tcp_timer_ctr;
    pcb->polltmr = 0;

#if LWIP_CALLBACK_API
    pcb->recv = tcp_recv_null;
#endif /* LWIP_CALLBACK_API */

    /* Init KEEPALIVE timer */
    pcb->keep_idle  = TCP_KEEPIDLE_DEFAULT;

#if LWIP_TCP_KEEPALIVE
    pcb->keep_intvl = TCP_KEEPINTVL_DEFAULT;
    pcb->keep_cnt   = TCP_KEEPCNT_DEFAULT;
#endif /* LWIP_TCP_KEEPALIVE */

    pcb->keep_cnt_sent = 0;
  }

  return pcb;
}

/**
 * Creates a new TCP protocol control block but doesn't place it on
 * any of the TCP PCB lists.
 * The pcb is not put on any list until binding using tcp_bind().
 *
 * @internal: Maybe there should be a idle TCP PCB list where these
 * PCBs are put on. Port reservation using tcp_bind() is implemented but
 * allocated pcbs that are not bound can't be killed automatically if wanting
 * to allocate a pcb with higher prio (@see tcp_kill_prio())
 *
 * @return a new tcp_pcb that initially is in state CLOSED
 */
struct tcp_pcb *
tcp_new(void)
{
  return tcp_alloc(TCP_PRIO_NORMAL);
}

/**
 * Creates a new TCP protocol control block but doesn't
 * place it on any of the TCP PCB lists.
 * The pcb is not put on any list until binding using tcp_bind().
 *
 * @param type IP address type, see IPADDR_TYPE_XX definitions.
 * @return a new tcp_pcb that initially is in state CLOSED
 */
struct tcp_pcb *
tcp_new_ip_type(u8_t type)
{
  struct tcp_pcb * pcb;
  pcb = tcp_alloc(TCP_PRIO_NORMAL);
#if LWIP_IPV4 && LWIP_IPV6
  if(pcb != NULL) {
    IP_SET_TYPE_VAL(pcb->local_ip, type);
    IP_SET_TYPE_VAL(pcb->remote_ip, type);
  }
#else
  LWIP_UNUSED_ARG(type);
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  return pcb;
}

/**
 * Used to specify the argument that should be passed callback
 * functions.
 *
 * @param pcb tcp_pcb to set the callback argument
 * @param arg void pointer argument to pass to callback functions
 */
void
tcp_arg(struct tcp_pcb *pcb, void *arg)
{
  /* This function is allowed to be called for both listen pcbs and
     connection pcbs. */
  pcb->callback_arg = arg;
}
#if LWIP_CALLBACK_API

/**
 * Used to specify the function that should be called when a TCP
 * connection receives data.
 *
 * @param pcb tcp_pcb to set the recv callback
 * @param recv callback function to call for this pcb when data is received
 */
void
tcp_recv(struct tcp_pcb *pcb, tcp_recv_fn recv)
{
  LWIP_ASSERT("invalid socket state for recv callback", pcb->state != LISTEN);
  pcb->recv = recv;
}

/**
 * Used to specify the function that should be called when TCP data
 * has been successfully delivered to the remote host.
 *
 * @param pcb tcp_pcb to set the sent callback
 * @param sent callback function to call for this pcb when data is successfully sent
 */
void
tcp_sent(struct tcp_pcb *pcb, tcp_sent_fn sent)
{
  LWIP_ASSERT("invalid socket state for sent callback", pcb->state != LISTEN);
  pcb->sent = sent;
}

/**
 * Used to specify the function that should be called when a fatal error
 * has occurred on the connection.
 *
 * @param pcb tcp_pcb to set the err callback
 * @param err callback function to call for this pcb when a fatal error
 *        has occurred on the connection
 */
void
tcp_err(struct tcp_pcb *pcb, tcp_err_fn err)
{
  LWIP_ASSERT("invalid socket state for err callback", pcb->state != LISTEN);
  pcb->errf = err;
}

/**
 * Used for specifying the function that should be called when a
 * LISTENing connection has been connected to another host.
 *
 * @param pcb tcp_pcb to set the accept callback
 * @param accept callback function to call for this pcb when LISTENing
 *        connection has been connected to another host
 */
void
tcp_accept(struct tcp_pcb *pcb, tcp_accept_fn accept)
{
  /* This function is allowed to be called for both listen pcbs and
     connection pcbs. */
  pcb->accept = accept;
}
#endif /* LWIP_CALLBACK_API */


/**
 * Used to specify the function that should be called periodically
 * from TCP. The interval is specified in terms of the TCP coarse
 * timer interval, which is called twice a second.
 *
 */
void
tcp_poll(struct tcp_pcb *pcb, tcp_poll_fn poll, u8_t interval)
{
  LWIP_ASSERT("invalid socket state for poll", pcb->state != LISTEN);
#if LWIP_CALLBACK_API
  pcb->poll = poll;
#else /* LWIP_CALLBACK_API */
  LWIP_UNUSED_ARG(poll);
#endif /* LWIP_CALLBACK_API */
  pcb->pollinterval = interval;
}

/**
 * Purges a TCP PCB. Removes any buffered data and frees the buffer memory
 * (pcb->ooseq, pcb->unsent and pcb->unacked are freed).
 *
 * @param pcb tcp_pcb to purge. The pcb itself is not deallocated!
 */
void
tcp_pcb_purge(struct tcp_pcb *pcb)
{
  if (pcb->state != CLOSED &&
     pcb->state != TIME_WAIT &&
     pcb->state != LISTEN) {

    LWIP_DEBUGF(TCP_DEBUG, ("tcp_pcb_purge\n"));

    tcp_backlog_accepted(pcb);

    if (pcb->refused_data != NULL) {
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_pcb_purge: data left on ->refused_data\n"));
      pbuf_free(pcb->refused_data);
      pcb->refused_data = NULL;
    }
    if (pcb->unsent != NULL) {
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_pcb_purge: not all data sent\n"));
    }
    if (pcb->unacked != NULL) {
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_pcb_purge: data left on ->unacked\n"));
    }
#if TCP_QUEUE_OOSEQ
    if (pcb->ooseq != NULL) {
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_pcb_purge: data left on ->ooseq\n"));
    }
    tcp_segs_free(pcb->ooseq);
    pcb->ooseq = NULL;
#endif /* TCP_QUEUE_OOSEQ */

    /* Stop the retransmission timer as it will expect data on unacked
       queue if it fires */
    pcb->rtime = -1;

    tcp_segs_free(pcb->unsent);
    tcp_segs_free(pcb->unacked);
    pcb->unacked = pcb->unsent = NULL;
#if TCP_OVERSIZE
    pcb->unsent_oversize = 0;
#endif /* TCP_OVERSIZE */
  }
}

/**
 * Purges the PCB and removes it from a PCB list. Any delayed ACKs are sent first.
 *
 * @param pcblist PCB list to purge.
 * @param pcb tcp_pcb to purge. The pcb itself is NOT deallocated!
 */
void
tcp_pcb_remove(struct tcp_pcb **pcblist, struct tcp_pcb *pcb)
{
  TCP_RMV(pcblist, pcb);

  tcp_pcb_purge(pcb);

  /* if there is an outstanding delayed ACKs, send it */
  if (pcb->state != TIME_WAIT &&
     pcb->state != LISTEN &&
     pcb->flags & TF_ACK_DELAY) {
    pcb->flags |= TF_ACK_NOW;
    tcp_output(pcb);
  }

  if (pcb->state != LISTEN) {
    LWIP_ASSERT("unsent segments leaking", pcb->unsent == NULL);
    LWIP_ASSERT("unacked segments leaking", pcb->unacked == NULL);
#if TCP_QUEUE_OOSEQ
    LWIP_ASSERT("ooseq segments leaking", pcb->ooseq == NULL);
#endif /* TCP_QUEUE_OOSEQ */
  }

  pcb->state = CLOSED;
  /* reset the local port to prevent the pcb from being 'bound' */
  pcb->local_port = 0;

  LWIP_ASSERT("tcp_pcb_remove: tcp_pcbs_sane()", tcp_pcbs_sane());
}

/**
 * Calculates a new initial sequence number for new connections.
 *
 * @return u32_t pseudo random sequence number
 */
u32_t
tcp_next_iss(void)
{
  static u32_t iss = 6510;

  iss += tcp_ticks;       /* XXX */
  return iss;
}

#if TCP_CALCULATE_EFF_SEND_MSS
/**
 * Calculates the effective send mss that can be used for a specific IP address
 * by using ip_route to determine the netif used to send to the address and
 * calculating the minimum of TCP_MSS and that netif's mtu (if set).
 */
u16_t
tcp_eff_send_mss_impl(u16_t sendmss, const ip_addr_t *dest
#if LWIP_IPV6 || LWIP_IPV4_SRC_ROUTING
                     , const ip_addr_t *src
#endif /* LWIP_IPV6 || LWIP_IPV4_SRC_ROUTING */
                     )
{
  u16_t mss_s;
  struct netif *outif;
  s16_t mtu;

  outif = ip_route(src, dest);
#if LWIP_IPV6
#if LWIP_IPV4
  if (IP_IS_V6(dest))
#endif /* LWIP_IPV4 */
  {
    /* First look in destination cache, to see if there is a Path MTU. */
    mtu = nd6_get_destination_mtu(ip_2_ip6(dest), outif);
  }
#if LWIP_IPV4
  else
#endif /* LWIP_IPV4 */
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
  {
    if (outif == NULL) {
      return sendmss;
    }
    mtu = outif->mtu;
  }
#endif /* LWIP_IPV4 */

  if (mtu != 0) {
#if LWIP_IPV6
#if LWIP_IPV4
    if (IP_IS_V6(dest))
#endif /* LWIP_IPV4 */
    {
      mss_s = mtu - IP6_HLEN - TCP_HLEN;
    }
#if LWIP_IPV4
    else
#endif /* LWIP_IPV4 */
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
    {
      mss_s = mtu - IP_HLEN - TCP_HLEN;
    }
#endif /* LWIP_IPV4 */
    /* RFC 1122, chap 4.2.2.6:
     * Eff.snd.MSS = min(SendMSS+20, MMS_S) - TCPhdrsize - IPoptionsize
     * We correct for TCP options in tcp_write(), and don't support IP options.
     */
    sendmss = LWIP_MIN(sendmss, mss_s);
  }
  return sendmss;
}
#endif /* TCP_CALCULATE_EFF_SEND_MSS */

/** Helper function for tcp_netif_ip4_addr_changed() that iterates a pcb list */
static void
tcp_netif_ip_addr_changed_pcblist(const ip4_addr_t* old_addr, struct tcp_pcb* pcb_list)
{
  struct tcp_pcb *pcb;
  pcb = pcb_list;
  while (pcb != NULL) {
    /* PCB bound to current local interface address? */
    if (ip4_addr_cmp(ip_2_ip4(&pcb->local_ip), old_addr)
#if LWIP_AUTOIP
      /* connections to link-local addresses must persist (RFC3927 ch. 1.9) */
      && (!IP_IS_V4_VAL(pcb->local_ip) || !ip4_addr_islinklocal(ip_2_ip4(&pcb->local_ip)))
#endif /* LWIP_AUTOIP */
      ) {
        /* this connection must be aborted */
        struct tcp_pcb *next = pcb->next;
        LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_set_ipaddr: aborting TCP pcb %p\n", (void *)pcb));
        tcp_abort(pcb);
        pcb = next;
    } else {
      pcb = pcb->next;
    }
  }
}

#if LWIP_IPV4
/** This function is called from netif.c when address is changed or netif is removed
 *
 * @param old_addr IPv4 address of the netif before change
 * @param new_addr IPv4 address of the netif after change or NULL if netif has been removed
 */
void tcp_netif_ipv4_addr_changed(const ip4_addr_t* old_addr, const ip4_addr_t* new_addr)
{
  struct tcp_pcb_listen *lpcb, *next;

  if (!ip4_addr_isany(old_addr)) {
#if ESP_TCP_KEEP_CONNECTION_WHEN_IP_CHANGES
    if ((new_addr == NULL) || ((!ip4_addr_isany_val(*new_addr)) && (!ip4_addr_cmp(old_addr, new_addr)))) {
#endif
      tcp_netif_ip_addr_changed_pcblist(old_addr, tcp_active_pcbs);
      tcp_netif_ip_addr_changed_pcblist(old_addr, tcp_bound_pcbs);
#if ESP_TCP_KEEP_CONNECTION_WHEN_IP_CHANGES
    }
#endif

    if (!ip4_addr_isany(new_addr)) {
      /* PCB bound to current local interface address? */
      for (lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb = next) {
        next = lpcb->next;
        /* Is this an IPv4 pcb? */
        if (!IP_IS_V6_VAL(lpcb->local_ip)) {
          /* PCB bound to current local interface address? */
          if ((!(ip4_addr_isany(ip_2_ip4(&lpcb->local_ip)))) &&
              (ip4_addr_cmp(ip_2_ip4(&lpcb->local_ip), old_addr))) {
            /* The PCB is listening to the old ipaddr and
             * is set to listen to the new one instead */
            ip_addr_copy_from_ip4(lpcb->local_ip, *new_addr);
          }
        }
      }
    }
  }
}
#endif /* LWIP_IPV4 */

const char*
tcp_debug_state_str(enum tcp_state s)
{
      return tcp_state_str[s];
}

#if TCP_DEBUG || TCP_INPUT_DEBUG || TCP_OUTPUT_DEBUG
/**
 * Print a tcp header for debugging purposes.
 *
 * @param tcphdr pointer to a struct tcp_hdr
 */
void
tcp_debug_print(struct tcp_hdr *tcphdr)
{
  LWIP_DEBUGF(TCP_DEBUG, ("TCP header:\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("|    %5"U16_F"      |    %5"U16_F"      | (src port, dest port)\n",
         ntohs(tcphdr->src), ntohs(tcphdr->dest)));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("|           %010"U32_F"          | (seq no)\n",
          ntohl(tcphdr->seqno)));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("|           %010"U32_F"          | (ack no)\n",
         ntohl(tcphdr->ackno)));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("| %2"U16_F" |   |%"U16_F"%"U16_F"%"U16_F"%"U16_F"%"U16_F"%"U16_F"|     %5"U16_F"     | (hdrlen, flags (",
       TCPH_HDRLEN(tcphdr),
         TCPH_FLAGS(tcphdr) >> 5 & 1,
         TCPH_FLAGS(tcphdr) >> 4 & 1,
         TCPH_FLAGS(tcphdr) >> 3 & 1,
         TCPH_FLAGS(tcphdr) >> 2 & 1,
         TCPH_FLAGS(tcphdr) >> 1 & 1,
         TCPH_FLAGS(tcphdr) & 1,
         ntohs(tcphdr->wnd)));
  tcp_debug_print_flags(TCPH_FLAGS(tcphdr));
  LWIP_DEBUGF(TCP_DEBUG, ("), win)\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(TCP_DEBUG, ("|    0x%04"X16_F"     |     %5"U16_F"     | (chksum, urgp)\n",
         ntohs(tcphdr->chksum), ntohs(tcphdr->urgp)));
  LWIP_DEBUGF(TCP_DEBUG, ("+-------------------------------+\n"));
}

/**
 * Print a tcp state for debugging purposes.
 *
 * @param s enum tcp_state to print
 */
void
tcp_debug_print_state(enum tcp_state s)
{
  LWIP_DEBUGF(TCP_DEBUG, ("State: %s\n", tcp_state_str[s]));
}

/**
 * Print tcp flags for debugging purposes.
 *
 * @param flags tcp flags, all active flags are printed
 */
void
tcp_debug_print_flags(u8_t flags)
{
  if (flags & TCP_FIN) {
    LWIP_DEBUGF(TCP_DEBUG, ("FIN "));
  }
  if (flags & TCP_SYN) {
    LWIP_DEBUGF(TCP_DEBUG, ("SYN "));
  }
  if (flags & TCP_RST) {
    LWIP_DEBUGF(TCP_DEBUG, ("RST "));
  }
  if (flags & TCP_PSH) {
    LWIP_DEBUGF(TCP_DEBUG, ("PSH "));
  }
  if (flags & TCP_ACK) {
    LWIP_DEBUGF(TCP_DEBUG, ("ACK "));
  }
  if (flags & TCP_URG) {
    LWIP_DEBUGF(TCP_DEBUG, ("URG "));
  }
  if (flags & TCP_ECE) {
    LWIP_DEBUGF(TCP_DEBUG, ("ECE "));
  }
  if (flags & TCP_CWR) {
    LWIP_DEBUGF(TCP_DEBUG, ("CWR "));
  }
  LWIP_DEBUGF(TCP_DEBUG, ("\n"));
}

/**
 * Print all tcp_pcbs in every list for debugging purposes.
 */
void
tcp_debug_print_pcbs(void)
{
  struct tcp_pcb *pcb;
  LWIP_DEBUGF(TCP_DEBUG, ("Active PCB states:\n"));
  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_DEBUGF(TCP_DEBUG, ("Local port %"U16_F", foreign port %"U16_F" snd_nxt %"U32_F" rcv_nxt %"U32_F" ",
                       pcb->local_port, pcb->remote_port,
                       pcb->snd_nxt, pcb->rcv_nxt));
    tcp_debug_print_state(pcb->state);
  }
  LWIP_DEBUGF(TCP_DEBUG, ("Listen PCB states:\n"));
  for (pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_DEBUGF(TCP_DEBUG, ("Local port %"U16_F", foreign port %"U16_F" snd_nxt %"U32_F" rcv_nxt %"U32_F" ",
                       pcb->local_port, pcb->remote_port,
                       pcb->snd_nxt, pcb->rcv_nxt));
    tcp_debug_print_state(pcb->state);
  }
  LWIP_DEBUGF(TCP_DEBUG, ("TIME-WAIT PCB states:\n"));
  for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_DEBUGF(TCP_DEBUG, ("Local port %"U16_F", foreign port %"U16_F" snd_nxt %"U32_F" rcv_nxt %"U32_F" ",
                       pcb->local_port, pcb->remote_port,
                       pcb->snd_nxt, pcb->rcv_nxt));
    tcp_debug_print_state(pcb->state);
  }
}

/**
 * Check state consistency of the tcp_pcb lists.
 */
s16_t
tcp_pcbs_sane(void)
{
  struct tcp_pcb *pcb;
  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_ASSERT("tcp_pcbs_sane: active pcb->state != CLOSED", pcb->state != CLOSED);
    LWIP_ASSERT("tcp_pcbs_sane: active pcb->state != LISTEN", pcb->state != LISTEN);
    LWIP_ASSERT("tcp_pcbs_sane: active pcb->state != TIME-WAIT", pcb->state != TIME_WAIT);
  }
  for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_ASSERT("tcp_pcbs_sane: tw pcb->state == TIME-WAIT", pcb->state == TIME_WAIT);
  }
  return 1;
}
#endif /* TCP_DEBUG */

#endif /* LWIP_TCP */
