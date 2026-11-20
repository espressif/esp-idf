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

/** Virtual address at which test_domain_load_app() loads a program */
#define TEST_APP_BASE 0x20000000u

/**
 * @brief Create a domain that runs a U-mode program that umode_elf_add_program() built
 *
 * Loads the program at TEST_APP_BASE with umode_elf_load(), maps a two-page stack above it with an unmapped
 * page in between, and finalizes the domain with the entry point of the program.
 *
 * @param[in] elf ELF file of the program
 * @param[in] size Size of the file in bytes
 * @param[out] domain Finalized domain. test_domain_destroy() destroys it and frees its memory
 * @return
 *      - ESP_OK
 *      - Other. The error of a domain function or of umode_elf_load(). Nothing is left allocated
 */
esp_err_t test_domain_load_app(const uint8_t *elf, size_t size, esp_privilege_umode_domain_handle_t *domain);

/**
 * @brief Like test_domain_load_app(), but allocates the program and its stack with the heap capabilities caps
 *
 * With caps 0, it allocates as test_domain_load_app() does.
 */
esp_err_t test_domain_load_app_caps(const uint8_t *elf, size_t size, uint32_t caps, esp_privilege_umode_domain_handle_t *domain);

/** Memory that the caller owns, mapped into a domain by test_domain_load_app_with() */
typedef struct {
    uint32_t address;     /**< Virtual address */
    void *memory;         /**< Page-aligned memory. The caller frees it, if needed, after the domain is destroyed */
    size_t size;          /**< Size in bytes, a multiple of the page size */
    uint32_t permissions; /**< ESP_PRIVILEGE_UMODE_DOMAIN_READ and so on */
} test_region_t;

/**
 * @brief Like test_domain_load_app_caps(), and also maps the regions before the domain is finalized
 */
esp_err_t test_domain_load_app_with(const uint8_t *elf, size_t size, uint32_t caps, const test_region_t *regions,
                                    size_t region_count, esp_privilege_umode_domain_handle_t *domain);

#ifdef __cplusplus
}
#endif
