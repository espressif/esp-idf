/**
 * @file
 * Point To Point Protocol Sequential API module
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

#include "lwip/opt.h"

#if LWIP_PPP_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/pppapi.h"
#include "lwip/priv/tcpip_priv.h"
#include "netif/ppp/pppoe.h"
#include "netif/ppp/pppol2tp.h"
#include "netif/ppp/pppos.h"


/**
 * Call ppp_set_default() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_set_default(struct tcpip_api_call *m)
{
  struct pppapi_msg_msg *msg = (struct pppapi_msg_msg *)m;
  
  ppp_set_default(msg->ppp);
  return ERR_OK;
}

/**
 * Call ppp_set_default() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
void
pppapi_set_default(ppp_pcb *pcb)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  tcpip_api_call(pppapi_do_ppp_set_default, &msg.call);
}


/**
 * Call ppp_set_auth() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_set_auth(struct tcpip_api_call *m)
{
  struct pppapi_msg *msg = (struct pppapi_msg *)m;

  ppp_set_auth(msg->msg.ppp, msg->msg.msg.setauth.authtype,
               msg->msg.msg.setauth.user, msg->msg.msg.setauth.passwd);
  return ERR_OK;
}

/**
 * Call ppp_set_auth() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
void
pppapi_set_auth(ppp_pcb *pcb, u8_t authtype, const char *user, const char *passwd)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  msg.msg.msg.setauth.authtype = authtype;
  msg.msg.msg.setauth.user = user;
  msg.msg.msg.setauth.passwd = passwd;
  tcpip_api_call(pppapi_do_ppp_set_auth, &msg.call);
}


#if PPP_NOTIFY_PHASE
/**
 * Call ppp_set_notify_phase_callback() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_set_notify_phase_callback(struct tcpip_api_call *m)
{
  struct pppapi_msg_msg *msg = (struct pppapi_msg_msg *)m;
  ppp_set_notify_phase_callback(msg->ppp, msg->msg.setnotifyphasecb.notify_phase_cb);
  return ERR_OK;
}

/**
 * Call ppp_set_notify_phase_callback() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
void
pppapi_set_notify_phase_callback(ppp_pcb *pcb, ppp_notify_phase_cb_fn notify_phase_cb)
{
  struct pppapi_msg msg;
  msg.function = pppapi_do_ppp_set_notify_phase_callback;
  msg.msg.ppp = pcb;
  msg.msg.msg.setnotifyphasecb.notify_phase_cb = notify_phase_cb;
  tcpip_api_call(pppapi_do_ppp_set_notify_phase_callback, &msg.call);
}
#endif /* PPP_NOTIFY_PHASE */


#if PPPOS_SUPPORT
/**
 * Call pppos_create() inside the tcpip_thread context.
 */
static err_t
pppapi_do_pppos_create(struct tcpip_api_call *m)
{
  struct pppapi_msg *msg = (struct pppapi_msg *)(m);

  msg->msg.ppp = pppos_create(msg->msg.msg.serialcreate.pppif, msg->msg.msg.serialcreate.output_cb,
    msg->msg.msg.serialcreate.link_status_cb, msg->msg.msg.serialcreate.ctx_cb);
  return ERR_OK;
}

/**
 * Call pppos_create() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
ppp_pcb*
pppapi_pppos_create(struct netif *pppif, pppos_output_cb_fn output_cb,
               ppp_link_status_cb_fn link_status_cb, void *ctx_cb)
{
  struct pppapi_msg msg;
  msg.msg.msg.serialcreate.pppif = pppif;
  msg.msg.msg.serialcreate.output_cb = output_cb;
  msg.msg.msg.serialcreate.link_status_cb = link_status_cb;
  msg.msg.msg.serialcreate.ctx_cb = ctx_cb;
  tcpip_api_call(pppapi_do_pppos_create, &msg.call);
  return msg.msg.ppp;
}
#endif /* PPPOS_SUPPORT */


#if PPPOE_SUPPORT
/**
 * Call pppoe_create() inside the tcpip_thread context.
 */
static err_t
pppapi_do_pppoe_create(struct tcpip_api_call *m)
{
  struct pppapi_msg_msg *msg = (struct pppapi_msg_msg *)m;

  msg->ppp = pppoe_create(msg->msg.ethernetcreate.pppif, msg->msg.ethernetcreate.ethif,
    msg->msg.ethernetcreate.service_name, msg->msg.ethernetcreate.concentrator_name,
    msg->msg.ethernetcreate.link_status_cb, msg->msg.ethernetcreate.ctx_cb);
  return ERR_OK;
}

/**
 * Call pppoe_create() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
ppp_pcb*
pppapi_pppoe_create(struct netif *pppif, struct netif *ethif, const char *service_name,
                            const char *concentrator_name, ppp_link_status_cb_fn link_status_cb,
                            void *ctx_cb)
{
  struct pppapi_msg msg;
  msg.msg.msg.ethernetcreate.pppif = pppif;
  msg.msg.msg.ethernetcreate.ethif = ethif;
  msg.msg.msg.ethernetcreate.service_name = service_name;
  msg.msg.msg.ethernetcreate.concentrator_name = concentrator_name;
  msg.msg.msg.ethernetcreate.link_status_cb = link_status_cb;
  msg.msg.msg.ethernetcreate.ctx_cb = ctx_cb;
  tcpip_api_call(pppapi_do_pppoe_create, &msg.call);
  return msg.msg.ppp;
}
#endif /* PPPOE_SUPPORT */


#if PPPOL2TP_SUPPORT
/**
 * Call pppol2tp_create() inside the tcpip_thread context.
 */
static err_t
pppapi_do_pppol2tp_create(struct tcpip_api_call *m)
{
  struct pppapi_msg_msg *msg = (struct pppapi_msg_msg *)m;

  msg->ppp = pppol2tp_create(msg->msg.l2tpcreate.pppif,
    msg->msg.l2tpcreate.netif, msg->msg.l2tpcreate.ipaddr, msg->msg.l2tpcreate.port,
#if PPPOL2TP_AUTH_SUPPORT
    msg->msg.l2tpcreate.secret,
    msg->msg.l2tpcreate.secret_len,
#else /* PPPOL2TP_AUTH_SUPPORT */
    NULL,
#endif /* PPPOL2TP_AUTH_SUPPORT */
    msg->msg.l2tpcreate.link_status_cb, msg->msg.l2tpcreate.ctx_cb);
  return ERR_OK;
}

/**
 * Call pppol2tp_create() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
ppp_pcb*
pppapi_pppol2tp_create(struct netif *pppif, struct netif *netif, ip_addr_t *ipaddr, u16_t port,
                        const u8_t *secret, u8_t secret_len,
                        ppp_link_status_cb_fn link_status_cb, void *ctx_cb)
{
  struct pppapi_msg msg;
  msg.msg.msg.l2tpcreate.pppif = pppif;
  msg.msg.msg.l2tpcreate.netif = netif;
  msg.msg.msg.l2tpcreate.ipaddr = ipaddr;
  msg.msg.msg.l2tpcreate.port = port;
#if PPPOL2TP_AUTH_SUPPORT
  msg.msg.msg.l2tpcreate.secret = secret;
  msg.msg.msg.l2tpcreate.secret_len = secret_len;
#endif /* PPPOL2TP_AUTH_SUPPORT */
  msg.msg.msg.l2tpcreate.link_status_cb = link_status_cb;
  msg.msg.msg.l2tpcreate.ctx_cb = ctx_cb;
  tcpip_api_call(pppapi_do_pppol2tp_create, &msg.call);
  return msg.msg.ppp;
}
#endif /* PPPOL2TP_SUPPORT */


/**
 * Call ppp_connect() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_connect(struct tcpip_api_call *m)
{
  struct pppapi_msg *msg = (struct pppapi_msg *)m;

  return ppp_connect(msg->msg.ppp, msg->msg.msg.connect.holdoff);
}

/**
 * Call ppp_connect() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
err_t
pppapi_connect(ppp_pcb *pcb, u16_t holdoff)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  msg.msg.msg.connect.holdoff = holdoff;
  return tcpip_api_call(pppapi_do_ppp_connect, &msg.call);
}


#if PPP_SERVER
/**
 * Call ppp_listen() inside the tcpip_thread context.
 */
static void
pppapi_do_ppp_listen(struct pppapi_msg_msg *msg)
{
  msg->err = ppp_listen(msg->ppp, msg->msg.listen.addrs);
  TCPIP_PPPAPI_ACK(msg);
}

/**
 * Call ppp_listen() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
err_t
pppapi_listen(ppp_pcb *pcb, struct ppp_addrs *addrs)
{
  struct pppapi_msg msg;
  msg.function = pppapi_do_ppp_listen;
  msg.msg.ppp = pcb;
  msg.msg.msg.listen.addrs = addrs;
  TCPIP_PPPAPI(&msg);
  return msg.msg.err;
}
#endif /* PPP_SERVER */


/**
 * Call ppp_close() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_close(struct tcpip_api_call *m)
{
  struct pppapi_msg *msg = (struct pppapi_msg *)m;

  return ppp_close(msg->msg.ppp, msg->msg.msg.close.nocarrier);
}

/**
 * Call ppp_close() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
err_t
pppapi_close(ppp_pcb *pcb, u8_t nocarrier)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  msg.msg.msg.close.nocarrier = nocarrier;
  return tcpip_api_call(pppapi_do_ppp_close, &msg.call);
}


/**
 * Call ppp_free() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_free(struct tcpip_api_call *m)
{
  struct pppapi_msg_msg *msg = (struct pppapi_msg_msg *)m;

  return ppp_free(msg->ppp);
}

/**
 * Call ppp_free() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
err_t
pppapi_free(ppp_pcb *pcb)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  return tcpip_api_call(pppapi_do_ppp_free, &msg.call);
}


/**
 * Call ppp_ioctl() inside the tcpip_thread context.
 */
static err_t
pppapi_do_ppp_ioctl(struct tcpip_api_call *m)
{
  struct pppapi_msg *msg = (struct pppapi_msg *)m;

  return ppp_ioctl(msg->msg.ppp, msg->msg.msg.ioctl.cmd, msg->msg.msg.ioctl.arg);
}

/**
 * Call ppp_ioctl() in a thread-safe way by running that function inside the
 * tcpip_thread context.
 */
err_t
pppapi_ioctl(ppp_pcb *pcb, u8_t cmd, void *arg)
{
  struct pppapi_msg msg;
  msg.msg.ppp = pcb;
  msg.msg.msg.ioctl.cmd = cmd;
  msg.msg.msg.ioctl.arg = arg;
  return tcpip_api_call(pppapi_do_ppp_ioctl, &msg.call);
}

#endif /* LWIP_PPP_API */
