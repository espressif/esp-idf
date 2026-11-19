/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TEE image code hash: SHA256 of the image segments (headers and data) alone, without
 * the app descriptor, so it identifies the compiled code alone.
 *
 * NOTE: The image header, the app descriptor, the XOR checksum (with its padding) and the
 * appended digest carry packaging metadata (flash config, chip revision bounds, version
 * strings, ELF digest), hence they are left out.
 *
 * NOTE: Also parsed by scripts/esp_tee_code_hash.py; bump the version on any change
 */
#define ESP_TEE_IMG_CODE_HASH_VER  (1)

#ifdef __cplusplus
}
#endif
