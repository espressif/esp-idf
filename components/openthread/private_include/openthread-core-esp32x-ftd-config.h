/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_OPENTHREAD_HEADER_CUSTOM
#include CONFIG_OPENTHREAD_CUSTOM_HEADER_FILE_NAME
#endif

#define OPENTHREAD_FTD 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#ifdef OPENTHREAD_CONFIG_PLATFORM_INFO
#error `OPENTHREAD_CONFIG_PLATFORM_INFO` is redefined.
#endif
#define OPENTHREAD_CONFIG_PLATFORM_INFO CONFIG_OPENTHREAD_PLATFORM_INFO

/**
 * @def PACKAGE_NAME
 *
 * Define to the full name of this package.
 *
 */
#ifdef PACKAGE_NAME
#error `PACKAGE_NAME` is redefined.
#endif
#define PACKAGE_NAME CONFIG_OPENTHREAD_PACKAGE_NAME

/**
 * @def PACKAGE_STRING
 *
 * Define to the full name and version of this package.
 *
 */
#ifdef PACKAGE_STRING
#error `PACKAGE_STRING` is redefined.
#endif
#define PACKAGE_STRING (PACKAGE_NAME " - " PACKAGE_VERSION)

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
 *
 * Configuration option to enable dynamic log level control.
 *
 */
#ifdef OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
#error `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is redefined.
#endif
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * The log level (used at compile time). If `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is set, this defines the most
 * verbose log level possible. See `OPENTHREAD_CONFIG_LOG_LEVEL_INIT` to set the initial log level.
 *
 */
#ifdef OPENTHREAD_CONFIG_LOG_LEVEL
#error `OPENTHREAD_CONFIG_LOG_LEVEL` is redefined.
#endif
#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_DEBG
#else
#define OPENTHREAD_CONFIG_LOG_LEVEL CONFIG_OPENTHREAD_LOG_LEVEL
#endif

/**
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#ifdef OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
#error `OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS` is redefined.
#endif
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS CONFIG_OPENTHREAD_NUM_MESSAGE_BUFFERS

/**
 * @def OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
 *
 * Set to 1 to enable support for Thread Radio Encapsulation Link (TREL).
 *
 */
#ifdef OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#error `OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_RADIO_TREL
#define OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE 1
#else
#define OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE 0
#endif // CONFIG_OPENTHREAD_RADIO_TREL

/**
 * @def OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
 *
 * Set to 1 to enable support for IEEE802.15.4 radio link.
 *
 */
#ifdef OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
#error `OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_RADIO_154_NONE
#define OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE 0
#else
#define OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE 1
#endif

/**
 * @def OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE
 *
 * Specifies the rx frame buffer size used by `SpinelInterface` in RCP host code. This is applicable/used when
 * `RadioSpinel` platform is used.
 *
 */
#ifdef OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE
#error `OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE` is redefined.
#endif
#define OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE CONFIG_OPENTHREAD_SPINEL_RX_FRAME_BUFFER_SIZE

/**
 * @def OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
 *
 * The maximum number of children.
 *
 */
#ifdef OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
#error `OPENTHREAD_CONFIG_MLE_MAX_CHILDREN` is redefined.
#endif
#define OPENTHREAD_CONFIG_MLE_MAX_CHILDREN CONFIG_OPENTHREAD_MLE_MAX_CHILDREN

/**
 * @def OPENTHREAD_CONFIG_TMF_ADDRESS_CACHE_ENTRIES
 *
 * The number of EID-to-RLOC cache entries.
 *
 */
#ifdef OPENTHREAD_CONFIG_TMF_ADDRESS_CACHE_ENTRIES
#error `OPENTHREAD_CONFIG_TMF_ADDRESS_CACHE_ENTRIES` is redefined.
#endif
#define OPENTHREAD_CONFIG_TMF_ADDRESS_CACHE_ENTRIES CONFIG_OPENTHREAD_TMF_ADDR_CACHE_ENTRIES

#ifdef OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
#error `OPENTHREAD_CONFIG_COMMISSIONER_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_COMMISSIONER

/**
 * @def OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
 *
 * Define to 1 to enable Commissioner support.
 *
 */
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_COMMISSIONER_MAX_JOINER_ENTRIES
 *
 * The maximum number of Joiner entries maintained by the Commissioner
 *
 */
#ifdef OPENTHREAD_CONFIG_COMMISSIONER_MAX_JOINER_ENTRIES
#error `OPENTHREAD_CONFIG_COMMISSIONER_MAX_JOINER_ENTRIES` is redefined.
#endif
#define OPENTHREAD_CONFIG_COMMISSIONER_MAX_JOINER_ENTRIES CONFIG_OPENTHREAD_COMM_MAX_JOINER_ENTRIES

#else // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 0
#endif // CONFIG_OPENTHREAD_COMMISSIONER

#ifdef OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
#error `OPENTHREAD_CONFIG_MAC_FILTER_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_MACFILTER_ENABLE
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 0
#endif

#ifdef OPENTHREAD_CONFIG_JOINER_ENABLE
#error `OPENTHREAD_CONFIG_JOINER_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_JOINER
#define OPENTHREAD_CONFIG_JOINER_ENABLE 1
#else
#define OPENTHREAD_CONFIG_JOINER_ENABLE 0
#endif

#ifdef OPENTHREAD_CONFIG_DIAG_ENABLE
#error `OPENTHREAD_CONFIG_DIAG_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_DIAG
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#else
#define OPENTHREAD_CONFIG_DIAG_ENABLE 0
#endif

#ifdef OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#error `OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_SRP_CLIENT

#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES
 *
 * Specifies number of service entries in the SRP client service pool.
 *
 * This config is applicable only when `OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE` is enabled.
 *
 */
#ifdef OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES
#error `OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES` is redefined.
#endif
#define OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES CONFIG_OPENTHREAD_SRP_CLIENT_MAX_SERVICES

#else

#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE 0
#endif // CONFIG_OPENTHREAD_SRP_CLIENT

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
 *
 * Define to 1 to enable DNS Client support.
 *
 */
#ifdef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#error `OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE` is redefined.
#endif
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE CONFIG_OPENTHREAD_DNS_CLIENT

#ifdef OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#error `OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_CSL_ENABLE

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
 *
 * Define as 1 to support Thread 1.2 CSL feature.
 *
 */
#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
 *
 * Define as 1 to enable support Thread 1.2 CSL debug.
 *
 */
#ifdef OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
#error `OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE` is redefined.
#endif
#define OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE CONFIG_OPENTHREAD_CSL_DEBUG_ENABLE

#else

#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE 0
#endif // CONFIG_OPENTHREAD_CSL_ENABLE

/**
 *
 * Define as 1 to enable support for allocating message pool buffer in PSRAM
 *
 */
#ifdef OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
#error `OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT` is redefined.
#endif
#if CONFIG_OPENTHREAD_PLATFORM_MSGPOOL_MANAGEMENT
/**
 * @def OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
 *
 * The message pool is managed by platform defined logic when this flag is set.
 * This feature would typically be used when operating in a multi-threaded system
 * and multiple threads need to access the message pool.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT 1
#else
#define OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT 0
#endif

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
 *
 * Define as 1 to enable the time synchronization service feature.
 *
 */
#ifdef OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#error `OPENTHREAD_CONFIG_TIME_SYNC_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_TIME_SYNC
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 1
#else
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_RADIO_STATS_ENABLE
 *
 * Set to 1 to enable support for Radio Statistics. Note that this option only works for OPENTHREAD_FTD and
 * OPENTHREAD_MTD.
 *
 */
#ifdef OPENTHREAD_CONFIG_RADIO_STATS_ENABLE
#error `OPENTHREAD_CONFIG_RADIO_STATS_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_RADIO_STATS_ENABLE
#define OPENTHREAD_CONFIG_RADIO_STATS_ENABLE 1
#else
#define OPENTHREAD_CONFIG_RADIO_STATS_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_TIMEOUT
 *
 * The timeout value (in seconds) waiting for a address notification response after sending an address query.
 *
 */
#ifdef OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_TIMEOUT
#error `OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_TIMEOUT` is redefined.
#endif
#define OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_TIMEOUT CONFIG_OPENTHREAD_ADDRESS_QUERY_TIMEOUT

/**
 * @def OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_INITIAL_RETRY_DELAY
 *
 * Initial retry delay for address query (in seconds).
 *
 */
#ifdef OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_INITIAL_RETRY_DELAY
#error `OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_INITIAL_RETRY_DELAY` is redefined.
#endif
#define OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_INITIAL_RETRY_DELAY CONFIG_OPENTHREAD_ADDRESS_QUERY_RETRY_DELAY

/**
 * @def OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_MAX_RETRY_DELAY
 *
 * Maximum retry delay for address query (in seconds).
 *
 */
#ifdef OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_MAX_RETRY_DELAY
#error `OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_MAX_RETRY_DELAY` is redefined.
#endif
#define OPENTHREAD_CONFIG_TMF_ADDRESS_QUERY_MAX_RETRY_DELAY CONFIG_OPENTHREAD_ADDRESS_QUERY_MAX_RETRY_DELAY

/**
 * @def OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT
 *
 * The maximum number of backoffs the CSMA-CA algorithm will attempt before declaring a channel access failure.
 *
 * Equivalent to macMaxCSMABackoffs in IEEE 802.15.4-2006, default value is 4.
 *
 */
#ifdef OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT
#error `OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT` is redefined.
#endif
#define OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT CONFIG_OPENTHREAD_MAC_MAX_CSMA_BACKOFFS_DIRECT

/*----The following options set fixed default values but can be overridden by the user header file.----*/

#if CONFIG_OPENTHREAD_BORDER_ROUTER
/**
 * @def OPENTHREAD_CONFIG_BORDER_AGENT_ID_ENABLE
 *
 * Define to 1 to enable Border Agent ID support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BORDER_AGENT_ID_ENABLE
#define OPENTHREAD_CONFIG_BORDER_AGENT_ID_ENABLE 1
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
 * @def OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
 *
 * Define to 1 to enable Backbone Router support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE 1
#endif
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER

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
 * @def OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
 *
 * Define to 1 to enable Border Agent support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE 1
#endif

#if !CONFIG_OPENTHREAD_RADIO_NATIVE
/**
 * @def OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE
 *
 * Enables compatibility error callback in Spinel
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE
#define OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE 1
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
 *
 * Defines the max count of RCP failures allowed to be recovered.
 * 0 means to disable RCP failure recovering.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 3
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_RCP_TIME_SYNC_INTERVAL
 *
 * This setting configures the interval (in units of microseconds) for host-rcp
 * time sync. The host will recalculate the time offset between host and RCP
 * every interval.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_RCP_TIME_SYNC_INTERVAL
#define OPENTHREAD_POSIX_CONFIG_RCP_TIME_SYNC_INTERVAL (60 * 1000 * 1000)
#endif
#endif // !CONFIG_OPENTHREAD_RADIO_NATIVE

#if CONFIG_OPENTHREAD_LINK_METRICS
/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
 *
 * Define as 1 to support Thread 1.2 Link Metrics Subject feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
 *
 * Define as 1 to support Thread 1.2 Link Metrics feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE 1
#endif
#endif //CONFIG_OPENTHREAD_LINK_METRICS

/**
 * @def OPENTHREAD_CONFIG_ECDSA_ENABLE
 *
 * Define to 1 to enable ECDSA support.
 *
 */
#if CONFIG_OPENTHREAD_BORDER_ROUTER || CONFIG_OPENTHREAD_SRP_CLIENT
#ifndef OPENTHREAD_CONFIG_ECDSA_ENABLE
#define OPENTHREAD_CONFIG_ECDSA_ENABLE 1
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_DUA_ENABLE
 *
 * Define as 1 to support Thread 1.2 Domain Unicast Address feature.
 */
#ifndef OPENTHREAD_CONFIG_DUA_ENABLE
#define OPENTHREAD_CONFIG_DUA_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * The ESP-IDF platform provides an otPlatLog() function.
 */
#ifndef OPENTHREAD_CONFIG_LOG_OUTPUT
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
 *
 * The assert is managed by platform defined logic when this flag is set.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_API_ENABLE
 *
 * Define to 1 to enable the CoAP API.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_API_ENABLE 1
#endif

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

/**
 * @def OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
 *
 * Define to 1 to enable Thread Test Harness reference device support.
 *
 */
#ifndef OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE 1
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
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable NCP HDLC support.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
 *
 * Define to 1 to support injecting Service entries into the Thread Network Data.
 *
 */
#ifndef OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
 *
 * Define as 1 to enable bultin-mbedtls.
 *
 * Note that the OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS determines whether to use bultin-mbedtls as well as
 * whether to manage mbedTLS internally, such as memory allocation and debug.
 *
 */
#ifndef OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS 0
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
 *
 * Define as 1 to enable support for adding of auto-configured SLAAC addresses by OpenThread.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
 *
 * The maximum number of state-changed callback handlers (set using `otSetStateChangedCallback()`).
 *
 */
#ifndef OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
#define OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS 3
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define as 1 to enable microsecond timer.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_ENABLE
 *
 * Define to 1 to enable ping sender module.
 *
 * Ping sender module implements sending ICMPv6 Echo Request messages and processing ICMPv6 Echo Reply messages.
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_ENABLE
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE 1
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
 * @def OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US
 *
 * Define as 1 to set the ahead time for CSL transmit timing.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US
#define OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US 20000
#endif

/**
 * @def OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT
 *
 * Define as 1 to enable support for locally initializing an Active Operational Dataset.
 *
 */
#ifndef OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT
#define OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT 1
#endif

/**
 * @OPENTHREAD_CONFIG_DELAY_AWARE_QUEUE_MANAGEMENT_MARK_ECN_INTERVAL
 *
 * Specifies the time-in-queue threshold interval in milliseconds to mark ECN on a message if it is ECN-capable or
 * drop the message if not ECN-capable.
 */
#ifndef OPENTHREAD_CONFIG_DELAY_AWARE_QUEUE_MANAGEMENT_MARK_ECN_INTERVAL
#define OPENTHREAD_CONFIG_DELAY_AWARE_QUEUE_MANAGEMENT_MARK_ECN_INTERVAL 1000
#endif

#ifndef OPENTHREAD_CONFIG_THREAD_VERSION
#define OPENTHREAD_CONFIG_THREAD_VERSION OT_THREAD_VERSION_1_4
#endif
