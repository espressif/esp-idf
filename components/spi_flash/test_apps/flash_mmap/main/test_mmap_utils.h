/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "spi_flash_mmap.h"

extern void setup_mmap_tests(void);
extern uint32_t test_start;
extern uint32_t test_end;

//Use and maintain these handles. When next test starts, setup_mmap_tests will unmap these handle if they are not zero.
extern spi_flash_mmap_handle_t handle1, handle2, handle3;

esp_err_t spi_flash_read_maybe_encrypted(size_t src_addr, void *des_addr, size_t size);
esp_err_t spi_flash_write_maybe_encrypted(size_t des_addr, const void *src_addr, size_t size);
