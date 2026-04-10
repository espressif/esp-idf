/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_NUM_0           0       /*!< I2S controller port 0 */
#define I2S_NUM_1           1       /*!< I2S controller port 1 */
#define I2S_NUM_2           2       /*!< I2S controller port 2 */
#define I2S_NUM_AUTO        -1      /*!< Select an available port automatically */

/**
 * @brief I2S controller communication mode
 */
typedef enum {
    I2S_COMM_MODE_STD,              /*!< I2S controller using standard communication mode, support Philips/MSB/PCM format */
#if SOC_I2S_SUPPORTS_PDM
    I2S_COMM_MODE_PDM,              /*!< I2S controller using PDM communication mode, support PDM output or input */
#endif
#if SOC_I2S_SUPPORTS_TDM
    I2S_COMM_MODE_TDM,              /*!< I2S controller using TDM communication mode, support up to 16 slots per frame */
#endif
    I2S_COMM_MODE_NONE,             /*!< Unspecified I2S controller mode */
} i2s_comm_mode_t;

/**
 * @brief The multiple of MCLK to sample rate
 * @note  MCLK is the minimum resolution of the I2S clock.
 *        Increasing mclk multiple can reduce the clock jitter of BCLK and WS,
 *        which is also useful for the codec that don't require MCLK but have strict requirement to BCLK.
 *        For the 24-bit slot width, please choose a multiple that can be divided by 3 (i.e. 24-bit compatible).
 */
typedef enum {
    I2S_MCLK_MULTIPLE_128       = 128,     /*!< MCLK = sample_rate * 128 */
    I2S_MCLK_MULTIPLE_192       = 192,     /*!< MCLK = sample_rate * 192 (24-bit compatible) */
    I2S_MCLK_MULTIPLE_256       = 256,     /*!< MCLK = sample_rate * 256 */
    I2S_MCLK_MULTIPLE_384       = 384,     /*!< MCLK = sample_rate * 384 (24-bit compatible) */
    I2S_MCLK_MULTIPLE_512       = 512,     /*!< MCLK = sample_rate * 512 */
    I2S_MCLK_MULTIPLE_576       = 576,     /*!< MCLK = sample_rate * 576 (24-bit compatible) */
    I2S_MCLK_MULTIPLE_768       = 768,     /*!< MCLK = sample_rate * 768 (24-bit compatible) */
    I2S_MCLK_MULTIPLE_1024      = 1024,    /*!< MCLK = sample_rate * 1024 */
    I2S_MCLK_MULTIPLE_1152      = 1152,    /*!< MCLK = sample_rate * 1152 (24-bit compatible) */
} i2s_mclk_multiple_t;

/**
 * @brief LP I2S transaction type
 */
typedef struct {
    void   *buffer;           ///< Pointer to buffer
    size_t  buflen;           ///< Buffer len, this should be in the multiple of 4
    size_t  received_size;    ///< Received size
} lp_i2s_trans_t;

/**
 * @brief Event structure used in I2S event queue
 */
typedef struct {
    void                *dma_buf;/**< The first level pointer of DMA buffer that just finished sending or receiving for `on_recv` and `on_sent` callback
                                  *  NULL for `on_recv_q_ovf` and `on_send_q_ovf` callback
                                  */
    size_t              size;   /**< The buffer size of DMA buffer when success to send or receive,
                                  *  also the buffer size that dropped when queue overflow.
                                  *  It is related to the dma_frame_num and data_bit_width, typically it is fixed when data_bit_width is not changed.
                                  */
} i2s_event_data_t;

/**
 * @brief I2S clock tuning operation
 *
 */
typedef enum {
    I2S_TUNING_MODE_ADDSUB,                 /*!< Add or subtract the tuning value based on the current clock */
    I2S_TUNING_MODE_SET,                    /*!< Set the tuning value to overwrite the current clock */
    I2S_TUNING_MODE_RESET,                  /*!< Set the clock to the initial value */
} i2s_tuning_mode_t;

/**
 * @brief I2S clock tuning configurations
 *
 */
typedef struct {
    i2s_tuning_mode_t   tune_mode;          /*!< Tuning mode, which decides how to tune the MCLK with the tuning value */
    int32_t             tune_mclk_val;      /*!< Tuning value */
    int32_t             max_delta_mclk;     /*!< The maximum frequency that can be increased comparing to the initial MCLK freuqnecy */
    int32_t             min_delta_mclk;     /*!< The minimum frequency that can be decreased comparing to the initial MCLK freuqnecy */
} i2s_tuning_config_t;

/**
 * @brief I2S clock tuning result
 *
 */
typedef struct {
    int32_t             curr_mclk_hz;        /*!< The current MCLK frequency after tuned */
    int32_t             delta_mclk_hz;       /*!< The current changed MCLK frequency comparing to the initial MCLK frequency */
    uint32_t            water_mark;          /*!< The water mark of the internal buffer, in percent */
} i2s_tuning_info_t;

/**
 * @brief Event data structure for LP I2S
 */
typedef struct {
    lp_i2s_trans_t trans;    ///< LP I2S transaction
} lp_i2s_evt_data_t;

typedef struct i2s_channel_obj_t       *i2s_chan_handle_t;     /*!< I2S channel object handle, the control unit of the I2S driver*/
typedef struct lp_i2s_channel_obj_t    *lp_i2s_chan_handle_t;  /*!< I2S channel object handle, the control unit of the I2S driver*/

/**
 * @brief I2S event callback
 * @param[in] handle    I2S channel handle, created from `i2s_new_channel()`
 * @param[in] event     I2S event data
 * @param[in] user_ctx  User registered context, passed from `i2s_channel_register_event_callback()`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*i2s_isr_callback_t)(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx);

/**
 * @brief LP I2S event callback type
 *
 * @param[in] handle     LP I2S channel handle
 * @param[in] event      Event data
 * @param[in] user_ctx   User data
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*lp_i2s_callback_t)(lp_i2s_chan_handle_t handle, lp_i2s_evt_data_t *event, void *user_ctx);

#ifdef __cplusplus
}
#endif
