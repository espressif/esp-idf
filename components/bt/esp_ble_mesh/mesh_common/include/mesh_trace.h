/*
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TRACE_H_
#define _BLE_MESH_TRACE_H_

#include <assert.h>
#include "esp_log.h"
#include "mesh_util.h"
#include "esp_rom_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define common tracing for all */
#ifndef BLE_MESH_LOG_LEVEL_ERROR
#define BLE_MESH_LOG_LEVEL_ERROR            1
#endif /* BLE_MESH_LOG_LEVEL_ERROR */

#ifndef BLE_MESH_LOG_LEVEL_WARN
#define BLE_MESH_LOG_LEVEL_WARN             2
#endif /* BLE_MESH_LOG_LEVEL_WARN */

#ifndef BLE_MESH_LOG_LEVEL_INFO
#define BLE_MESH_LOG_LEVEL_INFO             3
#endif /* BLE_MESH_LOG_LEVEL_INFO */

#ifndef BLE_MESH_LOG_LEVEL_DEBUG
#define BLE_MESH_LOG_LEVEL_DEBUG            4
#endif /* BLE_MESH_LOG_LEVEL_DEBUG */

#ifndef BLE_MESH_LOG_LEVEL_VERBOSE
#define BLE_MESH_LOG_LEVEL_VERBOSE          5
#endif /*BLE_MESH_LOG_LEVEL_VERBOSE */

#ifdef CONFIG_BLE_MESH_STACK_TRACE_LEVEL
#define BLE_MESH_LOG_LEVEL                  CONFIG_BLE_MESH_STACK_TRACE_LEVEL
#else
#define BLE_MESH_LOG_LEVEL                  BLE_MESH_LOG_LEVEL_WARN
#endif

#ifdef CONFIG_BLE_MESH_NET_BUF_TRACE_LEVEL
#define BLE_MESH_NET_BUF_LOG_LEVEL          CONFIG_BLE_MESH_NET_BUF_TRACE_LEVEL
#else
#define BLE_MESH_NET_BUF_LOG_LEVEL          BLE_MESH_LOG_LEVEL_WARN
#endif

#define BLE_MESH_TRACE_TAG                  "BLE_MESH"

#if (LOG_LOCAL_LEVEL >= 4)
#define BLE_MESH_LOG_LOCAL_LEVEL_MAPPING    (LOG_LOCAL_LEVEL + 1)
#else
#define BLE_MESH_LOG_LOCAL_LEVEL_MAPPING    LOG_LOCAL_LEVEL
#endif

#define BLE_MESH_LOG_LEVEL_CHECK(LAYER, LEVEL)  (MAX(LAYER##_LOG_LEVEL, BLE_MESH_LOG_LOCAL_LEVEL_MAPPING) >= BLE_MESH_LOG_LEVEL_##LEVEL)

#define BLE_MESH_PRINT_E(tag, format, ...)  {esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BLE_MESH_PRINT_W(tag, format, ...)  {esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BLE_MESH_PRINT_I(tag, format, ...)  {esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BLE_MESH_PRINT_D(tag, format, ...)  {esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BLE_MESH_PRINT_V(tag, format, ...)  {esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define printk          esp_rom_printf

#define _STRINGIFY(x)   #x
#define STRINGIFY(s)    _STRINGIFY(s)

#ifndef __ASSERT
#define __ASSERT(test, str) assert(test)
#endif

#ifndef __ASSERT_NO_MSG
#define __ASSERT_NO_MSG(x) assert(x)
#endif

#if !CONFIG_BLE_MESH_NO_LOG
#define BT_ERR(fmt, args...)    do {if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_ERROR) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, ERROR)) BLE_MESH_PRINT_E(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_WARN(fmt, args...)   do {if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_WARN)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, WARN))  BLE_MESH_PRINT_W(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_INFO(fmt, args...)   do {if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_INFO)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, INFO))  BLE_MESH_PRINT_I(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define BT_DBG(fmt, args...)    do {if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_DEBUG) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, DEBUG)) BLE_MESH_PRINT_D(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#else
#define BT_ERR(fmt, args...)
#define BT_WARN(fmt, args...)
#define BT_INFO(fmt, args...)
#define BT_DBG(fmt, args...)
#endif

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG) && (!CONFIG_BLE_MESH_NO_LOG)
#define NET_BUF_ERR(fmt, args...)   do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_ERROR) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, ERROR)) BLE_MESH_PRINT_E(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_WARN(fmt, args...)  do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_WARN)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, WARN))  BLE_MESH_PRINT_W(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_INFO(fmt, args...)  do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_INFO)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, INFO))  BLE_MESH_PRINT_I(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_DBG(fmt, args...)   do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_DEBUG) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, DEBUG)) BLE_MESH_PRINT_D(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_ASSERT(cond)        __ASSERT_NO_MSG(cond)
#else
#define NET_BUF_ERR(fmt, args...)
#define NET_BUF_WARN(fmt, args...)
#define NET_BUF_INFO(fmt, args...)
#define NET_BUF_DBG(fmt, args...)
#define NET_BUF_ASSERT(cond)
#endif

#if defined(CONFIG_BLE_MESH_NET_BUF_SIMPLE_LOG) && (!CONFIG_BLE_MESH_NO_LOG)
#define NET_BUF_SIMPLE_ERR(fmt, args...)    do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_ERROR) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, ERROR)) BLE_MESH_PRINT_E(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_SIMPLE_WARN(fmt, args...)   do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_WARN)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, WARN))  BLE_MESH_PRINT_W(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_SIMPLE_INFO(fmt, args...)   do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_INFO)  && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, INFO))  BLE_MESH_PRINT_I(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_SIMPLE_DBG(fmt, args...)    do {if ((BLE_MESH_NET_BUF_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_DEBUG) && BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH_NET_BUF, DEBUG)) BLE_MESH_PRINT_D(BLE_MESH_TRACE_TAG, fmt, ## args);} while(0)
#define NET_BUF_SIMPLE_ASSERT(cond)         __ASSERT_NO_MSG(cond)
#else
#define NET_BUF_SIMPLE_ERR(fmt, args...)
#define NET_BUF_SIMPLE_WARN(fmt, args...)
#define NET_BUF_SIMPLE_INFO(fmt, args...)
#define NET_BUF_SIMPLE_DBG(fmt, args...)
#define NET_BUF_SIMPLE_ASSERT(cond)
#endif

#if CONFIG_BLE_MESH_BQB_TEST_LOG
/**
 * For example, the test case "MESH/NODE/TNPT/BV-01-C"
 * could use BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_TNPT, "msg %s", msg)
 * to print some message.
 */
enum BLE_MESH_BQB_TEST_LOG_LEVEL {
    BLE_MESH_BQB_TEST_LOG_LEVEL_OUTPUT_ALL           = 0,        /* Output all BQB related test log */
    BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE      = BIT(0),
    BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_PVNR      = BIT(1),
    BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_CFGCL     = BIT(2),
    BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_SR        = BIT(3),
    BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_CL        = BIT(4),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_PBADV         = BIT(5),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_MPS           = BIT(6),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_PROV          = BIT(7),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_BCN           = BIT(8),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_NET           = BIT(9),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_RLY           = BIT(10),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_TNPT          = BIT(11),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_IVU           = BIT(12),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_KR            = BIT(13),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_FRND_FN       = BIT(14),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_FRND_LPN      = BIT(15),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_PROX          = BIT(16),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_MPXS          = BIT(17),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_CFG           = BIT(18),
    BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_HM            = BIT(19),
};

#define BLE_MESH_BQB_TEST_LOG_LEVEL_OUTPUT_NONE      0x000FFFFF

#endif /* CONFIG_BLE_MESH_BQB_TEST_LOG */

#if (CONFIG_BLE_MESH_BQB_TEST_LOG && !CONFIG_BLE_MESH_NO_LOG)
extern bool bt_mesh_bqb_test_flag_check(uint32_t flag_mask);
extern int bt_mesh_bqb_test_flag_set(uint32_t value);
#define BT_BQB(flag_mask, fmt, args...)                             \
            do {                                                    \
                if (bt_mesh_bqb_test_flag_check(flag_mask))         \
                    BLE_MESH_PRINT_I("BLE_MESH_BQB", fmt, ## args); \
            } while (0)
#else
#define BT_BQB(flag_mask, fmt, args...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_TRACE_H_ */
