// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include "soc/sdio_slave_periph.h"
#include "soc/io_mux_reg.h"
#include "soc/sdio_slave_pins.h"

// I/O slot of sdio slave:
// 0: GPIO 6, 11, 7, 8, 9, 10,
// 1: GPIO 14, 15, 2, 4, 12, 13 for CLK, CMD, D0, D1, D2, D3 respectively.
// only one peripheral for SDIO and only one slot can work at the same time.
// currently slot 0 is occupied by SPI for flash
const sdio_slave_slot_info_t sdio_slave_slot_info[2]  = {
    {
        .clk_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CLK,
        .cmd_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CMD,
        .d0_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D0,
        .d1_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D1,
        .d2_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D2,
        .d3_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D3,
        .func = SDIO_SLAVE_SLOT0_FUNC,
    }, {
        .clk_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CLK,
        .cmd_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CMD,
        .d0_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D0,
        .d1_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D1,
        .d2_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D2,
        .d3_gpio = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D3,
        .func = SDIO_SLAVE_SLOT1_FUNC,
    },
};
