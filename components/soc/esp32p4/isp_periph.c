/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/periph_defs.h"
#include "soc/isp_periph.h"

#ifdef __cplusplus
extern "C" {
#endif

const isp_info_t isp_hw_info = {
    .instances = {
        [0] = {
            .irq = ETS_ISP_INTR_SOURCE,
        }
    }
};

#ifdef __cplusplus
}
#endif
