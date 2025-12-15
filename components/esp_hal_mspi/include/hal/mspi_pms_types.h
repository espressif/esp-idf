/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MSPI memory type protected by PMS
 */
typedef enum {
    MSPI_PMS_MEM_FLASH = 0,  /*!< Flash memory */
    MSPI_PMS_MEM_PSRAM = 1,  /*!< PSRAM memory */
} mspi_pms_mem_t;

/**
 * @brief PMS security mode
 */
typedef enum {
    MSPI_PMS_MODE_TEE = 0,  /*!< Secure mode */
    MSPI_PMS_MODE_REE = 1,  /*!< Non-secure mode */
} mspi_pms_mode_t;

/**
 * @brief PMS region attribute bits (can be ORed together)
 */
typedef enum {
    MSPI_PMS_ATTR_NONE = 0,         /*!< No permissions */
    MSPI_PMS_ATTR_RD   = BIT(0),    /*!< Read permission */
    MSPI_PMS_ATTR_WR   = BIT(1),    /*!< Write permission */
    MSPI_PMS_ATTR_ECC  = BIT(2),    /*!< ECC enabled for the region */
} mspi_pms_attr_t;

/**
 * @brief PMS error types
 */
typedef enum {
    MSPI_PMS_ERR_NONE        = 0,       /*!< No error */
    MSPI_PMS_ERR_WRITE       = BIT(0),  /*!< Write access error */
    MSPI_PMS_ERR_READ        = BIT(1),  /*!< Read access error */
    MSPI_PMS_ERR_ADDR_MISS   = BIT(2),  /*!< Address miss error */
    MSPI_PMS_ERR_ADDR_MULTI  = BIT(3),  /*!< Address multi-hit error */
} mspi_pms_err_t;

#ifdef __cplusplus
}
#endif
