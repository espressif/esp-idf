/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "rom/aes.h"

#if CONFIG_IDF_TARGET_ESP32
enum AES_TYPE {
    AES_ENC,
    AES_DEC,
};
#endif /* CONFIG_IDF_TARGET_ESP32 */

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16]);
