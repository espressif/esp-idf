/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/gpio_num.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_SLAVE_SCL_IO     GPIO_NUM_32    /*!<gpio number for I3C slave clock  */
#define I3C_SLAVE_SDA_IO     GPIO_NUM_33    /*!<gpio number for I3C slave data */
#define I3C_MASTER_SCL_IO    GPIO_NUM_32     /*!< gpio number for I3C master clock */
#define I3C_MASTER_SDA_IO    GPIO_NUM_33     /*!< gpio number for I3C master data  */

#define TEST_STATIC_ADDRESS  0x6a
#define TEST_DYNAMIC_ADDRESS 0x06

#define DATA_LENGTH     16

#ifdef __cplusplus
}
#endif
