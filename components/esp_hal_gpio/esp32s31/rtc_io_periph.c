/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: [ESP32S31] IDF-14785
// RTC IO peripheral implementation is temporarily bypassed
// This file provides a stub implementation to allow the build to succeed.
// Full RTC IO support for ESP32-S31 needs to be implemented in the future.
// Related: Need to create soc/esp32s31/include/soc/rtc_io_channel.h and
// implement proper RTC IO pin mapping and descriptor tables.

#include "hal/rtc_io_periph.h"

const int8_t rtc_io_num_map[SOC_GPIO_PIN_COUNT] = {
    -1,//GPIO0
    -1,//GPIO1
    -1,//GPIO2
    -1,//GPIO3
    -1,//GPIO4
    -1,//GPIO5
    -1,//GPIO6
    -1,//GPIO7
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
    -1,//GPIO31
    -1,//GPIO32
    -1,//GPIO33
    -1,//GPIO34
    -1,//GPIO35
    -1,//GPIO36
    -1,//GPIO37
    -1,//GPIO38
    -1,//GPIO39
    -1,//GPIO40
    -1,//GPIO41
    -1,//GPIO42
    -1,//GPIO43
    -1,//GPIO44
    -1,//GPIO45
    -1,//GPIO46
    -1,//GPIO47
    -1,//GPIO48
    -1,//GPIO49
    -1,//GPIO50
    -1,//GPIO51
    -1,//GPIO52
    -1,//GPIO53
    -1,//GPIO54
    -1,//GPIO55
    -1,//GPIO56
    -1,//GPIO57
    -1,//GPIO58
    -1,//GPIO59
    -1,//GPIO60
    -1,//GPIO61
};

// TODO: [ESP32S31] IDF-14785
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
// This requires soc/esp32s31/include/soc/rtc_io_channel.h and
// soc/esp32s31/include/soc/rtc_io_reg.h to be created first
// Currently providing empty array as placeholder - all fields will be 0
const rtc_io_desc_t rtc_io_desc[SOC_RTCIO_PIN_COUNT] = {
    // TODO: Initialize 8 entries (SOC_RTCIO_PIN_COUNT = 8) with proper register addresses
    // and bit masks once rtc_io_channel.h and rtc_io_reg.h are available
};
#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
