/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief ESP Memprot API error code definition
*/
#define ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID     (ESP_ERR_MEMPROT_BASE + 1)   /**< Memory type invalid in given context */
#define ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID      (ESP_ERR_MEMPROT_BASE + 2)   /**< Splitting address invalid in given context */
#define ESP_ERR_MEMPROT_SPLIT_ADDR_OUT_OF_RANGE (ESP_ERR_MEMPROT_BASE + 3)   /**< Splitting address out of range */
#define ESP_ERR_MEMPROT_SPLIT_ADDR_UNALIGNED    (ESP_ERR_MEMPROT_BASE + 4)   /**< Splitting address not aligned to required boundaries */
#define ESP_ERR_MEMPROT_UNIMGMT_BLOCK_INVALID   (ESP_ERR_MEMPROT_BASE + 5)   /**< Required unified-management block is not valid */
#define ESP_ERR_MEMPROT_WORLD_INVALID           (ESP_ERR_MEMPROT_BASE + 6)   /**< Required World identifier is not valid */
#define ESP_ERR_MEMPROT_AREA_INVALID            (ESP_ERR_MEMPROT_BASE + 7)   /**< Required Area identifier is not valid */
#define ESP_ERR_MEMPROT_CPUID_INVALID           (ESP_ERR_MEMPROT_BASE + 8)   /**< Required CPU/Core identifier is not valid */

#ifdef __cplusplus
}
#endif
