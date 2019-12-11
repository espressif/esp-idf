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

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief   CAN2.0B Constants
 */
#define CAN_EXTD_ID_MASK                0x1FFFFFFF  /**< Bit mask for 29 bit Extended Frame Format ID */
#define CAN_STD_ID_MASK                 0x7FF       /**< Bit mask for 11 bit Standard Frame Format ID */
#define CAN_FRAME_MAX_DLC               8           /**< Max data bytes allowed in CAN2.0 */
#define CAN_FRAME_EXTD_ID_LEN_BYTES     4           /**< EFF ID requires 4 bytes (29bit) */
#define CAN_FRAME_STD_ID_LEN_BYTES      2           /**< SFF ID requires 2 bytes (11bit) */
#define CAN_ERR_PASS_THRESH             128         /**< Error counter threshold for error passive */

/** @cond */    //Doxy command to hide preprocessor definitions from docs
/**
 * @brief   CAN Message flags
 *
 * The message flags are used to indicate the type of message transmitted/received.
 * Some flags also specify the type of transmission.
 */
#define CAN_MSG_FLAG_NONE               0x00        /**< No message flags (Standard Frame Format) */
#define CAN_MSG_FLAG_EXTD               0x01        /**< Extended Frame Format (29bit ID)  */
#define CAN_MSG_FLAG_RTR                0x02        /**< Message is a Remote Transmit Request */
#define CAN_MSG_FLAG_SS                 0x04        /**< Transmit as a Single Shot Transmission. Unused for received. */
#define CAN_MSG_FLAG_SELF               0x08        /**< Transmit as a Self Reception Request.  Unused for received. */
#define CAN_MSG_FLAG_DLC_NON_COMP       0x10        /**< Message's Data length code is larger than 8. This will break compliance with CAN2.0B */

/**
 * @brief Initializer macros for timing configuration structure
 *
 * The following initializer macros offer commonly found bit rates.
 *
 * @note These timing values are based on the assumption APB clock is at 80MHz
 * @note The 20K, 16K and 12.5K bit rates are only available from ESP32 Revision 2 onwards
 */
#ifdef CAN_BRP_DIV_SUPPORTED
#define CAN_TIMING_CONFIG_12_5KBITS()   {.brp = 256, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_16KBITS()     {.brp = 200, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_20KBITS()     {.brp = 200, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#endif
#define CAN_TIMING_CONFIG_25KBITS()     {.brp = 128, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_50KBITS()     {.brp = 80, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_100KBITS()    {.brp = 40, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_125KBITS()    {.brp = 32, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_250KBITS()    {.brp = 16, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_500KBITS()    {.brp = 8, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_800KBITS()    {.brp = 4, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_1MBITS()      {.brp = 4, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}

/**
 * @brief   Initializer macro for filter configuration to accept all IDs
 */
#define CAN_FILTER_CONFIG_ACCEPT_ALL()  {.acceptance_code = 0, .acceptance_mask = 0xFFFFFFFF, .single_filter = true}
/** @endcond */

/**
 * @brief   CAN Controller operating modes
 */
typedef enum {
    CAN_MODE_NORMAL,                /**< Normal operating mode where CAN controller can send/receive/acknowledge messages */
    CAN_MODE_NO_ACK,                /**< Transmission does not require acknowledgment. Use this mode for self testing */
    CAN_MODE_LISTEN_ONLY,           /**< The CAN controller will not influence the bus (No transmissions or acknowledgments) but can receive messages */
} can_mode_t;

/**
 * @brief   Structure to store a CAN message
 *
 * @note
 * @note    The flags member is deprecated
 */
typedef struct {
    union {
        struct {
            //The order of these bits must match deprecated message flags for compatibility reasons
            uint32_t extd: 1;           /**< Extended Frame Format (29bit ID) */
            uint32_t rtr: 1;            /**< Message is a Remote Transmit Request */
            uint32_t ss: 1;             /**< Transmit as a Single Shot Transmission. Unused for received. */
            uint32_t self: 1;           /**< Transmit as a Self Reception Request. Unused for received. */
            uint32_t dlc_non_comp: 1;   /**< Message's Data length code is larger than 8. This will break compliance with CAN2.0B. */
            uint32_t reserved: 27;      /**< Reserved bits */
        };
        //Todo: Deprecate flags
        uint32_t flags;                 /**< Alternate way to set message flags using message flag macros (see documentation) */
    };
    uint32_t identifier;                /**< 11 or 29 bit identifier */
    uint8_t data_length_code;           /**< Data length code */
    uint8_t data[CAN_FRAME_MAX_DLC];    /**< Data bytes (not relevant in RTR frame) */
} can_message_t;

/**
 * @brief   Structure for bit timing configuration of the CAN driver
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    uint32_t brp;                   /**< Baudrate prescaler (i.e., APB clock divider) can be any even number from 2 to 128.
                                         For ESP32 Rev 2 or later, multiples of 4 from 132 to 256 are also supported */
    uint8_t tseg_1;                 /**< Timing segment 1 (Number of time quanta, between 1 to 16) */
    uint8_t tseg_2;                 /**< Timing segment 2 (Number of time quanta, 1 to 8) */
    uint8_t sjw;                    /**< Synchronization Jump Width (Max time quanta jump for synchronize from 1 to 4) */
    bool triple_sampling;           /**< Enables triple sampling when the CAN controller samples a bit */
} can_timing_config_t;

/**
 * @brief   Structure for acceptance filter configuration of the CAN driver (see documentation)
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    uint32_t acceptance_code;       /**< 32-bit acceptance code */
    uint32_t acceptance_mask;       /**< 32-bit acceptance mask */
    bool single_filter;             /**< Use Single Filter Mode (see documentation) */
} can_filter_config_t;

#ifdef __cplusplus
}
#endif
