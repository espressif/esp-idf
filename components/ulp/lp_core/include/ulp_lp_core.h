/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp_err.h"
#include "ulp_common.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,  // LP core is started by HP core (1 single wakeup)
} ulp_lp_core_wakeup_source_t;

/**
 * @brief ULP LP core init parameters
 *
 */
typedef struct {
    ulp_lp_core_wakeup_source_t wakeup_source;
} ulp_lp_core_cfg_t;

/**
 * @brief Configure the ULP
 *        and run the program loaded into RTC memory
 *
 * @return  ESP_OK on success
 */
esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg);

/**
 * @brief Load the program binary into RTC memory
 *
 * @param program_binary pointer to program binary
 * @param program_size_bytes size of the program binary
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if program_size_bytes is more than KiB
 */
esp_err_t ulp_lp_core_load_binary(const uint8_t* program_binary, size_t program_size_bytes);


#ifdef __cplusplus
}
#endif
