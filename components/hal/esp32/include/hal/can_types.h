// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#warning hal/can_types.h is deprecated, please use hal/twai_types.h instead

#include "hal/twai_types.h"

/* ---------------------------- Compatibility ------------------------------- */

#define CAN_EXTD_ID_MASK                TWAI_EXTD_ID_MASK
#define CAN_STD_ID_MASK                 TWAI_STD_ID_MASK
#define CAN_FRAME_MAX_DLC               TWAI_FRAME_MAX_DLC
#define CAN_FRAME_EXTD_ID_LEN_BYTES     TWAI_FRAME_EXTD_ID_LEN_BYTES
#define CAN_FRAME_STD_ID_LEN_BYTES      TWAI_FRAME_STD_ID_LEN_BYTES
#define CAN_ERR_PASS_THRESH             TWAI_ERR_PASS_THRESH

#define CAN_MSG_FLAG_NONE               TWAI_MSG_FLAG_NONE
#define CAN_MSG_FLAG_EXTD               TWAI_MSG_FLAG_EXTD
#define CAN_MSG_FLAG_RTR                TWAI_MSG_FLAG_RTR
#define CAN_MSG_FLAG_SS                 TWAI_MSG_FLAG_SS
#define CAN_MSG_FLAG_SELF               TWAI_MSG_FLAG_SELF
#define CAN_MSG_FLAG_DLC_NON_COMP       TWAI_MSG_FLAG_DLC_NON_COMP

#if (SOC_TWAI_BRP_MAX > 128) || (CONFIG_ESP32_REV_MIN >= 2)
#define CAN_TIMING_CONFIG_12_5KBITS()   TWAI_TIMING_CONFIG_12_5KBITS()
#define CAN_TIMING_CONFIG_16KBITS()     TWAI_TIMING_CONFIG_16KBITS()
#define CAN_TIMING_CONFIG_20KBITS()     TWAI_TIMING_CONFIG_20KBITS()
#endif
#define CAN_TIMING_CONFIG_25KBITS()     TWAI_TIMING_CONFIG_25KBITS()
#define CAN_TIMING_CONFIG_50KBITS()     TWAI_TIMING_CONFIG_50KBITS()
#define CAN_TIMING_CONFIG_100KBITS()    TWAI_TIMING_CONFIG_100KBITS()
#define CAN_TIMING_CONFIG_125KBITS()    TWAI_TIMING_CONFIG_125KBITS()
#define CAN_TIMING_CONFIG_250KBITS()    TWAI_TIMING_CONFIG_250KBITS()
#define CAN_TIMING_CONFIG_500KBITS()    TWAI_TIMING_CONFIG_500KBITS()
#define CAN_TIMING_CONFIG_800KBITS()    TWAI_TIMING_CONFIG_800KBITS()
#define CAN_TIMING_CONFIG_1MBITS()      TWAI_TIMING_CONFIG_1MBITS()

#define CAN_FILTER_CONFIG_ACCEPT_ALL()  TWAI_FILTER_CONFIG_ACCEPT_ALL()

typedef twai_mode_t                     can_mode_t;
#define CAN_MODE_NORMAL                 TWAI_MODE_NORMAL
#define CAN_MODE_NO_ACK                 TWAI_MODE_NO_ACK
#define CAN_MODE_LISTEN_ONLY            TWAI_MODE_LISTEN_ONLY

typedef twai_message_t                  can_message_t;
typedef twai_timing_config_t            can_timing_config_t;
typedef twai_filter_config_t            can_filter_config_t;

#ifdef __cplusplus
}
#endif
