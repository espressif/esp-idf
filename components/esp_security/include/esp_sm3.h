/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SHA_SUPPORT_SM3

/** Length of an SM3 digest in bytes. */
#define ESP_SM3_DIGEST_LEN  32

/** Size of an SM3 message block in bytes. */
#define ESP_SM3_BLOCK_LEN   64

/**
 * @brief Handle of one SM3 operation.
 *
 * esp_sm3_create() allocates the context. esp_sm3_delete() releases it. Do not
 * read or write the context.
 */
typedef struct esp_sm3_ctx_s *esp_sm3_ctx_handle_t;

/**
 * @brief Start a new SM3 operation.
 *
 * The function allocates the context. Call esp_sm3_delete() to release it.
 *
 * @param[out] ctx Receives the handle of the new context. The function writes
 *                 NULL if it returns ESP_ERR_NO_MEM or ESP_ERR_NOT_SUPPORTED.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx is NULL
 *      - ESP_ERR_NO_MEM if the allocation fails
 *      - ESP_ERR_NOT_SUPPORTED if an eFuse disables the SM crypto functions
 */
esp_err_t esp_sm3_create(esp_sm3_ctx_handle_t *ctx);

/**
 * @brief Add message bytes to an SM3 operation.
 *
 * Call this function as many times as necessary. The function acquires the SHA
 * peripheral only when the new bytes complete at least one 64-byte block. It
 * releases the peripheral before it returns.
 *
 * @param ctx   Context from esp_sm3_create().
 * @param input Message bytes. Can be NULL if ilen is 0.
 * @param ilen  Number of message bytes.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx is NULL, or if input is NULL and ilen is not 0
 *      - ESP_ERR_NOT_SUPPORTED if an eFuse disables the SM crypto functions
 */
esp_err_t esp_sm3_update(esp_sm3_ctx_handle_t ctx, const void *input, size_t ilen);

/**
 * @brief Complete an SM3 operation and read the digest.
 *
 * The function always acquires the SHA peripheral, because the padding
 * completes the last message block. It releases the peripheral before it
 * returns.
 *
 * @param ctx    Context from esp_sm3_create().
 * @param output Buffer for the digest.
 * @param olen   Size of output in bytes. Must be ESP_SM3_DIGEST_LEN or more.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx or output is NULL
 *      - ESP_ERR_INVALID_SIZE if olen is too small
 *      - ESP_ERR_NOT_SUPPORTED if an eFuse disables the SM crypto functions
 */
esp_err_t esp_sm3_finish(esp_sm3_ctx_handle_t ctx, uint8_t *output, size_t olen);

/**
 * @brief Erase and release an SM3 context.
 *
 * The function erases the message bytes and the digest state. Then it frees
 * the context. Call this function after esp_sm3_finish(), and also to abandon
 * an operation. The handle is not valid after the call. If you do not call
 * this function, the context stays allocated.
 *
 * @param ctx Context from esp_sm3_create(). The function accepts NULL.
 */
void esp_sm3_delete(esp_sm3_ctx_handle_t ctx);

/**
 * @brief Compute the SM3 digest of one buffer.
 *
 * @param input  Message bytes. Can be NULL if ilen is 0.
 * @param ilen   Number of message bytes.
 * @param output Buffer for the digest.
 * @param olen   Size of output in bytes. Must be ESP_SM3_DIGEST_LEN or more.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if output is NULL, or if input is NULL and ilen is not 0
 *      - ESP_ERR_INVALID_SIZE if olen is too small
 *      - ESP_ERR_NOT_SUPPORTED if an eFuse disables the SM crypto functions
 */
esp_err_t esp_sm3(const void *input, size_t ilen, uint8_t *output, size_t olen);

#endif /* SOC_SHA_SUPPORT_SM3 */

#ifdef __cplusplus
}
#endif
