/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_periph.h"
#include "esp_assert.h"

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
    IO_MUX_GPIO41_REG,
    IO_MUX_GPIO42_REG,
    IO_MUX_GPIO43_REG,
    IO_MUX_GPIO44_REG,
    IO_MUX_GPIO45_REG,
    IO_MUX_GPIO46_REG,
    IO_MUX_GPIO47_REG,
    IO_MUX_GPIO48_REG,
    IO_MUX_GPIO49_REG,
    IO_MUX_GPIO50_REG,
    IO_MUX_GPIO51_REG,
    IO_MUX_GPIO52_REG,
    IO_MUX_GPIO53_REG,
    IO_MUX_GPIO54_REG,
};

ESP_STATIC_ASSERT(sizeof(GPIO_PIN_MUX_REG) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_PIN_MUX_REG");

const uint32_t GPIO_HOLD_MASK[] = {
    BIT(0),          //GPIO0   // LP_IOMUX_LP_PAD_HOLD_REG
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
    BIT(0),          //GPIO16  // HP_SYSTEM_GPIO_O_HOLD_CTRL0_REG
    BIT(1),          //GPIO17
    BIT(2),          //GPIO18
    BIT(3),          //GPIO19
    BIT(4),          //GPIO20
    BIT(5),          //GPIO21
    BIT(6),          //GPIO22
    BIT(7),          //GPIO23
    BIT(8),          //GPIO24
    BIT(9),          //GPIO25
    BIT(10),         //GPIO26
    BIT(11),         //GPIO27
    BIT(12),         //GPIO28
    BIT(13),         //GPIO29
    BIT(14),         //GPIO30
    BIT(15),         //GPIO31
    BIT(16),         //GPIO32
    BIT(17),         //GPIO33
    BIT(18),         //GPIO34
    BIT(19),         //GPIO35
    BIT(20),         //GPIO36
    BIT(21),         //GPIO37
    BIT(22),         //GPIO38
    BIT(23),         //GPIO39
    BIT(24),         //GPIO40
    BIT(25),         //GPIO41
    BIT(26),         //GPIO42
    BIT(27),         //GPIO43
    BIT(28),         //GPIO44
    BIT(29),         //GPIO45
    BIT(30),         //GPIO46
    BIT(31),         //GPIO47
    BIT(0),          //GPIO48  // HP_SYSTEM_GPIO_O_HOLD_CTRL1_REG
    BIT(1),          //GPIO49
    BIT(2),          //GPIO50
    BIT(3),          //GPIO51
    BIT(4),          //GPIO52
    BIT(5),          //GPIO53
    BIT(6),          //GPIO54
};

ESP_STATIC_ASSERT(sizeof(GPIO_HOLD_MASK) == SOC_GPIO_PIN_COUNT * sizeof(uint32_t), "Invalid size of GPIO_HOLD_MASK");
