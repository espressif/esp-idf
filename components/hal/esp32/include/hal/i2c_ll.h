/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I2C register operations

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "soc/i2c_periph.h"
#include "soc/i2c_struct.h"
#include "soc/clk_tree_defs.h"
#include "hal/i2c_types.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C hardware cmd register fields.
 */
typedef union {
    struct {
        uint32_t byte_num:    8,
                 ack_en:      1,
                 ack_exp:     1,
                 ack_val:     1,
                 op_code:     3,
                 reserved14: 17,
                 done:        1;
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
    I2C_INTR_NACK = (1 << 10),
    I2C_INTR_TIMEOUT = (1 << 8),
    I2C_INTR_MST_COMPLETE = (1 << 7),
    I2C_INTR_ARBITRATION = (1 << 5),
    I2C_INTR_END_DETECT = (1 << 3),
    I2C_INTR_ST_TO = (1 << 13),
} i2c_ll_master_intr_t;


typedef enum {
    I2C_INTR_TXFIFO_WM = (1 << 1),
    I2C_INTR_RXFIFO_WM = (1 << 0),
    I2C_INTR_SLV_COMPLETE = (1 << 7),
    I2C_INTR_START = (1 << 15),
} i2c_ll_slave_intr_t;

// Get the I2C hardware instance
#define I2C_LL_GET_HW(i2c_num)        (((i2c_num) == 0) ? &I2C0 : &I2C1)
#define I2C_LL_MASTER_EVENT_INTR    (I2C_ACK_ERR_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
#define I2C_LL_SLAVE_EVENT_INTR     (I2C_RXFIFO_FULL_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_TXFIFO_EMPTY_INT_ENA_M)

/**
 * @brief  Calculate I2C bus frequency
 *
 * @param  source_clk I2C source clock
 * @param  bus_freq I2C bus frequency
 * @param  clk_cal Pointer to accept the clock configuration
 *
 * @return None
 */
static inline void i2c_ll_cal_bus_clk(uint32_t source_clk, uint32_t bus_freq, i2c_hal_clk_config_t *clk_cal)
{
    uint32_t half_cycle = source_clk / bus_freq / 2;
    clk_cal->scl_low = half_cycle;
    clk_cal->scl_high = half_cycle;
    clk_cal->sda_hold = half_cycle / 2;
    clk_cal->sda_sample = clk_cal->scl_high / 2;
    clk_cal->setup = half_cycle;
    clk_cal->hold = half_cycle;
    clk_cal->tout = half_cycle * 20; //default we set the timeout value to 10 bus cycles.
}

/**
 * @brief  Configure the I2C bus timing related register.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  bus_cfg Pointer to the data structure holding the register configuration.
 *
 * @return None
 */
static inline void i2c_ll_set_bus_timing(i2c_dev_t *hw, i2c_hal_clk_config_t *bus_cfg)
{
    /* SCL period. According to the TRM, we should always subtract 1 to SCL low period */
    assert(bus_cfg->scl_low > 0);
    hw->scl_low_period.period = bus_cfg->scl_low - 1;
    /* Still according to the TRM, if filter is not enbled, we have to subtract 7,
     * if SCL filter is enabled, we have to subtract:
     *   8 if SCL filter is between 0 and 2 (included)
     *   6 + SCL threshold if SCL filter is between 3 and 7 (included)
     * to SCL high period */
    uint16_t scl_high = bus_cfg->scl_high;
    /* In the "worst" case, we will subtract 13, make sure the result will still be correct */
    assert(scl_high > 13);
    if (hw->scl_filter_cfg.en) {
        if (hw->scl_filter_cfg.thres <= 2) {
            scl_high -= 8;
        } else {
            assert(hw->scl_filter_cfg.thres <= 7);
            scl_high -= hw->scl_filter_cfg.thres + 6;
        }
    } else {
        scl_high -= 7;
    }
    hw->scl_high_period.period = scl_high;
    //sda sample
    hw->sda_hold.time = bus_cfg->sda_hold;
    hw->sda_sample.time = bus_cfg->sda_sample;
    //setup
    hw->scl_rstart_setup.time = bus_cfg->setup;
    hw->scl_stop_setup.time = bus_cfg->setup;
    //hold
    hw->scl_start_hold.time = bus_cfg->hold;
    hw->scl_stop_hold.time = bus_cfg->hold;
    hw->timeout.tout = bus_cfg->tout;
}

/**
 * @brief  Reset I2C txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
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
static inline void i2c_ll_rxfifo_rst(i2c_dev_t *hw)
{
    hw->fifo_conf.rx_fifo_rst = 1;
    hw->fifo_conf.rx_fifo_rst = 0;
}

/**
 * @brief  Configure I2C SCL timing
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  hight_period The I2C SCL hight period (in APB cycle)
 * @param  low_period The I2C SCL low period (in APB cycle)
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_timing(i2c_dev_t *hw, int hight_period, int low_period)
{
    hw->scl_low_period.period = low_period;
    hw->scl_high_period.period = hight_period;
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
static inline void i2c_ll_set_fifo_mode(i2c_dev_t *hw, bool fifo_mode_en)
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
    hw->slave_addr.addr = slave_addr;
    hw->slave_addr.en_10bit = addr_10bit_en;
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
static inline void i2c_ll_write_cmd_reg(i2c_dev_t *hw, i2c_ll_hw_cmd_t cmd, int cmd_idx)
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
static inline void i2c_ll_set_start_timing(i2c_dev_t *hw, int start_setup, int start_hold)
{
    hw->scl_rstart_setup.time = start_setup;
    hw->scl_start_hold.time = start_hold;
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
static inline void i2c_ll_set_stop_timing(i2c_dev_t *hw, int stop_setup, int stop_hold)
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
    hw->fifo_conf.tx_fifo_empty_thrhd = empty_thr;
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
    hw->fifo_conf.rx_fifo_full_thrhd = full_thr;
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
    *tx_mode = hw->ctr.tx_lsb_first;
    *rx_mode = hw->ctr.rx_lsb_first;
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
 * @brief  Check if the I2C bus is busy
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return True if I2C state machine is busy, else false will be returned
 */
static inline bool i2c_ll_is_bus_busy(i2c_dev_t *hw)
{
    return hw->status_reg.bus_busy;
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
    *length = SOC_I2C_FIFO_LEN - hw->status_reg.tx_fifo_cnt;
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
static inline void i2c_ll_trans_start(i2c_dev_t *hw)
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
    *hold_time = hw->scl_start_hold.time;
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
    *high_period = hw->scl_high_period.period;
    *low_period = hw->scl_low_period.period;
}

/**
 * @brief  Write the I2C hardware txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs to be writen
 *
 * @return None.
 */
__attribute__((always_inline))
static inline void i2c_ll_write_txfifo(i2c_dev_t *hw, const uint8_t *ptr, uint8_t len)
{
    uint32_t fifo_addr = (hw == &I2C0) ? 0x6001301c : 0x6002701c;
    for(int i = 0; i < len; i++) {
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
    for(int i = 0; i < len; i++) {
        ptr[i] = HAL_FORCE_READ_U32_REG_FIELD(hw->fifo_data, data);
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
static inline void i2c_ll_set_filter(i2c_dev_t *hw, uint8_t filter_num)
{
    if(filter_num > 0) {
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
static inline void i2c_ll_get_filter(i2c_dev_t *hw, uint8_t *filter_conf)
{
    *filter_conf = hw->sda_filter_cfg.thres;
}

/**
 * @brief Reste I2C master FSM. When the master FSM is stuck, call this function to reset the FSM
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_fsm_rst(i2c_dev_t *hw)
{
   ;//ESP32 do not support
}

/**
 * @brief Clear I2C bus, when the slave is stuck in a deadlock and keeps pulling the bus low,
 *        master can controls the SCL bus to generate 9 CLKs.
 *
 * Note: The master cannot detect if deadlock happens, but when the scl_st_to interrupt is generated, a deadlock may occur.
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_clr_bus(i2c_dev_t *hw)
{
    ;//ESP32 do not support
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
    ;//Not support on ESP32
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
 * @brief  Update I2C configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
__attribute__((always_inline))
static inline void i2c_ll_update(i2c_dev_t *hw)
{
    ;// ESP32 do not support
}

/**
 * @brief Set whether slave should auto start, or only start with start signal from master
 *
 * @param hw Beginning address of the peripheral registers
 * @param slv_ex_auto_en 1 if slave auto start data transaction, otherwise, 0.
 */
static inline void i2c_ll_slave_tx_auto_start_en(i2c_dev_t *hw, bool slv_ex_auto_en)
{
    ;// ESP32 do not support
}

/**
 * @brief Get I2C interrupt status register address
 */
static inline volatile void *i2c_ll_get_interrupt_status_reg(i2c_dev_t *dev)
{
    return &dev->int_status;
}

//////////////////////////////////////////Deprecated Functions//////////////////////////////////////////////////////////
/////////////////////////////The following functions are only used by the legacy driver/////////////////////////////////
/////////////////////////////They might be removed in the next major release (ESP-IDF 6.0)//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// I2C master TX interrupt bitmap
#define I2C_LL_MASTER_TX_INT          (I2C_ACK_ERR_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// I2C master RX interrupt bitmap
#define I2C_LL_MASTER_RX_INT          (I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// I2C slave TX interrupt bitmap
#define I2C_LL_SLAVE_TX_INT           (I2C_TXFIFO_EMPTY_INT_ENA_M)
// I2C slave RX interrupt bitmap
#define I2C_LL_SLAVE_RX_INT           (I2C_RXFIFO_FULL_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M)
// I2C max timeout value
#define I2C_LL_MAX_TIMEOUT I2C_TIME_OUT_REG_V

#define I2C_LL_INTR_MASK          (0xffff) /*!< I2C all interrupt bitmap */

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
    *wait_high_period = 0; // Useless
    *high_period = hw->scl_high_period.period;
    *low_period = hw->scl_low_period.period;
}

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
static inline void i2c_ll_set_scl_clk_timing(i2c_dev_t *hw, int high_period, int low_period, int wait_high_period)
{
    (void)wait_high_period;
    hw->scl_low_period.period = low_period;
    hw->scl_high_period.period = high_period;
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
    typeof(hw->int_status) int_sts = hw->int_status;
    if (int_sts.tx_fifo_empty) {
        *event = I2C_INTR_EVENT_TXFIFO_EMPTY;
    } else if (int_sts.trans_complete) {
        *event = I2C_INTR_EVENT_TRANS_DONE;
    } else if (int_sts.rx_fifo_full) {
        *event = I2C_INTR_EVENT_RXFIFO_FULL;
    } else {
        *event = I2C_INTR_EVENT_ERR;
    }
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
    typeof(hw->int_status) int_sts = hw->int_status;
    if (int_sts.arbitration_lost) {
        *event = I2C_INTR_EVENT_ARBIT_LOST;
    } else if (int_sts.ack_err) {
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


#ifdef __cplusplus
}
#endif
