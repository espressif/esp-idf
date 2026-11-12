/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Memory of domains that the tests allocate, map and free */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_privilege_umode_domain.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate memory for a domain and map it
 *
 * Allocates size bytes, page-aligned, with the heap capabilities caps. With caps 0, the memory is
 * MALLOC_CAP_EXEC internal RAM for an executable region and 8-bit internal RAM otherwise. Copies
 * initial_size bytes of initial into it, zeroes the rest and maps it at address.
 *
 * @param[out] memory Receives the memory, can be NULL
 * @return
 *      - ESP_OK. The memory is mapped. test_domain_destroy() frees it
 *      - ESP_ERR_NO_MEM. Out of memory, or too many allocations of the fixture
 *      - Other. The error of esp_privilege_umode_domain_map(). Nothing is allocated
 */
esp_err_t test_domain_map(esp_privilege_umode_domain_handle_t domain, uint32_t address, size_t size, uint32_t permissions,
                          const void *initial, size_t initial_size, uint32_t caps, void **memory);

/**
 * @brief Destroy a domain, then free the memory that test_domain_map() allocated for it
 *
 * @return The result of esp_privilege_umode_domain_destroy(). The memory is freed only on ESP_OK
 */
esp_err_t test_domain_destroy(esp_privilege_umode_domain_handle_t domain);

#ifdef __cplusplus
}
#endif
