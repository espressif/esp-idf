/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// The following macros have a format SOC_[periph][instance_id] to make it work with `GDMA_MAKE_TRIGGER`
#define SOC_GDMA_TRIG_PERIPH_M2M0    (-1)
#define SOC_GDMA_TRIG_PERIPH_SPI2    (0)
#define SOC_GDMA_TRIG_PERIPH_UART0   (2)
#define SOC_GDMA_TRIG_PERIPH_SHA0    (7)
#define SOC_GDMA_TRIG_PERIPH_ADC0    (8)
