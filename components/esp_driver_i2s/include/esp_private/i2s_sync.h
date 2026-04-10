/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal use, public to other IDF components, but not for users' applications.

/**
 * This file is used for getting the bclk and fifo sending count
 * for the synchronization among different I2S ports.
 *
 * The APIs in this file might be called frequently, so they are made light-weight and flexible to be called
 *
 * NOTE: These APIs are private for ESP internal usages.
 *       Please be aware of the risk that APIs might be changed regarding the use case.
 */

#pragma once

#include <stdint.h>
#include "driver/i2s_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_TX_SYNC_CNT

/**
 * @brief Get the counter number of BCLK ticks
 * @note  The BCLK tick count reflects the real data that have sent on line
 * @note  It will be reset automatically when `I2S_ETM_TASK_SYNC_FIFO` is triggered
 *
 * @param[in] tx_handle The I2S tx channel handle
 * @return
 *      - BCLK tick count
 */
uint32_t i2s_sync_get_bclk_count(i2s_chan_handle_t tx_handle);

/**
 * @brief Get the counter number of fifo
 * @note  The FIFO count reflects how many slots have processed
 *        Normally, fifo_cnt = slot_bit_width * bclk_cnt
 *        If fifo_cnt < slot_bit_width * bclk_cnt, that means some data are still stuck in the I2S controller
 * @note  It will be reset automatically when `I2S_ETM_TASK_SYNC_FIFO` is triggered
 *
 * @param[in] tx_handle The I2S tx channel handle
 * @return
 *      - FIFO slot count
 */
uint32_t i2s_sync_get_fifo_count(i2s_chan_handle_t tx_handle);

/**
 * @brief Reset the bclk counter
 *
 * @param[in] tx_handle The I2S tx channel handle
 */
void i2s_sync_reset_bclk_count(i2s_chan_handle_t tx_handle);

/**
 * @brief Reset the fifo counter
 *
 * @param[in] tx_handle The I2S tx channel handle
 */
void i2s_sync_reset_fifo_count(i2s_chan_handle_t tx_handle);

#endif  // SOC_I2S_SUPPORTS_TX_SYNC_CNT

#if SOC_I2S_SUPPORTS_TX_FIFO_SYNC
/**
 * @brief I2S hardware FIFO synchronization supplement mode
 * @note  When the FIFO sync difference count is out of threshold, the hardware will supplement data automatically
 *        This type is to specify which data will be supplemented
 */
typedef enum {
    I2S_SYNC_SUPPL_MODE_LAST_DATA = 0,      /*!< Supplement with the last transmitted data */
    I2S_SYNC_SUPPL_MODE_STATIC_DATA = 1,    /*!< Supplement with static data specified in config */
} i2s_sync_suppl_mode_t;

/**
 * @brief I2S hardware FIFO synchronization configuration
 * @note  This configuration is used for multi I2S port synchronization via ETM
 */
typedef struct {
    uint32_t hw_low_thresh;                 /*!< Lower threshold for FIFO sync difference counter
                                                 - If difference count < hw_low_thresh, do nothing
                                                 - If difference count >= hw_low_thresh, the hardware will supplement data automatically */
    uint32_t sw_high_thresh;                /*!< Upper threshold for FIFO sync difference counter
                                                 - If difference count <= sw_high_thresh, the hardware supplement data automatically
                                                 - If difference count > sw_high_thresh, sync interrupt triggered and
                                                   the software is responsible to decide how to handle this severe asynchronization */
    uint32_t ideal_cnt;                     /*!< Ideal count for FIFO sync difference counter, it depends on the ETM sync task interval and the data rate */
    i2s_sync_suppl_mode_t suppl_mode;       /*!< Data supplement mode when FIFO sync difference is out of threshold */
    uint32_t suppl_data;                    /*!< Static supplement data, only valid when suppl_mode is I2S_SYNC_SUPPL_MODE_STATIC_DATA */
} i2s_sync_fifo_sync_config_t;

/**
 * @brief Get the counter number of FIFO sync difference
 * @note  The FIFO sync difference count reflects the difference between current FIFO count and ideal count
 *
 * @param[in] tx_handle The I2S tx channel handle
 * @return
 *      - FIFO sync difference count
 */
uint32_t i2s_sync_get_fifo_sync_diff_count(i2s_chan_handle_t tx_handle);

/**
 * @brief Reset the FIFO sync difference counter
 *
 * @param[in] tx_handle The I2S tx channel handle
 */
void i2s_sync_reset_fifo_sync_diff_count(i2s_chan_handle_t tx_handle);

/**
 * @brief Enable or disable hardware FIFO synchronization
 * @note  When enabled, hardware will automatically supplement data when FIFO sync difference is greater than hw_low_thresh
 *
 * @param[in] tx_handle The I2S tx channel handle
 * @param[in] enable    true to enable, false to disable
 * @return
 *      - ESP_OK                on success
 *      - ESP_ERR_NOT_SUPPORTED if called on RX channel
 */
esp_err_t i2s_sync_enable_hw_fifo_sync(i2s_chan_handle_t tx_handle, bool enable);

/**
 * @brief Configure hardware FIFO synchronization parameters
 * @note  This function configures the thresholds and supplement mode for hardware FIFO sync
 *
 * @param[in] tx_handle The I2S tx channel handle
 * @param[in] config    Configuration for hardware FIFO synchronization
 * @return
 *      - ESP_OK                on success
 *      - ESP_ERR_INVALID_ARG   if invalid arguments
 *      - ESP_ERR_NOT_SUPPORTED if called on RX channel
 */
esp_err_t i2s_sync_config_hw_fifo_sync(i2s_chan_handle_t tx_handle, const i2s_sync_fifo_sync_config_t *config);

#endif

#ifdef __cplusplus
}
#endif
