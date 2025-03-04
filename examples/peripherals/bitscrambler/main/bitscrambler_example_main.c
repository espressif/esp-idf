/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/bitscrambler_loopback.h"

//Assign a symbol to the example bitscrambler program. Note that the actual
//assembly and including in the binary happens in the CMakeLists.txt file.
BITSCRAMBLER_PROGRAM(bitscrambler_program_example, "example");

static const char *TAG = "bs_example";

static const uint8_t testdata[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //should give ff 00 ....
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, //should come out as 80 80 80...
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, //'unity matrix', should come out the same
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, //should come out as 00 ff 00 ff ...
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, //should come out as ff 00 ff 00 ...
};

void app_main(void)
{
    ESP_LOGI(TAG, "BitScrambler example main");

    size_t input_data_len = sizeof(testdata);
    size_t output_data_len = input_data_len; // because the example BitScrambler program doesn't increase or decrease the data length
    uint8_t* result_buf = heap_caps_calloc(output_data_len, 1, MALLOC_CAP_DMA);
    assert(result_buf);

    size_t result_buf_size = heap_caps_get_allocated_size(result_buf);
    assert(result_buf_size >= output_data_len);

    bitscrambler_handle_t bs;
    ESP_ERROR_CHECK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, result_buf_size));
    ESP_ERROR_CHECK(bitscrambler_load_program(bs, bitscrambler_program_example));

    size_t result_len;
    // we can even use a const array as the input data because the DMA can read data from the Flash region
    ESP_ERROR_CHECK(bitscrambler_loopback_run(bs, (void*)testdata, input_data_len, result_buf, result_buf_size, &result_len));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %zu, output %zu bytes:\n", input_data_len, result_len);
    for (size_t i = 0; i < result_len; i++) {
        printf("%02X ", result_buf[i]);
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n");
}
