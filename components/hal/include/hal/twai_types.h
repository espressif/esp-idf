/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/twai_types_deprecated.h"  //for backward competiblity, remove on 6.0

#ifdef __cplusplus
extern "C" {
#endif

/* valid bits in TWAI ID for frame formats */
#define TWAI_STD_ID_MASK    0x000007FFU /* Mask of the ID fields in a standard frame */
#define TWAI_EXT_ID_MASK    0x1FFFFFFFU /* Mask of the ID fields in an extended frame */

/* TWAI payload length and DLC definitions */
#define TWAI_FRAME_MAX_DLC      8
#define TWAI_FRAME_MAX_LEN      8

/* TWAI FD payload length and DLC definitions */
#define TWAIFD_FRAME_MAX_DLC    15
#define TWAIFD_FRAME_MAX_LEN    64

/**
 * @brief TWAI error states
 */
typedef enum {
    TWAI_ERROR_ACTIVE,          /**< Error active state: TEC/REC < 96 */
    TWAI_ERROR_WARNING,         /**< Error warning state: TEC/REC >= 96 and < 128 */
    TWAI_ERROR_PASSIVE,         /**< Error passive state: TEC/REC >= 128 and < 256 */
    TWAI_ERROR_BUS_OFF,         /**< Bus-off state: TEC >= 256 (node offline) */
} twai_error_state_t;

/**
 * @brief TWAI group clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
#if SOC_TWAI_SUPPORTED
typedef soc_periph_twai_clk_src_t twai_clock_source_t;
#else
typedef int twai_clock_source_t;
#endif

/**
 * @brief TWAI bitrate timing config advanced mode
 * @note  Setting one of `quanta_resolution_hz` and `brp` is enough, otherwise, `brp` is not used.
 */
typedef struct {
    twai_clock_source_t clk_src;    /**< Optional, clock source, remain 0 to using TWAI_CLK_SRC_DEFAULT by default */
    uint32_t quanta_resolution_hz;  /**< The resolution of one timing quanta, in Hz. If setting, brp will be ignored */
    uint32_t brp;                   /**< Bit rate pre-divider, clock_source_freq / brp = quanta_resolution_hz */
    uint8_t  prop_seg;              /**< Prop_seg length, in quanta time */
    uint8_t  tseg_1;                /**< Seg_1 length, in quanta time */
    uint8_t  tseg_2;                /**< Seg_2 length, in quanta time */
    uint8_t  sjw;                   /**< Sync jump width, in quanta time */
    uint8_t  ssp_offset;            /**< Secondary sample point offset refet to Sync seg, in quanta time, set 0 to disable ssp */
    bool triple_sampling;           /**< Deprecated, in favor of `ssp_offset` */
} twai_timing_config_t;

/**
 * @brief TWAI bitrate timing config advanced mode for esp_driver_twai
 * @note  `quanta_resolution_hz` is not supported in this driver
 */
typedef twai_timing_config_t twai_timing_advanced_config_t;

/**
 * @brief TWAI frame header/format struct type
 */
typedef struct {
    uint32_t id;                    /**< message arbitration identification */
    uint16_t dlc;                   /**< message data length code */
    struct {
        uint32_t ide: 1;            /**< Extended Frame Format (29bit ID) */
        uint32_t rtr: 1;            /**< Message is a Remote Frame */
        uint32_t fdf: 1;            /**< Message is FD format, allow max 64 byte of data */
        uint32_t brs: 1;            /**< Transmit message with Bit Rate Shift. */
        uint32_t esi: 1;            /**< Transmit side error indicator for received frame */
    };
    union {
        uint64_t timestamp;         /**< Timestamp for received message */
        uint64_t trigger_time;      /**< Trigger time for transmitting message*/
    };
} twai_frame_header_t;

#ifdef __cplusplus
}
#endif
