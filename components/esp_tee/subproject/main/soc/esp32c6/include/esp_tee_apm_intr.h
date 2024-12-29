/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/apm_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *esp_tee_apm_excp_mid_to_str(uint8_t mid);

const char *esp_tee_apm_excp_type_to_str(uint8_t type);

const char *esp_tee_apm_excp_ctrl_to_str(apm_ll_apm_ctrl_t apm_ctrl);

const char *esp_tee_apm_excp_mode_to_str(uint8_t mode);

#ifdef __cplusplus
}
#endif
