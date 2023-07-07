/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rtc_io_periph.h"

const int rtc_io_num_map[SOC_GPIO_PIN_COUNT] = {
    RTCIO_GPIO0_CHANNEL,    //GPIO0
    RTCIO_GPIO1_CHANNEL,    //GPIO1
    RTCIO_GPIO2_CHANNEL,    //GPIO2
    RTCIO_GPIO3_CHANNEL,    //GPIO3
    RTCIO_GPIO4_CHANNEL,    //GPIO4
    RTCIO_GPIO5_CHANNEL,    //GPIO5
    RTCIO_GPIO6_CHANNEL,    //GPIO6
    RTCIO_GPIO7_CHANNEL,    //GPIO7
    -1,//GPIO8
    -1,//GPIO9
    -1,//GPIO10
    -1,//GPIO11
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
    -1,//GPIO26
    -1,//GPIO27
    -1,//GPIO28
    -1,//GPIO29
    -1,//GPIO30
};
