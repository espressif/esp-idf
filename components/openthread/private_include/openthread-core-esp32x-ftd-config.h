/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO CONFIG_IDF_TARGET

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
 *
 * The assert is managed by platform defined logic when this flag is set.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * Define to 1 to enable otPlatFlash* APIs to support non-volatile storage.
 *
 * When defined to 1, the platform MUST implement the otPlatFlash* APIs instead of the otPlatSettings* APIs.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * The ESP-IDF platform provides an otPlatLog() function.
 */
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
 *
 * Configuration option to enable dynamic log level control.
 *
 */
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * The log level (used at compile time). If `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is set, this defines the most
 * verbose log level possible. See `OPENTHREAD_CONFIG_LOG_LEVEL_INIT` to set the initial log level.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_DEBG
#else
#define OPENTHREAD_CONFIG_LOG_LEVEL CONFIG_OPENTHREAD_LOG_LEVEL
#endif

#define OPENTHREAD_CONFIG_LOG_CLI 1
#define OPENTHREAD_CONFIG_LOG_PKT_DUMP 1
#define OPENTHREAD_CONFIG_LOG_PLATFORM 1

/**
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS CONFIG_OPENTHREAD_NUM_MESSAGE_BUFFERS

/**
 * @def OPENTHREAD_CONFIG_COAP_API_ENABLE
 *
 * Define to 1 to enable the CoAP API.
 *
 */
#define OPENTHREAD_CONFIG_COAP_API_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE
 *
 * Define to 1 to enable platform NETIF support.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
 *
 * Enable the external heap.
 *
 */
#ifndef OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_TCP_ENABLE
 *
 * Define to 0 to disable TCP
 *
 */
#ifndef OPENTHREAD_CONFIG_TCP_ENABLE
#define OPENTHREAD_CONFIG_TCP_ENABLE 0
#endif

#if CONFIG_OPENTHREAD_BORDER_ROUTER

/**
 * @def OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
 *
 * Define to 1 to enable Border Agent support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
 *
 * Define to 1 to enable Border Router support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
 *
 * Define to 1 to enable Border Routing support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
#define OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
 *
 * Define to 1 to enable platform UDP support.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_NAT64_BORDER_ROUTING_ENABLE
 *
 * Define to 1 to enable Border Routing NAT64 support.
 *
 */
#ifndef OPENTHREAD_CONFIG_NAT64_BORDER_ROUTING_ENABLE
#define OPENTHREAD_CONFIG_NAT64_BORDER_ROUTING_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_ECDSA_ENABLE
 *
 * Define to 1 to enable ECDSA support.
 *
 */
#ifndef OPENTHREAD_CONFIG_ECDSA_ENABLE
#define OPENTHREAD_CONFIG_ECDSA_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
 *
 * Define to 1 to enable SRP Server support.
 *
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
 *
 * Define to 1 to enable DNS-SD Server support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
#define OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
 *
 * Set to 1 to enable support for Thread Radio Encapsulation Link (TREL).
 *
 */
#ifndef OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#define OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
 *
 * Define to 1 to enable Backbone Router support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
 *
 * Defines the max count of RCP failures allowed to be recovered.
 * 0 means to disable RCP failure recovering.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 1
#endif

#endif // CONFIG_OPENTHREAD_BORDER_ROUTER

/**
 * @def OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
 *
 * Define to 1 to enable Thread Test Harness reference device support.
 *
 */
#define OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
 *
 * Define to 1 to enable Child Supervision support.
 *
 */
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
 *
 * Define to 1 to enable DHCPv6 Client support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
 *
 * Define to 1 to enable DHCPv6 Server support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
#define OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
 *
 * Define to 1 to enable DNS Client support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_SPI_ENABLE
 *
 * Define to 1 to enable NCP SPI support.
 *
 */
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE 0

/**
 * @def OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
 *
 * Define to 1 to enable NCP Spinel Encrypter.
 *
 */
#define OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER 0

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable NCP HDLC support.
 *
 */
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
 *
 * Define to 1 to support injecting Service entries into the Thread Network Data.
 *
 */
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1

/**
 * @def PACKAGE_NAME
 *
 * Define to the full name of this package.
 *
 */
#define PACKAGE_NAME "openthread-esp32"

/**
 * @def PACKAGE_STRING
 *
 * Define to the full name and version of this package.
 *
 */
#define PACKAGE_STRING (PACKAGE_NAME " - " PACKAGE_VERSION)

/**
 * @def OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
 *
 * Define as 1 to enable bultin-mbedtls.
 *
 * Note that the OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS determines whether to use bultin-mbedtls as well as
 * whether to manage mbedTLS internally, such as memory allocation and debug.
 *
 */
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS 0

/**
 * @def OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
 *
 * Define as 1 to enable support for adding of auto-configured SLAAC addresses by OpenThread.
 *
 */
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
 *
 * The maximum number of state-changed callback handlers (set using `otSetStateChangedCallback()`).
 *
 */
#define OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS 3

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE
 *
 * Specifies the rx frame buffer size used by `SpinelInterface` in RCP host code. This is applicable/used when
 * `RadioSpinel` platform is used.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE 1024

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define as 1 to enable microsecond timer.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_ENABLE
 *
 * Define to 1 to enable ping sender module.
 *
 * Ping sender module implements sending ICMPv6 Echo Request messages and processing ICMPv6 Echo Reply messages.
 *
 */
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_DUA_ENABLE
 *
 * Define as 1 to support Thread 1.2 Domain Unicast Address feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_DUA_ENABLE
#define OPENTHREAD_CONFIG_DUA_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLR_ENABLE
 *
 * Define as 1 to support Thread 1.2 Multicast Listener Registration feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLR_ENABLE
#define OPENTHREAD_CONFIG_MLR_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DTLS_MAX_CONTENT_LEN
 *
 * The max length of the OpenThread dtls content buffer.
 *
 */
#ifndef OPENTHREAD_CONFIG_DTLS_MAX_CONTENT_LEN
#define OPENTHREAD_CONFIG_DTLS_MAX_CONTENT_LEN 768
#endif

/**
 * The configurable definitions via Kconfig
 */
#if CONFIG_OPENTHREAD_COMMISSIONER
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 1
#endif

#if CONFIG_OPENTHREAD_JOINER
#define OPENTHREAD_CONFIG_JOINER_ENABLE 1
#endif

#if CONFIG_OPENTHREAD_DIAG
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#endif

#if CONFIG_OPENTHREAD_SRP_CLIENT
#ifndef OPENTHREAD_CONFIG_ECDSA_ENABLE
#define OPENTHREAD_CONFIG_ECDSA_ENABLE 1
#endif

#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE 1
#endif
#endif

#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 1

#define OPENTHREAD_FTD 1
