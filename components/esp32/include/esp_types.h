// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_TYPES_H__
#define __ESP_TYPES_H__

#ifdef __GNUC__
#include <sys/cdefs.h>
#endif /*__GNUC__*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define __ATTRIB_PACK           __attribute__ ((packed))
#define __ATTRIB_PRINTF         __attribute__ ((format (printf, 1, 2)))
#define __ATTRIB_NORETURN       __attribute__ ((noreturn))
#define __ATTRIB_ALIGN(x)       __attribute__ ((aligned((x))))
#define INLINE                  __inline__

#define LOCAL       static

/* probably should not put STATUS here */
typedef enum {
    OK = 0,
    FAIL,
    PENDING,
    BUSY,
    CANCEL,
} STATUS;

//#define _LITTLE_ENDIAN  1234
//#define _BYTE_ORDER == _LITTLE_ENDIAN

#define ASSERT( x ) do { \
    if (!(x)) {          \
        printf("%s %u\n", __FILE__, __LINE__); \
        while (1) { \
            asm volatile("nop"); \
        };              \
    }                   \
} while (0)

/* #if __GNUC_PREREQ__(4, 1) */
#ifndef __GNUC__
#if 1
#define __offsetof(type, field)  __builtin_offsetof(type, field)
#else
#define __offsetof(type, field) ((size_t)(&((type *)0)->field))
#endif
#endif /* __GNUC__ */


/* Macros for counting and rounding. */
#ifndef howmany
#define howmany(x, y)   (((x)+((y)-1))/(y))
#endif

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - __offsetof(type,member) );})

#endif /* __ESP_TYPES_H__ */
