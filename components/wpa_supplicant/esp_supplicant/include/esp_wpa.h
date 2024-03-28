/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_WPA_H__
#define __ESP_WPA_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_crypto_types.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPA_APIs  WPS APIs
  * @brief Supplicant APIs
  *
  */

/** @addtogroup WPA_APIs
  * @{
  */
/* Crypto callback functions */
extern const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs; // NOLINT(readability-redundant-declaration)

/* Mesh crypto callback functions */
extern const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs;

/**
  * @brief     Supplicant initialization
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_NO_MEM : out of memory
  */
esp_err_t esp_supplicant_init(void);

/**
  * @brief     Supplicant deinitialization
  *
  * @return
  *          - ESP_OK : succeed
  *          - others: failed
  */
esp_err_t esp_supplicant_deinit(void);

/**
 * @brief Disable or enable the caching of Pairwise Master Keys (PMK) in the supplicant.
 *
 * This function allows disabling or enabling the caching of Pairwise Master Keys (PMK).
 * PMK caching is used in Wi-Fi Protected Access (WPA/WPA2/WPA3) networks to speed up the reconnection process
 * by storing the PMK generated during the initial connection. Disabling PMK caching may result in slightly
 * longer reconnection times. PMK caching is enabled by default, this configuration has been provided
 * in case the AP is known not to support PMK caching or has a buggy implementation for PMK caching.
 *
 * @param disable Boolean indicating whether to disable (true) or enable (false) PMK caching.
 * @return
 *     - ESP_OK: Success
 *     - An error code if disabling or enabling PMK caching fails.
 */
esp_err_t esp_supplicant_disable_pmk_caching(bool disable);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPA_H__ */
