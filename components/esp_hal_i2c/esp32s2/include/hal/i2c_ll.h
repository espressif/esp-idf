/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I2C register operations

#pragma once
#include <stdbool.h>
#include "hal/i2c_periph.h"
#include "soc/i2c_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/system_reg.h"
#include "hal/i2c_types.h"
#include "esp_attr.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_LL_GET(_attr)       I2C_LL_ ## _attr

#define I2C_LL_FIFO_LEN         32 /*!< I2C hardware FIFO depth */
#define I2C_LL_CMD_REG_NUM      16 /*!< Number of I2C command registers */

// FSM_RST only resets the FSM, not using it. So I2C_LL_SUPPORT_HW_FSM_RST not defined.
#define I2C_LL_SUPPORT_HW_CLR_BUS            (1)  /*!< Support hardware clear bus */

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
#define I2C_LL_CMD_RESTART    0    /*!<I2C restart command */
#define I2C_LL_CMD_WRITE      1    /*!<I2C write command */
#define I2C_LL_CMD_READ       2    /*!<I2C read command */
#define I2C_LL_CMD_STOP       3    /*!<I2C stop command */
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
} i2c_ll_slave_intr_t;

// Get the I2C hardware instance
#define I2C_LL_GET_HW(i2c_num)        (((i2c_num) == 0) ? &I2C0 : &I2C1)
#define I2C_LL_MASTER_EVENT_INTR    (I2C_NACK_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
#define I2C_LL_SLAVE_EVENT_INTR     (I2C_TRANS_COMPLETE_INT_ENA_M|I2C_TXFIFO_WM_INT_ENA_M|I2C_RXFIFO_WM_INT_ENA_M | I2C_SLAVE_STRETCH_INT_ENA_M)
#define I2C_LL_SLAVE_RX_EVENT_INTR  (I2C_TRANS_COMPLETE_INT_ENA_M | I2C_RXFIFO_WM_INT_ENA_M | I2C_SLAVE_STRETCH_INT_ENA_M)
#define I2C_LL_SLAVE_TX_EVENT_INTR  (I2C_TXFIFO_WM_INT_ENA_M)
#define I2C_LL_RESET_SLV_SCL_PULSE_NUM_DEFAULT   (9)
#define I2C_LL_SCL_WAIT_US_VAL_DEFAULT   (2000)  // 2000 is not default value on esp32s2, but 0 is not good to be default

#define I2C_LL_STRETCH_PROTECT_TIME  (0x3ff)

/**
 * @brief  Calculate I2C bus frequency
 *
 * @param  source_clk I2C source clock
 * @param  bus_freq I2C bus frequency
 * @param  clk_cal Pointer to accept the clock configuration
 *
 * @return None
 */
static inline void i2c_ll_master_cal_bus_clk(uint32_t source_clk, uint32_t bus_freq, i2c_hal_clk_config_t *clk_cal)
{
    uint32_t half_cycle = source_clk / bus_freq / 2;
    //SCL
    clk_cal->scl_low = half_cycle;
    // default, scl_wait_high < scl_high
    clk_cal->scl_high = half_cycle / 2 + 2;
    clk_cal->scl_wait_high = half_cycle - clk_cal->scl_high;
    clk_cal->sda_hold = half_cycle / 2;
    // scl_wait_high < sda_sample <= scl_high
    clk_cal->sda_sample = half_cycle / 2 - 1;
    clk_cal->setup = half_cycle;
    clk_cal->hold = half_cycle;
    //default we set the timeout value to 10 bus cycles
    clk_cal->tout = half_cycle * 20;
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
    //scl period
    hw->scl_low_period.period = bus_cfg->scl_low - 1;
    hw->scl_high_period.period = bus_cfg->scl_high;
    hw->scl_high_period.scl_wait_high_period = bus_cfg->scl_wait_high;
    //sda sample
    hw->sda_hold.time = bus_cfg->sda_hold;
    hw->sda_sample.time = bus_cfg->sda_sample;
    //setup
    hw->scl_rstart_setup.time = bus_cfg->setup;
    hw->scl_stop_setup.time = bus_cfg->setup;
    //hold
    hw->scl_start_hold.time = bus_cfg->hold - 1;
    hw->scl_stop_hold.time = bus_cfg->hold;
    hw->timeout.tout = bus_cfg->tout;
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
    // Not supported on ESP32S2
}

/**
 * @brief Set fractional divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param div_a The denominator of the frequency divider factor of the i2c function clock
 * @param div_b The numerator of the frequency divider factor of the i2c function clock.
 */
static inline void i2c_ll_master_get_fractional_divider(i2c_dev_t *hw, uint32_t *div_a, uint32_t *div_b)
{
    // Not supported on ESP32S2
}

/**
 * @brief Get clock configurations from registers
 *
 * @param hw Beginning address of the peripheral registers
 * @param div_num div_num
 * @param clk_sel clk_sel
 * @param clk_active clk_active
 */
static inline void i2c_ll_master_save_clock_configurations(i2c_dev_t *hw, uint32_t *div_num, uint8_t *clk_sel, uint8_t *clk_active)
{
    // Not supported on ESP32S2
}

/**
 * @brief Get clock configurations from registers
 *
 * @param hw Beginning address of the peripheral registers
 * @param div_num div_num
 * @param clk_sel clk_sel
 * @param clk_active clk_active
 */
static inline void i2c_ll_master_restore_clock_configurations(i2c_dev_t *hw, uint32_t div_num, uint8_t clk_sel, uint8_t clk_active)
{
    // Not supported on ESP32S2
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
static inline void i2c_ll_enable_fifo_mode(i2c_dev_t *hw, bool fifo_mode_en)
{
    hw->fifo_conf.nonfifo_en = fifo_mode_en ? 0 : 1;
}

/**
 * @brief  Configure I2C timeout
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  tout_num The I2C timeout value needs to be set (in APB cycle)
 *
 * @return None
 */
static inline void i2c_ll_set_tout(i2c_dev_t *hw, int tout)
{
    hw->timeout.tout = tout;
    hw->timeout.time_out_en = tout > 0;
}

/**
 * @brief  Enable the I2C slave to respond to broadcast address
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  broadcast_en Set true to enable broadcast, else, set it false
 *
 * @return None
 */
static inline void i2c_ll_slave_broadcast_enable(i2c_dev_t *hw, bool broadcast_en)
{
    // Not supported on esp32s2
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
    switch (hw->status_reg.stretch_cause) {
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
    hw->slave_addr.en_10bit = addr_10bit_en;
    if (addr_10bit_en) {
        uint16_t addr_14_7 = (slave_addr & 0xff) << 7;
        uint8_t addr_6_0 = ((slave_addr & 0x300) >> 8) | 0x78;
        hw->slave_addr.addr = addr_14_7 | addr_6_0;
    } else {
        hw->slave_addr.addr = slave_addr;
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
 * @param  start_setup The start condition setup period (in APB cycle)
 * @param  start_hold The start condition hold period (in APB cycle)
 *
 * @return None
 */
static inline void i2c_ll_master_set_start_timing(i2c_dev_t *hw, int start_setup, int start_hold)
{
    hw->scl_rstart_setup.time = start_setup;
    hw->scl_start_hold.time = start_hold - 1;
}

/**
 * @brief Configure I2C stop timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  stop_setup The stop condition setup period (in APB cycle)
 * @param  stop_hold The stop condition hold period (in APB cycle)
 *
 * @return None
 */
static inline void i2c_ll_master_set_stop_timing(i2c_dev_t *hw, int stop_setup, int stop_hold)
{
    hw->scl_stop_setup.time = stop_setup;
    hw->scl_stop_hold.time = stop_hold;
}

/**
 * @brief Configure I2C stop timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  sda_sample The SDA sample time (in APB cycle)
 * @param  sda_hold The SDA hold time (in APB cycle)
 *
 * @return None
 */
static inline void i2c_ll_set_sda_timing(i2c_dev_t *hw, int sda_sample, int sda_hold)
{
    hw->sda_hold.time = sda_hold;
    hw->sda_sample.time = sda_sample;
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
    hw->fifo_conf.fifo_prt_en = 1;
    hw->fifo_conf.tx_fifo_wm_thrhd = empty_thr;
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
    hw->ctr.rx_full_ack_level = 0;
    hw->fifo_conf.rx_fifo_wm_thrhd = full_thr;
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
    *sda_hold = hw->sda_hold.time;
    *sda_sample = hw->sda_sample.time;
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
    return hw->date;
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
    return hw->status_reg.bus_busy;
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
    *length = hw->status_reg.rx_fifo_cnt;
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
    *length = (hw->status_reg.tx_fifo_cnt >= I2C_LL_GET(FIFO_LEN)) ? 0 : (I2C_LL_GET(FIFO_LEN) - hw->status_reg.tx_fifo_cnt);
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
    *timeout = hw->timeout.tout;
}

/**
 * @brief  Start I2C transfer
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_start_trans(i2c_dev_t *hw)
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
    *setup_time = hw->scl_rstart_setup.time;
    *hold_time = hw->scl_start_hold.time + 1;
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
    *setup_time = hw->scl_stop_setup.time;
    *hold_time = hw->scl_stop_hold.time;
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
    uint32_t fifo_addr = (hw == &I2C0) ? 0x6001301c : 0x6002701c;
    for (int i = 0; i < len; i++) {
        WRITE_PERI_REG(fifo_addr, ptr[i]);
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
    uint32_t fifo_addr = (hw == &I2C0) ? 0x6001301c : 0x6002701c;
    for (int i = 0; i < len; i++) {
        ptr[i] = READ_PERI_REG(fifo_addr);
    }
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
        hw->scl_filter_cfg.thres = filter_num;
        hw->sda_filter_cfg.thres = filter_num;
        hw->scl_filter_cfg.en = 1;
        hw->sda_filter_cfg.en = 1;
    } else {
        hw->scl_filter_cfg.en = 0;
        hw->sda_filter_cfg.en = 0;
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
    *filter_conf = hw->sda_filter_cfg.thres;
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
 * @param  enable True to start the state machine, otherwise, false
 *
 * @return None
 */
static inline void i2c_ll_master_clr_bus(i2c_dev_t *hw, uint32_t slave_pulses, bool enable)
{
    hw->scl_sp_conf.scl_rst_slv_num = slave_pulses;
    hw->scl_sp_conf.scl_rst_slv_en = 0;
    hw->scl_sp_conf.scl_rst_slv_en = 1;
}

/**
 * @brief Get the clear bus state
 *
 * @param hw Beginning address of the peripheral registers
 *
 * @return true: the clear bus not finish, otherwise, false.
 */
static inline bool i2c_ll_master_is_bus_clear_done(i2c_dev_t *hw)
{
    return false; // not supported on esp32s2
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
    // src_clk : (1) for APB_CLK, (0) for REF_CLK
    hw->ctr.ref_always_on = (src_clk == I2C_CLK_SRC_REF_TICK) ? 0 : 1;
}

/**
 * @brief Enable I2C peripheral controller clock
 *
 * @param dev Peripheral instance address
 * @param en True to enable, False to disable
 */
static inline void i2c_ll_enable_controller_clock(i2c_dev_t *hw, bool en)
{
    (void)hw;
    (void)en;
}

/**
 * @brief Set the I2C bus mode (Master or Slave)
 *
 * @param hw Pointer to the I2C hardware register structure.
 * @param mode The desired I2C bus mode (Master or Slave).
 */
static inline void i2c_ll_set_mode(i2c_dev_t *hw, i2c_bus_mode_t mode)
{
    hw->ctr.ms_mode = (mode == I2C_BUS_MODE_MASTER) ? 1 : 0;
}

/**
 * @brief Enable or disable open-drain mode for I2C pins
 *
 * @param hw Pointer to the I2C hardware register structure.
 * @param enable_od Boolean flag to enable or disable open-drain mode:
 */
static inline void i2c_ll_enable_pins_open_drain(i2c_dev_t *hw, bool enable_od)
{
    hw->ctr.sda_force_out = enable_od;
    hw->ctr.scl_force_out = enable_od;
}

/**
 * @brief Enable or disable arbitration for I2C communication.
 *
 * @param hw Pointer to the I2C hardware instance.
 * @param enable_arbi Boolean flag to enable (true) or disable (false) arbitration.
 */
static inline void i2c_ll_enable_arbitration(i2c_dev_t *hw, bool enable_arbi)
{
    hw->ctr.arbitration_en = enable_arbi;
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
    //Open-drain output via GPIO
    ctrl_reg.sda_force_out = 1;
    ctrl_reg.scl_force_out = 1;
    hw->ctr.val = ctrl_reg.val;
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
    ;// ESP32S2 do not support
}

/**
 * @brief Enable the bus clock for I2C module
 *
 * @param i2c_port I2C port id
 * @param enable true to enable, false to disable
 */
static inline void i2c_ll_enable_bus_clock(int i2c_port, bool enable)
{
    if (i2c_port == 0) {
        uint32_t reg_val = READ_PERI_REG(DPORT_PERIP_CLK_EN0_REG);
        reg_val &= ~DPORT_I2C_EXT0_CLK_EN_M;
        reg_val |= enable << DPORT_I2C_EXT0_CLK_EN_S;
        WRITE_PERI_REG(DPORT_PERIP_CLK_EN0_REG, reg_val);
    } else if (i2c_port == 1) {
        uint32_t reg_val = READ_PERI_REG(DPORT_PERIP_CLK_EN0_REG);
        reg_val &= ~DPORT_I2C_EXT1_CLK_EN_M;
        reg_val |= enable << DPORT_I2C_EXT1_CLK_EN_S;
        WRITE_PERI_REG(DPORT_PERIP_CLK_EN0_REG, reg_val);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i2c_ll_enable_bus_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i2c_ll_enable_bus_clock(__VA_ARGS__);} while(0)

/**
 * @brief Reset the I2C module
 *
 * @param i2c_port Group ID
 */
static inline void i2c_ll_reset_register(int i2c_port)
{
    if (i2c_port == 0) {
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, DPORT_I2C_EXT0_RST_M);
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, 0);
    } else if (i2c_port == 1) {
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, DPORT_I2C_EXT1_RST_M);
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, 0);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i2c_ll_reset_register(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i2c_ll_reset_register(__VA_ARGS__);} while(0)

/**
 * @brief Enable I2C slave to automatically send data when addressed by the master
 *
 * @param hw Beginning address of the peripheral registers
 * @param slv_ex_auto_en 1 if slave auto start data transaction, otherwise, 0.
 */
static inline void i2c_ll_slave_enable_auto_start(i2c_dev_t *hw, bool slv_ex_auto_en)
{
    ;// ESP32-S2 do not support
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
 * @brief Set I2C clock stretch protect num
 *
 * @param dev Beginning address of the peripheral registers
 */
static inline void i2c_ll_slave_set_stretch_protect_num(i2c_dev_t *dev, uint32_t protect_num)
{
    dev->scl_stretch_conf.stretch_protect_num = protect_num;
}

/**
 * @brief Check if i2c command is done.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  cmd_idx The index of the command register, must be less than 16
 *
 * @return True if the `cmd_idx` command is done. Otherwise false.
 */
__attribute__((always_inline))
static inline bool i2c_ll_master_is_cmd_done(i2c_dev_t *hw, int cmd_idx)
{
    return hw->command[cmd_idx].done;
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
    return clk_cycle_num_per_us * timeout_us;
}

/**
 * @brief Get status of i2c slave
 *
 * @param Beginning address of the peripheral registers
 * @return i2c slave working status
 */
__attribute__((always_inline))
static inline i2c_slave_read_write_status_t i2c_ll_slave_get_read_write_status(i2c_dev_t *hw)
{
    return (hw->status_reg.slave_rw == 0) ? I2C_SLAVE_WRITE_BY_MASTER : I2C_SLAVE_READ_BY_MASTER;
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
#define I2C_LL_MAX_TIMEOUT I2C_TIME_OUT_REG_V
// I2C max timeout period in clock cycles
#define I2C_LL_MAX_TIMEOUT_PERIOD    (I2C_LL_MAX_TIMEOUT)

#define I2C_LL_INTR_MASK          (0x1ffff) /*!< I2C all interrupt bitmap */

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
 * @param  high_period The I2C SCL high period (in core clock cycle, hight_period > 2)
 * @param  low_period The I2C SCL low period (in core clock cycle, low_period > 1)
 * @param  wait_high_period The I2C SCL wait rising edge period.
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_clk_timing(i2c_dev_t *hw, int high_period, int low_period, int wait_high_period)
{
    hw->scl_low_period.period = low_period;
    hw->scl_high_period.period = high_period;
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
    *high_period = hw->scl_high_period.period;
    *wait_high_period = hw->scl_high_period.scl_wait_high_period;
    *low_period = hw->scl_low_period.period;
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
    typeof(hw->int_status) int_sts;
    int_sts.val = hw->int_status.val;
    if (int_sts.arbitration_lost) {
        *event = I2C_INTR_EVENT_ARBIT_LOST;
    } else if (int_sts.nack) {
        *event = I2C_INTR_EVENT_NACK;
    } else if (int_sts.time_out) {
        *event = I2C_INTR_EVENT_TOUT;
    } else if (int_sts.end_detect) {
        *event = I2C_INTR_EVENT_END_DET;
    } else if (int_sts.trans_complete) {
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
    if (int_sts.tx_fifo_wm) {
        *event = I2C_INTR_EVENT_TXFIFO_EMPTY;
    } else if (int_sts.trans_complete) {
        *event = I2C_INTR_EVENT_TRANS_DONE;
    } else if (int_sts.rx_fifo_wm) {
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
 * @brief  Enable I2C slave TX interrupt
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
 * @param  hight_period The I2C SCL high period (in APB cycle, hight_period > 2)
 * @param  low_period The I2C SCL low period (in APB cycle, low_period > 1)
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_timing(i2c_dev_t *hw, int hight_period, int low_period)
{
    hw->scl_low_period.period = low_period - 1;
    hw->scl_high_period.period = hight_period / 2 + 2;
    hw->scl_high_period.scl_wait_high_period = hight_period - hw->scl_high_period.period;
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
    *high_period = hw->scl_high_period.period + hw->scl_high_period.scl_wait_high_period;
    *low_period = hw->scl_low_period.period + 1;
}

#ifdef __cplusplus
}
#endif
