/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "riscv/encoding.h"
#include "riscv/sv32.h"
#include "riscv/sv32_as.h"

/* Number of virtual pages covered by one second-level table */
#define PAGES_PER_TABLE  RISCV_SV32_PTE_COUNT

/* An address space is its root table */
static riscv_sv32_table_t *root_of(const riscv_sv32_as_t *as)
{
    return (riscv_sv32_table_t *)as;
}

static riscv_sv32_table_t *pte_table(riscv_sv32_pte_t pte)
{
    return (riscv_sv32_table_t *)(uintptr_t)riscv_sv32_pte_get_paddr_base(pte);
}

static bool table_is_aligned(const riscv_sv32_table_t *table)
{
    return ((uintptr_t)table & (RISCV_SV32_TABLE_SIZE - 1U)) == 0;
}

/* Leaf PTE of a page, or NULL if there is no second-level table for it */
static riscv_sv32_pte_t *leaf_pte(const riscv_sv32_as_t *as, uint32_t page)
{
    const riscv_sv32_pte_t l1 = root_of(as)->entries[RISCV_SV32_VPN1(page)];
    return riscv_sv32_pte_is_table(l1) ? &pte_table(l1)->entries[RISCV_SV32_VPN0(page)] : NULL;
}

static bool range_is_valid(uint32_t vaddr, size_t size)
{
    return size != 0 && (size & (RISCV_SV32_PAGE_SIZE - 1U)) == 0 && (vaddr & (RISCV_SV32_PAGE_SIZE - 1U)) == 0 &&
           size - 1 <= (size_t)(UINT32_MAX - vaddr);
}

/* Index in the root table of the last window of a valid range */
static uint32_t last_vpn1(uint32_t vaddr, size_t size)
{
    return RISCV_SV32_VPN1(vaddr + (uint32_t)(size - 1));
}

static bool permissions_are_valid(uint32_t permissions)
{
    return permissions != 0 && (permissions & ~RISCV_SV32_PTE_LEAF_MASK) == 0 &&
           ((permissions & RISCV_SV32_PTE_W) == 0 || (permissions & RISCV_SV32_PTE_R) != 0);
}

riscv_sv32_as_t *riscv_sv32_as_init(riscv_sv32_table_t *root)
{
    if (root == NULL || !table_is_aligned(root)) {
        return NULL;
    }
    memset(root, 0, sizeof(*root));
    return (riscv_sv32_as_t *)root;
}

size_t riscv_sv32_as_count_tables_needed(const riscv_sv32_as_t *as, uint32_t vaddr, size_t size)
{
    assert(as != NULL && range_is_valid(vaddr, size));
    size_t count = 0;
    for (uint32_t vpn1 = RISCV_SV32_VPN1(vaddr); vpn1 <= last_vpn1(vaddr, size); vpn1++) {
        if (!riscv_sv32_pte_is_valid(root_of(as)->entries[vpn1])) {
            count++;
        }
    }
    return count;
}

esp_err_t riscv_sv32_as_map(riscv_sv32_as_t *as, uint32_t vaddr, riscv_sv32_paddr_t paddr, size_t size,
                            uint32_t permissions, riscv_sv32_table_t *const tables[], size_t count)
{
    if (as == NULL || !range_is_valid(vaddr, size) || !permissions_are_valid(permissions) ||
            (paddr & (RISCV_SV32_PAGE_SIZE - 1U)) != 0 || paddr > RISCV_SV32_PADDR_LIMIT - size ||
            (count != 0 && tables == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < count; i++) {
        if (tables[i] == NULL || !table_is_aligned(tables[i]) || tables[i] == root_of(as)) {
            return ESP_ERR_INVALID_ARG;
        }
        /* A table linked into two windows would map each window's pages in the other */
        for (size_t j = 0; j < i; j++) {
            if (tables[j] == tables[i]) {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }
    /* Check every window and page before changing anything */
    size_t missing = 0;
    for (uint32_t vpn1 = RISCV_SV32_VPN1(vaddr); vpn1 <= last_vpn1(vaddr, size); vpn1++) {
        const riscv_sv32_pte_t l1 = root_of(as)->entries[vpn1];
        if (!riscv_sv32_pte_is_valid(l1)) {
            missing++;
        } else if (riscv_sv32_pte_is_leaf(l1)) {
            /* A 4 MiB page, which only a change outside this API can make */
            return ESP_ERR_INVALID_STATE;
        }
    }
    if (missing != count) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint32_t pages = size / RISCV_SV32_PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        const riscv_sv32_pte_t *pte = leaf_pte(as, vaddr + i * RISCV_SV32_PAGE_SIZE);
        /* The page is already mapped */
        if (pte != NULL && riscv_sv32_pte_is_valid(*pte)) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    size_t next = 0;
    for (uint32_t vpn1 = RISCV_SV32_VPN1(vaddr); vpn1 <= last_vpn1(vaddr, size); vpn1++) {
        riscv_sv32_pte_t *l1 = &root_of(as)->entries[vpn1];
        if (!riscv_sv32_pte_is_valid(*l1)) {
            memset(tables[next], 0, sizeof(*tables[next]));
            /* A core using the address space may walk the table as soon as it is linked */
            riscv_sv32_fence_pte_writes();
            *l1 = riscv_sv32_pte_make_table((uintptr_t)tables[next++]);
        }
    }
    for (uint32_t i = 0; i < pages; i++) {
        riscv_sv32_pte_t *pte = leaf_pte(as, vaddr + i * RISCV_SV32_PAGE_SIZE);
        /* Every window has a table now */
        assert(pte != NULL);
        *pte = riscv_sv32_pte_make_leaf(paddr + (riscv_sv32_paddr_t)i * RISCV_SV32_PAGE_SIZE,
                                        permissions | RISCV_SV32_PTE_U);
    }
    riscv_sv32_fence_pte_writes();
    return ESP_OK;
}

esp_err_t riscv_sv32_as_unmap(riscv_sv32_as_t *as, uint32_t vaddr, size_t size)
{
    if (as == NULL || !range_is_valid(vaddr, size)) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint32_t pages = size / RISCV_SV32_PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        const uint32_t page = vaddr + i * RISCV_SV32_PAGE_SIZE;
        riscv_sv32_pte_t *pte = leaf_pte(as, page);
        if (pte == NULL) {
            /* Skip the rest of the range that the missing table would cover */
            i += PAGES_PER_TABLE - 1 - RISCV_SV32_VPN0(page);
            continue;
        }
        riscv_sv32_pte_clear(pte);
    }
    riscv_sv32_fence_pte_writes();
    return ESP_OK;
}

esp_err_t riscv_sv32_as_protect(riscv_sv32_as_t *as, uint32_t vaddr, size_t size, uint32_t permissions)
{
    if (as == NULL || !range_is_valid(vaddr, size) || !permissions_are_valid(permissions)) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint32_t pages = size / RISCV_SV32_PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        const riscv_sv32_pte_t *pte = leaf_pte(as, vaddr + i * RISCV_SV32_PAGE_SIZE);
        if (pte == NULL || !riscv_sv32_pte_is_leaf(*pte)) {
            return ESP_ERR_NOT_FOUND;
        }
    }
    for (uint32_t i = 0; i < pages; i++) {
        riscv_sv32_pte_t *pte = leaf_pte(as, vaddr + i * RISCV_SV32_PAGE_SIZE);
        *pte = riscv_sv32_pte_make_leaf(riscv_sv32_pte_get_paddr_base(*pte), permissions | RISCV_SV32_PTE_U);
    }
    riscv_sv32_fence_pte_writes();
    return ESP_OK;
}

riscv_sv32_table_t *riscv_sv32_as_detach_table(riscv_sv32_as_t *as)
{
    assert(as != NULL);
    for (uint32_t vpn1 = 0; vpn1 < RISCV_SV32_PTE_COUNT; vpn1++) {
        riscv_sv32_pte_t *l1 = &root_of(as)->entries[vpn1];
        if (riscv_sv32_pte_is_table(*l1)) {
            riscv_sv32_table_t *table = pte_table(*l1);
            riscv_sv32_pte_clear(l1);
            riscv_sv32_fence_pte_writes();
            return table;
        }
    }
    return NULL;
}

uint32_t riscv_sv32_as_satp(const riscv_sv32_as_t *as)
{
    assert(as != NULL);
    return riscv_sv32_make_satp((uintptr_t)root_of(as));
}
