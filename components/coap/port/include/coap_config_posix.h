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

#define HAVE_SYS_SOCKET_H
#define HAVE_MALLOC
#define HAVE_ARPA_INET_H

#define IP_PKTINFO   IP_MULTICAST_IF
#define IPV6_PKTINFO IPV6_V6ONLY

#define PACKAGE_NAME "libcoap-posix"
#define PACKAGE_VERSION "?"

#define CUSTOM_COAP_NETWORK_ENDPOINT
#define CUSTOM_COAP_NETWORK_SEND
#define CUSTOM_COAP_NETWORK_READ

#endif

#endif /* COAP_CONFIG_POSIX_H_ */
