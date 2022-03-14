/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "i2s_types_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S controller communication mode
 */
typedef enum {
    I2S_COMM_MODE_STD,                      /*!< I2S controller using standard communication mode, support philip/MSB/PCM format */
#if SOC_I2S_SUPPORTS_PDM
    I2S_COMM_MODE_PDM,                      /*!< I2S controller using PDM communication mode, support PDM output or input */
#endif
#if SOC_I2S_SUPPORTS_TDM
    I2S_COMM_MODE_TDM,                      /*!< I2S controller using TDM communication mode, support up to 16 slots per frame */
#endif
    I2S_COMM_MODE_MAX
} i2s_comm_mode_t;

/**
 * @brief I2S channel slot mode
 */
typedef enum {
    I2S_SLOT_MODE_MONO = 1,                 /*!< I2S channel slot format mono, transmit same data in all slots for tx mode, only receive the data in the first slots for rx mode. */
    I2S_SLOT_MODE_STEREO = 2,               /*!< I2S channel slot format stereo, transmit different data in different slots for tx mode, receive the data in all slots for rx mode. */
} i2s_slot_mode_t;

/**
 * @brief I2S channel direction
 */
typedef enum {
    I2S_DIR_RX = BIT(0),                    /*!< I2S channel direction RX */
    I2S_DIR_TX = BIT(1),                    /*!< I2S channel direction TX */
} i2s_dir_t;

/**
 * @brief I2S controller role
 */
typedef enum {
    I2S_ROLE_MASTER,                        /*!< I2S controller master role, bclk and ws signal will be set to output */
    I2S_ROLE_SLAVE                          /*!< I2S controller slave role, bclk and ws signal will be set to input */
} i2s_role_t;

/**
 * @brief Available data bit width in one slot
 */
typedef enum {
    I2S_DATA_BIT_WIDTH_8BIT    = 8,                 /*!< I2S channel data bit-width: 8 */
    I2S_DATA_BIT_WIDTH_16BIT   = 16,                /*!< I2S channel data bit-width: 16 */
    I2S_DATA_BIT_WIDTH_24BIT   = 24,                /*!< I2S channel data bit-width: 24 */
    I2S_DATA_BIT_WIDTH_32BIT   = 32,                /*!< I2S channel data bit-width: 32 */
} i2s_data_bit_width_t;

/**
 * @brief Total slot bit width in one slot
 *
 */
typedef enum {
    I2S_SLOT_BIT_WIDTH_DEFAULT        = (0),        /*!< I2S channel slot bit-width equals to data bit-width */
    I2S_SLOT_BIT_WIDTH_8BIT           = (8),        /*!< I2S channel slot bit-width: 8 */
    I2S_SLOT_BIT_WIDTH_16BIT          = (16),       /*!< I2S channel slot bit-width: 16 */
    I2S_SLOT_BIT_WIDTH_24BIT          = (24),       /*!< I2S channel slot bit-width: 24 */
    I2S_SLOT_BIT_WIDTH_32BIT          = (32),       /*!< I2S channel slot bit-width: 32 */
} i2s_slot_bit_width_t;

#if SOC_I2S_SUPPORTS_TDM
/**
 * @brief tdm slot number
 * @note  There are 16 slots in TDM mode.
 *        For TX module, only the active slot send the audio data, the inactive slot send a constant or will be skipped if 'skip_msk' is set.
 *        For RX module, only receive the audio data in active slots, the data in inactive slots will be ignored.
 *        the bit map of active slot can not exceed (0x1<<total_slot_num).
 *        e.g: slot_mask = (I2S_TDM_SLOT0 | I2S_TDM_SLOT3), here the active slot number is 2 and total_slot is not supposed to be smaller than 4.
 */
typedef enum {
    I2S_TDM_SLOT0  = BIT(0),               /*!< I2S slot 0 enabled */
    I2S_TDM_SLOT1  = BIT(1),               /*!< I2S slot 1 enabled */
    I2S_TDM_SLOT2  = BIT(2),               /*!< I2S slot 2 enabled */
    I2S_TDM_SLOT3  = BIT(3),               /*!< I2S slot 3 enabled */
    I2S_TDM_SLOT4  = BIT(4),               /*!< I2S slot 4 enabled */
    I2S_TDM_SLOT5  = BIT(5),               /*!< I2S slot 5 enabled */
    I2S_TDM_SLOT6  = BIT(6),               /*!< I2S slot 6 enabled */
    I2S_TDM_SLOT7  = BIT(7),               /*!< I2S slot 7 enabled */
    I2S_TDM_SLOT8  = BIT(8),               /*!< I2S slot 8 enabled */
    I2S_TDM_SLOT9  = BIT(9),               /*!< I2S slot 9 enabled */
    I2S_TDM_SLOT10 = BIT(10),              /*!< I2S slot 10 enabled */
    I2S_TDM_SLOT11 = BIT(11),              /*!< I2S slot 11 enabled */
    I2S_TDM_SLOT12 = BIT(12),              /*!< I2S slot 12 enabled */
    I2S_TDM_SLOT13 = BIT(13),              /*!< I2S slot 13 enabled */
    I2S_TDM_SLOT14 = BIT(14),              /*!< I2S slot 14 enabled */
    I2S_TDM_SLOT15 = BIT(15),              /*!< I2S slot 15 enabled */
} i2s_tdm_slot_mask_t;
#endif // SOC_I2S_SUPPORTS_TDM

#ifdef __cplusplus
}
#endif
