/*
 * SPDX-FileCopyrightText: 2001 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "arch/sys_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif // BYTE_ORDER

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)

#ifndef CONFIG_LWIP_ESP_LWIP_ASSERT
#define LWIP_NOASSERT 1
#endif

typedef uint8_t  u8_t;
typedef int8_t   s8_t;
typedef uint16_t u16_t;
typedef int16_t  s16_t;
typedef uint32_t u32_t;
typedef int32_t  s32_t;


typedef int sys_prot_t;

#define S16_F "d"
#define U16_F "d"
#define X16_F "x"

#define S32_F PRId32
#define U32_F PRIu32
#define X32_F PRIx32

#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#include <stdio.h>

#ifdef CONFIG_LWIP_DEBUG_ESP_LOG
// lwip debugs routed to ESP_LOGD
#include "esp_log.h"
#define LWIP_ESP_LOG_FUNC(format, ...) ESP_LOG_LEVEL(ESP_LOG_DEBUG, "lwip", format, ##__VA_ARGS__)
#define LWIP_PLATFORM_DIAG(x) LWIP_ESP_LOG_FUNC x
#else
// lwip debugs routed to printf
#define LWIP_PLATFORM_DIAG(x)   do {printf x;} while(0)
#endif

#ifdef NDEBUG

#define LWIP_NOASSERT 1

#else // Assertions enabled

#if CONFIG_OPTIMIZATION_ASSERTIONS_SILENT
#define LWIP_PLATFORM_ASSERT(message) abort()
#else
// __assert_func is the assertion failure handler from newlib, defined in assert.h
#define LWIP_PLATFORM_ASSERT(message) __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, message)
#endif

// If assertions are on, the default LWIP_ERROR handler behaviour is to
// abort w/ an assertion failure. Don't do this, instead just print the error (if LWIP_DEBUG is set)
// and run the handler (same as the LWIP_ERROR behaviour if LWIP_NOASSERT is set).
#ifdef LWIP_DEBUG
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  puts(message); handler;}} while(0)
#else
// If LWIP_DEBUG is not set, return the error silently (default LWIP behaviour, also.)
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  handler;}} while(0)
#endif // LWIP_DEBUG

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __ARCH_CC_H__ */
