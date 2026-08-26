/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
