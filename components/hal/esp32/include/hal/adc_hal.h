/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for ADC (esp32 specific part)

#pragma once

#include "hal/adc_ll.h"
#include "hal/adc_types.h"

#include_next "hal/adc_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Hall sensor setting
---------------------------------------------------------------*/
/**
 *  Start hall convert and return the hall value.
 *
 *  @return Hall value.
 */
int adc_hal_hall_convert(void);

#ifdef __cplusplus
}
#endif
