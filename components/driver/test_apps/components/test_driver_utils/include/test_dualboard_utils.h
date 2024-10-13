/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Fill tx_buffers used in dual-board test with random numbers
 *
 * dut0_buffer and dut1_buffer will be filled with random data
 *
 * this can be used by a fullduplex dualboard transaction
 *
 * @param seed            Random number seed
 * @param dut0_buffer     TX buffer used by dut0
 * @param dut1_buffer     TX buffer used by dut1
 * @param buffer_size     Buffer size
 */
void test_fill_random_to_buffers_dualboard(uint32_t seed, uint8_t *dut0_buffer, uint8_t *dut1_buffer, size_t buffer_size);

/**
 * Fill tx buffer with random numbers
 *
 * @param seed            Random number seed
 * @param buffer          Buffer
 * @param buffer_size     Buffer size
 */
void test_fill_random_to_buffer(uint32_t seed, uint8_t *buffer, size_t buffer_size);

/**
 * Calloc a pool, filled with random numbers
 *
 * @param pool_size  Size of the pool
 * @param flags      Calloc flags, see `esp_heap_caps.h`
 */
void test_prepare_buffer_pool(size_t pool_size, uint32_t flags);

/**
 * Get a buffer from the pool
 *
 * @param offset           A random offset
 * @param size             Buffer size
 * @param[out] out_buffer  Out buffer
 */
void test_get_buffer_from_pool(uint32_t offset, size_t size, void **out_buffer);

/**
 * Destroy the pool
 */
void test_destroy_buffer_pool(void);
