/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "esp_err.h"

#include "ulp_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Kept for backwards compatibilty */
#define ulp_riscv_adc_cfg_t ulp_adc_cfg_t
#define ulp_riscv_adc_init ulp_adc_init

#ifdef __cplusplus
}
#endif
