/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#include "esp_types.h"
#include <stdbool.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file psa_crypto_driver_esp_sha_contexts.h
 *
 * \brief Context structure definitions for ESP SHA hardware driver.
 *
 * This file contains the context structures used by the ESP SHA driver
 * for PSA Crypto API. These definitions are completely standalone and
 * do not include any PSA Crypto headers to avoid circular dependencies.
 *
 * \note This file may not be included directly. It is included by
 * crypto_driver_contexts_primitives.h.
 */

typedef enum {
    ESP_SHA_OPERATION_TYPE_SHA1,
    ESP_SHA_OPERATION_TYPE_SHA256,
    ESP_SHA_OPERATION_TYPE_SHA224,
    ESP_SHA_OPERATION_TYPE_SHA512,
    ESP_SHA_OPERATION_TYPE_SHA384
} esp_sha_operation_type_t;

/**
 * \brief ESP SHA1 state enumeration
 */
typedef enum {
    ESP_SHA1_STATE_INIT,
    ESP_SHA1_STATE_IN_PROCESS
} esp_sha1_state;

typedef enum {
    ESP_SHA256_STATE_INIT,
    ESP_SHA256_STATE_IN_PROCESS
} esp_sha256_state;

typedef enum {
    ESP_SHA512_STATE_INIT,
    ESP_SHA512_STATE_IN_PROCESS
} esp_sha512_state;

#if SOC_SHA_SUPPORT_PARALLEL_ENG
typedef enum {
    ESP_SHA_MODE_UNUSED,
    ESP_SHA_MODE_HARDWARE,
    ESP_SHA_MODE_SOFTWARE
} esp_sha_mode_t;
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */

/**
 * \brief ESP SHA1 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[5];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
    bool first_block;           /*!< First block flag for hardware initialization */
    int sha_state;              /*!< SHA operation state */
#if SOC_SHA_SUPPORT_PARALLEL_ENG
    esp_sha_mode_t operation_mode;        /*!< Hardware or Software mode */
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */
} esp_sha1_context;

/**
 * \brief ESP SHA256 context structure
 */
typedef struct {
    unsigned char buffer[64];   /*!< The data block being processed. */
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    bool first_block;           /*!< First block flag for hardware initialization */
    int sha_state;              /*!< SHA operation state */
    int mode;                   /*!< SHA2_224 or SHA2_256 */
#if SOC_SHA_SUPPORT_PARALLEL_ENG
    esp_sha_mode_t operation_mode;        /*!< Hardware or Software mode */
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */
} esp_sha256_context;

/**
 * \brief ESP SHA512 context structure
 *
 */
typedef struct {
    uint64_t total[2];          /*!< The number of Bytes processed. */
    uint64_t state[8];          /*!< The intermediate digest state. */
    unsigned char buffer[128];  /*!< The data block being processed. */
    bool first_block;
    int sha_state;
    int mode;
    uint32_t t_val;             /*!< t_val for 512/t mode */
#if SOC_SHA_SUPPORT_PARALLEL_ENG
    esp_sha_mode_t operation_mode;        /*!< Hardware or Software mode */
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */
} esp_sha512_context;

typedef void *esp_sha_context_t;
/**
 * \brief ESP SHA driver operation context
 *
 * This structure contains the contexts for different SHA algorithms
 * supported by the ESP hardware accelerator.
 */
typedef struct {
    esp_sha_context_t sha_ctx;
    esp_sha_operation_type_t sha_type;
} esp_sha_hash_operation_t;

#ifdef __cplusplus
}
#endif
