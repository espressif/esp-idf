/*
 * Network function implementation with socket for ESP32 platform.
 *
 * Uses libcoap software implementation for failover when concurrent
 * network operations are in use.
 *
 * coap_io.h -- Default network I/O functions for libcoap
 *
 * Copyright (C) 2012,2014 Olaf Bergmann <bergmann@tzi.org>
 *
 * Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#ifdef HAVE_STDIO_H
#  include <stdio.h>
#endif

#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_SYS_UIO_H
# include <sys/uio.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <errno.h>

#ifdef WITH_CONTIKI
# include "uip.h"
#endif

#include "pdu.h"
#include "debug.h"
#include "mem.h"
#include "coap_io.h"

#ifdef WITH_POSIX
/* define generic PKTINFO for IPv4 */
#if defined(IP_PKTINFO)
#  define GEN_IP_PKTINFO IP_PKTINFO
#elif defined(IP_RECVDSTADDR)
#  define GEN_IP_PKTINFO IP_RECVDSTADDR
#else
#  error "Need IP_PKTINFO or IP_RECVDSTADDR to request ancillary data from OS."
#endif /* IP_PKTINFO */

/* define generic KTINFO for IPv6 */
#ifdef IPV6_RECVPKTINFO
#  define GEN_IPV6_PKTINFO IPV6_RECVPKTINFO
#elif defined(IPV6_PKTINFO)
#  define GEN_IPV6_PKTINFO IPV6_PKTINFO
#else
#  error "Need IPV6_PKTINFO or IPV6_RECVPKTINFO to request ancillary data from OS."
#endif /* IPV6_RECVPKTINFO */

struct coap_packet_t {
  coap_if_handle_t hnd;	      /**< the interface handle */
  coap_address_t src;	      /**< the packet's source address */
  coap_address_t dst;	      /**< the packet's destination address */
  const coap_endpoint_t *interface;

  int ifindex;
  void *session;		/**< opaque session data */

  size_t length;		/**< length of payload */
  unsigned char payload[];	/**< payload */
};
#endif

#ifdef CUSTOM_COAP_NETWORK_ENDPOINT

#ifdef WITH_CONTIKI
static int ep_initialized = 0;

static inline struct coap_endpoint_t *
coap_malloc_contiki_endpoint() {
  static struct coap_endpoint_t ep;

  if (ep_initialized) {
    return NULL;
  } else {
    ep_initialized = 1;
    return &ep;
  }
}

static inline void
coap_free_contiki_endpoint(struct coap_endpoint_t *ep) {
  ep_initialized = 0;
}

coap_endpoint_t *
coap_new_endpoint(const coap_address_t *addr, int flags) {
  struct coap_endpoint_t *ep = coap_malloc_contiki_endpoint();

  if (ep) {
    memset(ep, 0, sizeof(struct coap_endpoint_t));
    ep->handle.conn = udp_new(NULL, 0, NULL);

    if (!ep->handle.conn) {
      coap_free_endpoint(ep);
      return NULL;
    }

    coap_address_init(&ep->addr);
    uip_ipaddr_copy(&ep->addr.addr, &addr->addr);
    ep->addr.port = addr->port;
    udp_bind((struct uip_udp_conn *)ep->handle.conn, addr->port);
  }
  return ep;
}

void
coap_free_endpoint(coap_endpoint_t *ep) {
  if (ep) {
    if (ep->handle.conn) {
      uip_udp_remove((struct uip_udp_conn *)ep->handle.conn);
    }
    coap_free_contiki_endpoint(ep);
  }
}

#else /* WITH_CONTIKI */
static inline struct coap_endpoint_t *
coap_malloc_posix_endpoint(void) {
  return (struct coap_endpoint_t *)coap_malloc(sizeof(struct coap_endpoint_t));
}

static inline void
coap_free_posix_endpoint(struct coap_endpoint_t *ep) {
  coap_free(ep);
}

coap_endpoint_t *
coap_new_endpoint(const coap_address_t *addr, int flags) {
  int sockfd = socket(addr->addr.sa.sa_family, SOCK_DGRAM, 0);
  int on = 1;
  struct coap_endpoint_t *ep;

  if (sockfd < 0) {
    coap_log(LOG_WARNING, "coap_new_endpoint: socket");
    return NULL;
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    coap_log(LOG_WARNING, "coap_new_endpoint: setsockopt SO_REUSEADDR");

  if (bind(sockfd, &addr->addr.sa, addr->size) < 0) {
    coap_log(LOG_WARNING, "coap_new_endpoint: bind");
    close (sockfd);
    return NULL;
  }

  ep = coap_malloc_posix_endpoint();
  if (!ep) {
    coap_log(LOG_WARNING, "coap_new_endpoint: malloc");
    close(sockfd);
    return NULL;
  }

  memset(ep, 0, sizeof(struct coap_endpoint_t));
  ep->handle.fd = sockfd;
  ep->flags = flags;

  ep->addr.size = addr->size;
  if (getsockname(sockfd, &ep->addr.addr.sa, &ep->addr.size) < 0) {
    coap_log(LOG_WARNING, "coap_new_endpoint: cannot determine local address");
    close (sockfd);
    return NULL;
  }

#ifndef NDEBUG
  if (LOG_DEBUG <= coap_get_log_level()) {
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif
    unsigned char addr_str[INET6_ADDRSTRLEN+8];

    if (coap_print_addr(&ep->addr, addr_str, INET6_ADDRSTRLEN+8)) {
      debug("created %sendpoint %s\n",
	    ep->flags & COAP_ENDPOINT_DTLS ? "DTLS " : "",
	    addr_str);
    }
  }
#endif /* NDEBUG */

  return (coap_endpoint_t *)ep;
}

void
coap_free_endpoint(coap_endpoint_t *ep) {
  if(ep) {
    if (ep->handle.fd >= 0)
      close(ep->handle.fd);
    coap_free_posix_endpoint((struct coap_endpoint_t *)ep);
  }
}

#endif /* WITH_CONTIKI */
#endif /* CUSTOM_COAP_NETWORK_ENDPOINT */

#ifdef CUSTOM_COAP_NETWORK_SEND

#if defined(WITH_POSIX) != defined(HAVE_NETINET_IN_H)
/* define struct in6_pktinfo and struct in_pktinfo if not available
   FIXME: check with configure
*/
struct in6_pktinfo {
  struct in6_addr ipi6_addr;	/* src/dst IPv6 address */
  unsigned int ipi6_ifindex;	/* send/recv interface index */
};

struct in_pktinfo {
  int ipi_ifindex;
  struct in_addr ipi_spec_dst;
  struct in_addr ipi_addr;
};
#endif

#if defined(WITH_POSIX) && !defined(SOL_IP)
/* Solaris expects level IPPROTO_IP for ancillary data. */
#define SOL_IP IPPROTO_IP
#endif

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

ssize_t
coap_network_send(struct coap_context_t *context UNUSED_PARAM,
		  const coap_endpoint_t *local_interface,
		  const coap_address_t *dst,
		  unsigned char *data,
		  size_t datalen) {

  struct coap_endpoint_t *ep =
    (struct coap_endpoint_t *)local_interface;

#ifndef WITH_CONTIKI
  return sendto(ep->handle.fd, data, datalen, 0, (struct sockaddr*)&dst->addr.sa, sizeof(struct sockaddr));
#else /* WITH_CONTIKI */
  /* FIXME: untested */
  /* FIXME: is there a way to check if send was successful? */
  uip_udp_packet_sendto((struct uip_udp_conn *)ep->handle.conn, data, datalen,
			&dst->addr, dst->port);
  return datalen;
#endif /* WITH_CONTIKI */
}

#endif /* CUSTOM_COAP_NETWORK_SEND */

#ifdef CUSTOM_COAP_NETWORK_READ

#define SIN6(A) ((struct sockaddr_in6 *)(A))

#ifdef WITH_POSIX
static coap_packet_t *
coap_malloc_packet(void) {
  coap_packet_t *packet;
  const size_t need = sizeof(coap_packet_t) + COAP_MAX_PDU_SIZE;

  packet = (coap_packet_t *)coap_malloc(need);
  if (packet) {
    memset(packet, 0, need);
  }
  return packet;
}

void
coap_free_packet(coap_packet_t *packet) {
  coap_free(packet);
}
#endif /* WITH_POSIX */
#ifdef WITH_CONTIKI
static inline coap_packet_t *
coap_malloc_packet(void) {
  return (coap_packet_t *)coap_malloc_type(COAP_PACKET, 0);
}

void
coap_free_packet(coap_packet_t *packet) {
  coap_free_type(COAP_PACKET, packet);
}
#endif /* WITH_CONTIKI */

static inline size_t
coap_get_max_packetlength(const coap_packet_t *packet UNUSED_PARAM) {
  return COAP_MAX_PDU_SIZE;
}

void
coap_packet_populate_endpoint(coap_packet_t *packet, coap_endpoint_t *target)
{
  target->handle = packet->interface->handle;
  memcpy(&target->addr, &packet->dst, sizeof(target->addr));
  target->ifindex = packet->ifindex;
  target->flags = 0; /* FIXME */
}
void
coap_packet_copy_source(coap_packet_t *packet, coap_address_t *target)
{
  memcpy(target, &packet->src, sizeof(coap_address_t));
}
void
coap_packet_get_memmapped(coap_packet_t *packet, unsigned char **address, size_t *length)
{
	*address = packet->payload;
	*length = packet->length;
}

/**
 * Checks if a message with destination address @p dst matches the
 * local interface with address @p local. This function returns @c 1
 * if @p dst is a valid match, and @c 0 otherwise.
 */
static inline int
is_local_if(const coap_address_t *local, const coap_address_t *dst) {
  return coap_address_isany(local) || coap_address_equals(dst, local) ||
    coap_is_mcast(dst);
}

ssize_t
coap_network_read(coap_endpoint_t *ep, coap_packet_t **packet) {
  ssize_t len = -1;

#ifdef WITH_POSIX
  #define SOC_APPDATA_LEN 1460
  char *soc_appdata = NULL;
  struct sockaddr_in soc_srcipaddr;
  socklen_t soc_srcsize = sizeof(struct sockaddr_in);
#endif /* WITH_POSIX */

  assert(ep);
  assert(packet);

  *packet = coap_malloc_packet();

  if (!*packet) {
    warn("coap_network_read: insufficient memory, drop packet\n");
    return -1;
  }

  coap_address_init(&(*packet)->dst); /* the local interface address */
  coap_address_init(&(*packet)->src); /* the remote peer */

#ifdef WITH_POSIX
  soc_appdata = coap_malloc(SOC_APPDATA_LEN);
  if (soc_appdata){
	  len = recvfrom(ep->handle.fd, soc_appdata, SOC_APPDATA_LEN, 0, (struct sockaddr *)&soc_srcipaddr, (socklen_t *)&soc_srcsize);

	  if (len < 0){
		  coap_log(LOG_WARNING, "coap_network_read: %s\n", strerror(errno));
		  goto error;
	  } else {
		  /* use getsockname() to get the local port */
	      (*packet)->dst.size = sizeof((*packet)->dst.addr);
	      if (getsockname(ep->handle.fd, &(*packet)->dst.addr.sa, &(*packet)->dst.size) < 0) {
	         coap_log(LOG_DEBUG, "cannot determine local port\n");
	         goto error;
	      }

	      /* local interface for IPv4 */
	      (*packet)->src.size = sizeof((*packet)->src.addr);
	      memcpy(&(*packet)->src.addr.sa, &soc_srcipaddr, (*packet)->src.size);

	      if (len > coap_get_max_packetlength(*packet)) {
	         /* FIXME: we might want to send back a response */
	         warn("discarded oversized packet\n");
	         goto error;
	      }

	      if (!is_local_if(&ep->addr, &(*packet)->dst)) {
	      	 coap_log(LOG_DEBUG, "packet received on wrong interface, dropped\n");
	      	printf("error 3\n");
	      	 goto error;
	      }

	      (*packet)->length = len;

	      memcpy(&(*packet)->payload, soc_appdata, len);
	  }

	  coap_free(soc_appdata);
	  soc_appdata = NULL;
  } else {
	  goto error;
  }
#endif /* WITH_POSIX */
#ifdef WITH_CONTIKI
  /* FIXME: untested, make this work */
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

  if(uip_newdata()) {
    uip_ipaddr_copy(&(*packet)->src.addr, &UIP_IP_BUF->srcipaddr);
    (*packet)->src.port = UIP_UDP_BUF->srcport;
    uip_ipaddr_copy(&(*packet)->dst.addr, &UIP_IP_BUF->destipaddr);
    (*packet)->dst.port = UIP_UDP_BUF->destport;

    if (!is_local_if(&ep->addr, &(*packet)->dst)) {
      coap_log(LOG_DEBUG, "packet received on wrong interface, dropped\n");
      goto error;
    }

    len = uip_datalen();

    if (len > coap_get_max_packetlength(*packet)) {
      /* FIXME: we might want to send back a response */
      warn("discarded oversized packet\n");
      return -1;
    }

    ((char *)uip_appdata)[len] = 0;
#ifndef NDEBUG
    if (LOG_DEBUG <= coap_get_log_level()) {
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif
      unsigned char addr_str[INET6_ADDRSTRLEN+8];

      if (coap_print_addr(&(*packet)->src, addr_str, INET6_ADDRSTRLEN+8)) {
	debug("received %zd bytes from %s\n", len, addr_str);
      }
    }
#endif /* NDEBUG */

    (*packet)->length = len;
    memcpy(&(*packet)->payload, uip_appdata, len);
  }

#undef UIP_IP_BUF
#undef UIP_UDP_BUF
#endif /* WITH_CONTIKI */
#ifdef WITH_LWIP
#error "coap_network_read() not implemented on this platform"
#endif

  (*packet)->interface = ep;

  return len;
 error:
#ifdef WITH_POSIX
 	if (soc_appdata)
 		coap_free(soc_appdata);
 	soc_appdata = NULL;
#endif
  coap_free_packet(*packet);
  *packet = NULL;
  return -1;
}

#undef SIN6

#endif /*  CUSTOM_COAP_NETWORK_READ */
