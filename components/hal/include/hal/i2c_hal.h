/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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

#include "soc/soc_caps.h"
#include "hal/i2c_types.h"
#if SOC_I2C_SUPPORTED
#include "hal/i2c_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2C_SUPPORTED

/**
 * @brief I2C hal Context definition
 */
typedef struct {
    i2c_dev_t *dev;
} i2c_hal_context_t;

/**
 * @brief I2C hal clock configurations
 */
typedef struct {
    uint8_t clk_sel;    // clock select
    uint8_t clk_active; // clock active
    hal_utils_clk_div_t clk_div;  // clock dividers
} i2c_hal_sclk_info_t;

/**
 * @brief Timing configuration structure. Used for I2C reset internally.
 */
typedef struct {
    int high_period; /*!< high_period time */
    int low_period; /*!< low_period time */
    int wait_high_period; /*!< wait_high_period time */
    int rstart_setup; /*!< restart setup */
    int start_hold; /*!< start hold time */
    int stop_setup; /*!< stop setup */
    int stop_hold; /*!< stop hold time */
    int sda_sample; /*!< high_period time */
    int sda_hold; /*!< sda hold time */
    int timeout; /*!< timeout value */
    i2c_hal_sclk_info_t clk_cfg; /*!< clock configuration */
} i2c_hal_timing_config_t;

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
void _i2c_hal_set_bus_timing(i2c_hal_context_t *hal, int scl_freq, i2c_clock_source_t src_clk, int source_freq);

#if SOC_PERIPH_CLK_CTRL_SHARED
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i2c_hal_set_bus_timing(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _i2c_hal_set_bus_timing(__VA_ARGS__);} while(0)
#else
#define i2c_hal_set_bus_timing(...)   _i2c_hal_set_bus_timing(__VA_ARGS__)
#endif

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
 * @brief Set scl timeout reg value according to given timeout us and source clock frequency
 *
 * @param hal Context of the HAL layer
 * @param timeout_us timeout us
 * @param sclk_clock_hz source clock hz
 */
void i2c_hal_master_set_scl_timeout_val(i2c_hal_context_t *hal, uint32_t timeout_us, uint32_t sclk_clock_hz);

/**
 * @brief Init I2C hal layer
 *
 * @param hal Context of the HAL
 * @param i2c_port I2C port number.
 */
void _i2c_hal_init(i2c_hal_context_t *hal, int i2c_port);

#if SOC_PERIPH_CLK_CTRL_SHARED
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i2c_hal_init(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _i2c_hal_init(__VA_ARGS__);} while(0)
#else
#define i2c_hal_init(...)   _i2c_hal_init(__VA_ARGS__)
#endif

/**
 * @brief Deinit I2C hal layer
 *
 * @param hal Context of the HAL
 */
void _i2c_hal_deinit(i2c_hal_context_t *hal);

#if SOC_PERIPH_CLK_CTRL_SHARED
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i2c_hal_deinit(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _i2c_hal_deinit(__VA_ARGS__);} while(0)
#else
#define i2c_hal_deinit(...)   _i2c_hal_deinit(__VA_ARGS__)
#endif

/**
 * @brief Start I2C master transaction
 *
 * @param hal Context of the HAL
 */
void i2c_hal_master_trans_start(i2c_hal_context_t *hal);

#if !SOC_I2C_SUPPORT_HW_FSM_RST

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

#endif // !SOC_I2C_SUPPORT_HW_FSM_RST

#endif  // #if SOC_I2C_SUPPORTED

#ifdef __cplusplus
}
#endif
