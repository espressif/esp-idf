/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* U-mode domain API. Creates domains, maps the memory provided by the caller into their page tables,
 * checks the entry point and the stack, runs the domain code on the calling task and destroys domains. */

#include <assert.h>
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "riscv/rv_utils.h"
#include "esp_privilege_umode_domain.h"
#include "esp_private/privilege_domain_address_space.h"
#include "esp_private/privilege_entry.h"

#define DOMAIN_PERMISSIONS (ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE | ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE)
/* Largest region, which needs at most two second-level page tables */
#define DOMAIN_MAX_REGION_SIZE (4u * 1024 * 1024)

struct esp_privilege_umode_domain {
    privilege_domain_address_space_t *space;
    esp_privilege_umode_domain_entry_t start;
    bool finalized; /* The memory map is fixed, and the domain can run */
};

/* The esp_privilege_umode_domain API allocates, blocks and signals other cores, so it needs a task with interrupts
 * enabled. MIE is checked directly, because the IDF interrupt-mask API reports individual
 * sources. */
static bool domain_in_task_context(void)
{
    return !xPortInIsrContext() && xPortCanYield() &&
           (RV_READ_CSR(mstatus) & MSTATUS_MIE) != 0 &&
           xTaskGetSchedulerState() == taskSCHEDULER_RUNNING &&
           RV_READ_CSR(medeleg) == 0 && RV_READ_CSR(mideleg) == 0;
}

/* The domain code can use the memory with the permissions */
static bool usable_memory(const void *memory, size_t size, uint32_t permissions)
{
    const void *last = (const uint8_t *)memory + (size - 1);
    if (!esp_ptr_internal(memory) || !esp_ptr_internal(last)) {
        return false;
    }
    return !(permissions & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE) || (esp_ptr_executable(memory) && esp_ptr_executable(last));
}

/* Checks the region of esp_privilege_umode_domain_map() */
static bool valid_region(uint32_t virtual_address, const void *memory, size_t size, uint32_t permissions)
{
    if (size == 0 || ((virtual_address | (uintptr_t)memory | size) & (ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE - 1))) {
        return false;
    }
    /* The region ends at or below the reserved page, and its memory does not wrap around */
    if ((uint64_t)virtual_address + size > ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT || size - 1 > UINTPTR_MAX - (uintptr_t)memory) {
        return false;
    }
    if (!permissions || (permissions & ~DOMAIN_PERMISSIONS)) {
        return false;
    }
    /* Writable memory is readable and never executable */
    const bool writable = permissions & ESP_PRIVILEGE_UMODE_DOMAIN_WRITE;
    if (writable && (!(permissions & ESP_PRIVILEGE_UMODE_DOMAIN_READ) || (permissions & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE))) {
        return false;
    }
    return usable_memory(memory, size, permissions);
}

/* Checks the entry point and the stack of esp_privilege_umode_domain_finalize() before the page tables are read */
static bool valid_start(const esp_privilege_umode_domain_entry_t *start)
{
    /* RISC-V instructions are 2-byte aligned, and the calling convention keeps the stack 16-byte aligned */
    if ((start->entry & 1) || (start->stack & 15) || (start->stack_size & 15)) {
        return false;
    }
    return start->stack_size != 0 && start->stack_size <= UINT32_MAX - start->stack;
}

esp_err_t esp_privilege_umode_domain_create(esp_privilege_umode_domain_handle_t *domain)
{
    if (!domain || !domain_in_task_context()) {
        return ESP_ERR_INVALID_ARG;
    }
    /* The handle and the page tables are the bookkeeping of the domain, so they are allocated here */
    esp_privilege_umode_domain_handle_t created = heap_caps_calloc(1, sizeof(*created), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!created) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t err = privilege_domain_address_space_create(&created->space);
    if (err != ESP_OK) {
        heap_caps_free(created);
        return err;
    }
    *domain = created;
    return ESP_OK;
}

esp_err_t esp_privilege_umode_domain_map(esp_privilege_umode_domain_handle_t domain, uint32_t virtual_address, void *memory, size_t size,
                                         uint32_t permissions)
{
    if (!domain || !memory || !domain_in_task_context()) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size > DOMAIN_MAX_REGION_SIZE) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (!valid_region(virtual_address, memory, size, permissions)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (domain->finalized) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = privilege_domain_address_space_map(domain->space, virtual_address, size, (uintptr_t)memory, permissions);
    /* The page tables refuse a range that overlaps a region of the domain */
    return err == ESP_ERR_INVALID_STATE ? ESP_ERR_INVALID_ARG : err;
}

esp_err_t esp_privilege_umode_domain_finalize(esp_privilege_umode_domain_handle_t domain, const esp_privilege_umode_domain_entry_t *start)
{
    if (!domain || !start || !domain_in_task_context() || !valid_start(start)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (domain->finalized) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = ESP_OK;
    /* The entry point is executable, and the stack is readable and writable */
    if (!privilege_domain_address_space_check(domain->space, start->entry, 2, ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE) ||
            !privilege_domain_address_space_check(domain->space, start->stack, start->stack_size,
                                                  ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE)) {
        err = ESP_ERR_INVALID_ARG;
    }
    /* A stack overflow must raise an exception, so U-mode code must not be able to write right below its stack */
    if (err == ESP_OK && start->stack != 0 &&
            privilege_domain_address_space_check(domain->space, start->stack - 1, 1, ESP_PRIVILEGE_UMODE_DOMAIN_WRITE)) {
        err = ESP_ERR_INVALID_ARG;
    }
    /* Page tables and executable pages do not change after finalization, so address translation and
     * instruction fetch on every core are synchronized once here instead of on each entry to U-mode. */
    if (err == ESP_OK) {
        err = privilege_domain_address_space_flush_all_cores(NULL);
    }
    if (err == ESP_OK) {
        domain->start = *start;
        domain->finalized = true;
    }
    return err;
}

esp_err_t esp_privilege_umode_domain_run(esp_privilege_umode_domain_handle_t domain, uint32_t arg0, uint32_t arg1,
                                         esp_privilege_umode_domain_result_t *result)
{
    if (!domain || !result || !domain_in_task_context()) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!domain->finalized) {
        return ESP_ERR_INVALID_STATE;
    }
    const privilege_domain_run_context_t context = {
        .gp = domain->start.global_pointer,
        .tp = domain->start.thread_pointer,
        .return_address = PRIVILEGE_DOMAIN_RETURN_ADDRESS,
    };
    /* Runs the entry function in U-mode on this task and returns when the run ends */
    uint64_t returned = esp_privilege_call_user_code((esp_privilege_user_fn_t)domain->start.entry, arg0, arg1,
                                                     (void *)(domain->start.stack + domain->start.stack_size),
                                                     privilege_domain_address_space_satp(domain->space), &context);
    /* The high word reports a fault, and no run ends by a fault yet */
    assert((returned >> 32) == 0);
    result->reason = ESP_PRIVILEGE_UMODE_DOMAIN_EXIT_RETURN;
    result->value = (int32_t)returned;
    return ESP_OK;
}

esp_err_t esp_privilege_umode_domain_destroy(esp_privilege_umode_domain_handle_t domain)
{
    if (!domain || !domain_in_task_context()) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Stale translations are flushed on every core before the page tables are freed and the caller may free
     * the memory they map. */
    esp_err_t err = privilege_domain_address_space_flush_all_cores(domain->space);
    if (err != ESP_OK) {
        return err;
    }
    privilege_domain_address_space_delete(domain->space);
    heap_caps_free(domain);
    return ESP_OK;
}
