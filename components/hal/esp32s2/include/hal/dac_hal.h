// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for DAC (esp32s2 specific part)

#pragma once

#include "hal/dac_ll.h"
#include "hal/dac_types.h"

#include_next "hal/dac_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * Digital controller initialization.
 */
void dac_hal_digi_init(void);

/**
 * Digital controller deinitialization.
 */
void dac_hal_digi_deinit(void);

/**
 * Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter.
 */
void dac_hal_digi_controller_config(const dac_digi_config_t *cfg);

/**
 * DAC digital controller start output voltage.
 */
void dac_hal_digi_start(void);

/**
 * DAC digital controller stop output voltage.
 */
void dac_hal_digi_stop(void);

/**
 * Reset DAC digital controller FIFO.
 */
#define dac_hal_digi_fifo_reset() dac_ll_digi_fifo_reset()

/**
 * Reset DAC digital controller.
 */
#define dac_hal_digi_reset() dac_ll_digi_reset()

#ifdef __cplusplus
}
#endif
