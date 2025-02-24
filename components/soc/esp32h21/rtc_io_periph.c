/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rtc_io_periph.h"

const int rtc_io_num_map[SOC_GPIO_PIN_COUNT] = {
    -1,//GPIO0
    -1,//GPIO1
    -1,//GPIO2
    -1,//GPIO3
    -1,//GPIO4
    RTCIO_GPIO5_CHANNEL,//GPIO5
    RTCIO_GPIO6_CHANNEL,//GPIO6
    RTCIO_GPIO7_CHANNEL,//GPIO7
    RTCIO_GPIO8_CHANNEL,//GPIO8
    RTCIO_GPIO9_CHANNEL,//GPIO9
    RTCIO_GPIO10_CHANNEL,//GPIO10
    RTCIO_GPIO11_CHANNEL,//GPIO11
    -1,//GPIO12
    -1,//GPIO13
    -1,//GPIO14
    -1,//GPIO15
    -1,//GPIO16
    -1,//GPIO17
    -1,//GPIO18
    -1,//GPIO19
    -1,//GPIO20
    -1,//GPIO21
    -1,//GPIO22
    -1,//GPIO23
    -1,//GPIO24
    -1,//GPIO25
};
