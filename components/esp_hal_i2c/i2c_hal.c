/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/i2c_hal.h"
#include "hal/i2c_ll.h"
#include "hal/i2c_types.h"

#if SOC_I2C_SUPPORT_SLAVE

void i2c_hal_slave_init(i2c_hal_context_t *hal)
{
    i2c_ll_set_mode(hal->dev, I2C_BUS_MODE_SLAVE);
    i2c_ll_enable_pins_open_drain(hal->dev, true);
    i2c_ll_enable_arbitration(hal->dev, false);
    //MSB
    i2c_ll_set_data_mode(hal->dev, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    //Reset fifo
    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_rxfifo_rst(hal->dev);
}
#endif

void _i2c_hal_set_bus_timing(i2c_hal_context_t *hal, int scl_freq, i2c_clock_source_t src_clk, int source_freq)
{
    i2c_hal_clk_config_t clk_cal = {0};
    i2c_ll_master_cal_bus_clk(source_freq, scl_freq, &clk_cal);
    i2c_ll_master_set_bus_timing(hal->dev, &clk_cal);
}

void i2c_hal_master_fsm_rst(i2c_hal_context_t *hal)
{
    i2c_ll_master_fsm_rst(hal->dev);
}

void i2c_hal_master_init(i2c_hal_context_t *hal)
{
    i2c_ll_set_mode(hal->dev, I2C_BUS_MODE_MASTER);
    i2c_ll_enable_pins_open_drain(hal->dev, true);
    i2c_ll_enable_arbitration(hal->dev, false);
    i2c_ll_master_rx_full_ack_level(hal->dev, false);
    //MSB
    i2c_ll_set_data_mode(hal->dev, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    //Reset fifo
    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_rxfifo_rst(hal->dev);
}

void _i2c_hal_init(i2c_hal_context_t *hal, int i2c_port)
{
    if (hal->dev == NULL) {
        hal->dev = I2C_LL_GET_HW(i2c_port);
    }
    i2c_ll_enable_controller_clock(hal->dev, true);
}

void _i2c_hal_deinit(i2c_hal_context_t *hal)
{
    i2c_ll_enable_controller_clock(hal->dev, false);
    hal->dev = NULL;
}

void i2c_hal_master_set_scl_timeout_val(i2c_hal_context_t *hal, uint32_t timeout_us, uint32_t sclk_clock_hz)
{
    uint32_t reg_val = i2c_ll_calculate_timeout_us_to_reg_val(sclk_clock_hz, timeout_us);
    i2c_ll_set_tout(hal->dev, reg_val);
}

#if !I2C_LL_SUPPORT_HW_FSM_RST

void i2c_hal_get_timing_config(i2c_hal_context_t *hal, i2c_hal_timing_config_t *timing_config)
{
    i2c_ll_get_scl_clk_timing(hal->dev, &timing_config->high_period, &timing_config->low_period, &timing_config->wait_high_period);
    i2c_ll_get_start_timing(hal->dev, &timing_config->rstart_setup, &timing_config->start_hold);
    i2c_ll_get_stop_timing(hal->dev, &timing_config->stop_setup, &timing_config->stop_hold);
    i2c_ll_get_sda_timing(hal->dev, &timing_config->sda_sample, &timing_config->sda_hold);
    i2c_ll_get_tout(hal->dev, &timing_config->timeout);
    i2c_ll_master_save_clock_configurations(hal->dev, &timing_config->clk_cfg.clk_div.integer, &timing_config->clk_cfg.clk_sel, &timing_config->clk_cfg.clk_active);
    i2c_ll_master_get_fractional_divider(hal->dev, &timing_config->clk_cfg.clk_div.numerator, &timing_config->clk_cfg.clk_div.denominator);
}

void i2c_hal_set_timing_config(i2c_hal_context_t *hal, i2c_hal_timing_config_t *timing_config)
{
    i2c_ll_set_scl_clk_timing(hal->dev, timing_config->high_period, timing_config->low_period, timing_config->wait_high_period);
    i2c_ll_master_set_start_timing(hal->dev, timing_config->rstart_setup, timing_config->start_hold);
    i2c_ll_master_set_stop_timing(hal->dev, timing_config->stop_setup, timing_config->stop_hold);
    i2c_ll_set_sda_timing(hal->dev, timing_config->sda_sample, timing_config->sda_hold);
    i2c_ll_set_tout(hal->dev, timing_config->timeout);
    i2c_ll_master_restore_clock_configurations(hal->dev, timing_config->clk_cfg.clk_div.integer, timing_config->clk_cfg.clk_sel, timing_config->clk_cfg.clk_active);
    i2c_ll_master_set_fractional_divider(hal->dev, timing_config->clk_cfg.clk_div.numerator, timing_config->clk_cfg.clk_div.denominator);
}

#endif // !I2C_LL_SUPPORT_HW_FSM_RST

void i2c_hal_master_trans_start(i2c_hal_context_t *hal)
{
    i2c_ll_update(hal->dev);
    i2c_ll_start_trans(hal->dev);
}
