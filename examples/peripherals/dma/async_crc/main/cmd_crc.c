/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_async_crc.h"
#include "cmd_crc.h"

static const char *TAG = "crc";

static async_crc_handle_t s_crc_handle = NULL;

static struct {
    struct arg_int *width;          /*!< CRC width (8, 16, or 32) */
    struct arg_str *poly;           /*!< CRC polynomial (hex string) */
    struct arg_str *init;          /*!< Initial CRC value (hex string) */
    struct arg_str *xor;           /*!< Final XOR value (hex string) */
    struct arg_int *reverse_input; /*!< Reverse input bits */
    struct arg_int *reverse_output; /*!< Reverse output bits */
    struct arg_str *data;          /*!< Input data string */
    struct arg_end *end;           /*!< argtable end marker */
} crc_args;

static uint32_t parse_hex_value(const char *str)
{
    uint32_t value = 0;
    if (str && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        value = (uint32_t)strtoul(str, NULL, 16);
    } else {
        value = (uint32_t)strtoul(str, NULL, 0);
    }
    return value;
}

static int do_crc_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&crc_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, crc_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_crc_handle) {
        ESP_LOGE(TAG, "CRC driver not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // By default, calculate an 8-bit CRC with polynomial 0x07, initial value 0x00, and final XOR value 0x00, without bit reversal
    async_crc_params_t params = {
        .width = 8,
        .polynomial = 0x07,
        .init_value = 0x00,
        .final_xor_value = 0x00,
        .reverse_input = false,
        .reverse_output = false,
    };

    if (crc_args.width->count) {
        int width = crc_args.width->ival[0];
        if (width != 8 && width != 16 && width != 32) {
            ESP_LOGE(TAG, "Invalid width %d. Must be 8, 16, or 32", width);
            return ESP_ERR_INVALID_ARG;
        }
        params.width = width;
    }

    if (crc_args.poly->count) {
        params.polynomial = parse_hex_value(crc_args.poly->sval[0]);
    }

    if (crc_args.init->count) {
        params.init_value = parse_hex_value(crc_args.init->sval[0]);
    }

    if (crc_args.xor->count) {
        params.final_xor_value = parse_hex_value(crc_args.xor->sval[0]);
    }

    if (crc_args.reverse_input->count) {
        params.reverse_input = crc_args.reverse_input->ival[0] ? true : false;
    }

    if (crc_args.reverse_output->count) {
        params.reverse_output = crc_args.reverse_output->ival[0] ? true : false;
    }

    const char *data_str = crc_args.data->sval[0];
    size_t data_len = strlen(data_str);

    uint32_t result = 0;
    esp_err_t err = esp_crc_calc_blocking(s_crc_handle, data_str, data_len, &params, -1, &result);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "CRC calculation failed: %s", esp_err_to_name(err));
        return err;
    }

    if (params.width == 8) {
        printf("CRC result: 0x%02" PRIX32 "\n", result);
    } else if (params.width == 16) {
        printf("CRC result: 0x%04" PRIX32 "\n", result);
    } else {
        printf("CRC result: 0x%08" PRIX32 "\n", result);
    }
    return ESP_OK;
}

void register_crc(void)
{
    async_crc_config_t config = {
        .backlog = 1, // backlog of 1 is sufficient since we wait for each calculation to finish before starting the next one
        .dma_burst_size = 32,
    };
    ESP_ERROR_CHECK(esp_async_crc_install_gdma_ahb(&config, &s_crc_handle));

    crc_args.width = arg_int0(NULL, "width", "<width>", "CRC width (8, 16, or 32). Default: 8");
    crc_args.poly = arg_str0(NULL, "poly", "<hex>", "CRC polynomial in hex. Default: 0x07");
    crc_args.init = arg_str0(NULL, "init", "<hex>", "Initial CRC value in hex. Default: 0x00");
    crc_args.xor = arg_str0(NULL, "xor", "<hex>", "Final XOR value in hex. Default: 0x00");
    crc_args.reverse_input = arg_int0(NULL, "reverse-input", "<0|1>", "Reverse input bits (0 or 1). Default: 0");
    crc_args.reverse_output = arg_int0(NULL, "reverse-output", "<0|1>", "Reverse output bits (0 or 1). Default: 0");
    crc_args.data = arg_str1(NULL, NULL, "<data>", "Input data string");
    crc_args.end = arg_end(7);

    const esp_console_cmd_t crc_cmd = {
        .command = "crc",
        .help = "Calculate CRC checksum using hardware async CRC driver",
        .hint = NULL,
        .func = &do_crc_cmd,
        .argtable = &crc_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&crc_cmd));
}
