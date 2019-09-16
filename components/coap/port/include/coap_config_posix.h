/*
 * libcoap configure implementation for ESP32 platform.
 *
 * Uses libcoap software implementation for failover when concurrent
 * configure operations are in use.
 *
 * coap.h -- main header file for CoAP stack of libcoap
 *
 * Copyright (C) 2010-2012,2015-2016 Olaf Bergmann <bergmann@tzi.org>
 *               2015 Carsten Schoenert <c.schoenert@t-online.de>
 *
 * Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef COAP_CONFIG_POSIX_H_
#define COAP_CONFIG_POSIX_H_

#ifdef WITH_POSIX

#include <sys/socket.h>
#include <net/if.h>

#define HAVE_SYS_SOCKET_H
#define HAVE_MALLOC
#define HAVE_ARPA_INET_H
#define HAVE_TIME_H
#define HAVE_NETDB_H
#define HAVE_NETINET_IN_H
#define HAVE_STRUCT_CMSGHDR

#define ipi_spec_dst ipi_addr
struct in6_pktinfo {
  struct in6_addr ipi6_addr;        /* src/dst IPv6 address */
  unsigned int ipi6_ifindex;        /* send/recv interface index */
};
#define IN6_IS_ADDR_V4MAPPED(a) \
        ((((__const uint32_t *) (a))[0] == 0)                                 \
         && (((__const uint32_t *) (a))[1] == 0)                              \
         && (((__const uint32_t *) (a))[2] == htonl (0xffff)))

/* As not defined, just need to define is as something innocuous */
#define IPV6_PKTINFO IPV6_CHECKSUM

#define PACKAGE_NAME "libcoap-posix"
#define PACKAGE_VERSION "?"

#ifdef CONFIG_MBEDTLS_TLS_ENABLED
#define HAVE_MBEDTLS
#endif /* CONFIG_MBEDTLS_TLS_ENABLED */
#define COAP_CONSTRAINED_STACK 1
#define ESPIDF_VERSION

#define _POSIX_TIMERS 1

#define gai_strerror(x) "gai_strerror() not supported"

#endif /* WITH_POSIX */
#endif /* COAP_CONFIG_POSIX_H_ */
