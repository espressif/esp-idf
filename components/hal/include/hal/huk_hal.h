/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The HAL layer for Hardware Unique Key (HUK) Generator

#pragma once

#include "soc/soc_caps.h"

#if SOC_HUK_SUPPORTED
#include "hal/huk_types.h"
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Configure HUK: Generate new HUK information or Recover key from recovery information
 *     Generation Mode: In this case the Generation mode of the HUK Generator is used. A new HUK is generated and the respective HUK information is copied to the given buffer. This info can be again used to recover the same HUK.
 *     Recovery Mode: In this case the Recovery mode of the HUK Generator is used. The HUK is recovered from the given HUK information. This is the HUK information generated previously when HUK info was generated with a previous call to huk_hal_configure.
 *
 * @input
 *      huk_info_buf(I/O)    Pointer to the buffer for the HUK info, size of the given buffer must equal to HUK_INFO_SIZE
 *          In Generation Mode the buffer shall be populated with the huk_info_buf
 *          In recovery mode the huk_info stored in the buffer shall be consumed for HUK recovery
 *
 * @return
 *      ESP_OK      on success
 *      ESP_FAIL    on failure
 */
esp_err_t huk_hal_configure(const esp_huk_mode_t huk_mode, uint8_t *huk_info_buf);

/**
 * @brief Read state of Hardware Unique Key Generator
 *
 * @return esp_huk_state_t
 */
esp_huk_state_t huk_hal_get_state(void);

/**
 * @brief Get the HUK generation status: esp_huk_gen_status_t
 */
uint8_t huk_hal_get_risk_level(void);

/**
 * @brief Read the HUK date information
 */
uint32_t huk_hal_get_date_info(void);

#if SOC_HUK_MEM_NEEDS_RECHARGE
/**
 * @brief Recharge HUK memory
 */
void huk_hal_recharge_huk_memory(void);
#endif

#ifdef __cplusplus
}
#endif
#endif
