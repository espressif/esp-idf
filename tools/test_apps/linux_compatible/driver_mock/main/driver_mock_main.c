/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/* Test that mock headers can be included */
#include "driver/i2c.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"

void app_main(void)
{
    /*Test that mock functions exist*/
    i2c_driver_delete(0);
}
