/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_random.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "example_tee_srv.h"

#include "psa/crypto.h"
#include "psa/initial_attestation.h"

static const char *TAG = "tee_attest";

static int tee_dump_att_token(int argc, char **argv)
{
    if (argc != 1) {
        ESP_LOGE(TAG, "Incorrect number of arguments given!");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_FAIL;

    // Prepare authentication challenge
    uint8_t auth_challenge[PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32];
    size_t challenge_size = sizeof(auth_challenge);
    esp_fill_random(auth_challenge, challenge_size);

    // Get the required token buffer size
    size_t token_buf_size = 0;
    psa_status_t status = psa_initial_attest_get_token_size(challenge_size, &token_buf_size);
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get token size: %x (PSA status)", status);
        return err;
    }

    // Allocate buffer based on the required size
    uint8_t *token_buf = calloc(token_buf_size, sizeof(uint8_t));
    if (token_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Generating the attestation token
    size_t token_len = 0;
    status = psa_initial_attest_get_token(auth_challenge, challenge_size, token_buf, token_buf_size, &token_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to generate token: %x (PSA status)", status);
        free(token_buf);
        return err;
    }

    ESP_LOGI(TAG, "Attestation token - Length: %zu", token_len);
    ESP_LOGI(TAG, "Attestation token - Data:\n'%.*s'", (int)token_len, token_buf);

    free(token_buf);
    return ESP_OK;
}

void register_srv_attestation(void)
{
    const esp_console_cmd_t tee_att_cmd = {
        .command = "tee_att_info",
        .help = "Dump the TEE-generated entity attestation token",
        .hint = NULL,
        .func = &tee_dump_att_token,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_att_cmd));
}
