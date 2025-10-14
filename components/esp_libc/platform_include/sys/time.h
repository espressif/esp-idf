/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
/* Newlib sys/time.h defines timerisset, timerclear, timercmp, timeradd, timersub macros
   for __CYGWIN__ and __rtems__. We want to define these macros in IDF as well.
   Since we wish to use un-modified newlib headers until a patched newlib version is
   available, temporarily define __rtems__ here before including sys/time.h.
   __rtems__ is chosen instead of __CYGWIN__ since there are no other checks in sys/time.h
   which depend on __rtems__.

   Also, so that __rtems__ define does not affect other headers included from sys/time.h,
   we include them here in advance (_ansi.h and sys/types.h).
 */

#include <_ansi.h>
#include <sys/types.h>
#define __rtems__
#include_next <sys/time.h>
#undef __rtems__
