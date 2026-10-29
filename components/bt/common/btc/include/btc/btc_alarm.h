/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef __BTC_ALARM_H__
#define __BTC_ALARM_H__

#include <stdint.h>
#include "osi/alarm.h"

/* btc_alarm_args_t */
typedef struct {
    osi_alarm_t *alarm;
    uint8_t generation;
} btc_alarm_args_t;

void alarm_invoke(osi_alarm_t *alarm, uint8_t generation);
void btc_alarm_handler(btc_msg_t *msg);

#endif /* __BTC_ALARM_H__ */
