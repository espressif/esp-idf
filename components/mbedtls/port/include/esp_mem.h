/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>

void *esp_mbedtls_mem_calloc(size_t n, size_t size);
void esp_mbedtls_mem_free(void *ptr);
