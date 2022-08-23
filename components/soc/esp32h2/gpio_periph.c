/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
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
    IO_MUX_GPIO22_REG,
    IO_MUX_GPIO23_REG,
    IO_MUX_GPIO24_REG,
    IO_MUX_GPIO25_REG,
#if CONFIG_IDF_TARGET_ESP32H2_BETA_VERSION_1
    IO_MUX_GPIO26_REG,
    IO_MUX_GPIO27_REG,
    IO_MUX_GPIO28_REG,
    IO_MUX_GPIO29_REG,
    IO_MUX_GPIO30_REG,
    IO_MUX_GPIO31_REG,
    IO_MUX_GPIO32_REG,
    IO_MUX_GPIO33_REG,
    IO_MUX_GPIO34_REG,
    IO_MUX_GPIO35_REG,
    IO_MUX_GPIO36_REG,
    IO_MUX_GPIO37_REG,
    IO_MUX_GPIO38_REG,
    IO_MUX_GPIO39_REG,
    IO_MUX_GPIO40_REG,
#endif
};

_Static_assert(sizeof(GPIO_PIN_MUX_REG) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_PIN_MUX_REG");

const uint32_t GPIO_HOLD_MASK[] = {
#if CONFIG_IDF_TARGET_ESP32H2_BETA_VERSION_1
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
    BIT(22),         //GPIO22
    BIT(23),         //GPIO23
    BIT(24),         //GPIO24
    BIT(25),         //GPIO25
    BIT(26),         //GPIO26
    BIT(27),         //GPIO27
    BIT(28),         //GPIO28
    BIT(29),         //GPIO29
    BIT(30),         //GPIO30
    BIT(31),         //GPIO31
    BIT(0),          //GPIO32
    BIT(1),          //GPIO33
    BIT(2),          //GPIO34
    BIT(3),          //GPIO35
    BIT(4),          //GPIO36
    BIT(5),          //GPIO37
    BIT(6),          //GPIO38
    BIT(7),          //GPIO39
    BIT(8),          //GPIO40
#elif CONFIG_IDF_TARGET_ESP32H2_BETA_VERSION_2
    BIT(0),          //GPIO0
    BIT(1),          //GPIO1
    BIT(2),          //GPIO2
    BIT(3),          //GPIO3
    BIT(4),          //GPIO4
    BIT(5),          //GPIO5
    BIT(6),          //GPIO6
    BIT(0),          //GPIO7
    BIT(1),          //GPIO8
    BIT(2),          //GPIO9
    BIT(4),          //GPIO10
    BIT(3),          //GPIO11
    BIT(5),          //GPIO12
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
#endif
};

_Static_assert(sizeof(GPIO_HOLD_MASK) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_HOLD_MASK");
