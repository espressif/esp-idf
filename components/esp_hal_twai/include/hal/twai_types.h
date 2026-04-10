/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/assert.h"
#include "hal/twai_types_deprecated.h"  //for backward competiblity, remove on 6.0

#ifdef __cplusplus
extern "C" {
#endif

/* valid bits in TWAI ID for frame formats */
#define TWAI_STD_ID_MASK        0x000007FFU /**< Mask of the ID fields in a standard frame */
#define TWAI_EXT_ID_MASK        0x1FFFFFFFU /**< Mask of the ID fields in an extended frame */

/* TWAI payload length and DLC definitions */
#define TWAI_FRAME_MAX_DLC      8
#define TWAI_FRAME_MAX_LEN      8

/* TWAI FD payload length and DLC definitions */
#define TWAIFD_FRAME_MAX_DLC    15
#define TWAIFD_FRAME_MAX_LEN    64

/**
 * @brief TWAI node error fsm states
 */
typedef enum {
    TWAI_ERROR_ACTIVE,              /**< Error active state: TEC/REC < 96 */
    TWAI_ERROR_WARNING,             /**< Error warning state: TEC/REC >= 96 and < 128 */
    TWAI_ERROR_PASSIVE,             /**< Error passive state: TEC/REC >= 128 and < 256 */
    TWAI_ERROR_BUS_OFF,             /**< Bus-off state: TEC >= 256 (node offline) */
} twai_error_state_t;

/**
 * @brief TWAI transmit error type structure
 */
typedef union {
    struct {
        uint32_t arb_lost: 1;       /**< Arbitration lost error (lost arbitration during transmission) */
        uint32_t bit_err: 1;        /**< Bit error detected (dominant/recessive mismatch during transmission) */
        uint32_t form_err: 1;       /**< Form error detected (frame fixed-form bit violation) */
        uint32_t stuff_err: 1;      /**< Stuff error detected (e.g. dominant error frame received) */
        uint32_t ack_err: 1;        /**< ACK error (no ack), transmission without acknowledge received */
    };
    uint32_t val;                   /**< Integrated error flags */
} twai_error_flags_t;

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
 * @brief TWAI bitrate timing advanced config structure
 */
typedef struct {
    uint32_t brp;                   /**< Bitrate pre-divider, which decides the quanta time */
    uint8_t  prop_seg;              /**< Prop_seg length, in quanta time */
    uint8_t  tseg_1;                /**< Seg_1 length, in quanta time */
    uint8_t  tseg_2;                /**< Seg_2 length, in quanta time */
    uint8_t  sjw;                   /**< Sync jump width, in quanta time */
    uint8_t  ssp_offset;            /**< Secondary sample point offset refet to Sync seg, in quanta time, set 0 to disable ssp */
} twai_timing_advanced_config_t;

/**
 * @brief Configuration for TWAI mask filter
 */
typedef struct {
    union {
        uint32_t id;                /**< Single base ID for filtering */
        struct {
            uint32_t *id_list;      /**< Base ID list array for filtering, which share the same `mask` */
            uint32_t num_of_ids;    /**< List length of `id_list`, remain empty to using single `id` instead of `id_list` */
        };
    };
    uint32_t mask;                  /**< Mask to determine the matching bits (1 = match bit, 0 = any bit) */
    struct {
        uint32_t is_ext: 1;         /**< True for extended ID filtering, false for standard ID */
        uint32_t no_classic: 1;     /**< If true, Classic TWAI frames are excluded (only TWAI FD allowed) */
        uint32_t no_fd: 1;          /**< If true, TWAI FD frames are excluded (only Classic TWAI allowed) */
        uint32_t dual_filter: 1;    /**< Set filter as dual-16bits filter mode, see `twai_make_dual_filter()` for easy config */
    };
} twai_mask_filter_config_t;

/**
 * @brief Range-based filter configuration structure
 */
typedef struct {
    uint32_t range_low;             /**< Lower bound of the ID filtering range, included */
    uint32_t range_high;            /**< Upper bound of the ID filtering range, included */
    struct {
        uint32_t is_ext: 1;         /**< True for extended ID filtering, false for standard ID */
        uint32_t no_classic: 1;     /**< If true, Classic TWAI frames are excluded (only TWAI FD allowed) */
        uint32_t no_fd: 1;          /**< If true, TWAI FD frames are excluded (only Classic TWAI allowed) */
    };
} twai_range_filter_config_t;

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

/**
 * @brief Translate TWAIFD format DLC code to bytes length
 * @param[in] dlc The frame DLC code follow the FD spec
 * @return        The byte length of DLC stand for
 */
__attribute__((always_inline))
static inline uint16_t twaifd_dlc2len(uint16_t dlc)
{
    HAL_ASSERT(dlc <= TWAIFD_FRAME_MAX_DLC);
    return (dlc <= 8) ? dlc :
           (dlc <= 12) ? (dlc - 8) * 4 + 8 :
           (dlc <= 13) ? (dlc - 12) * 8 + 24 :
           (dlc - 13) * 16 + 32;
}

/**
 * @brief Translate TWAIFD format bytes length to DLC code
 * @param[in] byte_len The byte length of the message
 * @return             The FD adopted frame DLC code
 */
__attribute__((always_inline))
static inline uint16_t twaifd_len2dlc(uint16_t byte_len)
{
    HAL_ASSERT(byte_len <= TWAIFD_FRAME_MAX_LEN);
    return (byte_len <= 8) ? byte_len :
           (byte_len <= 24) ? (byte_len - 8 + 3) / 4 + 8 :
           (byte_len <= 32) ? (byte_len - 24 + 7) / 8 + 12 :
           (byte_len - 32 + 15) / 16 + 13;
}

#ifdef __cplusplus
}
#endif
