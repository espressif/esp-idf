/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/i2c_hal.h"

void i2c_hal_txfifo_rst(i2c_hal_context_t *hal)
{
    i2c_ll_txfifo_rst(hal->dev);
}

void i2c_hal_rxfifo_rst(i2c_hal_context_t *hal)
{
    i2c_ll_rxfifo_rst(hal->dev);
}

void i2c_hal_set_data_mode(i2c_hal_context_t *hal, i2c_trans_mode_t tx_mode, i2c_trans_mode_t rx_mode)
{
    i2c_ll_set_data_mode(hal->dev, tx_mode, rx_mode);
}

void i2c_hal_get_data_mode(i2c_hal_context_t *hal, i2c_trans_mode_t *tx_mode, i2c_trans_mode_t *rx_mode)
{
    i2c_ll_get_data_mode(hal->dev, tx_mode, rx_mode);
}

void i2c_hal_set_filter(i2c_hal_context_t *hal, uint8_t filter_num)
{
    i2c_ll_set_filter(hal->dev, filter_num);
}

void i2c_hal_get_filter(i2c_hal_context_t *hal, uint8_t *filter_num)
{
    *filter_num = i2c_ll_get_filter(hal->dev);
}

void i2c_hal_set_scl_timing(i2c_hal_context_t *hal, int hight_period, int low_period)
{
    i2c_ll_set_scl_timing(hal->dev, hight_period, low_period);
}

void i2c_hal_clr_intsts_mask(i2c_hal_context_t *hal, uint32_t mask)
{
    i2c_ll_clr_intsts_mask(hal->dev, mask);
}

void i2c_hal_enable_intr_mask(i2c_hal_context_t *hal, uint32_t mask)
{
    i2c_ll_enable_intr_mask(hal->dev, mask);
}

void i2c_hal_disable_intr_mask(i2c_hal_context_t *hal, uint32_t mask)
{
    i2c_ll_disable_intr_mask(hal->dev, mask);
}

void i2c_hal_set_fifo_mode(i2c_hal_context_t *hal, bool fifo_mode_en)
{
    i2c_ll_set_fifo_mode(hal->dev, fifo_mode_en);
}

void i2c_hal_set_tout(i2c_hal_context_t *hal, int tout_num)
{
    i2c_ll_set_tout(hal->dev, tout_num);
}

void i2c_hal_set_stop_timing(i2c_hal_context_t *hal, int stop_setup, int stop_hold)
{
    i2c_ll_set_stop_timing(hal->dev, stop_setup, stop_hold);
}

void i2c_hal_set_start_timing(i2c_hal_context_t *hal, int start_setup, int start_hold)
{
    i2c_ll_set_start_timing(hal->dev, start_setup, start_hold);
}

void i2c_hal_set_sda_timing(i2c_hal_context_t *hal, int sda_sample, int sda_hold)
{
    i2c_ll_set_sda_timing(hal->dev, sda_sample, sda_hold);
}

void i2c_hal_set_txfifo_empty_thr(i2c_hal_context_t *hal, uint8_t empty_thr)
{
    i2c_ll_set_txfifo_empty_thr(hal->dev, empty_thr);
}

void i2c_hal_set_rxfifo_full_thr(i2c_hal_context_t *hal, uint8_t full_thr)
{
    i2c_ll_set_rxfifo_full_thr(hal->dev, full_thr);
}

bool i2c_hal_is_bus_busy(i2c_hal_context_t *hal)
{
    return i2c_ll_is_bus_busy(hal->dev);
}

void i2c_hal_get_sda_timing(i2c_hal_context_t *hal, int *sample_time, int *hold_time)
{
    i2c_ll_get_sda_timing(hal->dev, sample_time, hold_time);
}

void i2c_hal_get_tout(i2c_hal_context_t *hal, int *tout_val)
{
    *tout_val = i2c_ll_get_tout(hal->dev);
}

void i2c_hal_get_start_timing(i2c_hal_context_t *hal, int *setup_time, int *hold_time)
{
    i2c_ll_get_start_timing(hal->dev, setup_time, hold_time);
}

void i2c_hal_get_stop_timing(i2c_hal_context_t *hal, int *setup_time, int *hold_time)
{
    i2c_ll_get_stop_timing(hal->dev, setup_time, hold_time);
}

void i2c_hal_get_scl_timing(i2c_hal_context_t *hal, int *high_period, int *low_period)
{
    i2c_ll_get_scl_timing(hal->dev, high_period, low_period);
}

bool i2c_hal_is_master_mode(i2c_hal_context_t *hal)
{
    return i2c_ll_is_master_mode(hal->dev);
}

#if SOC_I2C_SUPPORT_SLAVE
void i2c_hal_set_slave_addr(i2c_hal_context_t *hal, uint16_t slave_addr, bool addr_10bit_en)
{
    i2c_ll_set_slave_addr(hal->dev, slave_addr, addr_10bit_en);
}

void i2c_hal_enable_slave_tx_it(i2c_hal_context_t *hal)
{
    i2c_ll_slave_enable_tx_it(hal->dev);
}

void i2c_hal_enable_slave_rx_it(i2c_hal_context_t *hal)
{
    i2c_ll_slave_enable_rx_it(hal->dev);
}

void i2c_hal_disable_slave_rx_it(i2c_hal_context_t *hal)
{
    i2c_ll_slave_disable_rx_it(hal->dev);
}

void i2c_hal_slave_init(i2c_hal_context_t *hal, int i2c_num)
{
    i2c_ll_slave_init(hal->dev);
    //Use fifo mode
    i2c_ll_set_fifo_mode(hal->dev, true);
    //MSB
    i2c_ll_set_data_mode(hal->dev, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    //Reset fifo
    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_rxfifo_rst(hal->dev);
}
#endif

void i2c_hal_set_bus_timing(i2c_hal_context_t *hal, int scl_freq, i2c_clock_source_t src_clk, int source_freq)
{
    i2c_ll_set_source_clk(hal->dev, src_clk);
    i2c_clk_cal_t clk_cal = {0};
    i2c_ll_cal_bus_clk(source_freq, scl_freq, &clk_cal);
    i2c_ll_set_bus_timing(hal->dev, &clk_cal);
}

void i2c_hal_master_clr_bus(i2c_hal_context_t *hal)
{
    i2c_ll_master_clr_bus(hal->dev);
}

void i2c_hal_master_fsm_rst(i2c_hal_context_t *hal)
{
    i2c_ll_master_fsm_rst(hal->dev);
}

void i2c_hal_master_init(i2c_hal_context_t *hal, int i2c_num)
{
    hal->version = i2c_ll_get_hw_version(hal->dev);
    i2c_ll_master_init(hal->dev);
    //Use fifo mode
    i2c_ll_set_fifo_mode(hal->dev, true);
    //MSB
    i2c_ll_set_data_mode(hal->dev, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    //Reset fifo
    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_rxfifo_rst(hal->dev);
}
