/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/sdio_slave_periph.h"
#include "soc/gpio_periph.h"

/**
 * I/O slot of sdio slave:
 * Slot 0: GPIO 6, 11, 7, 8, 9, 10.
 * Slot 1: GPIO 14, 15, 2, 4, 12, 13 for CLK, CMD, D0, D1, D2, D3 respectively.
 *
 * @note 1: Only one peripheral for SDIO and only one slot can work at the same time.
 * @note 2: Slot 0 is occupied by SPI for Flash, therefore we only use Slot 1
 */
const sdio_slave_slot_info_t sdio_slave_slot_info[1]  = {
    {
        .clk_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CLK,
        .cmd_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CMD,
        .d0_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D0,
        .d1_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D1,
        .d2_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D2,
        .d3_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D3,
        .func = SDIO_SLAVE_SLOT1_FUNC,
    },
};
