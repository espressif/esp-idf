/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Page tables that give a U-mode domain its own address space, built with the riscv Sv32 helpers */

#include <assert.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include "esp_ipc.h"
#include "sdkconfig.h"
#include "esp_privilege_umode_domain.h"
#include "riscv/sv32.h"
#include "riscv/sv32_as.h"
#include "esp_private/privilege_domain_address_space.h"

/* Allocates a page table, aligned to its size as Sv32 requires */
static riscv_sv32_table_t *page_table_alloc(void)
{
    return heap_caps_aligned_alloc(RISCV_SV32_TABLE_SIZE, sizeof(riscv_sv32_table_t),
                                   MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

void privilege_domain_address_space_delete(privilege_domain_address_space_t *as)
{
    if (as == NULL) {
        return;
    }
    riscv_sv32_table_t *table;
    while ((table = riscv_sv32_as_detach_table(as)) != NULL) {
        heap_caps_free(table);
    }
    /* The address space is its root table */
    heap_caps_free(as);
}

uint32_t privilege_domain_address_space_satp(const privilege_domain_address_space_t *as)
{
    return as == NULL ? 0 : riscv_sv32_as_satp(as);
}

esp_err_t privilege_domain_address_space_create(privilege_domain_address_space_t **out)
{
    /* The root table maps each 4 MiB of virtual addresses to a second-level table. It starts empty. */
    riscv_sv32_table_t *root = page_table_alloc();
    if (root == NULL) {
        return ESP_ERR_NO_MEM;
    }
    *out = riscv_sv32_as_init(root);
    return ESP_OK;
}

esp_err_t privilege_domain_address_space_map(privilege_domain_address_space_t *as, uint32_t va, size_t size, uintptr_t backing,
                                             uint32_t flags)
{
    /* A region is at most 4 MiB, so it needs at most two second-level tables. They are allocated before
     * the mapping starts, so that the mapping cannot fail halfway for lack of memory. */
    riscv_sv32_table_t *tables[2] = { NULL };
    const size_t count = riscv_sv32_as_count_tables_needed(as, va, size);
    assert(count <= sizeof(tables) / sizeof(tables[0]));
    if (count > sizeof(tables) / sizeof(tables[0])) {
        /* Kept for builds without assertions, so that tables cannot overflow */
        return ESP_ERR_INVALID_SIZE;
    }
    for (size_t i = 0; i < count; i++) {
        tables[i] = page_table_alloc();
        if (tables[i] == NULL) {
            while (i-- > 0) {
                heap_caps_free(tables[i]);
            }
            return ESP_ERR_NO_MEM;
        }
    }
    uint32_t permissions = 0;
    permissions |= (flags & ESP_PRIVILEGE_UMODE_DOMAIN_READ) ? RISCV_SV32_PTE_R : 0;
    permissions |= (flags & ESP_PRIVILEGE_UMODE_DOMAIN_WRITE) ? RISCV_SV32_PTE_W : 0;
    permissions |= (flags & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE) ? RISCV_SV32_PTE_X : 0;
    const esp_err_t err = riscv_sv32_as_map(as, va, backing, size, permissions, tables, count);
    /* On failure, for example an overlap with a mapped page, the address space did not take the tables */
    if (err != ESP_OK) {
        for (size_t i = 0; i < count; i++) {
            heap_caps_free(tables[i]);
        }
        return err;
    }
    return ESP_OK;
}

static void flush_on_core(void *arg)
{
    /* A core keeps the satp value of the last U-mode code it ran until other U-mode code runs */
    const uint32_t released = (uint32_t)(uintptr_t)arg;
    if (released != 0 && riscv_sv32_read_satp() == released) {
        riscv_sv32_deactivate();
    }
    riscv_sv32_invalidate_all();
    /* Instruction fetch of this core sees the code written before */
    __asm__ volatile("fence.i" ::: "memory");
}

esp_err_t privilege_domain_address_space_flush_all_cores(const privilege_domain_address_space_t *released)
{
    void *arg = (void *)(uintptr_t)privilege_domain_address_space_satp(released);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 1
    flush_on_core(arg);
#else /* CONFIG_FREERTOS_NUMBER_OF_CORES == 1 */
    for (unsigned core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; ++core) {
        esp_err_t err = esp_ipc_call_blocking(core, flush_on_core, arg);
        if (err != ESP_OK) {
            return err;
        }
    }
#endif /* CONFIG_FREERTOS_NUMBER_OF_CORES == 1 */
    return ESP_OK;
}
