/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S controller port number, the max port number is (I2S_NUM_MAX -1).
 */
typedef enum {
    I2S_NUM_0 = 0,                 /*!< I2S controller port 0 */
#if SOC_I2S_NUM > 1
    I2S_NUM_1 = 1,                 /*!< I2S controller port 1 */
#endif
    I2S_NUM_MAX,                   /*!< I2S controller port max */
    I2S_NUM_AUTO,                  /*!< Select whichever port is available */
} i2s_port_t;

/**
 * @brief I2S controller communication mode
 */
typedef enum {
    I2S_COMM_MODE_STD,              /*!< I2S controller using standard communication mode, support philip/MSB/PCM format */
#if SOC_I2S_SUPPORTS_PDM
    I2S_COMM_MODE_PDM,              /*!< I2S controller using PDM communication mode, support PDM output or input */
#endif
#if SOC_I2S_SUPPORTS_TDM
    I2S_COMM_MODE_TDM,              /*!< I2S controller using TDM communication mode, support up to 16 slots per frame */
#endif
    I2S_COMM_MODE_NONE,             /*!< Unspecific I2S controller mode */
} i2s_comm_mode_t;

/**
 * @brief I2S channel slot mode
 */
typedef enum {
    I2S_SLOT_MODE_MONO = 1,          /*!< I2S channel slot format mono, transmit same data in all slots for tx mode, only receive the data in the first slots for rx mode. */
    I2S_SLOT_MODE_STEREO = 2,        /*!< I2S channel slot format stereo, transmit different data in different slots for tx mode, receive the data in all slots for rx mode. */
} i2s_slot_mode_t;

/**
 * @brief I2S slot select in standard mode
 */
typedef enum {
    I2S_STD_SLOT_ONLY_LEFT  = 0x01,         /*!< I2S only transmits or receives left slot */
    I2S_STD_SLOT_ONLY_RIGHT = 0x02,         /*!< I2S only transmits or receives right slot */
    I2S_STD_SLOT_LEFT_RIGHT = 0x03,         /*!< I2S only transmits or receives both left and right slot */
} i2s_std_slot_sel_t;

/**
 * @brief I2S channel direction
 */
typedef enum {
    I2S_DIR_RX = BIT(0),            /*!< I2S channel direction RX */
    I2S_DIR_TX = BIT(1),            /*!< I2S channel direction TX */
} i2s_dir_t;

/**
 * @brief I2S controller role
 */
typedef enum {
    I2S_ROLE_MASTER,                /*!< I2S controller master role, bclk and ws signal will be set to output */
    I2S_ROLE_SLAVE                  /*!< I2S controller slave role, bclk and ws signal will be set to input */
} i2s_role_t;

/**
 * @brief Available data bit width in one slot
 */
typedef enum {
    I2S_DATA_BIT_WIDTH_8BIT    = 8,            /*!< I2S channel data bit-width: 8 */
    I2S_DATA_BIT_WIDTH_16BIT   = 16,           /*!< I2S channel data bit-width: 16 */
    I2S_DATA_BIT_WIDTH_24BIT   = 24,           /*!< I2S channel data bit-width: 24 */
    I2S_DATA_BIT_WIDTH_32BIT   = 32,           /*!< I2S channel data bit-width: 32 */
} i2s_data_bit_width_t;

/**
 * @brief Total slot bit width in one slot
 *
 */
typedef enum {
    I2S_SLOT_BIT_WIDTH_AUTO           = (0),      /*!< I2S channel slot bit-width equals to data bit-width */
    I2S_SLOT_BIT_WIDTH_8BIT           = (8),      /*!< I2S channel slot bit-width: 8 */
    I2S_SLOT_BIT_WIDTH_16BIT          = (16),     /*!< I2S channel slot bit-width: 16 */
    I2S_SLOT_BIT_WIDTH_24BIT          = (24),     /*!< I2S channel slot bit-width: 24 */
    I2S_SLOT_BIT_WIDTH_32BIT          = (32),     /*!< I2S channel slot bit-width: 32 */
} i2s_slot_bit_width_t;

/**
 * @brief The multiple of mclk to sample rate
 */
typedef enum {
    I2S_MCLK_MULTIPLE_128       = 128,     /*!< mclk = sample_rate * 128 */
    I2S_MCLK_MULTIPLE_256       = 256,     /*!< mclk = sample_rate * 256 */
    I2S_MCLK_MULTIPLE_384       = 384,     /*!< mclk = sample_rate * 384 */
} i2s_mclk_multiple_t;

typedef enum {
    I2S_CLK_160M_PLL = 0,            /*!< I2S controller clock source PLL_D2_CLK(160M)*/
#if SOC_I2S_SUPPORTS_APLL
    I2S_CLK_APLL,                    /*!< I2S controller clock source APLL*/
#endif
} i2s_clock_src_t;

/**
 * @brief I2S event queue types
 */
typedef enum {
    I2S_EVENT_DMA_ERROR,
    I2S_EVENT_TX_DONE,     /*!< I2S DMA finish sent 1 buffer*/
    I2S_EVENT_RX_DONE,     /*!< I2S DMA finish received 1 buffer*/
    I2S_EVENT_TX_Q_OVF,    /*!< I2S DMA sent queue overflow*/
    I2S_EVENT_RX_Q_OVF,    /*!< I2S DMA receive queue overflow*/
    I2S_EVENT_MAX,         /*!< I2S event max index*/
} i2s_event_type_t;

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief A/U-law decompress or compress configuration.
 *
 */
typedef enum {
    I2S_PCM_DISABLE = 0,      /*!< Disable A/U law decompress or compress*/
    I2S_PCM_A_DECOMPRESS,     /*!< A-law decompress*/
    I2S_PCM_A_COMPRESS,       /*!< A-law compress*/
    I2S_PCM_U_DECOMPRESS,     /*!< U-law decompress*/
    I2S_PCM_U_COMPRESS,       /*!< U-law compress*/
} i2s_pcm_compress_t;
#endif // SOC_I2S_SUPPORTS_PCM

#if SOC_I2S_SUPPORTS_PDM_RX
/**
 * @brief I2S PDM RX down-sampling mode
 */
typedef enum {
    I2S_PDM_DSR_8S = 0,  /*!< downsampling number is 8 for PDM RX mode*/
    I2S_PDM_DSR_16S,     /*!< downsampling number is 16 for PDM RX mode*/
    I2S_PDM_DSR_MAX,
} i2s_pdm_dsr_t;
#endif // SOC_I2S_SUPPORTS_PDM_RX

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief pdm tx singnal scaling mode
 */
typedef enum {
    I2S_PDM_SIG_SCALING_DIV_2 = 0,   /*!< I2S TX PDM signal scaling: /2 */
    I2S_PDM_SIG_SCALING_MUL_1 = 1,   /*!< I2S TX PDM signal scaling: x1 */
    I2S_PDM_SIG_SCALING_MUL_2 = 2,   /*!< I2S TX PDM signal scaling: x2 */
    I2S_PDM_SIG_SCALING_MUL_4 = 3,   /*!< I2S TX PDM signal scaling: x4 */
} i2s_pdm_sig_scale_t;
#endif // SOC_I2S_SUPPORTS_PDM_TX

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

/**
 * @brief Event structure used in I2S event queue
 */
typedef struct {
    i2s_event_type_t    type;   /*!< I2S event type */
    size_t              size;   /*!< I2S data size for I2S_DATA event*/
} i2s_event_t;

typedef struct i2s_channel_t    *i2s_chan_handle_t; /*!< i2s channel handle, the control unit of the i2s driver*/

#ifdef __cplusplus
}
#endif
