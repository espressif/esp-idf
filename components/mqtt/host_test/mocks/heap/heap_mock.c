/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_heap_caps.h"
#include <stdint.h>
#include <stdlib.h>



void *heap_caps_calloc(size_t n, size_t size, uint32_t caps) {
  (void)caps;
  return calloc(n, size);

}
