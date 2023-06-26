/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2022-2023 Espressif Systems (Shanghai) CO LTD
 */
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

/* see https://sourceforge.net/p/predef/wiki/OperatingSystems/ */
#if defined __ANDROID__
#define LWIP_UNIX_ANDROID
#elif defined __linux__
#define LWIP_UNIX_LINUX
#elif defined __APPLE__
#define LWIP_UNIX_MACH
#elif defined __OpenBSD__
#define LWIP_UNIX_OPENBSD
#elif defined __CYGWIN__
#define LWIP_UNIX_CYGWIN
#elif defined __GNU__
#define LWIP_UNIX_HURD
#endif

#define LWIP_TIMEVAL_PRIVATE 0
#include <sys/time.h>
#include "esp_linux_helper.h"

#define LWIP_ERRNO_INCLUDE <errno.h>

#if defined(LWIP_UNIX_LINUX) || defined(LWIP_UNIX_HURD)
#define LWIP_ERRNO_STDINCLUDE	1
#endif

/* different handling for unit test, normally not needed */
#ifdef LWIP_NOASSERT_ON_ERROR
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  handler;}} while(0)
#endif

#if defined(LWIP_UNIX_ANDROID) && defined(FD_SET)
typedef __kernel_fd_set fd_set;
#endif

#if defined(LWIP_UNIX_MACH)
/* sys/types.h and signal.h bring in Darwin byte order macros. pull the
   header here and disable LwIP's version so that apps still can get
   the macros via LwIP headers and use system headers */
#include <sys/types.h>
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#endif

struct sio_status_s;
typedef struct sio_status_s sio_status_t;
#define sio_fd_t sio_status_t*
#define __sio_fd_t_defined

typedef unsigned int sys_prot_t;

#endif /* LWIP_ARCH_CC_H */
