/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Memory of domains that the tests allocate, map and free */

#include <string.h>
#include "esp_heap_caps.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "esp_privilege_umode_domain.h"
#include "privilege_test_fixture.h"
#include "umode_elf.h"

#define TEST_DOMAIN_ALLOCATIONS 32

/* A two-page stack, one unmapped guard page above the program */
#define TEST_APP_STACK_SIZE (2 * ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE)

/* Memory that test_domain_map() allocated, freed by test_domain_destroy() */
typedef struct {
    esp_privilege_umode_domain_handle_t domain;
    void *memory;
} test_allocation_t;

static test_allocation_t s_allocations[TEST_DOMAIN_ALLOCATIONS];
static portMUX_TYPE s_allocations_lock = portMUX_INITIALIZER_UNLOCKED;

/* Record memory that test_domain_destroy() frees. Returns NULL if the table is full. */
static test_allocation_t *track_allocation(esp_privilege_umode_domain_handle_t domain, void *memory)
{
    test_allocation_t *slot = NULL;
    portENTER_CRITICAL(&s_allocations_lock);
    for (size_t i = 0; i < TEST_DOMAIN_ALLOCATIONS && !slot; i++) {
        if (!s_allocations[i].memory) {
            slot = &s_allocations[i];
            slot->domain = domain;
            slot->memory = memory;
        }
    }
    portEXIT_CRITICAL(&s_allocations_lock);
    return slot;
}

static void untrack_allocation(test_allocation_t *slot)
{
    if (slot) {
        portENTER_CRITICAL(&s_allocations_lock);
        slot->domain = NULL;
        slot->memory = NULL;
        portEXIT_CRITICAL(&s_allocations_lock);
    }
}

esp_err_t test_domain_map(esp_privilege_umode_domain_handle_t domain, uint32_t address, size_t size, uint32_t permissions,
                          const void *initial, size_t initial_size, uint32_t caps, void **memory)
{
    if (initial_size > size || (initial_size && !initial)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (caps == 0) {
        caps = (permissions & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE) ? MALLOC_CAP_INTERNAL | MALLOC_CAP_EXEC :
               MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
    }
    uint8_t *allocation = heap_caps_aligned_alloc(ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE, size, caps);
    if (!allocation) {
        return ESP_ERR_NO_MEM;
    }
    if (initial_size) {
        memcpy(allocation, initial, initial_size);
    }
    memset(allocation + initial_size, 0, size - initial_size);
    /* U-mode code must see the written contents in external RAM, also as instructions */
    if (esp_ptr_external_ram(allocation)) {
        esp_err_t err = esp_cache_msync(allocation, size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        if (err == ESP_OK && (permissions & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE)) {
            err = esp_cache_msync(allocation, size, ESP_CACHE_MSYNC_FLAG_DIR_M2C | ESP_CACHE_MSYNC_FLAG_TYPE_INST);
        }
        if (err != ESP_OK) {
            heap_caps_free(allocation);
            return err;
        }
    }
    test_allocation_t *slot = track_allocation(domain, allocation);
    esp_err_t err = slot ? esp_privilege_umode_domain_map(domain, address, allocation, size, permissions) : ESP_ERR_NO_MEM;
    if (err != ESP_OK) {
        untrack_allocation(slot);
        heap_caps_free(allocation);
        return err;
    }
    if (memory) {
        *memory = allocation;
    }
    return ESP_OK;
}

esp_err_t test_domain_destroy(esp_privilege_umode_domain_handle_t domain)
{
    esp_err_t err = esp_privilege_umode_domain_destroy(domain);
    if (err != ESP_OK) {
        return err;
    }
    for (size_t i = 0; i < TEST_DOMAIN_ALLOCATIONS; i++) {
        void *memory = NULL;
        portENTER_CRITICAL(&s_allocations_lock);
        if (s_allocations[i].domain == domain) {
            memory = s_allocations[i].memory;
            s_allocations[i].domain = NULL;
            s_allocations[i].memory = NULL;
        }
        portEXIT_CRITICAL(&s_allocations_lock);
        heap_caps_free(memory);
    }
    return ESP_OK;
}

esp_err_t test_domain_load_app(const uint8_t *elf, size_t size, esp_privilege_umode_domain_handle_t *domain)
{
    return test_domain_load_app_caps(elf, size, 0, domain);
}

esp_err_t test_domain_load_app_caps(const uint8_t *elf, size_t size, uint32_t caps, esp_privilege_umode_domain_handle_t *domain)
{
    return test_domain_load_app_with(elf, size, caps, NULL, 0, domain);
}

esp_err_t test_domain_load_app_with(const uint8_t *elf, size_t size, uint32_t caps, const test_region_t *regions,
                                    size_t region_count, esp_privilege_umode_domain_handle_t *domain)
{
    if (!domain) {
        return ESP_ERR_INVALID_ARG;
    }
    *domain = NULL;
    esp_privilege_umode_domain_handle_t created = NULL;
    esp_err_t err = esp_privilege_umode_domain_create(&created);
    if (err != ESP_OK) {
        return err;
    }
    umode_elf_image_t image;
    err = umode_elf_load_caps(created, elf, size, TEST_APP_BASE, caps, &image);
    /* Hand the program memory to the table, so that test_domain_destroy() frees it after the domain */
    for (size_t i = 0; i < image.count && err == ESP_OK; i++) {
        if (track_allocation(created, image.memory[i])) {
            image.memory[i] = NULL;
        } else {
            err = ESP_ERR_NO_MEM;
        }
    }
    const uint32_t stack = image.end + ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE;
    if (err == ESP_OK) {
        err = test_domain_map(created, stack, TEST_APP_STACK_SIZE, ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE,
                              NULL, 0, caps, NULL);
    }
    for (size_t i = 0; i < region_count && err == ESP_OK; i++) {
        err = esp_privilege_umode_domain_map(created, regions[i].address, regions[i].memory, regions[i].size,
                                             regions[i].permissions);
    }
    if (err == ESP_OK) {
        /* Without linker relaxation the program does not use gp, and it has no thread-local storage */
        const esp_privilege_umode_domain_entry_t start = {
            .entry = image.entry, .stack = stack, .stack_size = TEST_APP_STACK_SIZE,
        };
        err = esp_privilege_umode_domain_finalize(created, &start);
    }
    if (err != ESP_OK) {
        if (test_domain_destroy(created) == ESP_OK) {
            /* Frees what was not handed to the table yet; heap_caps_free() ignores the NULL entries */
            umode_elf_free(&image);
        }
        return err;
    }
    *domain = created;
    return ESP_OK;
}
