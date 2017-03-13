// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

/**
 * bt_defs.h    Defines useful API for whole Bluedroid
 *
 */
#ifndef _BT_DEFS_H_
#define _BT_DEFS_H_

#include <stdlib.h>
#include <stdint.h>
#include "bt_trace.h"

#include "osi_arch.h"

#define UNUSED(x)                   (void)(x)

#ifndef SIZE_MAX
#define SIZE_MAX                    254
#endif
/*Timer Related Defination*/

#define alarm_timer_t               uint32_t
#define alarm_timer_setfn(timer, cb, data)           \
do {                                                 \
} while (0)
#define alarm_timer_arm(timer, to, periodic)         \
do {                                                 \
} while (0)
#define alarm_timer_disarm(timer)                    \
do {                                                 \
} while (0)
#define alarm_timer_now()             (0)


/*Thread and locker related defination*/
#define RTOS_SUPPORT
#ifdef RTOS_SUPPORT
#define pthread_mutex_t               osi_mutex_t
#define pthread_mutex_init(mutex, a)  osi_mutex_new(mutex)
#define pthread_mutex_destroy(mutex)  osi_mutex_free(mutex)
#define pthread_mutex_lock            osi_mutex_lock
#define pthread_mutex_unlock          osi_mutex_unlock
#else
#define pthread_mutex_t               uint8_t
#define pthread_mutex_init(x1, x2)
#define pthread_mutex_destroy(mutex)
#define pthread_mutex_lock(mutex)
#define pthread_mutex_unlock(mutex)
#endif


/*Bluetooth Address*/
typedef struct {
    uint8_t address[6];
} __attribute__ ((__packed__)) bt_bdaddr_t;

/** Bluetooth 128-bit UUID */
typedef struct {
    uint8_t uu[16];
} bt_uuid_t;

/** Bluetooth Error Status */
/** We need to build on this */

typedef enum {
    BT_STATUS_SUCCESS,
    BT_STATUS_FAIL,
    BT_STATUS_NOT_READY,
    BT_STATUS_NOMEM,
    BT_STATUS_BUSY,
    BT_STATUS_DONE,        /* request already completed */
    BT_STATUS_UNSUPPORTED,
    BT_STATUS_PARM_INVALID,
    BT_STATUS_UNHANDLED,
    BT_STATUS_AUTH_FAILURE,
    BT_STATUS_RMT_DEV_DOWN,
    BT_STATUS_AUTH_REJECTED
} bt_status_t;

#ifndef CPU_LITTLE_ENDIAN
#define CPU_LITTLE_ENDIAN
#endif

inline uint16_t swap_byte_16(uint16_t x)
{
    return (((x & 0x00ffU) << 8) |
            ((x & 0xff00U) >> 8));
}

inline uint32_t swap_byte_32(uint32_t x)
{
    return (((x & 0x000000ffUL) << 24) |
            ((x & 0x0000ff00UL) << 8) |
            ((x & 0x00ff0000UL) >> 8) |
            ((x & 0xff000000UL) >> 24));
}

#ifndef ntohs
inline uint16_t ntohs(uint16_t x)
{
#ifdef CPU_LITTLE_ENDIAN
    return swap_byte_16(x);
#else
    return x;
#endif
}
#endif /* #ifndef ntohs */

#ifndef htons
inline uint16_t htons(uint16_t x)
{
#ifdef CPU_LITTLE_ENDIAN
    return swap_byte_16(x);
#else
    return x;
#endif
}
#endif /* #ifndef htons */

#ifndef ntohl
inline uint32_t ntohl(uint32_t x)
{
#ifdef CPU_LITTLE_ENDIAN
    return swap_byte_32(x);
#else
    return x;
#endif
}
#endif /* #ifndef ntohl*/

#ifndef htonl
inline uint32_t htonl(uint32_t x)
{
#ifdef CPU_LITTLE_ENDIAN
    return swap_byte_32(x);
#else
    return x;
#endif
}
#endif /* #ifndef htonl*/

#endif /* _BT_DEFS_H_ */
