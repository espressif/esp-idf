/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Memory of domains that the tests allocate, map and free */

#include <string.h>
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_privilege_umode_domain.h"
#include "privilege_test_fixture.h"

#define TEST_DOMAIN_ALLOCATIONS 32

/* Memory that test_domain_map() allocated, freed by test_domain_destroy() */
typedef struct {
    esp_privilege_umode_domain_handle_t domain;
    void *memory;
} test_allocation_t;

static test_allocation_t s_allocations[TEST_DOMAIN_ALLOCATIONS];
static portMUX_TYPE s_allocations_lock = portMUX_INITIALIZER_UNLOCKED;

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
    test_allocation_t *slot = NULL;
    portENTER_CRITICAL(&s_allocations_lock);
    for (size_t i = 0; i < TEST_DOMAIN_ALLOCATIONS && !slot; i++) {
        if (!s_allocations[i].memory) {
            slot = &s_allocations[i];
            slot->domain = domain;
            slot->memory = allocation;
        }
    }
    portEXIT_CRITICAL(&s_allocations_lock);
    esp_err_t err = slot ? esp_privilege_umode_domain_map(domain, address, allocation, size, permissions) : ESP_ERR_NO_MEM;
    if (err != ESP_OK) {
        if (slot) {
            portENTER_CRITICAL(&s_allocations_lock);
            slot->domain = NULL;
            slot->memory = NULL;
            portEXIT_CRITICAL(&s_allocations_lock);
        }
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
