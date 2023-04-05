/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#include "nvs_flash.h"
#include "esp_partition.h"

#if SOC_HMAC_SUPPORTED
#include "esp_hmac.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_NVS_SEC_BASE                        0xF000                         /*!< Starting number of error codes */

#define ESP_ERR_NVS_SEC_HMAC_KEY_NOT_FOUND          (ESP_ERR_NVS_SEC_BASE + 0x01)  /*!< HMAC Key required to generate the NVS encryption keys not found */
#define ESP_ERR_NVS_SEC_HMAC_KEY_BLK_ALREADY_USED   (ESP_ERR_NVS_SEC_BASE + 0x02)  /*!< Provided eFuse block for HMAC key generation is already in use */
#define ESP_ERR_NVS_SEC_HMAC_KEY_GENERATION_FAILED  (ESP_ERR_NVS_SEC_BASE + 0x03)  /*!< Failed to generate/write the HMAC key to eFuse */
#define ESP_ERR_NVS_SEC_HMAC_XTS_KEYS_DERIV_FAILED  (ESP_ERR_NVS_SEC_BASE + 0x04)  /*!< Failed to derive the NVS encryption keys based on the HMAC-based scheme */

/**
 * @brief NVS Encryption Keys Protection Scheme
 */
typedef enum {
    NVS_SEC_SCHEME_FLASH_ENC = 0, /*!<  Protect NVS encryption keys using Flash Encryption */
    NVS_SEC_SCHEME_HMAC,          /*!<  Protect NVS encryption keys using HMAC peripheral */
    NVS_SEC_SCHEME_MAX
} nvs_sec_scheme_id_t;

/**
 * @brief Flash encryption-based scheme specific configuration data
 */
typedef struct {
    const esp_partition_t *nvs_keys_part; /*!<  Partition of subtype `nvs_keys` holding the NVS encryption keys */
} nvs_sec_config_flash_enc_t;

/**
 * @brief Helper for populating the Flash encryption-based scheme specific configuration data
 */
#define NVS_SEC_PROVIDER_CFG_FLASH_ENC_DEFAULT() {                                             \
        .nvs_keys_part      = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,                \
                                                       ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS,    \
                                                       NULL),                                  \
}

#if SOC_HMAC_SUPPORTED
/**
 * @brief HMAC-based scheme specific configuration data
 */
typedef struct {
    hmac_key_id_t hmac_key_id; /*!<  HMAC Key ID used for generating the NVS encryption keys */
} nvs_sec_config_hmac_t;

/**
 * @brief Helper for populating the HMAC-based scheme specific configuration data
 */
#define NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT() {                                     \
        .hmac_key_id      = (hmac_key_id_t)(CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID),    \
}
#endif  /* SOC_HMAC_SUPPORTED */

/**
 * @brief Register the Flash-Encryption based scheme for NVS Encryption
 *
 * @param[in]  sec_scheme_cfg          Security scheme specific configuration data
 * @param[out] sec_scheme_handle_out   Security scheme specific configuration handle
 *
 * @return
 *      - ESP_OK, if `sec_scheme_handle_out` was populated successfully with the scheme configuration;
 *      - ESP_ERR_INVALID_ARG, if `scheme_cfg_hmac` is NULL;
 *      - ESP_ERR_NO_MEM, No memory for the scheme-specific handle `sec_scheme_handle_out`
 *      - ESP_ERR_NOT_FOUND, if no `nvs_keys` partition is found
 */
esp_err_t nvs_sec_provider_register_flash_enc(const nvs_sec_config_flash_enc_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out);

#if SOC_HMAC_SUPPORTED
/**
 * @brief Register the HMAC-based scheme for NVS Encryption
 *
 * @param[in]  sec_scheme_cfg          Security scheme specific configuration data
 * @param[out] sec_scheme_handle_out   Security scheme specific configuration handle
 *
 * @return
 *      - ESP_OK, if `sec_scheme_handle_out` was populated successfully with the scheme configuration;
 *      - ESP_ERR_INVALID_ARG, if `scheme_cfg_hmac` is NULL;
 *      - ESP_ERR_NO_MEM, No memory for the scheme-specific handle `sec_scheme_handle_out`
 */
esp_err_t nvs_sec_provider_register_hmac(const nvs_sec_config_hmac_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out);
#endif  /* SOC_HMAC_SUPPORTED */

/**
 * @brief Deregister the NVS encryption scheme registered with the given handle
 *
 * @param[in] sec_scheme_handle  Security scheme specific configuration handle

 * @return
 *      - ESP_OK, if the scheme registered with `sec_scheme_handle` was deregistered successfully
 *      - ESP_ERR_INVALID_ARG, if `sec_scheme_handle` is NULL;
 */
esp_err_t nvs_sec_provider_deregister(nvs_sec_scheme_t *sec_scheme_handle);

#ifdef __cplusplus
}
#endif
