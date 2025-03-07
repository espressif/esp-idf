/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND
/**
 * @brief Default pseudo rounds configs of the XTS-AES accelerator
 */
typedef enum {
    ESP_XTS_AES_PSEUDO_ROUNDS_DISABLE = 0,
    ESP_XTS_AES_PSEUDO_ROUNDS_LOW,
    ESP_XTS_AES_PSEUDO_ROUNDS_MEDIUM,
    ESP_XTS_AES_PSEUDO_ROUNDS_HIGH,
} esp_xts_aes_psuedo_rounds_state_t;

/* The total number of pseudo-rounds randomly inserted in an XTS-AES operation are controlled by
 * configuring the PSEUDO_MODE, PSEUDO_BASE, PSEUDO_INC parameters.
 * Users can also set the frequency of random key updates by configuring the PSEUDO_RNG_CNT.
 * Here, we would be using some pre-decided values for these parameters corresponding to the security needed.
 * For more information regarding these parameters please refer the TRM.
 */
#define XTS_AES_PSEUDO_ROUNDS_BASE      4
#define XTS_AES_PSEUDO_ROUNDS_INC       2
#define XTS_AES_PSEUDO_ROUNDS_RNG_CNT   7

#endif /* SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND */

#ifdef __cplusplus
}
#endif
