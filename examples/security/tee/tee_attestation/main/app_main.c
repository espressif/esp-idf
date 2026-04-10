/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_random.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "psa/crypto.h"
#include "psa/initial_attestation.h"

static const char *TAG = "example_tee_attest";

void app_main(void)
{
    ESP_LOGI(TAG, "TEE Attestation Service");

    // Prepare authentication challenge for freshness
    uint8_t auth_challenge[PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32];
    size_t challenge_size = sizeof(auth_challenge);
    esp_fill_random(auth_challenge, challenge_size);

    // Get the required token buffer size
    size_t token_buf_size = 0;
    psa_status_t status = psa_initial_attest_get_token_size(challenge_size, &token_buf_size);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to get token size: %x", status);
        abort();
    }

    // Allocate buffer based on the required size
    uint8_t *token_buf = calloc(token_buf_size, sizeof(uint8_t));
    if (token_buf == NULL) {
        abort();
    }

    /* Generate entity attestation token using PSA interface
     * - Authentication challenge for freshness
     * - Dynamic buffer allocation based on required size
     */
    size_t token_len = 0;
    status = psa_initial_attest_get_token(auth_challenge, challenge_size, token_buf, token_buf_size, &token_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to generate entity attestation token: %x (PSA status)", status);
        free(token_buf);
        abort();
    }

    /* Print the generated token details - length and contents */
    ESP_LOGI(TAG, "Attestation token - Length: %zu", token_len);
    ESP_LOGI(TAG, "Attestation token - Data:\n'%.*s'", (int)token_len, token_buf);

    free(token_buf);
}
