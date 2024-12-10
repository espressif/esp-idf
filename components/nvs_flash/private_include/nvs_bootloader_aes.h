/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_ESP32) || !defined(SOC_AES_SUPPORTED)
enum AES_TYPE {
    AES_ENC,
    AES_DEC,
};

#if !SOC_AES_SUPPORTED
enum AES_BITS {
    AES128,
    AES192,
    AES256
};
#endif /* !SOC_AES_SUPPORTED */
#endif /* CONFIG_IDF_TARGET_ESP32 || !SOC_AES_SUPPORTED */

#if SOC_AES_SUPPORTED
#include "rom/aes.h"

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16]);

#endif /* SOC_AES_SUPPORTED */
