/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
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
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPA_H__ */
