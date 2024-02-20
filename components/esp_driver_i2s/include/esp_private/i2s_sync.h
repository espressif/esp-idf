/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_TX_SYNC_CNT

/**
 * @brief Get the counter number of BCLK ticks
 * @note  The BCLK tick count reflects the real data that have sent on line
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

#ifdef __cplusplus
}
#endif
