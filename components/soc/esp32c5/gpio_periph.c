/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_periph.h"

const uint32_t GPIO_HOLD_MASK[] = {
    BIT(0),          //GPIO0   // LP_AON_GPIO_HOLD0_REG
    BIT(1),          //GPIO1
    BIT(2),          //GPIO2
    BIT(3),          //GPIO3
    BIT(4),          //GPIO4
    BIT(5),          //GPIO5
    BIT(6),          //GPIO6
    BIT(7),          //GPIO7
    BIT(8),          //GPIO8
    BIT(9),          //GPIO9
    BIT(10),         //GPIO10
    BIT(11),         //GPIO11
    BIT(12),         //GPIO12
    BIT(13),         //GPIO13
    BIT(14),         //GPIO14
    BIT(15),         //GPIO15
    BIT(16),         //GPIO16
    BIT(17),         //GPIO17
    BIT(18),         //GPIO18
    BIT(19),         //GPIO19
    BIT(20),         //GPIO20
    BIT(21),         //GPIO21
    BIT(22),         //GPIO22
    BIT(23),         //GPIO23
    BIT(24),         //GPIO24
    BIT(25),         //GPIO25
    BIT(26),         //GPIO26
    BIT(27),         //GPIO27
    BIT(28),         //GPIO28
};

_Static_assert(sizeof(GPIO_HOLD_MASK) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_HOLD_MASK");
