// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for I2C

#pragma once
#include "hal/i2c_ll.h"
#include "hal/i2c_types.h"

/**
 * @brief I2C hal Context definition
 */
typedef struct {
    i2c_dev_t *dev;
    uint32_t version;
} i2c_hal_context_t;

/**
 * @brief  Write the I2C rxfifo with the given length
 *
 * @param  hal Context of the HAL layer
 * @param  wr_data Pointer to data buffer
 * @param  wr_size Amount of data needs write
 *
 * @return None
 */
#define i2c_hal_write_txfifo(hal,wr_data,wr_size)    i2c_ll_write_txfifo((hal)->dev,wr_data,wr_size)

/**
 * @brief  Read the I2C rxfifo with the given length
 *
 * @param  hal Context of the HAL layer
 * @param  buf Pointer to data buffer
 * @param  rd_size Amount of data needs read
 *
 * @return None
 */
#define i2c_hal_read_rxfifo(hal,buf,rd_size)    i2c_ll_read_rxfifo((hal)->dev,buf,rd_size)

/**
 * @brief  Write I2C cmd register
 *
 * @param  hal Context of the HAL layer
 * @param  cmd I2C hardware command
 * @param  cmd_idx The index of the command register, should be less than 16
 *
 * @return None
 */
#define i2c_hal_write_cmd_reg(hal,cmd, cmd_idx)    i2c_ll_write_cmd_reg((hal)->dev,cmd,cmd_idx)

/**
 * @brief  Configure the I2C to triger a transaction
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
#define i2c_hal_trans_start(hal)    i2c_ll_trans_start((hal)->dev)

/**
 * @brief  Enable I2C master RX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
#define i2c_hal_enable_master_rx_it(hal)    i2c_ll_master_enable_rx_it((hal)->dev)

/**
 * @brief  Enable I2C master TX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
#define i2c_hal_enable_master_tx_it(hal)    i2c_ll_master_enable_tx_it((hal)->dev)

/**
 * @brief  Clear I2C slave TX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
#define i2c_hal_slave_clr_tx_it(hal)    i2c_ll_slave_clr_tx_it((hal)->dev)

/**
 * @brief  Clear I2C slave RX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
#define i2c_hal_slave_clr_rx_it(hal)    i2c_ll_slave_clr_rx_it((hal)->dev)

/**
 * @brief  Set the source clock. This function is meant to be used in
 *         slave mode, in order to select a source clock abe to handle
 *         the expected SCL frequency.
 *
 * @param  hal Context of the HAL layer
 * @param  src_clk Source clock to use choosen from `i2c_sclk_t` type
 */
#define i2c_hal_set_source_clk(hal, src_clk) i2c_ll_set_source_clk((hal)->dev, src_clk)

/**
 * @brief  Configure I2C SCL timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  high_period The I2C SCL hight period (in core clock cycle, hight_period > 2)
 * @param  low_period The I2C SCL low period (in core clock cycle, low_period > 1)
 * @param  wait_high_period The I2C SCL wait rising edge period.
 *
 * @return None.
 */
#define i2c_hal_set_scl_clk_timing(hal, high_period, low_period, wait_high_period) i2c_ll_set_scl_clk_timing((hal)->dev, high_period, low_period, wait_high_period)

/**
 * @brief  Get I2C SCL timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  high_period Pointer to accept the SCL high period
 * @param  low_period Pointer to accept the SCL low period
 *
 * @return None
 */
#define i2c_hal_get_scl_clk_timing(hal, high_period, low_period, wait_high_period) i2c_ll_get_scl_clk_timing((hal)->dev, high_period, low_period, wait_high_period)

/**
 * @brief  Init the I2C master.
 *
 * @param  hal Context of the HAL layer
 * @param  i2c_num I2C port number
 *
 * @return None
 */
void i2c_hal_master_init(i2c_hal_context_t *hal, i2c_port_t i2c_num);

/**
 * @brief  Init the I2C slave.
 *
 * @param  hal Context of the HAL layer
 * @param  i2c_num I2C port number
 *
 * @return None
 */
void i2c_hal_slave_init(i2c_hal_context_t *hal, i2c_port_t i2c_num);

/**
 * @brief  Reset the I2C hw txfifo
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_txfifo_rst(i2c_hal_context_t *hal);

/**
 * @brief  Reset the I2C hw rxfifo
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_rxfifo_rst(i2c_hal_context_t *hal);

/**
 * @brief  Configure the I2C data MSB bit shifted first or LSB bit shifted first.
 *
 * @param  hal Context of the HAL layer
 * @param  tx_mode Data format of TX
 * @param  rx_mode Data format of RX
 *
 * @return None
 */
void i2c_hal_set_data_mode(i2c_hal_context_t *hal, i2c_trans_mode_t tx_mode, i2c_trans_mode_t rx_mode);

/**
 * @brief  Configure the I2C hardware filter function.
 *
 * @param  hal Context of the HAL layer
 * @param  filter_num If the glitch period on the line is less than this value(in APB cycle), it will be filtered out
 *                    If `filter_num == 0`, the filter will be disabled
 *
 * @return None
 */
void i2c_hal_set_filter(i2c_hal_context_t *hal, uint8_t filter_num);

/**
 * @brief Get the I2C hardware filter configuration
 *
 * @param  hal Context of the HAL layer
 * @param  filter_num Pointer to accept the hardware filter configuration
 *
 * @return None
 */
void i2c_hal_get_filter(i2c_hal_context_t *hal, uint8_t *filter_num);

/**
 * @brief  Configure the I2C SCL timing
 *
 * @param  hal Context of the HAL layer
 * @param  hight_period SCL high period
 * @param  low_period SCL low period
 *
 * @return None
 */
void i2c_hal_set_scl_timing(i2c_hal_context_t *hal, int hight_period, int low_period);

/**
 * @brief Configure the I2C master SCL frequency
 *
 * @param  hal Context of the HAL layer
 * @param  src_clk The I2C Source clock frequency
 * @param  scl_freq The SCL frequency to be set
 *
 * @return None
 */
void i2c_hal_set_scl_freq(i2c_hal_context_t *hal, uint32_t src_clk, uint32_t scl_freq);

/**
 * @brief  Clear the I2C interrupt status with the given mask
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt bitmap needs to be clearned
 *
 * @return None
 */
void i2c_hal_clr_intsts_mask(i2c_hal_context_t *hal, uint32_t mask);

/**
 * @brief  Enable the I2C interrupt with the given mask
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt bitmap needs to be enabled
 *
 * @return None
 */
void i2c_hal_enable_intr_mask(i2c_hal_context_t *hal, uint32_t mask);

/**
 * @brief  Disable the I2C interrupt with the given mask
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt bitmap needs to be disabled
 *
 * @return None
 */
void i2c_hal_disable_intr_mask(i2c_hal_context_t *hal, uint32_t mask);

/**
 * @brief  Configure the I2C memory access mode, FIFO mode or none FIFO mode
 *
 * @param  hal Context of the HAL layer
 * @param  fifo_mode_en Set true to enable FIFO access mode, else set it false
 *
 * @return None
 */
void i2c_hal_set_fifo_mode(i2c_hal_context_t *hal, bool fifo_mode_en);

/**
 * @brief  Configure the I2C timeout value
 *
 * @param  hal Context of the HAL layer
 * @param  tout_val the timeout value to be set
 *
 * @return None
 */
void i2c_hal_set_tout(i2c_hal_context_t *hal, int tout_val);

/**
 * @brief  Get the I2C time out configuration
 *
 * @param  tout_val Pointer to accept the timeout configuration
 *
 * @return None
 */
void i2c_hal_get_tout(i2c_hal_context_t *hal, int *tout_val);

/**
 * @brief Configure the I2C slave address
 *
 * @param  hal Context of the HAL layer
 * @param  slave_addr Slave address
 * @param  addr_10bit_en Set true to enable 10-bit slave address mode, Set false to enable 7-bit address mode
 *
 * @return None
 */
void i2c_hal_set_slave_addr(i2c_hal_context_t *hal, uint16_t slave_addr, bool addr_10bit_en);

/**
 * @brief  Configure the I2C stop timing
 *
 * @param  hal Context of the HAL layer
 * @param  stop_setup The stop condition setup period (in APB cycle)
 * @param  stop_hold The stop condition hold period (in APB cycle)
 *
 * @return None
 */
void i2c_hal_set_stop_timing(i2c_hal_context_t *hal, int stop_setup, int stop_hold);

/**
 * @brief  Configure the I2C start timing
 *
 * @param  hal Context of the HAL layer
 * @param  start_setup The start condition setup period (in APB cycle)
 * @param  start_hold The start condition hold period (in APB cycle)
 *
 * @return None
 */
void i2c_hal_set_start_timing(i2c_hal_context_t *hal, int start_setup, int start_hold);

/**
 * @brief  Configure the I2C sda sample timing
 *
 * @param  hal Context of the HAL layer
 * @param  sda_sample The SDA sample time (in APB cycle)
 * @param  sda_hold The SDA hold time (in APB cycle)
 *
 * @return None
 */
void i2c_hal_set_sda_timing(i2c_hal_context_t *hal, int sda_sample, int sda_hold);

/**
 * @brief  Configure the I2C txfifo empty threshold value
 *
 * @param  hal Context of the HAL layer.
 * @param  empty_thr TxFIFO empty threshold value
 *
 * @return None
 */
void i2c_hal_set_txfifo_empty_thr(i2c_hal_context_t *hal, uint8_t empty_thr);

/**
 * @brief  Configure the I2C rxfifo full threshold value
 *
 * @param  hal Context of the HAL layer
 * @param  full_thr RxFIFO full threshold value
 *
 * @return None
 */
void i2c_hal_set_rxfifo_full_thr(i2c_hal_context_t *hal, uint8_t full_thr);

/**
 * @brief  Get the I2C interrupt status
 *
 * @param  hal Context of the HAL layer
 * @param  mask Pointer to accept the interrupt status
 *
 * @return None
 */
void i2c_hal_get_intsts_mask(i2c_hal_context_t *hal, uint32_t *mask);

/**
 * @brief  Check if the I2C bus is busy
 *
 * @param  hal Context of the HAL layer
 *
 * @return True if the bus is busy, otherwise, fale will be returned
 */
bool i2c_hal_is_bus_busy(i2c_hal_context_t *hal);

/**
 * @brief  Get the I2C sda sample timing configuration
 *
 * @param  hal Context of the HAL layer
 * @param  sample_time Pointer to accept the SDA sample time
 * @param  hold_time Pointer to accept the SDA hold time
 *
 * @return None
 */
void i2c_hal_get_sda_timing(i2c_hal_context_t *hal, int *sample_time, int *hold_time);

/**
 * @brief  Get the I2C stop timing configuration
 *
 * @param  hal Context of the HAL layer
 * @param  setup_time Pointer to accept the stop condition setup period
 * @param  hold_time Pointer to accept the stop condition hold period
 *
 * @return None
 */
void i2c_hal_get_stop_timing(i2c_hal_context_t *hal, int *setup_time, int *hold_time);

/**
 * @brief  Get the I2C scl timing configuration
 *
 * @param  hal Context of the HAL layer
 * @param  high_period Pointer to accept the scl high period
 * @param  low_period Pointer to accept the scl low period
 *
 * @return None
 */
void i2c_hal_get_scl_timing(i2c_hal_context_t *hal, int *high_period, int *low_period);

/**
 * @brief  Get the I2C start timing configuration
 *
 * @param  hal Context of the HAL layer
 * @param  setup_time Pointer to accept the start condition setup period
 * @param  hold_time Pointer to accept the start condition hold period
 *
 * @return None
 */
void i2c_hal_get_start_timing(i2c_hal_context_t *hal, int *setup_time, int *hold_time);

/**
 * @brief  Check if the I2C is master mode
 *
 * @param  hal Context of the HAL layer
 *
 * @return True if in master mode, otherwise, false will be returned
 */
bool i2c_hal_is_master_mode(i2c_hal_context_t *hal);

/**
 * @brief  Get the rxFIFO readable length
 *
 * @param  hal Context of the HAL layer
 * @param  len Pointer to accept the rxFIFO readable length
 *
 * @return None
 */
void i2c_hal_get_rxfifo_cnt(i2c_hal_context_t *hal, uint32_t *len);

/**
 * @brief  Set I2C bus timing with the given frequency
 *
 * @param  hal Context of the HAL layer
 * @param  scl_freq The scl frequency to be set
 * @param  src_clk Source clock of I2C
 *
 * @return None
 */
void i2c_hal_set_bus_timing(i2c_hal_context_t *hal, int scl_freq, i2c_sclk_t src_clk);

/**
 * @brief  Get I2C txFIFO writeable length
 *
 * @param  hal Context of the HAL layer
 * @param  len Pointer to accept the txFIFO writeable length
 *
 * @return None
 */
void i2c_hal_get_txfifo_cnt(i2c_hal_context_t *hal, uint32_t *len);

/**
 * @brief  Check if the I2C is master mode
 *
 * @param  hal Context of the HAL layer
 * @param  tx_mode Pointer to accept the TX data mode
 * @param  rx_mode Pointer to accept the RX data mode
 *
 * @return None
 */
void i2c_hal_get_data_mode(i2c_hal_context_t *hal, i2c_trans_mode_t *tx_mode, i2c_trans_mode_t *rx_mode);

/**
 * @brief  I2C hardware FSM reset
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_master_fsm_rst(i2c_hal_context_t *hal);

/**
 * @brief  @brief Clear I2C bus
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_master_clr_bus(i2c_hal_context_t *hal);

/**
 * @brief  Enable I2C slave TX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_enable_slave_tx_it(i2c_hal_context_t *hal);

/**
 * @brief  Disable I2C slave TX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_disable_slave_tx_it(i2c_hal_context_t *hal);

/**
 * @brief  Enable I2C slave RX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_enable_slave_rx_it(i2c_hal_context_t *hal);

/**
 * @brief  Disable I2C slave RX interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void i2c_hal_disable_slave_rx_it(i2c_hal_context_t *hal);

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
 * @brief  I2C slave handle interrupt event
 *
 * @param  hal Context of the HAL layer
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
void i2c_hal_slave_handle_event(i2c_hal_context_t *hal, i2c_intr_event_t *event);

/**
 * @brief Synchronize I2C status
 *
 * @param hal Context of the HAL layer
 *
 * @return None
 *
 */
void i2c_hal_update_config(i2c_hal_context_t *hal);
