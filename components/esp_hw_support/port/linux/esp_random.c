/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/param.h>

#include "esp_log.h"

static const char* TAG = "esp-random";

static void __attribute__((constructor)) esp_random_init(void)
{
    srand(time(NULL));
    ESP_LOGW(TAG, "esp_random do not provide a cryptographically secure numbers on Linux, and should never be used for anything security related");
}

uint32_t esp_random(void)
{
    /* Adding INT32_MAX to shift the results such that after conversion to uint32_t we still get 32 bits of random data */
    return (rand() + INT32_MAX);
}

void esp_fill_random(void *buf, size_t len)
{
    assert(buf != NULL);
    uint8_t *buf_bytes = (uint8_t *)buf;
    while (len > 0) {
        uint32_t word = esp_random();
        uint32_t to_copy = MIN(sizeof(word), len);
        memcpy(buf_bytes, &word, to_copy);
        buf_bytes += to_copy;
        len -= to_copy;
    }
}
