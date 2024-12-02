/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "esp_tee_attestation.h"
#include "example_tee_srv.h"

static const char *TAG = "tee_attest";

#define ESP_ATT_TK_BUF_SIZE      (1792)
#define ESP_ATT_TK_PSA_CERT_REF  ("0716053550477-10100")

static uint8_t token_buf[ESP_ATT_TK_BUF_SIZE] = {0};

static int tee_dump_att_token(int argc, char **argv)
{
    if (argc != 1) {
        ESP_LOGE(TAG, "Incorrect number of arguments given!");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t token_len = 0;
    esp_err_t err = esp_tee_att_generate_token(0xA1B2C3D4, 0x0FACADE0, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                               token_buf, sizeof(token_buf), &token_len);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "Attestation token - Length: %lu", token_len);
    ESP_LOGI(TAG, "Attestation token - Data:\n'%.*s'", (int)token_len, token_buf);

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
