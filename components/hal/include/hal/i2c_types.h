// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

/**
 * @brief I2C port number, can be I2C_NUM_0 ~ (I2C_NUM_MAX-1).
 */
typedef int i2c_port_t;

typedef enum{
    I2C_MODE_SLAVE = 0,   /*!< I2C slave mode */
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
 * @brief I2C clock source, sorting from smallest to largest,
 *        place them in order.
 *        This can be expanded in the future use.
 */
typedef enum {
    I2C_SCLK_DEFAULT = 0,    /*!< I2C source clock not selected*/
#if SOC_I2C_SUPPORT_APB
    I2C_SCLK_APB,            /*!< I2C source clock from APB, 80M*/
#endif
#if SOC_I2C_SUPPORT_XTAL
    I2C_SCLK_XTAL,           /*!< I2C source clock from XTAL, 40M */
#endif
#if SOC_I2C_SUPPORT_RTC
    I2C_SCLK_RTC,            /*!< I2C source clock from 8M RTC, 8M */
#endif
#if SOC_I2C_SUPPORT_REF_TICK
    I2C_SCLK_REF_TICK,       /*!< I2C source clock from REF_TICK, 1M */
#endif
    I2C_SCLK_MAX,
} i2c_sclk_t;

/// Use the highest speed that is available for the clock source picked by clk_flags
#define I2C_CLK_FREQ_MAX                  (-1)

#if CONFIG_IDF_TARGET_ESP32
typedef enum{
    I2C_CMD_RESTART = 0,   /*!<I2C restart command */
    I2C_CMD_WRITE,         /*!<I2C write command */
    I2C_CMD_READ,          /*!<I2C read command */
    I2C_CMD_STOP,          /*!<I2C stop command */
    I2C_CMD_END            /*!<I2C end command */
} i2c_opmode_t __attribute__((deprecated));
#endif

#ifdef __cplusplus
}
#endif
