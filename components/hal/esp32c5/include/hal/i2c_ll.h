/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I2C register operations

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/misc.h"
#include "hal/assert.h"
#if SOC_I2C_SUPPORTED  // TODO: [ESP32C5] IDF-8694
#include "soc/i2c_periph.h"
#include "soc/soc_caps.h"
#include "soc/i2c_struct.h"
#include "soc/pcr_struct.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lpperi_struct.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C hardware cmd register fields.
 */
typedef union {
    struct {
        uint32_t byte_num:    8,  /*!< Specifies the length of data (in bytes) to be read or written */
                 ack_en:      1,  /*!< Used to enable the I2C controller during a write operation to check whether ACK */
                 ack_exp:     1,  /*!< Used to configure the level of the ACK bit expected by the I2C controller during a write operation */
                 ack_val:     1,  /*!< Used to configure the level of the ACK bit sent by the I2C controller during a read operation */
                 op_code:     3,  /*!< Indicates the command */
                 reserved14: 17, /*!< Reserved bits */
                 done:        1;  /*!< Indicates that a command has been executed */
    };
    uint32_t val;
} i2c_ll_hw_cmd_t;

// I2C operation mode command
#define I2C_LL_CMD_RESTART    6    /*!<I2C restart command */
#define I2C_LL_CMD_WRITE      1    /*!<I2C write command */
#define I2C_LL_CMD_READ       3    /*!<I2C read command */
#define I2C_LL_CMD_STOP       2    /*!<I2C stop command */
#define I2C_LL_CMD_END        4    /*!<I2C end command */

typedef enum {
    I2C_INTR_MST_TXFIFO_WM = (1 << 1),
    I2C_INTR_MST_RXFIFO_WM = (1 << 0),
    I2C_LL_INTR_NACK = (1 << 10),
    I2C_LL_INTR_TIMEOUT = (1 << 8),
    I2C_LL_INTR_MST_COMPLETE = (1 << 7),
    I2C_LL_INTR_ARBITRATION = (1 << 5),
    I2C_LL_INTR_END_DETECT = (1 << 3),
    I2C_LL_INTR_ST_TO = (1 << 13),
} i2c_ll_master_intr_t;

typedef enum {
    I2C_INTR_SLV_TXFIFO_WM = (1 << 1),
    I2C_INTR_SLV_RXFIFO_WM = (1 << 0),
    I2C_INTR_SLV_COMPLETE = (1 << 7),
    I2C_INTR_START = (1 << 15),
    I2C_INTR_STRETCH = (1 << 16),
    I2C_INTR_UNMATCH = (1 << 18),
} i2c_ll_slave_intr_t;

// Get the I2C hardware instance
#define I2C_LL_GET_HW(i2c_num)      (((i2c_num) == I2C_NUM_0) ? (&I2C0) : (&LP_I2C))
#define I2C_LL_MASTER_EVENT_INTR    (I2C_NACK_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
#define I2C_LL_SLAVE_EVENT_INTR     (I2C_TRANS_COMPLETE_INT_ENA_M|I2C_TXFIFO_WM_INT_ENA_M|I2C_RXFIFO_WM_INT_ENA_M | I2C_SLAVE_STRETCH_INT_ENA_M)
#define I2C_LL_SLAVE_RX_EVENT_INTR  (I2C_TRANS_COMPLETE_INT_ENA_M | I2C_RXFIFO_WM_INT_ENA_M | I2C_SLAVE_STRETCH_INT_ENA_M)
#define I2C_LL_SLAVE_TX_EVENT_INTR  (I2C_TXFIFO_WM_INT_ENA_M)
#define I2C_LL_RESET_SLV_SCL_PULSE_NUM_DEFAULT   (9)
#define I2C_LL_SCL_WAIT_US_VAL_DEFAULT   (2500)  // Approximate value for SCL timeout regs (in us).

// Record for Pins usage logs

#define LP_I2C_SCL_PIN_ERR_LOG   "SCL pin can only be configured as GPIO#7"
#define LP_I2C_SDA_PIN_ERR_LOG   "SDA pin can only be configured as GPIO#6"

#define LP_I2C_SDA_IOMUX_PAD 6
#define LP_I2C_SCL_IOMUX_PAD 7

/**
 * @brief  Calculate I2C bus frequency
 *         Note that the clock accuracy is affected by the external pull-up resistor,
 *         here we try to to calculate a configuration parameter which is close to the required clock.
 *         But in I2C communication, the clock accuracy is not very concerned.
 *
 * @param  source_clk I2C source clock
 * @param  bus_freq I2C bus frequency
 * @param  clk_cal Pointer to accept the clock configuration
 *
 * @return None
 */
static inline void i2c_ll_master_cal_bus_clk(uint32_t source_clk, uint32_t bus_freq, i2c_hal_clk_config_t *clk_cal)
{
    uint32_t clkm_div = source_clk / (bus_freq * 1024) + 1;
    uint32_t sclk_freq = source_clk / clkm_div;
    uint32_t half_cycle = sclk_freq / bus_freq / 2;
    //SCL
    clk_cal->clkm_div = clkm_div;
    clk_cal->scl_low = half_cycle;
    // default, scl_wait_high < scl_high
    // Make 80KHz as a boundary here, because when working at lower frequency, too much scl_wait_high will faster the frequency
    // according to some hardware behaviors.
    clk_cal->scl_wait_high = (bus_freq >= 80 * 1000) ? (half_cycle / 2 - 2) : (half_cycle / 4);
    clk_cal->scl_high = half_cycle - clk_cal->scl_wait_high;
    clk_cal->sda_hold = half_cycle / 4;
    clk_cal->sda_sample = half_cycle / 2;
    clk_cal->setup = half_cycle;
    clk_cal->hold = half_cycle;
    //default we set the timeout value to about 10 bus cycles
    // log(20*half_cycle)/log(2) = log(half_cycle)/log(2) +  log(20)/log(2)
    clk_cal->tout = (int)(sizeof(half_cycle) * 8 - __builtin_clz(5 * half_cycle)) + 2;

    /* Verify the assumptions made by the hardware */
    HAL_ASSERT(clk_cal->scl_wait_high < clk_cal->sda_sample &&
               clk_cal->sda_sample < clk_cal->scl_high);
}

/**
 * @brief  Update I2C configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_update(i2c_dev_t *hw)
{
    hw->ctr.conf_upgate = 1;
}

/**
 * @brief Enable the bus clock for I2C module
 *
 * @param i2c_port I2C port id
 * @param enable true to enable, false to disable
 */
static inline void i2c_ll_enable_bus_clock(int i2c_port, bool enable)
{
    PCR.i2c[i2c_port].i2c_conf.i2c_clk_en = enable;
}

/**
 * @brief Reset the I2C module
 *
 * @param i2c_port Group ID
 */
static inline void i2c_ll_reset_register(int i2c_port)
{
    PCR.i2c[i2c_port].i2c_conf.i2c_rst_en = 1;
    PCR.i2c[i2c_port].i2c_conf.i2c_rst_en = 0;
}

/**
 * @brief  Configure the I2C bus timing related register.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  bus_cfg Pointer to the data structure holding the register configuration.
 *
 * @return None
 */
static inline void i2c_ll_master_set_bus_timing(i2c_dev_t *hw, i2c_hal_clk_config_t *bus_cfg)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.i2c[0].i2c_sclk_conf, i2c_sclk_div_num, bus_cfg->clkm_div - 1);

    /* According to the Technical Reference Manual, the following timings must be subtracted by 1.
     * However, according to the practical measurement and some hardware behaviour, if wait_high_period and scl_high minus one.
     * The SCL frequency would be a little higher than expected. Therefore, the solution
     * here is not to minus scl_high as well as scl_wait high, and the frequency will be absolutely accurate to all frequency
     * to some extent. */
    hw->scl_low_period.scl_low_period = bus_cfg->scl_low - 1;
    hw->scl_high_period.scl_high_period = bus_cfg->scl_high;
    hw->scl_high_period.scl_wait_high_period = bus_cfg->scl_wait_high;
    //sda sample
    hw->sda_hold.sda_hold_time = bus_cfg->sda_hold - 1;
    hw->sda_sample.sda_sample_time = bus_cfg->sda_sample - 1;
    //setup
    hw->scl_rstart_setup.scl_rstart_setup_time = bus_cfg->setup - 1;
    hw->scl_stop_setup.scl_stop_setup_time = bus_cfg->setup - 1;
    //hold
    hw->scl_start_hold.scl_start_hold_time = bus_cfg->hold - 1;
    hw->scl_stop_hold.scl_stop_hold_time = bus_cfg->hold - 1;
    hw->to.time_out_value = bus_cfg->tout;
    hw->to.time_out_en = 1;
}

/**
 * @brief Set fractional divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param div_a The denominator of the frequency divider factor of the i2c function clock
 * @param div_b The numerator of the frequency divider factor of the i2c function clock.
 */
static inline void i2c_ll_master_set_fractional_divider(i2c_dev_t *hw, uint8_t div_a, uint8_t div_b)
{
    /* Set div_a and div_b to 0, as it's not necessary to use them */
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.i2c[0].i2c_sclk_conf, i2c_sclk_div_a, div_a);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.i2c[0].i2c_sclk_conf, i2c_sclk_div_b, div_b);
}

/**
 * @brief  Reset I2C txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_txfifo_rst(i2c_dev_t *hw)
{
    hw->fifo_conf.tx_fifo_rst = 1;
    hw->fifo_conf.tx_fifo_rst = 0;
}

/**
 * @brief  Reset I2C rxFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_rxfifo_rst(i2c_dev_t *hw)
{
    hw->fifo_conf.rx_fifo_rst = 1;
    hw->fifo_conf.rx_fifo_rst = 0;
}

/**
 * @brief  Clear I2C interrupt status
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  mask Interrupt mask needs to be cleared
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_clear_intr_mask(i2c_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

/**
 * @brief  Enable I2C interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  mask Interrupt mask needs to be enabled
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_enable_intr_mask(i2c_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val |= mask;
}

/**
 * @brief  Disable I2C interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  mask Interrupt mask needs to be disabled
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_disable_intr_mask(i2c_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val &= (~mask);
}

/**
 * @brief  Get I2C interrupt status
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return I2C interrupt status
 */
__attribute__((always_inline))
static inline void i2c_ll_get_intr_mask(i2c_dev_t *hw, uint32_t *intr_status)
{
    *intr_status = hw->int_status.val;
}

/**
 * @brief  Configure I2C memory access mode, FIFO mode or non-FIFO mode
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  fifo_mode_en Set true to enable FIFO access mode, else, set it false
 *
 * @return None
 */
static inline void i2c_ll_slave_set_fifo_mode(i2c_dev_t *hw, bool fifo_mode_en)
{
    hw->fifo_conf.nonfifo_en = fifo_mode_en ? 0 : 1;
}

/**
 * @brief  Configure I2C timeout
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  tout_num The I2C timeout value needs to be set (2^tout in core clock cycle)
 *
 * @return None
 */
static inline void i2c_ll_set_tout(i2c_dev_t *hw, int tout)
{
    hw->to.time_out_value = tout;
}

/**
 * @brief  Configure I2C slave broadcasting mode.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  broadcast_en Set true to enable broadcast, else, set it false
 *
 * @return None
 */
static inline void i2c_ll_slave_broadcast_enable(i2c_dev_t *hw, bool broadcast_en)
{
    hw->ctr.addr_broadcasting_en = broadcast_en;
}

/**
 * @brief Get the cause of SCL clock stretching in slave mode
 *
 * @param hw Beginning address of the peripheral registers
 * @param stretch_cause Pointer to stretch cause in the slave mode.
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_slave_get_stretch_cause(i2c_dev_t *hw, i2c_slave_stretch_cause_t *stretch_cause)
{
    switch (hw->sr.stretch_cause) {
    case 0:
        *stretch_cause = I2C_SLAVE_STRETCH_CAUSE_ADDRESS_MATCH;
        break;
    case 1:
        *stretch_cause = I2C_SLAVE_STRETCH_CAUSE_TX_EMPTY;
        break;
    case 2:
        *stretch_cause = I2C_SLAVE_STRETCH_CAUSE_RX_FULL;
        break;
    case 3:
        *stretch_cause = I2C_SLAVE_STRETCH_CAUSE_SENDING_ACK;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief  Configure I2C slave address
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  slave_addr I2C slave address needs to be set
 * @param  addr_10bit_en Set true to enable 10-bit slave address mode, set false to enable 7-bit address mode
 *
 * @return None
 */
static inline void i2c_ll_set_slave_addr(i2c_dev_t *hw, uint16_t slave_addr, bool addr_10bit_en)
{
    hw->slave_addr.addr_10bit_en = addr_10bit_en;
    if (addr_10bit_en) {
        uint16_t addr_14_7 = (slave_addr & 0xff) << 7;
        uint8_t addr_6_0 = ((slave_addr & 0x300) >> 8) | 0x78;
        hw->slave_addr.slave_addr = addr_14_7 | addr_6_0;
        hw->ctr.addr_10bit_rw_check_en = addr_10bit_en;
    } else {
        hw->slave_addr.slave_addr = slave_addr;
    }
}

/**
 * @brief Write I2C hardware command register
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  cmd I2C hardware command
 * @param  cmd_idx The index of the command register, should be less than 16
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_write_cmd_reg(i2c_dev_t *hw, i2c_ll_hw_cmd_t cmd, int cmd_idx)
{
    hw->command[cmd_idx].val = cmd.val;
}

/**
 * @brief Configure I2C start timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  start_setup The start condition setup period (in core clock cycle)
 * @param  start_hold The start condition hold period (in core clock cycle)
 *
 * @return None
 */
static inline void i2c_ll_master_set_start_timing(i2c_dev_t *hw, int start_setup, int start_hold)
{
    hw->scl_rstart_setup.scl_rstart_setup_time = start_setup;
    hw->scl_start_hold.scl_start_hold_time = start_hold - 1;
}

/**
 * @brief Configure I2C stop timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  stop_setup The stop condition setup period (in core clock cycle)
 * @param  stop_hold The stop condition hold period (in core clock cycle)
 *
 * @return None
 */
static inline void i2c_ll_master_set_stop_timing(i2c_dev_t *hw, int stop_setup, int stop_hold)
{
    hw->scl_stop_setup.scl_stop_setup_time = stop_setup;
    hw->scl_stop_hold.scl_stop_hold_time = stop_hold;
}

/**
 * @brief Configure I2C stop timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  sda_sample The SDA sample time (in core clock cycle)
 * @param  sda_hold The SDA hold time (in core clock cycle)
 *
 * @return None
 */
static inline void i2c_ll_set_sda_timing(i2c_dev_t *hw, int sda_sample, int sda_hold)
{
    hw->sda_hold.sda_hold_time = sda_hold;
    hw->sda_sample.sda_sample_time = sda_sample;
}

/**
 * @brief  Set I2C txFIFO empty threshold
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  empty_thr The txFIFO empty threshold
 *
 * @return None
 */
static inline void i2c_ll_set_txfifo_empty_thr(i2c_dev_t *hw, uint8_t empty_thr)
{
    hw->fifo_conf.txfifo_wm_thrhd = empty_thr;
}

/**
 * @brief  Set I2C rxFIFO full threshold
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  full_thr The rxFIFO full threshold
 *
 * @return None
 */
static inline void i2c_ll_set_rxfifo_full_thr(i2c_dev_t *hw, uint8_t full_thr)
{
    hw->fifo_conf.fifo_prt_en = 1;
    hw->fifo_conf.rxfifo_wm_thrhd = full_thr;
}

/**
 * @brief  Set the I2C data mode, LSB or MSB
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  tx_mode Tx data bit mode
 * @param  rx_mode Rx data bit mode
 *
 * @return None
 */
static inline void i2c_ll_set_data_mode(i2c_dev_t *hw, i2c_trans_mode_t tx_mode, i2c_trans_mode_t rx_mode)
{
    hw->ctr.tx_lsb_first = tx_mode;
    hw->ctr.rx_lsb_first = rx_mode;
}

/**
 * @brief Get I2C sda timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  sda_sample Pointer to accept the SDA sample timing configuration
 * @param  sda_hold Pointer to accept the SDA hold timing configuration
 *
 * @return None
 */
static inline void i2c_ll_get_sda_timing(i2c_dev_t *hw, int *sda_sample, int *sda_hold)
{
    *sda_hold = hw->sda_hold.sda_hold_time;
    *sda_sample = hw->sda_sample.sda_sample_time;
}

/**
 * @brief Get the I2C hardware version
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return The I2C hardware version
 */
static inline uint32_t i2c_ll_get_hw_version(i2c_dev_t *hw)
{
    return hw->date.date;
}

/**
 * @brief  Check if the I2C bus is busy
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return True if I2C state machine is busy, else false will be returned
 */
__attribute__((always_inline))
static inline bool i2c_ll_is_bus_busy(i2c_dev_t *hw)
{
    return hw->sr.bus_busy;
}

/**
 * @brief  Check if I2C is master mode
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return True if I2C is master mode, else false will be returned
 */
static inline bool i2c_ll_is_master_mode(i2c_dev_t *hw)
{
    return hw->ctr.ms_mode;
}

/**
 * @brief Get the rxFIFO readable length
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return RxFIFO readable length
 */
__attribute__((always_inline))
static inline void i2c_ll_get_rxfifo_cnt(i2c_dev_t *hw, uint32_t *length)
{
    *length = hw->sr.rxfifo_cnt;
}

/**
 * @brief  Get I2C txFIFO writable length
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return TxFIFO writable length
 */
__attribute__((always_inline))
static inline void i2c_ll_get_txfifo_len(i2c_dev_t *hw, uint32_t *length)
{
    *length = SOC_I2C_FIFO_LEN - hw->sr.txfifo_cnt;
}

/**
 * @brief Get I2C timeout configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return The I2C timeout value
 */
static inline void i2c_ll_get_tout(i2c_dev_t *hw, int *timeout)
{
    *timeout = hw->to.time_out_value;
}

/**
 * @brief  Start I2C transfer
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_trans_start(i2c_dev_t *hw)
{
    hw->ctr.trans_start = 1;
}

/**
 * @brief Get I2C start timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  setup_time Pointer to accept the start condition setup period
 * @param  hold_time Pointer to accept the start condition hold period
 *
 * @return None
 */
static inline void i2c_ll_get_start_timing(i2c_dev_t *hw, int *setup_time, int *hold_time)
{
    *setup_time = hw->scl_rstart_setup.scl_rstart_setup_time;
    *hold_time = hw->scl_start_hold.scl_start_hold_time + 1;
}

/**
 * @brief  Get I2C stop timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  setup_time Pointer to accept the stop condition setup period
 * @param  hold_time Pointer to accept the stop condition hold period
 *
 * @return None
 */
static inline void i2c_ll_get_stop_timing(i2c_dev_t *hw, int *setup_time, int *hold_time)
{
    *setup_time = hw->scl_stop_setup.scl_stop_setup_time;
    *hold_time = hw->scl_stop_hold.scl_stop_hold_time;
}

/**
 * @brief  Write the I2C hardware txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs to be written
 *
 * @return None.
 */
__attribute__((always_inline))
static inline void i2c_ll_write_txfifo(i2c_dev_t *hw, const uint8_t *ptr, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->data, fifo_rdata, ptr[i]);
    }
}

/**
 * @brief  Read the I2C hardware rxFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs read
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_read_rxfifo(i2c_dev_t *hw, uint8_t *ptr, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        ptr[i] = HAL_FORCE_READ_U32_REG_FIELD(hw->data, fifo_rdata);
    }
}

/**
 * @brief  Write the I2C hardware txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ram_offset Offset value of I2C RAM.
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs to be written
 */
static inline void i2c_ll_write_by_nonfifo(i2c_dev_t *hw, uint8_t ram_offset, const uint8_t *ptr, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        hw->txfifo_mem[i + ram_offset] = ptr[i];
    }
}

/**
 * @brief  Read the I2C hardware ram
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ram_offset Offset value of I2C RAM.
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs read
 */
static inline void i2c_ll_read_by_nonfifo(i2c_dev_t *hw, uint8_t ram_offset, uint8_t *ptr, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        ptr[i] = hw->rxfifo_mem[i + ram_offset];
    }
}

/**
 * @brief Get access to I2C RAM address directly
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  addr_wr_en Enable I2C ram address read and write
 *
 * @return None
*/
static inline void i2c_ll_enable_mem_access_nonfifo(i2c_dev_t *hw, bool addr_wr_en)
{
    hw->fifo_conf.fifo_addr_cfg_en = addr_wr_en;
}

/**
 * @brief  Configure I2C hardware filter
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  filter_num If the glitch period on the line is less than this value, it can be filtered out
 *                    If `filter_num == 0`, the filter will be disabled
 *
 * @return None
 */
static inline void i2c_ll_master_set_filter(i2c_dev_t *hw, uint8_t filter_num)
{
    if (filter_num > 0) {
        hw->filter_cfg.scl_filter_thres = filter_num;
        hw->filter_cfg.sda_filter_thres = filter_num;
        hw->filter_cfg.scl_filter_en = 1;
        hw->filter_cfg.sda_filter_en = 1;
    } else {
        hw->filter_cfg.scl_filter_en = 0;
        hw->filter_cfg.sda_filter_en = 0;
    }
}

/**
 * @brief  Get I2C hardware filter configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return The hardware filter configuration
 */
static inline void i2c_ll_master_get_filter(i2c_dev_t *hw, uint8_t *filter_conf)
{
    *filter_conf = hw->filter_cfg.scl_filter_thres;
}

/**
 * @brief Reset I2C master FSM. When the master FSM is stuck, call this function to reset the FSM
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_fsm_rst(i2c_dev_t *hw)
{
    // fsm_rst is a self cleared bit.
    hw->ctr.fsm_rst = 1;
}

/**
 * @brief Clear I2C bus, when the slave is stuck in a deadlock and keeps pulling the bus low,
 *        master can controls the SCL bus to generate 9 CLKs.
 *
 * Note: The master cannot detect if deadlock happens, but when the scl_st_to interrupt is generated, a deadlock may occur.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  slave_pulses When I2C master is IDLE, the number of pulses will be sent out.
 *
 * @return None
 */
static inline void i2c_ll_master_clr_bus(i2c_dev_t *hw, uint32_t slave_pulses)
{
    hw->scl_sp_conf.scl_rst_slv_num = slave_pulses;
    hw->scl_sp_conf.scl_rst_slv_en = 1;
    hw->ctr.conf_upgate = 1;
    // hardware will clear scl_rst_slv_en after sending SCL pulses,
    // and we should set conf_upgate bit to synchronize register value.
    while (hw->scl_sp_conf.scl_rst_slv_en);
    hw->ctr.conf_upgate = 1;
}

/**
 * @brief Set the ACK level that the I2C master must send when the Rx FIFO count has reached the threshold value.
 *        ack_level: 1 (NACK)
 *        ack_level: 0 (ACK)
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_rx_full_ack_level(i2c_dev_t *hw, int ack_level)
{
    hw->ctr.rx_full_ack_level = ack_level;
}

/**
 * @brief Set I2C source clock
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  src_clk Source clock of the I2C
 *
 * @return None
 */
static inline void i2c_ll_set_source_clk(i2c_dev_t *hw, i2c_clock_source_t src_clk)
{
    if (hw == &LP_I2C) {
        // Do nothing
        return;
    }

    // src_clk : (1) for RTC_CLK, (0) for XTAL
    PCR.i2c[0].i2c_sclk_conf.i2c_sclk_sel = (src_clk == I2C_CLK_SRC_RC_FAST) ? 1 : 0;
}

/**
 * @brief Set LP I2C source clock
 *
 * @param  hw Address offset of the LP I2C peripheral registers
 * @param  src_clk Source clock for the LP I2C peripheral
 *
 * @return None
 */
static inline void lp_i2c_ll_set_source_clk(i2c_dev_t *hw, soc_periph_lp_i2c_clk_src_t src_clk)
{
    (void)hw;
    // src_clk : (0) for LP_FAST_CLK (RTC Fast), (1) for XTAL_D2_CLK
    switch (src_clk) {
    case LP_I2C_SCLK_LP_FAST:
        LP_CLKRST.lpperi.lp_i2c_clk_sel = 0;
        break;
    case LP_I2C_SCLK_XTAL_D2:
        LP_CLKRST.lpperi.lp_i2c_clk_sel = 1;
        break;
    default:
        // Invalid source clock selected
        abort();
    }
}

/// LP_CLKRST.lpperi is a shared register, so this function must be used in an atomic way
#define lp_i2c_ll_set_source_clk(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2c_ll_set_source_clk(__VA_ARGS__)

/**
 * @brief Enable bus clock for the LP I2C module
 *
 * @param hw_id LP I2C instance ID
 * @param enable True to enable, False to disable
 */
static inline void _lp_i2c_ll_enable_bus_clock(int hw_id, bool enable)
{
    (void)hw_id;
    LPPERI.clk_en.lp_ext_i2c_ck_en = enable;
}

/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_i2c_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_i2c_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset LP I2C module
 *
 * @param hw_id LP I2C instance ID
 */
static inline void lp_i2c_ll_reset_register(int hw_id)
{
    (void)hw_id;
    LPPERI.reset_en.lp_ext_i2c_reset_en = 1;
    LPPERI.reset_en.lp_ext_i2c_reset_en = 0;
}

/// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define lp_i2c_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2c_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable I2C peripheral controller clock
 *
 * @param dev Peripheral instance address
 * @param en True to enable, False to disable
 */
static inline void i2c_ll_enable_controller_clock(i2c_dev_t *hw, bool en)
{
    if (hw == &LP_I2C) {
        // Do nothing
        return;
    }

    PCR.i2c[0].i2c_sclk_conf.i2c_sclk_en = en;
}

/**
 * @brief  Init I2C master
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_init(i2c_dev_t *hw)
{
    typeof(hw->ctr) ctrl_reg;
    ctrl_reg.val = 0;
    ctrl_reg.ms_mode = 1;
    ctrl_reg.sda_force_out = 1;
    ctrl_reg.scl_force_out = 1;
    hw->ctr.val = ctrl_reg.val;
}

/**
 * @brief  Init I2C slave
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_slave_init(i2c_dev_t *hw)
{
    typeof(hw->ctr) ctrl_reg;
    ctrl_reg.val = 0;
    ctrl_reg.sda_force_out = 1;
    ctrl_reg.scl_force_out = 1;
    hw->ctr.val = ctrl_reg.val;
    hw->fifo_conf.fifo_addr_cfg_en = 0;
}

/**
 * @brief Set whether slave should auto start, or only start with start signal from master
 *
 * @param hw Beginning address of the peripheral registers
 * @param slv_ex_auto_en 1 if slave auto start data transaction, otherwise, 0.
 */
static inline void i2c_ll_slave_tx_auto_start_en(i2c_dev_t *hw, bool slv_ex_auto_en)
{
    hw->ctr.slv_tx_auto_start_en = slv_ex_auto_en;
}

/**
 * @brief Get I2C interrupt status register address
 */
static inline volatile void *i2c_ll_get_interrupt_status_reg(i2c_dev_t *dev)
{
    return &dev->int_status;
}

/**
 * @brief Enable I2C slave clock stretch.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param enable true: Enable, false: Disable.
 */
static inline void i2c_ll_slave_enable_scl_stretch(i2c_dev_t *dev, bool enable)
{
    dev->scl_stretch_conf.slave_scl_stretch_en = enable;
}

/**
 * @brief Clear I2C clock stretch status
 *
 * @param dev Beginning address of the peripheral registers
 */
__attribute__((always_inline))
static inline void i2c_ll_slave_clear_stretch(i2c_dev_t *dev)
{
    dev->scl_stretch_conf.slave_scl_stretch_clr = 1;
}

/**
 * @brief Check if i2c command is done.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  cmd_idx The index of the command register, must be less than 8
 *
 * @return True if the `cmd_idx` command is done. Otherwise false.
 */
__attribute__((always_inline))
static inline bool i2c_ll_master_is_cmd_done(i2c_dev_t *hw, int cmd_idx)
{
    return hw->command[cmd_idx].command_done;
}

/**
 * @brief Calculate SCL timeout us to reg value
 *
 * @param timeout_us timeout value in us
 * @param src_clk_hz source clock frequency
 * @return uint32_t reg value
 */
static inline uint32_t i2c_ll_calculate_timeout_us_to_reg_val(uint32_t src_clk_hz, uint32_t timeout_us)
{
    uint32_t clk_cycle_num_per_us = src_clk_hz / (1 * 1000 * 1000);
    return 31 - __builtin_clz(clk_cycle_num_per_us * timeout_us);
}

//////////////////////////////////////////Deprecated Functions//////////////////////////////////////////////////////////
/////////////////////////////The following functions are only used by the legacy driver/////////////////////////////////
/////////////////////////////They might be removed in the next major release (ESP-IDF 6.0)//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// I2C master TX interrupt bitmap
#define I2C_LL_MASTER_TX_INT          (I2C_NACK_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// I2C master RX interrupt bitmap
#define I2C_LL_MASTER_RX_INT          (I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// I2C slave TX interrupt bitmap
#define I2C_LL_SLAVE_TX_INT           (I2C_TXFIFO_WM_INT_ENA_M)
// I2C slave RX interrupt bitmap
#define I2C_LL_SLAVE_RX_INT           (I2C_RXFIFO_WM_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M)
// I2C max timeout value
#define I2C_LL_MAX_TIMEOUT I2C_TIME_OUT_VALUE

#define I2C_LL_INTR_MASK          (0x3fff) /*!< I2C all interrupt bitmap */

/**
 * @brief I2C interrupt event
 */
typedef enum {
    I2C_INTR_EVENT_ERR,
    I2C_INTR_EVENT_ARBIT_LOST,   /*!< I2C arbition lost event */
    I2C_INTR_EVENT_NACK,         /*!< I2C NACK event */
    I2C_INTR_EVENT_TOUT,         /*!< I2C time out event */
    I2C_INTR_EVENT_END_DET,      /*!< I2C end detected event */
    I2C_INTR_EVENT_TRANS_DONE,   /*!< I2C trans done event */
    I2C_INTR_EVENT_RXFIFO_FULL,  /*!< I2C rxfifo full event */
    I2C_INTR_EVENT_TXFIFO_EMPTY, /*!< I2C txfifo empty event */
} i2c_intr_event_t;

/**
 * @brief  Configure I2C SCL timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  high_period The I2C SCL height period (in core clock cycle, height_period > 2)
 * @param  low_period The I2C SCL low period (in core clock cycle, low_period > 1)
 * @param  wait_high_period The I2C SCL wait rising edge period.
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_clk_timing(i2c_dev_t *hw, int high_period, int low_period, int wait_high_period)
{
    hw->scl_low_period.scl_low_period = low_period;
    hw->scl_high_period.scl_high_period = high_period;
    hw->scl_high_period.scl_wait_high_period = wait_high_period;
}

/**
 * @brief  Get I2C SCL timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  high_period Pointer to accept the SCL high period
 * @param  low_period Pointer to accept the SCL low period
 *
 * @return None
 */
static inline void i2c_ll_get_scl_clk_timing(i2c_dev_t *hw, int *high_period, int *low_period, int *wait_high_period)
{
    *high_period = hw->scl_high_period.scl_high_period;
    *wait_high_period = hw->scl_high_period.scl_wait_high_period;
    *low_period = hw->scl_low_period.scl_low_period;
}

/**
 * @brief  Get I2C master interrupt event
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_get_event(i2c_dev_t *hw, i2c_intr_event_t *event)
{
    i2c_int_status_reg_t int_sts;
    int_sts.val = hw->int_status.val;

    if (int_sts.arbitration_lost_int_st) {
        *event = I2C_INTR_EVENT_ARBIT_LOST;
    } else if (int_sts.nack_int_st) {
        *event = I2C_INTR_EVENT_NACK;
    } else if (int_sts.time_out_int_st) {
        *event = I2C_INTR_EVENT_TOUT;
    } else if (int_sts.end_detect_int_st) {
        *event = I2C_INTR_EVENT_END_DET;
    } else if (int_sts.trans_complete_int_st) {
        *event = I2C_INTR_EVENT_TRANS_DONE;
    } else {
        *event = I2C_INTR_EVENT_ERR;
    }
}

/**
 * @brief  Get I2C slave interrupt event
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_slave_get_event(i2c_dev_t *hw, i2c_intr_event_t *event)
{
    typeof(hw->int_status) int_sts;
    int_sts.val = hw->int_status.val;
    if (int_sts.txfifo_wm_int_st) {
        *event = I2C_INTR_EVENT_TXFIFO_EMPTY;
    } else if (int_sts.trans_complete_int_st) {
        *event = I2C_INTR_EVENT_TRANS_DONE;
    } else if (int_sts.rxfifo_wm_int_st) {
        *event = I2C_INTR_EVENT_RXFIFO_FULL;
    } else {
        *event = I2C_INTR_EVENT_ERR;
    }
}

/**
 * @brief  Enable I2C master TX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_enable_tx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = UINT32_MAX;
    hw->int_ena.val =  I2C_LL_MASTER_TX_INT;
}

/**
 * @brief  Enable I2C master RX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_enable_rx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = UINT32_MAX;
    hw->int_ena.val = I2C_LL_MASTER_RX_INT;
}

/**
 * @brief  Disable I2C master TX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_disable_tx_it(i2c_dev_t *hw)
{
    hw->int_ena.val &= (~I2C_LL_MASTER_TX_INT);
}

/**
 * @brief  Disable I2C master RX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_master_disable_rx_it(i2c_dev_t *hw)
{
    hw->int_ena.val &= (~I2C_LL_MASTER_RX_INT);
}

/**
 * @brief
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_slave_enable_tx_it(i2c_dev_t *hw)
{
    hw->int_ena.val |= I2C_LL_SLAVE_TX_INT;
}

/**
 * @brief Enable I2C slave RX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_slave_enable_rx_it(i2c_dev_t *hw)
{
    hw->int_ena.val |= I2C_LL_SLAVE_RX_INT;
}

/**
 * @brief Disable I2C slave TX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_slave_disable_tx_it(i2c_dev_t *hw)
{
    hw->int_ena.val &= (~I2C_LL_SLAVE_TX_INT);
}

/**
 * @brief  Disable I2C slave RX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_slave_disable_rx_it(i2c_dev_t *hw)
{
    hw->int_ena.val &= (~I2C_LL_SLAVE_RX_INT);
}

/**
 * @brief  Configure I2C SCL timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  height_period The I2C SCL height period (in core clock cycle, height_period > 2)
 * @param  low_period The I2C SCL low period (in core clock cycle, low_period > 1)
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_timing(i2c_dev_t *hw, int height_period, int low_period)
{
    hw->scl_low_period.scl_low_period = low_period - 1;
    hw->scl_high_period.scl_high_period = height_period - 10;
    hw->scl_high_period.scl_wait_high_period = height_period - hw->scl_high_period.scl_high_period;
}

/**
 * @brief  Get the I2C data mode
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  tx_mode Pointer to accept the received bytes mode
 * @param  rx_mode Pointer to accept the sended bytes mode
 *
 * @return None
 */
static inline void i2c_ll_get_data_mode(i2c_dev_t *hw, i2c_trans_mode_t *tx_mode, i2c_trans_mode_t *rx_mode)
{
    *tx_mode = (i2c_trans_mode_t)(hw->ctr.tx_lsb_first);
    *rx_mode = (i2c_trans_mode_t)(hw->ctr.rx_lsb_first);
}

/**
 * @brief  Get I2C SCL timing configuration
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  high_period Pointer to accept the SCL high period
 * @param  low_period Pointer to accept the SCL low period
 *
 * @return None
 */
static inline void i2c_ll_get_scl_timing(i2c_dev_t *hw, int *high_period, int *low_period)
{
    *high_period = hw->scl_high_period.scl_high_period + hw->scl_high_period.scl_wait_high_period;
    *low_period = hw->scl_low_period.scl_low_period + 1;
}

#ifdef __cplusplus
}
#endif

#endif  // SOC_I2C_SUPPORTED
