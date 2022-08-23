/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I2C register operations

#pragma once

#include "hal/misc.h"
#include "soc/i2c_periph.h"
#include "soc/soc_caps.h"
#include "soc/i2c_struct.h"
#include "hal/i2c_types.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/clk_tree_defs.h"
#include "esp_rom_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_LL_INTR_MASK          (0x3fff) /*!< I2C all interrupt bitmap */

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
} i2c_hw_cmd_t;

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
 * @brief Data structure for calculating I2C bus timing.
 */
typedef struct {
    uint16_t clkm_div;          /*!< I2C core clock devider */
    uint16_t scl_low;           /*!< I2C scl low period */
    uint16_t scl_high;          /*!< I2C scl hight period */
    uint16_t scl_wait_high;     /*!< I2C scl wait_high period */
    uint16_t sda_hold;          /*!< I2C scl low period */
    uint16_t sda_sample;        /*!< I2C sda sample time */
    uint16_t setup;             /*!< I2C start and stop condition setup period */
    uint16_t hold;              /*!< I2C start and stop condition hold period  */
    uint16_t tout;              /*!< I2C bus timeout period */
} i2c_clk_cal_t;

// I2C operation mode command
#define I2C_LL_CMD_RESTART    6    /*!<I2C restart command */
#define I2C_LL_CMD_WRITE      1    /*!<I2C write command */
#define I2C_LL_CMD_READ       3    /*!<I2C read command */
#define I2C_LL_CMD_STOP       2    /*!<I2C stop command */
#define I2C_LL_CMD_END        4    /*!<I2C end command */

// Get the I2C hardware instance
#define I2C_LL_GET_HW(i2c_num)        (&I2C0)
// Get the I2C hardware FIFO address
#define I2C_LL_GET_FIFO_ADDR(i2c_num) (I2C_DATA_APB_REG(i2c_num))
// I2C master TX interrupt bitmap
#define I2C_LL_MASTER_TX_INT          (I2C_NACK_INT_ENA_M|I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// I2C master RX interrupt bitmap
#define I2C_LL_MASTER_RX_INT          (I2C_TIME_OUT_INT_ENA_M|I2C_TRANS_COMPLETE_INT_ENA_M|I2C_ARBITRATION_LOST_INT_ENA_M|I2C_END_DETECT_INT_ENA_M)
// delay time after rtc_clk swiching on
#define DELAY_RTC_CLK_SWITCH          (5)
// I2C max timeout value
#define I2C_LL_MAX_TIMEOUT I2C_TIME_OUT_VALUE

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
static inline void i2c_ll_cal_bus_clk(uint32_t source_clk, uint32_t bus_freq, i2c_clk_cal_t *clk_cal)
{
    uint32_t clkm_div = source_clk / (bus_freq * 1024) +1;
    uint32_t sclk_freq = source_clk / clkm_div;
    uint32_t half_cycle = sclk_freq / bus_freq / 2;
    //SCL
    clk_cal->clkm_div = clkm_div;
    clk_cal->scl_low = half_cycle;
    // default, scl_wait_high < scl_high
    // Make 80KHz as a boundary here, because when working at lower frequency, too much scl_wait_high will faster the frequency
    // according to some hardware behaviors.
    clk_cal->scl_wait_high = (bus_freq >= 80*1000) ? (half_cycle / 2 - 2) : (half_cycle / 4);
    clk_cal->scl_high = half_cycle - clk_cal->scl_wait_high;
    clk_cal->sda_hold = half_cycle / 4;
    clk_cal->sda_sample = half_cycle / 2 + clk_cal->scl_wait_high;
    clk_cal->setup = half_cycle;
    clk_cal->hold = half_cycle;
    //default we set the timeout value to about 10 bus cycles
    // log(20*half_cycle)/log(2) = log(half_cycle)/log(2) +  log(20)/log(2)
    clk_cal->tout = (int)(sizeof(half_cycle) * 8 - __builtin_clz(5 * half_cycle)) + 2;
}

/**
 * @brief  Update I2C configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_update(i2c_dev_t *hw)
{
    hw->ctr.conf_upgate = 1;
}

/**
 * @brief  Configure the I2C bus timing related register.
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  bus_cfg Pointer to the data structure holding the register configuration.
 *
 * @return None
 */
static inline void i2c_ll_set_bus_timing(i2c_dev_t *hw, i2c_clk_cal_t *bus_cfg)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clk_conf, sclk_div_num, bus_cfg->clkm_div - 1);
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
 * @param  hight_period The I2C SCL hight period (in core clock cycle, hight_period > 2)
 * @param  low_period The I2C SCL low period (in core clock cycle, low_period > 1)
 *
 * @return None.
 */
static inline void i2c_ll_set_scl_timing(i2c_dev_t *hw, int hight_period, int low_period)
{
    hw->scl_low_period.scl_low_period = low_period - 1;
    hw->scl_high_period.scl_high_period = hight_period - 10;
    hw->scl_high_period.scl_wait_high_period = hight_period - hw->scl_high_period.scl_high_period;
}

/**
 * @brief  Clear I2C interrupt status
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  mask Interrupt mask needs to be cleared
 *
 * @return None
 */
static inline void i2c_ll_clr_intsts_mask(i2c_dev_t *hw, uint32_t mask)
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
static inline uint32_t i2c_ll_get_intsts_mask(i2c_dev_t *hw)
{
    return hw->int_status.val;
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
 * @param  tout_num The I2C timeout value needs to be set (2^tout in core clock cycle)
 *
 * @return None
 */
static inline void i2c_ll_set_tout(i2c_dev_t *hw, int tout)
{
    hw->to.time_out_value = tout;
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
static inline void i2c_ll_write_cmd_reg(i2c_dev_t *hw, i2c_hw_cmd_t cmd, int cmd_idx)
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
static inline void i2c_ll_set_start_timing(i2c_dev_t *hw, int start_setup, int start_hold)
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
static inline void i2c_ll_set_stop_timing(i2c_dev_t *hw, int stop_setup, int stop_hold)
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
static inline uint32_t i2c_ll_get_rxfifo_cnt(i2c_dev_t *hw)
{
    return hw->sr.rxfifo_cnt;
}

/**
 * @brief  Get I2C txFIFO writable length
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return TxFIFO writable length
 */
static inline uint32_t i2c_ll_get_txfifo_len(i2c_dev_t *hw)
{
    return SOC_I2C_FIFO_LEN - hw->sr.txfifo_cnt;
}

/**
 * @brief Get I2C timeout configuration
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return The I2C timeout value
 */
static inline uint32_t i2c_ll_get_tout(i2c_dev_t *hw)
{
    return hw->to.time_out_value;
}

/**
 * @brief  Start I2C transfer
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
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

/**
 * @brief  Write the I2C hardware txFIFO
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  ptr Pointer to data buffer
 * @param  len Amount of data needs to be writen
 *
 * @return None.
 */
static inline void i2c_ll_write_txfifo(i2c_dev_t *hw, uint8_t *ptr, uint8_t len)
{
    for (int i = 0; i< len; i++) {
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
static inline void i2c_ll_read_rxfifo(i2c_dev_t *hw, uint8_t *ptr, uint8_t len)
{
    for(int i = 0; i < len; i++) {
        ptr[i] = HAL_FORCE_READ_U32_REG_FIELD(hw->data, fifo_rdata);
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
static inline uint8_t i2c_ll_get_filter(i2c_dev_t *hw)
{
    return hw->filter_cfg.scl_filter_thres;
}

/**
 * @brief  Enable I2C master TX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_enable_tx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = ~0;
    hw->int_ena.val =  I2C_LL_MASTER_TX_INT;
}

/**
 * @brief  Enable I2C master RX interrupt
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_enable_rx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = ~0;
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
static inline void i2c_ll_master_disable_rx_it(i2c_dev_t *hw)
{
    hw->int_ena.val &= (~I2C_LL_MASTER_RX_INT);
}

/**
 * @brief  Clear I2C master TX interrupt status register
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_clr_tx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = I2C_LL_MASTER_TX_INT;
}

/**
 * @brief  Clear I2C master RX interrupt status register
 *
 * @param  hw Beginning address of the peripheral registers
 *
 * @return None
 */
static inline void i2c_ll_master_clr_rx_it(i2c_dev_t *hw)
{
    hw->int_clr.val = I2C_LL_MASTER_RX_INT;
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
    hw->ctr.fsm_rst = 1;
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
    hw->scl_sp_conf.scl_rst_slv_num = 9;
    hw->scl_sp_conf.scl_rst_slv_en = 1;
    hw->ctr.conf_upgate = 1;
    // hardward will clear scl_rst_slv_en after sending SCL pulses,
    // and we should set conf_upgate bit to synchronize register value.
    while (hw->scl_sp_conf.scl_rst_slv_en);
    hw->ctr.conf_upgate = 1;
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
    // src_clk : (1) for RTC_CLK, (0) for XTAL
    hw->clk_conf.sclk_sel = (src_clk == I2C_CLK_SRC_RC_FAST) ? 1 : 0;
}

/**
 * @brief  Get I2C master interrupt event
 *
 * @param  hw Beginning address of the peripheral registers
 * @param  event Pointer to accept the interrupt event
 *
 * @return None
 */
static inline void i2c_ll_master_get_event(i2c_dev_t *hw, i2c_intr_event_t *event)
{
    i2c_int_status_reg_t int_sts = hw->int_status;
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
    ctrl_reg.clk_en = 1;
    ctrl_reg.sda_force_out = 1;
    ctrl_reg.scl_force_out = 1;
    hw->ctr.val = ctrl_reg.val;
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

#ifdef __cplusplus
}
#endif
