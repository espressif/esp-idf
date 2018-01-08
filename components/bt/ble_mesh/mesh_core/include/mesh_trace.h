/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "sdkconfig.h"

/* Define common tracing for all */
#ifndef LOG_LEVEL_ERROR
#define LOG_LEVEL_ERROR     1
#endif /* LOG_LEVEL_ERROR */

#ifndef LOG_LEVEL_WARN
#define LOG_LEVEL_WARN      2
#endif /* LOG_LEVEL_WARN */

#ifndef LOG_LEVEL_INFO
#define LOG_LEVEL_INFO      3
#endif /* LOG_LEVEL_INFO */

#ifndef LOG_LEVEL_DEBUG
#define LOG_LEVEL_DEBUG     4
#endif /* LOG_LEVEL_DEBUG */

#ifndef LOG_LEVEL_VERBOSE
#define LOG_LEVEL_VERBOSE   5
#endif /*LOG_LEVEL_VERBOSE */

#ifdef  CONFIG_BT_MESH_STACK_INITIAL_TRACE_LEVEL
#define MESH_LOG_LEVEL             CONFIG_BT_MESH_STACK_INITIAL_TRACE_LEVEL
#else
#define MESH_LOG_LEVEL             LOG_LEVEL_WARN
#endif

#ifdef  CONFIG_BT_MESH_NET_BUF_TRACE_LEVEL
#define NET_BUF_LOG_LEVEL          CONFIG_BT_MESH_NET_BUF_TRACE_LEVEL
#else
#define NET_BUF_LOG_LEVEL          LOG_LEVEL_WARN
#endif


#define MESH_TRACE_TAG      "BLE_MESH"

#if (LOG_LOCAL_LEVEL >= 4)
#define BT_MESH_LOG_LOCAL_LEVEL_MAPPING (LOG_LOCAL_LEVEL + 1)
#else
#define BT_MESH_LOG_LOCAL_LEVEL_MAPPING LOG_LOCAL_LEVEL
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif /* MAX(a, b) */
#define BT_MESH_LOG_LEVEL_CHECK(LAYER, LEVEL) (MAX(LAYER##_LOG_LEVEL, BT_MESH_LOG_LOCAL_LEVEL_MAPPING) >= LOG_LEVEL_##LEVEL)

#define BT_MESH_PRINT_E(tag, format, ...)   {esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_MESH_PRINT_W(tag, format, ...)   {esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_MESH_PRINT_I(tag, format, ...)   {esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_MESH_PRINT_D(tag, format, ...)   {esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_MESH_PRINT_V(tag, format, ...)   {esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#ifndef __ASSERT_NO_MSG
#define __ASSERT_NO_MSG(x)   do { if (!(x)) BT_MESH_PRINT_E(MESH_TRACE_TAG, "bt mesh error %s %u\n", __FILE__, __LINE__); } while (0)
#endif

#if !CONFIG_BT_MESH_NO_LOG
#define BT_ERR(fmt, args...)             do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) && BT_MESH_LOG_LEVEL_CHECK(MESH, ERROR)) BT_MESH_PRINT_E(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_WARN(fmt, args...)            do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_WARN) && BT_MESH_LOG_LEVEL_CHECK(MESH, WARN)) BT_MESH_PRINT_W(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_INFO(fmt, args...)            do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_INFO) && BT_MESH_LOG_LEVEL_CHECK(MESH, INFO)) BT_MESH_PRINT_I(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_DBG(fmt, args...)             do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_DEBUG) && BT_MESH_LOG_LEVEL_CHECK(MESH, DEBUG)) BT_MESH_PRINT_D(MESH_TRACE_TAG, fmt, ## args);} while(0)
#else /* #if !CONFIG_BT_STACK_NO_LOG */
#define BT_ERR(fmt, args...)
#define BT_WARN(fmt, args...)
#define BT_INFO(fmt, args...)
#define BT_DBG(fmt, args...)
#endif /* #if !CONFIG_BT_STACK_NO_LOG */

#if defined(CONFIG_NET_BUF_LOG) && (!CONFIG_BT_MESH_NO_LOG)
#define NET_BUF_DBG(fmt, ...)            do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) && BT_MESH_LOG_LEVEL_CHECK(NET_BUF, DEBUG)) BT_MESH_PRINT_D(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_ERR(fmt, ...)            do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) && BT_MESH_LOG_LEVEL_CHECK(NET_BUF, ERROR)) BT_MESH_PRINT_E(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_WARN(fmt, ...)           do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) && BT_MESH_LOG_LEVEL_CHECK(NET_BUF, WARN)) BT_MESH_PRINT_W(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_INFO(fmt, ...)           do {if ((MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) && BT_MESH_LOG_LEVEL_CHECK(NET_BUF, INFO)) BT_MESH_PRINT_I(MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_ASSERT(cond) do { if (!(cond)) {              \
            NET_BUF_ERR("assert: '" #cond "' failed");        \
        } } while (0)
#else
#define NET_BUF_DBG(fmt, ...)
#define NET_BUF_ERR(fmt, ...)
#define NET_BUF_WARN(fmt, ...)
#define NET_BUF_INFO(fmt, ...)
#define NET_BUF_ASSERT(cond)
#endif /* CONFIG_NET_BUF_LOG */

#if defined(CONFIG_NET_BUF_SIMPLE_LOG) && (!CONFIG_BT_MESH_NO_LOG)
#define NET_BUF_SIMPLE_DBG(fmt, ...)     do {if (MESH_LOG_LEVEL >= LOG_LEVEL_DEBUG) esp_log_write(ESP_LOG_ERROR, MESH_TRACE_TAG, LOG_FORMAT(D, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__);} while(0)
#define NET_BUF_SIMPLE_ERR(fmt, ...)     do {if (MESH_LOG_LEVEL >= LOG_LEVEL_ERROR) esp_log_write(ESP_LOG_ERROR, MESH_TRACE_TAG, LOG_FORMAT(E, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__);} while(0)
#define NET_BUF_SIMPLE_WARN(fmt, ...)    do {if (MESH_LOG_LEVEL >= LOG_LEVEL_WARN) esp_log_write(ESP_LOG_ERROR, MESH_TRACE_TAG, LOG_FORMAT(W, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__);} while(0)
#define NET_BUF_SIMPLE_INFO(fmt, ...)    do {if (MESH_LOG_LEVEL >= LOG_LEVEL_INFO) esp_log_write(ESP_LOG_ERROR, MESH_TRACE_TAG, LOG_FORMAT(I, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__);} while(0)
#define NET_BUF_SIMPLE_ASSERT(cond)      assert(cond != NULL)
#else /* CONFIG_NET_BUF_SIMPLE_LOG */
#define NET_BUF_SIMPLE_DBG(fmt, ...)
#define NET_BUF_SIMPLE_ERR(fmt, ...)
#define NET_BUF_SIMPLE_WARN(fmt, ...)
#define NET_BUF_SIMPLE_INFO(fmt, ...)
#define NET_BUF_SIMPLE_ASSERT(cond)

#endif /* CONFIG_NET_BUF_SIMPLE_LOG */


#define printk                           ets_printf

