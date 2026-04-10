/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "esp_assert.h"

const uint32_t GPIO_HOLD_MASK[] = {
    BIT(8),          //GPIO0, LP_SYSTEM_REG_PADCTRL_REG[8]
    BIT(9),          //GPIO1
    BIT(10),         //GPIO2
    BIT(11),         //GPIO3
    BIT(12),         //GPIO4
    BIT(13),         //GPIO5
    BIT(14),         //GPIO6
    BIT(15),         //GPIO7
    BIT(0),          //GPIO8, LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL0_REG[0]
    BIT(1),          //GPIO9
    BIT(2),          //GPIO10
    BIT(3),          //GPIO11
    BIT(4),          //GPIO12
    BIT(5),          //GPIO13
    BIT(6),          //GPIO14
    BIT(7),          //GPIO15
    BIT(8),          //GPIO16
    BIT(9),          //GPIO17
    BIT(10),         //GPIO18
    BIT(11),         //GPIO19
    BIT(12),         //GPIO20
    BIT(13),         //GPIO21
    BIT(14),         //GPIO22
    BIT(15),         //GPIO23
    BIT(16),         //GPIO24
    BIT(17),         //GPIO25
    BIT(18),         //GPIO26
    BIT(19),         //GPIO27
    BIT(20),         //GPIO28
    0,               //GPIO29 (Not bonded/Dummy)
    BIT(22),         //GPIO30
    BIT(23),         //GPIO31
    BIT(24),         //GPIO32
    BIT(25),         //GPIO33
    BIT(26),         //GPIO34
    BIT(27),         //GPIO35
    BIT(28),         //GPIO36
    BIT(29),         //GPIO37
    BIT(30),         //GPIO38
    BIT(31),         //GPIO39
    BIT(0),          //GPIO40, LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL1_REG[0]
    0,               //GPIO41 (Not bonded/Dummy)
    BIT(2),          //GPIO42
    BIT(3),          //GPIO43
    BIT(4),          //GPIO44
    BIT(5),          //GPIO45
    BIT(6),          //GPIO46
    BIT(7),          //GPIO47
    BIT(8),          //GPIO48
    BIT(9),          //GPIO49
    BIT(10),         //GPIO50
    BIT(11),         //GPIO51
    BIT(12),         //GPIO52
    BIT(13),         //GPIO53
    BIT(14),         //GPIO54
    BIT(15),         //GPIO55
    BIT(16),         //GPIO56
    BIT(17),         //GPIO57
    BIT(18),         //GPIO58
    BIT(19),         //GPIO59
    BIT(20),         //GPIO60
    BIT(21),         //GPIO61
};

ESP_STATIC_ASSERT(sizeof(GPIO_HOLD_MASK) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_HOLD_MASK");
