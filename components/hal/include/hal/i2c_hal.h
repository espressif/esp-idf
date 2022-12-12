/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for I2C

#pragma once

#include "hal/i2c_ll.h"
#include "hal/i2c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C hal Context definition
 */
typedef struct {
    i2c_dev_t *dev;
} i2c_hal_context_t;

#if SOC_I2C_SUPPORT_SLAVE

/**
 * @brief  Init the I2C slave.
 *
 * @param  hal Context of the HAL layer
 * @param  i2c_num I2C port number
 *
 * @return None
 */
void i2c_hal_slave_init(i2c_hal_context_t *hal);

#endif // SOC_I2C_SUPPORT_SLAVE

/**
 * @brief  Init the I2C master.
 *
 * @param  hal Context of the HAL layer
 * @param  i2c_num I2C port number
 *
 * @return None
 */
void i2c_hal_master_init(i2c_hal_context_t *hal);

/**
 * @brief  Set I2C bus timing with the given frequency
 *
 * @param  hal Context of the HAL layer
 * @param  scl_freq The scl frequency to be set
 * @param  src_clk The source clock of I2C
 * @param  source_freq Source clock frequency of I2C
 *
 * @return None
 */
void i2c_hal_set_bus_timing(i2c_hal_context_t *hal, int scl_freq, i2c_clock_source_t src_clk, int source_freq);

/**
 * @brief  I2C hardware FSM reset
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_master_fsm_rst(i2c_hal_context_t *hal);

/**
 * @brief  I2C master handle tx interrupt event
 *
 * @param  hal Context of the HAL layer
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
void i2c_hal_master_handle_tx_event(i2c_hal_context_t *hal, i2c_intr_event_t *event);

/**
 * @brief  I2C master handle rx interrupt event
 *
 * @param  hal Context of the HAL layer
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
void i2c_hal_master_handle_rx_event(i2c_hal_context_t *hal, i2c_intr_event_t *event);

/**
 * @brief Init I2C hal layer
 *
 * @param hal Context of the HAL
 * @param i2c_port I2C port number.
 */
void i2c_hal_init(i2c_hal_context_t *hal, int i2c_port);

/**
 * @brief Deinit I2C hal layer
 *
 * @param hal Context of the HAL
 */
void i2c_hal_deinit(i2c_hal_context_t *hal);

/**
 * @brief Start I2C master transaction
 *
 * @param hal Context of the HAL
 */
void i2c_hal_master_trans_start(i2c_hal_context_t *hal);

/**
 * @brief Get timing configuration
 *
 * @param hal Context of the HAL
 * @param timing_config Pointer to timing config structure.
 */
void i2c_hal_get_timing_config(i2c_hal_context_t *hal, i2c_hal_timing_config_t *timing_config);

/**
 * @brief Set timing configuration
 *
 * @param hal Context of the HAL
 * @param timing_config Timing config structure.
 */
void i2c_hal_set_timing_config(i2c_hal_context_t *hal, i2c_hal_timing_config_t *timing_config);

#ifdef __cplusplus
}
#endif
