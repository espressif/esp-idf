/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "esp_hmac.h"
#include "mbedtls/md.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#define HMAC_KEY_SIZE 32
#define TOKEN_DATA_SIZE 32

static const char* TAG = "jtag_re_enable";

typedef struct {
    struct arg_str *token_data;
    struct arg_end *end;
} command_args_t;

static command_args_t command_args;

static bool is_hexadecimal(const char *str)
{
    if (str == NULL || *str == '\0') {
        return false;
    }

    while (*str != '\0') {
        if (!isxdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }

    return true;
}

const char *esp_efuse_purpose_str(esp_efuse_purpose_t purpose)
{
    switch (purpose) {
    case ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL:
        return "HMAC_DOWN_ALL";
    case ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG:
        return "HMAC_DOWN_JTAG";
    default:
        return "Unknown";
    }
}

static esp_err_t is_device_ready_to_re_enable(void)
{
    bool flag_jtag_hard_dis;
#if SOC_EFUSE_HARD_DIS_JTAG
    flag_jtag_hard_dis = esp_efuse_read_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
#else
    flag_jtag_hard_dis = esp_efuse_read_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
#endif

    if (flag_jtag_hard_dis) {
        ESP_LOGE(TAG, "JTAG permenently disabled. Can't re-enable.");
        return ESP_FAIL;
    }

    size_t out_cnt = 0;
    if (ESP_OK != esp_efuse_read_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, &out_cnt)) {
        ESP_LOGE(TAG, "Error obtaining value for SOFT_DIS_JTAG");
        return ESP_FAIL;
    }

    if (out_cnt == 0) {
        ESP_LOGI(TAG, "JTAG soft disable efuse bit is not programmed, hence JTAG is already enabled");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t re_enable_jtag_utility(uint8_t token_data[])
{
    esp_err_t status;

    // Read the configured key block number from Kconfig
    int configured_block_number = CONFIG_EXAMPLE_JTAG_SEC_HMAC_EFUSE_KEY_ID;

    if (configured_block_number != -1) {
        // User has configured a specific key block number
        ESP_LOGI(TAG, "Using user-configured key block number %d for authentication", configured_block_number);
        status = esp_hmac_jtag_enable(configured_block_number, token_data);
    } else {
        esp_efuse_purpose_t purposes[] = {
            ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL,
            ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG
        };

        // Check if there is already a key block with the desired purpose
        esp_efuse_block_t block;
        status = ESP_FAIL;
        for (unsigned i = 0; i < sizeof(purposes) / sizeof(esp_efuse_purpose_t); i++) {
            if (esp_efuse_find_purpose(purposes[i], &block)) {
                // key block with appropriate purpose found
                ESP_LOGI(TAG, "Using HMAC key at block %d with purpose %s for authentication", block - (int)EFUSE_BLK4, esp_efuse_purpose_str(purposes[i]));
                status = status && esp_hmac_jtag_enable(block - (int)EFUSE_BLK4, token_data);
            }
        }

        if (EFUSE_BLK_KEY_MAX == block) {
            ESP_LOGI(TAG, "HMAC key is not burned with required purpose. Please refer to device configuration in example readme for more details.");
            return ESP_FAIL;
        }
    }

    if (ESP_OK != status) {
        ESP_LOGI(TAG, "Error in re-enabling JTAG");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t re_enable_jtag(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &command_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, command_args.end, argv[0]);
        return ESP_FAIL;
    }

    if (ESP_OK != is_device_ready_to_re_enable()) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Device is ready to re-enable.");

    const char* arg_token_data_str = command_args.token_data->sval[0];
    if (strlen(arg_token_data_str) != TOKEN_DATA_SIZE * 2 || !is_hexadecimal(arg_token_data_str)) {
        ESP_LOGE(TAG, "Invalid token_data. The token data should be 64 hexadecimal characters.");
        return ESP_FAIL;
    }

    // Convert the string to uint8_t array
    uint8_t token_data[TOKEN_DATA_SIZE];
    for (int i = 0; i < TOKEN_DATA_SIZE; ++i) {
        sscanf(arg_token_data_str + 2 * i, "%2hhx", &token_data[i]);
    }

    if (ESP_OK != re_enable_jtag_utility(token_data)) {
        ESP_LOGI(TAG, "JTAG re-enabling failed");
        return  ESP_FAIL;
    }

    ESP_LOGI("jtag", "JTAG re-enablement workflow performed, please check the JTAG connection manually");
    return ESP_OK;
}

static void register_enable_jtag(void)
{
    command_args.token_data = arg_str1(NULL, NULL, "<token_data>", "token_data");
    command_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "enable_jtag",
        .help = "Re-enables software JTAG access.\n\t Usage: enable_jtag <token_data>",
        .hint = NULL,
        .func = &re_enable_jtag,
        .argtable = &command_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static esp_err_t disable_jtag(int argc, char **argv)
{
    if (ESP_OK != esp_hmac_jtag_disable()) {
        ESP_LOGE(TAG, "Failed to disable JTAG");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "JTAG disabled temporarily");
    return ESP_OK;
}

static void register_disable_jtag(void)
{
    const esp_console_cmd_t cmd = {
        .command = "disable_jtag",
        .help = "Disables software JTAG access",
        .hint = NULL,
        .func = &disable_jtag,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void register_jtag_commands(void)
{
    register_enable_jtag();
    register_disable_jtag();
}
