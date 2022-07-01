/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_periph.h"

const uint32_t GPIO_PIN_MUX_REG[] = {
    IO_MUX_GPIO0_REG,
    IO_MUX_GPIO1_REG,
    IO_MUX_GPIO2_REG,
    IO_MUX_GPIO3_REG,
    IO_MUX_GPIO4_REG,
    IO_MUX_GPIO5_REG,
    IO_MUX_GPIO6_REG,
    IO_MUX_GPIO7_REG,
    IO_MUX_GPIO8_REG,
    IO_MUX_GPIO9_REG,
    IO_MUX_GPIO10_REG,
    IO_MUX_GPIO11_REG,
    IO_MUX_GPIO12_REG,
    IO_MUX_GPIO13_REG,
    IO_MUX_GPIO14_REG,
    IO_MUX_GPIO15_REG,
    IO_MUX_GPIO16_REG,
    IO_MUX_GPIO17_REG,
    IO_MUX_GPIO18_REG,
    IO_MUX_GPIO19_REG,
    IO_MUX_GPIO20_REG,
    IO_MUX_GPIO21_REG,
};

_Static_assert(sizeof(GPIO_PIN_MUX_REG) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_PIN_MUX_REG");

const uint32_t GPIO_HOLD_MASK[] = {
    BIT(0),          //GPIO0
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
};

_Static_assert(sizeof(GPIO_HOLD_MASK) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_HOLD_MASK");
