/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "nvs_bootloader_aes.h"
#include "sdkconfig.h"

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16])
{
    int ret = -1;
    ets_aes_enable();

#if CONFIG_IDF_TARGET_ESP32
    if (mode == AES_ENC) {
        ret = ets_aes_setkey_enc(key, key_bits);
    } else {
        ret = ets_aes_setkey_dec(key, key_bits);
    }

    if (ret) {
        ets_aes_crypt(input, output);
        // In case of esp32, ets_aes_setkey_dec returns 1 on success,
        // whereas for other targets ets_aes_setkey return 0 on success
        ret = 0;
    }
#else /* !CONFIG_IDF_TARGET_ESP32m*/
    ret = ets_aes_setkey(mode, key, key_bits);

    if (ret == 0) {
        ets_aes_block(input, output);
    }
#endif /* CONFIG_IDF_TARGET_ESP32 */

    ets_aes_disable();
    return ret;
}
