/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* Provide a SHA256 API for bootloader_support code,
   that can be used from bootloader or app code.

   This header is available to source code in the bootloader & bootloader_support components only.
   Use PSA APIs or include esp32/sha.h to calculate SHA256 in IDF apps.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *bootloader_sha256_handle_t;
typedef bootloader_sha256_handle_t bootloader_sha_handle_t;

bootloader_sha256_handle_t bootloader_sha256_start(void);

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len);

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest);

#if SOC_SHA_SUPPORT_SHA512
bootloader_sha_handle_t bootloader_sha512_start(bool is384);

void bootloader_sha512_data(bootloader_sha_handle_t handle, const void *data, size_t data_len);

void bootloader_sha512_finish(bootloader_sha_handle_t handle, uint8_t *digest);
#endif /* SOC_SHA_SUPPORT_SHA512 */

#ifdef __cplusplus
}
#endif
