/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

//CLKOUT channels
#define CLKOUT_IOMUX_FUNC_NUM                3

#define CLKOUT_GPIO20_DIRECT_CHANNEL         CLKOUT_CHANNEL_1
#define CLKOUT_CHANNEL_1_DIRECT_GPIO_NUM     20
#define CLKOUT_GPIO19_DIRECT_CHANNEL         CLKOUT_CHANNEL_2
#define CLKOUT_CHANNEL_2_DIRECT_GPIO_NUM     19
#define CLKOUT_GPIO18_DIRECT_CHANNEL         CLKOUT_CHANNEL_3
#define CLKOUT_CHANNEL_3_DIRECT_GPIO_NUM     18

// ESP32S3 has two other sets of GPIO pins which could route to CLKOUT_CHANNEL_1/2/3 through IO_MUX
// Please check TRM IO MUX Function List table if needed
