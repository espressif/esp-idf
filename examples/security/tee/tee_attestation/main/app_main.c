/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "esp_system.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_tee_attestation.h"

static const char *TAG = "example_tee_attest";

#define ESP_ATT_TK_NONCE         (0xA1B2C3D4)
#define ESP_ATT_TK_CLIENT_ID     (0x0FACADE0)

#define ESP_ATT_TK_BUF_SIZE      (1792)
#define ESP_ATT_TK_PSA_CERT_REF  ("0716053550477-10100")

static uint8_t token_buf[ESP_ATT_TK_BUF_SIZE] = {0};

void app_main(void)
{
    ESP_LOGI(TAG, "TEE Attestation Service");

    uint32_t token_len = 0;

    /* Generate entity attestation token using the following parameters
     * and return the token length in token_len:
     * - Nonce value for freshness
     * - Client ID to identify requester
     * - PSA certification ID reference string
     * - Buffer to store the generated token
     */
    esp_err_t err = esp_tee_att_generate_token(ESP_ATT_TK_NONCE, ESP_ATT_TK_CLIENT_ID, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                               token_buf, sizeof(token_buf), &token_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate entity attestation token!");
        abort();
    }

    /* Print the generated token details - length and contents */
    ESP_LOGI(TAG, "Attestation token - Length: %lu", token_len);
    ESP_LOGI(TAG, "Attestation token - Data:\n'%.*s'", (int)token_len, token_buf);
}
