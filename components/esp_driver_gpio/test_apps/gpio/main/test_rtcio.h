/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/gpio_num.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
// The input-only rtcio pins do not have pull-up/down resistors (not support pull-up/down)
#define RTCIO_SUPPORT_PU_PD(num)    (rtc_io_desc[num].pullup != 0)
#define TEST_GPIO_PIN_COUNT 16
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    // GPIO_NUM_0,    //GPIO0   // Workaround: GPIO0 is strap pin, can not be used pullup/pulldown test.
    GPIO_NUM_2,    //GPIO2
    GPIO_NUM_4,    //GPIO4
    // GPIO_NUM_12,   //GPIO12  // Workaround: GPIO12 is strap pin, can not be used pullup/pulldown test.
    GPIO_NUM_13,   //GPIO13
    GPIO_NUM_14,   //GPIO14
    GPIO_NUM_15,   //GPIO15
    GPIO_NUM_25,   //GPIO25
    GPIO_NUM_26,   //GPIO26
    GPIO_NUM_27,   //GPIO27
    GPIO_NUM_32,   //GPIO32
    GPIO_NUM_33,   //GPIO33
    GPIO_NUM_34,   //GPIO34
    GPIO_NUM_35,   //GPIO35
    GPIO_NUM_36,   //GPIO36
    GPIO_NUM_37,   //GPIO37
    GPIO_NUM_38,   //GPIO38
    GPIO_NUM_39,   //GPIO39
};
#elif defined CONFIG_IDF_TARGET_ESP32S2
// Has no input-only rtcio pins, all pins support pull-up/down
#define RTCIO_SUPPORT_PU_PD(num)    1
#define TEST_GPIO_PIN_COUNT 20
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    // GPIO_NUM_0,    //GPIO0   // Workaround: GPIO0 is strap pin, can not be used pullup/pulldown test.
    GPIO_NUM_1,    //GPIO1
    GPIO_NUM_2,    //GPIO2
    GPIO_NUM_3,    //GPIO3
    GPIO_NUM_4,    //GPIO4
    GPIO_NUM_5,    //GPIO5
    GPIO_NUM_6,    //GPIO6
    GPIO_NUM_7,    //GPIO7
    GPIO_NUM_8,    //GPIO8
    GPIO_NUM_9,    //GPIO9
    GPIO_NUM_10,   //GPIO10
    GPIO_NUM_11,   //GPIO11
    GPIO_NUM_12,   //GPIO12
    GPIO_NUM_13,   //GPIO13
    GPIO_NUM_14,   //GPIO14
    GPIO_NUM_15,   //GPIO15
    GPIO_NUM_16,   //GPIO16
    GPIO_NUM_17,   //GPIO17
    // GPIO_NUM_18,   //GPIO18  // Workaround: IO18 is pullup outside in ESP32S2-Saola Runner.
    GPIO_NUM_19,   //GPIO19
    GPIO_NUM_20,   //GPIO20
    GPIO_NUM_21,   //GPIO21
};
#elif defined CONFIG_IDF_TARGET_ESP32S3
// Has no input-only rtcio pins, all pins support pull-up/down
#define RTCIO_SUPPORT_PU_PD(num)    1
#define TEST_GPIO_PIN_COUNT 21
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    // GPIO_NUM_0,    //GPIO0   // Workaround: GPIO0 is strap pin, can not be used pullup/pulldown test.
    GPIO_NUM_1,    //GPIO1
    GPIO_NUM_2,    //GPIO2
    GPIO_NUM_3,    //GPIO3
    GPIO_NUM_4,    //GPIO4
    GPIO_NUM_5,    //GPIO5
    GPIO_NUM_6,    //GPIO6
    GPIO_NUM_7,    //GPIO7
    GPIO_NUM_8,    //GPIO8
    GPIO_NUM_9,    //GPIO9
    GPIO_NUM_10,   //GPIO10
    GPIO_NUM_11,   //GPIO11
    GPIO_NUM_12,   //GPIO12
    GPIO_NUM_13,   //GPIO13
    GPIO_NUM_14,   //GPIO14
    GPIO_NUM_15,   //GPIO15
    GPIO_NUM_16,   //GPIO16
    GPIO_NUM_17,   //GPIO17
    GPIO_NUM_18,   //GPIO18
    GPIO_NUM_19,   //GPIO19
    GPIO_NUM_20,   //GPIO20
    GPIO_NUM_21,   //GPIO21
};
#elif CONFIG_IDF_TARGET_ESP32C6
// Has no input-only rtcio pins, all pins support pull-up/down
#define RTCIO_SUPPORT_PU_PD(num)    1
#define TEST_GPIO_PIN_COUNT 8
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    GPIO_NUM_0,    //GPIO0
    GPIO_NUM_1,    //GPIO1
    GPIO_NUM_2,    //GPIO2
    GPIO_NUM_3,    //GPIO3
    GPIO_NUM_4,    //GPIO4
    GPIO_NUM_5,    //GPIO5
    GPIO_NUM_6,    //GPIO6
    GPIO_NUM_7,    //GPIO7
};
#elif CONFIG_IDF_TARGET_ESP32H2
#define TEST_GPIO_PIN_COUNT 8
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    GPIO_NUM_7,    //GPIO7
    GPIO_NUM_8,    //GPIO8
    GPIO_NUM_9,    //GPIO9
    GPIO_NUM_10,   //GPIO10
    GPIO_NUM_11,   //GPIO11
    GPIO_NUM_12,   //GPIO12
    GPIO_NUM_13,   //GPIO13
    GPIO_NUM_14,   //GPIO14
};
#elif CONFIG_IDF_TARGET_ESP32P4
// Has no input-only rtcio pins, all pins support pull-up/down
#define RTCIO_SUPPORT_PU_PD(num)    1
#define TEST_GPIO_PIN_COUNT 16
const int s_test_map[TEST_GPIO_PIN_COUNT] = {
    GPIO_NUM_0,    //GPIO0
    GPIO_NUM_1,    //GPIO1
    GPIO_NUM_2,    //GPIO2
    GPIO_NUM_3,    //GPIO3
    GPIO_NUM_4,    //GPIO4
    GPIO_NUM_5,    //GPIO5
    GPIO_NUM_6,    //GPIO6
    GPIO_NUM_7,    //GPIO7
    GPIO_NUM_8,    //GPIO8
    GPIO_NUM_9,    //GPIO9
    GPIO_NUM_10,   //GPIO10
    GPIO_NUM_11,   //GPIO11
    GPIO_NUM_12,   //GPIO12
    GPIO_NUM_13,   //GPIO13
    GPIO_NUM_14,   //GPIO14
    GPIO_NUM_15,   //GPIO15
};
#endif

#ifdef __cplusplus
}
#endif
