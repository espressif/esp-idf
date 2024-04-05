/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include "esp_log.h"

#define GETENTROPY_MAX_LEN 256

static void __attribute__((constructor)) esp_random_init(void) { }

uint32_t esp_random(void)
{
    uint32_t random_number;
    assert(getentropy(&random_number, sizeof(random_number)) == 0);
    return random_number;
}

void esp_fill_random(void *buf, size_t len)
{
    assert(buf != NULL);

    // Note that we can't use getentropy() with len > 256 directly (see getentropy man page),
    // hence reading in chunks
    const size_t FULL_CHUNKS_NUM = (len / GETENTROPY_MAX_LEN);
    const size_t REST_CHUNK_SIZE = len % GETENTROPY_MAX_LEN;

    for (size_t chunk_num = 0; chunk_num < FULL_CHUNKS_NUM; chunk_num++) {
        assert(getentropy(buf + chunk_num * GETENTROPY_MAX_LEN, GETENTROPY_MAX_LEN) == 0);
    }

    assert(getentropy(buf + FULL_CHUNKS_NUM * GETENTROPY_MAX_LEN, REST_CHUNK_SIZE) == 0);
}
