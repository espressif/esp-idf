/**
 * @file
 * Sequential API Main thread module
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

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/api_msg.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"

#define TCPIP_MSG_VAR_REF(name)     API_VAR_REF(name)
#define TCPIP_MSG_VAR_DECLARE(name) API_VAR_DECLARE(struct tcpip_msg, name)
#define TCPIP_MSG_VAR_ALLOC(name)   API_VAR_ALLOC(struct tcpip_msg, MEMP_TCPIP_MSG_API, name)
#define TCPIP_MSG_VAR_FREE(name)    API_VAR_FREE(MEMP_TCPIP_MSG_API, name)

/* global variables */
static tcpip_init_done_fn tcpip_init_done;
static void *tcpip_init_done_arg;
static sys_mbox_t mbox;
sys_thread_t g_lwip_task = NULL;

#if LWIP_TCPIP_CORE_LOCKING
/** The global semaphore to lock the stack. */
sys_mutex_t lock_tcpip_core;
#endif /* LWIP_TCPIP_CORE_LOCKING */

#if LWIP_TIMERS
/* wait for a message, timeouts are processed while waiting */
#define TCPIP_MBOX_FETCH(mbox, msg) sys_timeouts_mbox_fetch(mbox, msg)
#else /* LWIP_TIMERS */
/* wait for a message with timers disabled (e.g. pass a timer-check trigger into tcpip_thread) */
#define TCPIP_MBOX_FETCH(mbox, msg) sys_mbox_fetch(mbox, msg)
#endif /* LWIP_TIMERS */

/**
 * The main lwIP thread. This thread has exclusive access to lwIP core functions
 * (unless access to them is not locked). Other threads communicate with this
 * thread using message boxes.
 *
 * It also starts all the timers to make sure they are running in the right
 * thread context.
 *
 * @param arg unused argument
 */
static void ESP_IRAM_ATTR
tcpip_thread(void *arg)
{


  struct tcpip_msg *msg;
  LWIP_UNUSED_ARG(arg);

  if (tcpip_init_done != NULL) {
    tcpip_init_done(tcpip_init_done_arg);
  }



  LOCK_TCPIP_CORE();
  while (1) {         


    /* MAIN Loop */
    UNLOCK_TCPIP_CORE();
    LWIP_TCPIP_THREAD_ALIVE();
    /* wait for a message, timeouts are processed while waiting */
    TCPIP_MBOX_FETCH(&mbox, (void **)&msg);
    LOCK_TCPIP_CORE();
    

    
    if (msg == NULL) {
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: NULL\n"));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);

      continue;
    }

    

    
    switch (msg->type) {
#if !LWIP_TCPIP_CORE_LOCKING
    case TCPIP_MSG_API:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API message %p\n", (void *)msg));
      msg->msg.api.function(msg->msg.api.msg);
      break;
    case TCPIP_MSG_API_CALL:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API CALL message %p\n", (void *)msg));
      msg->msg.api_call->err = msg->msg.api_call->function(msg->msg.api_call);
#if LWIP_NETCONN_SEM_PER_THREAD
      sys_sem_signal(msg->msg.api_call->sem);
#else /* LWIP_NETCONN_SEM_PER_THREAD */
      sys_sem_signal(&msg->msg.api_call->sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
      break;
#endif /* LWIP_TCPIP_CORE_LOCKING */

#if !LWIP_TCPIP_CORE_LOCKING_INPUT
    case TCPIP_MSG_INPKT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: PACKET %p\n", (void *)msg));

#if ESP_LWIP
        if(msg->msg.inp.p != NULL && msg->msg.inp.netif != NULL) {
#endif
            msg->msg.inp.input_fn(msg->msg.inp.p, msg->msg.inp.netif);
#if ESP_LWIP
        }
#endif

        memp_free(MEMP_TCPIP_MSG_INPKT, msg);

      break;
#endif /* LWIP_TCPIP_CORE_LOCKING_INPUT */

#if LWIP_TCPIP_TIMEOUT
    case TCPIP_MSG_TIMEOUT:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: TIMEOUT %p\n", (void *)msg));
      sys_timeout(msg->msg.tmo.msecs, msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
    case TCPIP_MSG_UNTIMEOUT:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: UNTIMEOUT %p\n", (void *)msg));
      sys_untimeout(msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
#endif /* LWIP_TCPIP_TIMEOUT */

    case TCPIP_MSG_CALLBACK:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;

    case TCPIP_MSG_CALLBACK_STATIC:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK_STATIC %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      break;

    default:

      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: %d\n", msg->type));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);
      break;
    }

  }
}

/**
 * Pass a received packet to tcpip_thread for input processing
 *
 * @param p the received packet
 * @param inp the network interface on which the packet was received
 * @param input_fn input function to call
 */
err_t ESP_IRAM_ATTR
tcpip_inpkt(struct pbuf *p, struct netif *inp, netif_input_fn input_fn)
{
#if LWIP_TCPIP_CORE_LOCKING_INPUT
  err_t ret;
  LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_inpkt: PACKET %p/%p\n", (void *)p, (void *)inp));
  LOCK_TCPIP_CORE();
  ret = input_fn(p, inp);
  UNLOCK_TCPIP_CORE();
  return ret;
#else /* LWIP_TCPIP_CORE_LOCKING_INPUT */
  struct tcpip_msg *msg;

  if (!sys_mbox_valid_val(mbox)) {
    return ERR_VAL;
  }
  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_INPKT);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_INPKT;
  msg->msg.inp.p = p;
  msg->msg.inp.netif = inp;
  msg->msg.inp.input_fn = input_fn;
#if ESP_PERF
  if (p->len > DBG_PERF_FILTER_LEN) {
    DBG_PERF_PATH_SET(DBG_PERF_DIR_RX, DBG_PERF_POINT_WIFI_OUT);
  }
#endif

  if (sys_mbox_trypost(&mbox, msg) != ERR_OK) {
    ESP_STATS_DROP_INC(esp.tcpip_inpkt_post_fail);
    memp_free(MEMP_TCPIP_MSG_INPKT, msg);
    return ERR_MEM;
  }
  return ERR_OK;
#endif /* LWIP_TCPIP_CORE_LOCKING_INPUT */
}

/**
 * Pass a received packet to tcpip_thread for input processing with
 * ethernet_input or ip_input
 *
 * @param p the received packet, p->payload pointing to the Ethernet header or
 *          to an IP header (if inp doesn't have NETIF_FLAG_ETHARP or
 *          NETIF_FLAG_ETHERNET flags)
 * @param inp the network interface on which the packet was received
 */
err_t ESP_IRAM_ATTR
tcpip_input(struct pbuf *p, struct netif *inp)
{
#if LWIP_ETHERNET
  if (inp->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)) {
    return tcpip_inpkt(p, inp, ethernet_input);
  } else
#endif /* LWIP_ETHERNET */
  return tcpip_inpkt(p, inp, ip_input);
}

/**
 * Call a specific function in the thread context of
 * tcpip_thread for easy access synchronization.
 * A function called in that way may access lwIP core code
 * without fearing concurrent access.
 *
 * @param f the function to call
 * @param ctx parameter passed to f
 * @param block 1 to block until the request is posted, 0 to non-blocking mode
 * @return ERR_OK if the function was called, another err_t if not
 */
err_t
tcpip_callback_with_block(tcpip_callback_fn function, void *ctx, u8_t block)
{
  struct tcpip_msg *msg;

  if (sys_mbox_valid_val(mbox)) {
    msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
    if (msg == NULL) {
      return ERR_MEM;
    }

    msg->type = TCPIP_MSG_CALLBACK;
    msg->msg.cb.function = function;
    msg->msg.cb.ctx = ctx;
    if (block) {
      sys_mbox_post(&mbox, msg);
    } else {
      if (sys_mbox_trypost(&mbox, msg) != ERR_OK) {
        ESP_STATS_DROP_INC(esp.tcpip_cb_post_fail);
        memp_free(MEMP_TCPIP_MSG_API, msg);
        return ERR_MEM;
      }
    }
    return ERR_OK;
  }
  return ERR_VAL;
}

#if LWIP_TCPIP_TIMEOUT
/**
 * call sys_timeout in tcpip_thread
 *
 * @param msec time in milliseconds for timeout
 * @param h function to be called on timeout
 * @param arg argument to pass to timeout function h
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t
tcpip_timeout(u32_t msecs, sys_timeout_handler h, void *arg)
{
  struct tcpip_msg *msg;

  if (sys_mbox_valid_val(mbox)) {
    msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
    if (msg == NULL) {
      return ERR_MEM;
    }

    msg->type = TCPIP_MSG_TIMEOUT;
    msg->msg.tmo.msecs = msecs;
    msg->msg.tmo.h = h;
    msg->msg.tmo.arg = arg;
    sys_mbox_post(&mbox, msg);
    return ERR_OK;
  }
  return ERR_VAL;
}

/**
 * call sys_untimeout in tcpip_thread
 *
 * @param msec time in milliseconds for timeout
 * @param h function to be called on timeout
 * @param arg argument to pass to timeout function h
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t
tcpip_untimeout(sys_timeout_handler h, void *arg)
{
  struct tcpip_msg *msg;

  if (sys_mbox_valid_val(mbox)) {
    msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
    if (msg == NULL) {
      return ERR_MEM;
    }

    msg->type = TCPIP_MSG_UNTIMEOUT;
    msg->msg.tmo.h = h;
    msg->msg.tmo.arg = arg;
    sys_mbox_post(&mbox, msg);
    return ERR_OK;
  }
  return ERR_VAL;
}
#endif /* LWIP_TCPIP_TIMEOUT */


#if !LWIP_TCPIP_CORE_LOCKING
/**
 * Generic way to dispatch an API message in TCPIP thread.
 *
 * @param fn function to be called from TCPIP thread
 * @param apimsg argument to API function
 * @param sem semaphore to wait on
 * @return ERR_OK if the function was called, another err_t if not
 */
err_t ESP_IRAM_ATTR
tcpip_send_api_msg(tcpip_callback_fn fn, void *apimsg, sys_sem_t* sem)
{
  LWIP_ASSERT("semaphore not initialized", sys_sem_valid(sem));

  if (sys_mbox_valid_val(mbox)) {
    TCPIP_MSG_VAR_DECLARE(msg);
    
    TCPIP_MSG_VAR_ALLOC(msg);
    TCPIP_MSG_VAR_REF(msg).type = TCPIP_MSG_API;
    TCPIP_MSG_VAR_REF(msg).msg.api.function = fn;
    TCPIP_MSG_VAR_REF(msg).msg.api.msg = apimsg;
    sys_mbox_post(&mbox, &TCPIP_MSG_VAR_REF(msg));
    sys_arch_sem_wait(sem, 0);
    TCPIP_MSG_VAR_FREE(msg);

    return ERR_OK;
  }
  return ERR_VAL;
}
#endif /* !LWIP_TCPIP_CORE_LOCKING */

err_t tcpip_api_call(tcpip_api_call_fn fn, struct tcpip_api_call *call)
{
#if LWIP_TCPIP_CORE_LOCKING
  err_t err;
  LOCK_TCPIP_CORE();
  err = fn(call);
  UNLOCK_TCPIP_CORE();
  return err;
#else
  if (sys_mbox_valid_val(mbox)) {
    TCPIP_MSG_VAR_DECLARE(msg);
    err_t err;

#if LWIP_NETCONN_SEM_PER_THREAD
    call->sem = LWIP_NETCONN_THREAD_SEM_GET();
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    err = sys_sem_new(&call->sem, 0);
    if (err != ERR_OK) {
      return err;
    }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
    
    TCPIP_MSG_VAR_ALLOC(msg);
    TCPIP_MSG_VAR_REF(msg).type = TCPIP_MSG_API_CALL;
    TCPIP_MSG_VAR_REF(msg).msg.api_call = call;
    TCPIP_MSG_VAR_REF(msg).msg.api_call->function = fn;
    sys_mbox_post(&mbox, &TCPIP_MSG_VAR_REF(msg));
#if LWIP_NETCONN_SEM_PER_THREAD
    sys_arch_sem_wait(call->sem, 0);
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    sys_arch_sem_wait(&call->sem, 0);
    sys_sem_free(&call->sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
    TCPIP_MSG_VAR_FREE(msg);
    return ERR_OK;
  }
  return ERR_VAL;
#endif
}

/**
 * Allocate a structure for a static callback message and initialize it.
 * This is intended to be used to send "static" messages from interrupt context.
 *
 * @param function the function to call
 * @param ctx parameter passed to function
 * @return a struct pointer to pass to tcpip_trycallback().
 */
struct tcpip_callback_msg*
tcpip_callbackmsg_new(tcpip_callback_fn function, void *ctx)
{
  struct tcpip_msg *msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return NULL;
  }
  msg->type = TCPIP_MSG_CALLBACK_STATIC;
  msg->msg.cb.function = function;
  msg->msg.cb.ctx = ctx;
  return (struct tcpip_callback_msg*)msg;
}

/**
 * Free a callback message allocated by tcpip_callbackmsg_new().
 *
 * @param msg the message to free
 */
void
tcpip_callbackmsg_delete(struct tcpip_callback_msg* msg)
{
  memp_free(MEMP_TCPIP_MSG_API, msg);
}

/**
 * Try to post a callback-message to the tcpip_thread mbox
 * This is intended to be used to send "static" messages from interrupt context.
 *
 * @param msg pointer to the message to post
 * @return sys_mbox_trypost() return code
 */
err_t
tcpip_trycallback(struct tcpip_callback_msg* msg)
{
  if (!sys_mbox_valid_val(mbox)) {
    return ERR_VAL;
  }
  return sys_mbox_trypost(&mbox, msg);
}

/**
 * Initialize this module:
 * - initialize all sub modules
 * - start the tcpip_thread
 *
 * @param initfunc a function to call when tcpip_thread is running and finished initializing
 * @param arg argument to pass to initfunc
 */
void
tcpip_init(tcpip_init_done_fn initfunc, void *arg)
{

  lwip_init();

  tcpip_init_done = initfunc;
  tcpip_init_done_arg = arg;
  if (sys_mbox_new(&mbox, TCPIP_MBOX_SIZE) != ERR_OK) {
    LWIP_ASSERT("failed to create tcpip_thread mbox", 0);
  }
#if LWIP_TCPIP_CORE_LOCKING
  if (sys_mutex_new(&lock_tcpip_core) != ERR_OK) {
    LWIP_ASSERT("failed to create lock_tcpip_core", 0);
  }
#endif /* LWIP_TCPIP_CORE_LOCKING */


  g_lwip_task = sys_thread_new(TCPIP_THREAD_NAME
                , tcpip_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);

  LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_task_hdlxxx : %x, prio:%d,stack:%d\n",
		 (u32_t)g_lwip_task,TCPIP_THREAD_PRIO,TCPIP_THREAD_STACKSIZE));

}

/**
 * Simple callback function used with tcpip_callback to free a pbuf
 * (pbuf_free has a wrong signature for tcpip_callback)
 *
 * @param p The pbuf (chain) to be dereferenced.
 */
static void
pbuf_free_int(void *p)
{
  struct pbuf *q = (struct pbuf *)p;
  pbuf_free(q);
}

/**
 * A simple wrapper function that allows you to free a pbuf from interrupt context.
 *
 * @param p The pbuf (chain) to be dereferenced.
 * @return ERR_OK if callback could be enqueued, an err_t if not
 */
err_t
pbuf_free_callback(struct pbuf *p)
{
  return tcpip_callback_with_block(pbuf_free_int, p, 0);
}

/**
 * A simple wrapper function that allows you to free heap memory from
 * interrupt context.
 *
 * @param m the heap memory to free
 * @return ERR_OK if callback could be enqueued, an err_t if not
 */
 
#if ESP_LWIP
static void mem_free_local(void *arg)
{
	mem_free(arg);
}
err_t mem_free_callback(void *m)
{
      return tcpip_callback_with_block(mem_free_local, m, 0);
      
#else
err_t mem_free_callback(void *m)
{
#endif

  return tcpip_callback_with_block(mem_free, m, 0);  
}

#endif /* !NO_SYS */
