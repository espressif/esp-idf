/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "hal/sdio_slave_periph.h"
#include "soc/sdio_slave_pins.h"

const sdio_slave_slot_info_t sdio_slave_slot_info[1]  = {
    {
        .clk_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CLK,
        .cmd_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CMD,
        .d0_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D0,
        .d1_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D1,
        .d2_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D2,
        .d3_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D3,
        .func = SDIO_SLAVE_SLOT0_FUNC,
    },
};
