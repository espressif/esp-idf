/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_types.h"
#include "esp_err.h"
#include "esp_intr_alloc.h" // for intr_alloc_flags
#include "soc/soc_caps.h"
#include "hal/i2c_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C clk flags for users to use, can be expanded in the future.
#define I2C_SCLK_SRC_FLAG_FOR_NOMAL       (0)         /*!< Any one clock source that is available for the specified frequency may be chosen*/
#define I2C_SCLK_SRC_FLAG_AWARE_DFS       (1 << 0)    /*!< For REF tick clock, it won't change with APB.*/
#define I2C_SCLK_SRC_FLAG_LIGHT_SLEEP     (1 << 1)    /*!< For light sleep mode.*/

/**
 * @brief Minimum size, in bytes, of the internal private structure used to describe
 * I2C commands link.
 */
#define I2C_INTERNAL_STRUCT_SIZE (24)

/**
 * @brief The following macro is used to determine the recommended size of the
 * buffer to pass to `i2c_cmd_link_create_static()` function.
 * It requires one parameter, `TRANSACTIONS`, describing the number of transactions
 * intended to be performed on the I2C port.
 * For example, if one wants to perform a read on an I2C device register, `TRANSACTIONS`
 * must be at least 2, because the commands required are the following:
 *  - write device register
 *  - read register content
 *
 * Signals such as "(repeated) start", "stop", "nack", "ack" shall not be counted.
 */
#define I2C_LINK_RECOMMENDED_SIZE(TRANSACTIONS)     (2 * I2C_INTERNAL_STRUCT_SIZE + I2C_INTERNAL_STRUCT_SIZE * \
                                                        (5 * TRANSACTIONS)) /* Make the assumption that each transaction
                                                                             * of the user is surrounded by a "start", device address
                                                                             * and a "nack/ack" signal. Allocate one more room for
                                                                             * "stop" signal at the end.
                                                                             * Allocate 2 more internal struct size for headers.
                                                                             */

/**
 * @brief I2C initialization parameters
 */
typedef struct {
    i2c_mode_t mode;     /*!< I2C mode */
    gpio_num_t sda_io_num; /*!< GPIO number for I2C sda signal */
    gpio_num_t scl_io_num; /*!< GPIO number for I2C scl signal */
    bool sda_pullup_en;  /*!< Internal GPIO pull mode for I2C sda signal*/
    bool scl_pullup_en;  /*!< Internal GPIO pull mode for I2C scl signal*/

    union {
        struct {
            uint32_t clk_speed;      /*!< I2C clock frequency for master mode, (no higher than 1MHz for now) */
        } master;                    /*!< I2C master config */
#if SOC_I2C_SUPPORT_SLAVE
        struct {
            uint8_t addr_10bit_en;   /*!< I2C 10bit address mode enable for slave mode */
            uint16_t slave_addr;     /*!< I2C address for slave mode */
            uint32_t maximum_speed;  /*!< I2C expected clock speed from SCL. */
        } slave;                     /*!< I2C slave config */
#endif // SOC_I2C_SUPPORT_SLAVE
    };
    uint32_t clk_flags;              /*!< Bitwise of ``I2C_SCLK_SRC_FLAG_**FOR_DFS**`` for clk source choice*/
} i2c_config_t;

typedef void *i2c_cmd_handle_t;    /*!< I2C command handle  */

#ifdef __cplusplus
}
#endif
