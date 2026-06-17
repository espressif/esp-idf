/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   TWAI Constants
 */
#define TWAI_EXTD_ID_MASK               0x1FFFFFFF  /**< Bit mask for 29 bit Extended Frame Format ID */
#define TWAI_FRAME_EXTD_ID_LEN_BYTES    4           /**< EFF ID requires 4 bytes (29bit) */
#define TWAI_FRAME_STD_ID_LEN_BYTES     2           /**< SFF ID requires 2 bytes (11bit) */
#define TWAI_ERR_PASS_THRESH            128         /**< Error counter threshold for error passive */

/** @cond */    //Doxy command to hide preprocessor definitions from docs

/**
 * @brief   TWAI Message flags
 *
 * The message flags are used to indicate the type of message transmitted/received.
 * Some flags also specify the type of transmission.
 */
#define TWAI_MSG_FLAG_NONE              0x00        /**< No message flags (Standard Frame Format) */
#define TWAI_MSG_FLAG_EXTD              0x01        /**< Extended Frame Format (29bit ID) */
#define TWAI_MSG_FLAG_RTR               0x02        /**< Message is a Remote Frame */
#define TWAI_MSG_FLAG_SS                0x04        /**< Transmit as a Single Shot Transmission. Unused for received. */
#define TWAI_MSG_FLAG_SELF              0x08        /**< Transmit as a Self Reception Request. Unused for received. */
#define TWAI_MSG_FLAG_DLC_NON_COMP      0x10        /**< Message's Data length code is larger than 8. This will break compliance with TWAI */

/**
 * @brief Initializer macros for timing configuration structure
 *
 * The following initializer macros offer commonly found bit rates. These macros
 * place the sample point at 80% or 67% of a bit time.
 *
 * @note The available bit rates are dependent on the chip target and ECO version.
 */
#define TWAI_TIMING_CONFIG_1KBITS()     {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 20000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_5KBITS()     {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 100000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_10KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 200000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}

#define TWAI_TIMING_CONFIG_12_5KBITS()  {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 312500, .brp = 0, .prop_seg = 0, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_16KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 400000, .brp = 0, .prop_seg = 0, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_20KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 400000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}

#if SOC_IS(ESP32) || SOC_IS(ESP32S2) || SOC_IS(ESP32C3) || SOC_IS(ESP32S3)
// default clock source is APB=80MHz
#define TWAI_TIMING_CONFIG_25KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 625000, .brp = 0, .prop_seg = 0, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#else
#define TWAI_TIMING_CONFIG_25KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 500000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#endif

#define TWAI_TIMING_CONFIG_50KBITS()    {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 1000000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_100KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 2000000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_125KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 2000000, .brp = 0, .prop_seg = 0, .tseg_1 = 11, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_250KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 4000000, .brp = 0, .prop_seg = 0, .tseg_1 = 11, .tseg_2 = 4, .sjw = 2, .ssp_offset = 0, .triple_sampling = false}

#if SOC_IS(ESP32C6) || SOC_IS(ESP32P4)
// default clock source is XTAL=40MHz
#define TWAI_TIMING_CONFIG_500KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 10000000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_800KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 20000000, .brp = 0, .prop_seg = 0, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_1MBITS()     {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 20000000, .brp = 0, .prop_seg = 0, .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .ssp_offset = 0, .triple_sampling = false}
#else
#define TWAI_TIMING_CONFIG_500KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 8000000, .brp = 0, .prop_seg = 0, .tseg_1 = 11, .tseg_2 = 4, .sjw = 2, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_800KBITS()   {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 8000000, .brp = 0, .prop_seg = 0, .tseg_1 = 6, .tseg_2 = 3, .sjw = 1, .ssp_offset = 0, .triple_sampling = false}
#define TWAI_TIMING_CONFIG_1MBITS()     {.clk_src = TWAI_CLK_SRC_DEFAULT, .quanta_resolution_hz = 8000000, .brp = 0, .prop_seg = 0, .tseg_1 = 5, .tseg_2 = 2, .sjw = 1, .ssp_offset = 0, .triple_sampling = false}
#endif

/**
 * @brief   Initializer macro for filter configuration to accept all IDs
 */
#define TWAI_FILTER_CONFIG_ACCEPT_ALL() {.acceptance_code = 0, .acceptance_mask = 0xFFFFFFFF, .single_filter = true}
/** @endcond */

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
    uint32_t brp;                   /**< Bit rate pre-divider, f(clk_src) / brp = quanta_resolution_hz, f(clk_src) can be obtained using esp_clk_tree_src_get_freq_hz(clk_src,,)*/
    uint8_t  prop_seg;              /**< Prop_seg length, in quanta time */
    uint8_t  tseg_1;                /**< Seg_1 length, in quanta time */
    uint8_t  tseg_2;                /**< Seg_2 length, in quanta time */
    uint8_t  sjw;                   /**< Sync jump width, in quanta time */
    uint8_t  ssp_offset;            /**< Secondary sample point offset refet to Sync seg, in quanta time, set 0 to disable ssp */
    bool triple_sampling;           /**< Deprecated, in favor of `ssp_offset` */
} twai_timing_config_t;

/**
 * @brief   TWAI Controller operating modes
 */
typedef enum {
    TWAI_MODE_NORMAL,               /**< Normal operating mode where TWAI controller can send/receive/acknowledge messages */
    TWAI_MODE_NO_ACK,               /**< Transmission does not require acknowledgment. Use this mode for self testing */
    TWAI_MODE_LISTEN_ONLY,          /**< The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages */
} twai_mode_t;

/**
 * @brief   Structure to store a TWAI message
 *
 * @note    The flags member is deprecated
 */
typedef struct {
    union {
        struct {
            //The order of these bits must match deprecated message flags for compatibility reasons
            uint32_t extd: 1;           /**< Extended Frame Format (29bit ID) */
            uint32_t rtr: 1;            /**< Message is a Remote Frame */
            uint32_t ss: 1;             /**< Transmit as a Single Shot Transmission. Unused for received. */
            uint32_t self: 1;           /**< Transmit as a Self Reception Request. Unused for received. */
            uint32_t dlc_non_comp: 1;   /**< Message's Data length code is larger than 8. This will break compliance with ISO 11898-1 */
            uint32_t reserved: 27;      /**< Reserved bits */
        };
        //Todo: Deprecate flags
        uint32_t flags;                 /**< Deprecated: Alternate way to set bits using message flags */
    };
    uint32_t identifier;                /**< 11 or 29 bit identifier */
    uint8_t data_length_code;           /**< Data length code */
    uint8_t data[8];                    /**< Data bytes (not relevant in RTR frame) */
} twai_message_t;

/**
 * @brief   Structure for acceptance filter configuration of the TWAI driver (see documentation)
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    uint32_t acceptance_code;       /**< 32-bit acceptance code */
    uint32_t acceptance_mask;       /**< 32-bit acceptance mask */
    bool single_filter;             /**< Use Single Filter Mode (see documentation) */
} twai_filter_config_t;

#ifdef __cplusplus
}
#endif
