/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Pin definition header file. The long term plan is to have a single soc_pins.h for all
 * peripherals. Now we temporarily separate these information into periph_pins/channels.h for each
 * peripheral and include them here to avoid developing conflicts in those header files.
 */

#pragma once

#include "soc/gpio_pins.h"
#include "soc/spi_pins.h"
