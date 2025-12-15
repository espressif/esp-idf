/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32S31 GMAC register operations

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void emac_ll_gpio_init(uint32_t gpio_num)
{
    (void)gpio_num;
    // TODO: [ESP32S31] IDF-14730
    // GPIO13-19 are located in the CNNT domain and correspond to GMAC RX functionality.
    // When the GMAC peripheral needs to use these pads, additional configuration is required:
    // - Set CNNT_IO_MUX_CTRL_REG.CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL to 1
    // - In this case, the pad configuration registers in cnnt_io_mux_reg take effect
    //   instead of hp_iomux_reg, which requires special handling
}

#ifdef __cplusplus
}
#endif
