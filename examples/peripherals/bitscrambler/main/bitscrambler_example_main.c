/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include <stdio.h>
#include "driver/bitscrambler_loopback.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

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

    uint8_t *data_in, *data_out;
    int len = sizeof(testdata);

    data_in = heap_caps_calloc(len, 1, MALLOC_CAP_DMA);
    data_out = heap_caps_calloc(len, 1, MALLOC_CAP_DMA);
    assert(data_in);
    assert(data_out);
    memcpy(data_in, testdata, len);

    bitscrambler_handle_t bs;
    ESP_ERROR_CHECK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len));
    ESP_ERROR_CHECK(bitscrambler_load_program(bs, bitscrambler_program_example));
    size_t size;
    ESP_ERROR_CHECK(bitscrambler_loopback_run(bs, data_in, len, data_out, len, &size));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %d, output %d bytes:\n", len, size);
    for (int i = 0; i < size; i++) {
        printf("%02X ", data_out[i]);
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n");
}
