/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/periph_defs.h"
#include "hal/mspi_periph.h"
#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

const mspi_info_t mspi_hw_info = {
    .instances = {
        [0] = {
            .irq = ETS_SPI0_INTR_SOURCE,
        },
        [1] = {
            .irq = -1,
        },
    }
};

#ifdef __cplusplus
}
#endif
