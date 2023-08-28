/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "ulp_riscv_register_ops.h"
#include "hal/adc_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start an ADC conversion and get the converted value.
 *
 * @note Will block until the conversion is completed
 *
 * @note ADC should be initilized for ULP by main CPU by calling ulp_riscv_adc_init()
 *       before calling this.
 *
 * @param      adc_n   ADC unit.
 * @param      channel ADC channel number.
 *
 * @return             Converted value, -1 if conversion failed
 */
int32_t ulp_riscv_adc_read_channel(adc_unit_t adc_n, int channel);

#ifdef __cplusplus
}
#endif
