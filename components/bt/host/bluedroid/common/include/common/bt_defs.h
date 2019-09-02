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
 * common/bt_defs.h    Defines useful API for whole Bluedroid
 *
 */
#ifndef _BT_DEFS_H_
#define _BT_DEFS_H_

#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "bt_common.h"
#include "common/bt_target.h"

#define UNUSED(x)                   (void)(x)

/*Timer Related Defination*/

//by Snake.T
typedef void (TIMER_CBACK)(void *p_tle);
typedef struct _tle {
    struct _tle  *p_next;
    struct _tle  *p_prev;
    TIMER_CBACK  *p_cback;
    INT32         ticks;
    INT32         ticks_initial;
    TIMER_PARAM_TYPE   param;
    TIMER_PARAM_TYPE   data;
    UINT16        event;
    UINT8         in_use;
} TIMER_LIST_ENT;

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


/*Bluetooth Address*/
typedef struct {
    uint8_t address[6];
} __attribute__ ((__packed__)) bt_bdaddr_t;

/** Bluetooth 128-bit UUID */
typedef struct {
    uint8_t uu[16];
} bt_uuid_t;

#ifndef CPU_LITTLE_ENDIAN
#define CPU_LITTLE_ENDIAN
#endif

#endif /* _BT_DEFS_H_ */
