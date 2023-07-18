/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

/* NOTE: this file is created manually for compatibility */

#pragma once

#include <stdint.h>
#include "soc/gpio_struct.h"
#include "soc/lp_system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The Analog Comparator Device struct
 * @note  The field in it are register pointers, which point to the physical address
 *        of the corresponding configuration register
 * @note  see 'ana_cmpr_periph.c' for the device instance
 */
typedef struct {
    volatile lp_system_reg_pad_comp_reg_t *pad_comp_config;
    volatile gpio_zero_det_filter_cnt_reg_t *pad_comp_filter;
    volatile gpio_int_st_reg_t *int_st;
    volatile gpio_int_ena_reg_t *int_ena;
    volatile gpio_int_clr_reg_t *int_clr;
} analog_cmpr_dev_t;

extern analog_cmpr_dev_t ANALOG_CMPR[2];

#ifdef __cplusplus
}
#endif
