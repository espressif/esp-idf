/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_rom_sys.h"

#include "hal/aes_hal.h"
#include "aes/esp_aes.h"

#include "esp_tee.h"
#include "secure_service_num.h"

/* Fixed key */
static const uint8_t key[AES_256_KEY_BYTES] =  {[0 ... 31] = 0xA5};

esp_err_t _ss_example_sec_serv_aes_op(int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output)
{
    if (length == 0 || iv == NULL || input == NULL || output == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_cpu_get_curr_privilege_level() != ESP_CPU_S_MODE) {
        esp_rom_printf("Operation executing from illegal privilege level!\n");
        return ESP_ERR_INVALID_STATE;
    }

    esp_rom_printf("TEE: Secure service call for AES-256-CBC operation\n");
    esp_rom_printf("TEE: In PROTECTED M-mode\n");

    esp_aes_context ctx = {};
    ctx.key_bytes = AES_256_KEY_BYTES;
    ctx.key_in_hardware = 0;
    memcpy(ctx.key, key, ctx.key_bytes);

    return (esp_err_t)esp_aes_crypt_cbc(&ctx, mode, length, iv, input, output);
}
