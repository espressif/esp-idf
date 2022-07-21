/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/touch_sensor_channel.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "soc/rtc_io_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const int touch_sensor_channel_io_map[];

#ifdef __cplusplus
}
#endif
