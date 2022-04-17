/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
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

#define S32_F "d"
#define U32_F "u"
#define X32_F "x"

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
