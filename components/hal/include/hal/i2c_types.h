/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/hal_utils.h"

/**
 * @brief I2C port number, can be I2C_NUM_0 ~ (I2C_NUM_MAX-1).
 */
typedef enum {
    I2C_NUM_0 = 0,              /*!< I2C port 0 */
#if SOC_HP_I2C_NUM >= 2
    I2C_NUM_1,                  /*!< I2C port 1 */
#endif /* SOC_HP_I2C_NUM >= 2 */
#if SOC_LP_I2C_NUM >= 1
    LP_I2C_NUM_0,               /*< LP_I2C port 0 */
#endif /* SOC_LP_I2C_NUM >= 1 */
    I2C_NUM_MAX,                /*!< I2C port max */
} i2c_port_t;

/**
 * @brief Enumeration for I2C device address bit length
 */
typedef enum {
    I2C_ADDR_BIT_LEN_7 = 0,       /*!< i2c address bit length 7 */
#if SOC_I2C_SUPPORT_10BIT_ADDR
    I2C_ADDR_BIT_LEN_10 = 1,      /*!< i2c address bit length 10 */
#endif
} i2c_addr_bit_len_t;

/**
 * @brief Data structure for calculating I2C bus timing.
 */
typedef struct {
    uint16_t clkm_div;          /*!< I2C core clock divider */
    uint16_t scl_low;           /*!< I2C scl low period */
    uint16_t scl_high;          /*!< I2C scl high period */
    uint16_t scl_wait_high;     /*!< I2C scl wait_high period */
    uint16_t sda_hold;          /*!< I2C scl low period */
    uint16_t sda_sample;        /*!< I2C sda sample time */
    uint16_t setup;             /*!< I2C start and stop condition setup period */
    uint16_t hold;              /*!< I2C start and stop condition hold period  */
    uint16_t tout;              /*!< I2C bus timeout period */
} i2c_hal_clk_config_t;

typedef enum{
#if SOC_I2C_SUPPORT_SLAVE
    I2C_MODE_SLAVE = 0,   /*!< I2C slave mode */
#endif
    I2C_MODE_MASTER,      /*!< I2C master mode */
    I2C_MODE_MAX,
} i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C write data */
    I2C_MASTER_READ,        /*!< I2C read data */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C data msb first */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C data lsb first */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

__attribute__((deprecated("please use 'i2c_addr_bit_len_t' instead")))
typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C 7bit address for slave mode */
    I2C_ADDR_BIT_10,       /*!< I2C 10bit address for slave mode */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;

typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< I2C ack for each byte read */
    I2C_MASTER_NACK = 0x1,       /*!< I2C nack for each byte read */
    I2C_MASTER_LAST_NACK = 0x2,   /*!< I2C nack for the last byte*/
    I2C_MASTER_ACK_MAX,
} i2c_ack_type_t;

/**
 * @brief Enum for I2C slave stretch causes
 */
typedef enum {
    I2C_SLAVE_STRETCH_CAUSE_ADDRESS_MATCH = 0,   /*!< Stretching SCL low when the slave is read by the master and the address just matched */
    I2C_SLAVE_STRETCH_CAUSE_TX_EMPTY = 1,        /*!< Stretching SCL low when TX FIFO is empty in slave mode */
    I2C_SLAVE_STRETCH_CAUSE_RX_FULL = 2,         /*!< Stretching SCL low when RX FIFO is full in slave mode */
    I2C_SLAVE_STRETCH_CAUSE_SENDING_ACK = 3,     /*!< Stretching SCL low when slave sending ACK */
} i2c_slave_stretch_cause_t;

typedef enum {
    I2C_SLAVE_WRITE_BY_MASTER = 0,
    I2C_SLAVE_READ_BY_MASTER = 1,
} i2c_slave_read_write_status_t;

#if SOC_I2C_SUPPORTED
/**
 * @brief I2C group clock source
 */
typedef soc_periph_i2c_clk_src_t i2c_clock_source_t;

#if SOC_LP_I2C_SUPPORTED
/**
 * @brief LP_UART source clock
 */
typedef soc_periph_lp_i2c_clk_src_t lp_i2c_clock_source_t;
#endif

#else
/**
 * @brief Default type
 */
typedef int                      i2c_clock_source_t;
#endif


#ifdef __cplusplus
}
#endif
