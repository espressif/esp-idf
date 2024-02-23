/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "test_dualboard_utils.h"

/*---------------------------------------------------------------
                Fill buffer with random numbers
---------------------------------------------------------------*/
void test_fill_random_to_buffers_dualboard(uint32_t seed, uint8_t *dut0_buffer, uint8_t *dut1_buffer, size_t buffer_size)
{
    srand(seed);
    for (int i = 0; i < buffer_size; i++) {
        dut1_buffer[i] = rand() % 256;
        dut0_buffer[i] = rand() % 256;
    }
}

void test_fill_random_to_buffer(uint32_t seed, uint8_t *buffer, size_t buffer_size)
{
    srand(seed);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = rand() % 256;
    }
}

/*---------------------------------------------------------------
    Create a pool with random numbers, get buffer from pool
---------------------------------------------------------------*/
static uint8_t *s_pool;
static size_t s_pool_size;

void test_prepare_buffer_pool(size_t pool_size, uint32_t flags)
{
    s_pool_size = pool_size;
    s_pool = (uint8_t *)heap_caps_calloc(1, pool_size, flags);
    TEST_ASSERT(s_pool);
    test_fill_random_to_buffer(199, s_pool, pool_size);
}

void test_get_buffer_from_pool(uint32_t offset, size_t size, void **out_buffer)
{
    //to make sure the out_buffer is within the pool
    offset = ((offset % (s_pool_size - size)) + 3) & ~3;
    // TEST_ASSERT(offset + size < (uint32_t)s_pool + s_pool_size)

    *out_buffer = (void *)(s_pool + offset);
}

void test_destroy_buffer_pool(void)
{
    free(s_pool);
    s_pool = NULL;
    s_pool_size = 0;
}
