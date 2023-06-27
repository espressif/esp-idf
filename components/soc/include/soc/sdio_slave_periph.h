/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
//include soc related (generated) definitions
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#if SOC_SDIO_SLAVE_SUPPORTED
#include "soc/slc_reg.h"
#include "soc/slc_struct.h"
#include "soc/host_reg.h"
#include "soc/host_struct.h"
#include "soc/hinf_reg.h"
#include "soc/hinf_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** pin and signal information of each slot */
typedef struct {
    uint32_t clk_gpio;
    uint32_t cmd_gpio;
    uint32_t d0_gpio;
    uint32_t d1_gpio;
    uint32_t d2_gpio;
    uint32_t d3_gpio;
    int      func;
} sdio_slave_slot_info_t;

extern const sdio_slave_slot_info_t sdio_slave_slot_info[];

#ifdef __cplusplus
}
#endif
