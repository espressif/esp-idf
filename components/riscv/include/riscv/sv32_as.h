/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "riscv/sv32.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Sv32 address space: page tables that map 4 KiB pages of U-mode virtual addresses to physical
 * addresses. Every leaf has the U bit set.
 *
 * The caller provides the root table, the second-level tables and the mapped memory, and frees
 * them once no core uses the address space. A table must be aligned to RISCV_SV32_TABLE_SIZE and
 * readable by the page-table walker. To map a range:
 *
 *     size_t count = riscv_sv32_as_count_tables_needed(as, vaddr, size);
 *     ... allocate count tables ...
 *     esp_err_t err = riscv_sv32_as_map(as, vaddr, paddr, size, permissions, tables, count);
 *     ... on failure, free the tables ...
 *
 * To free an address space, free every table that riscv_sv32_as_detach_table() returns, then the
 * root.
 *
 * No function takes a lock. The PTE writes of a function are ordered before it returns; only the
 * TLB flush is up to the caller. After a change to an address space that a core uses, that core
 * needs an SFENCE.VMA to be sure to see it; only the current core can be flushed from this
 * component.
 */

/** Sv32 address space. It is the root table it was initialized with. */
typedef struct riscv_sv32_as riscv_sv32_as_t;

/**
 * @brief Initialize an empty address space in a root table
 *
 * @param root Root table, zeroed by the call
 * @return Address space, or NULL if root is NULL or not aligned
 */
riscv_sv32_as_t *riscv_sv32_as_init(riscv_sv32_table_t *root);

/**
 * @brief Count the second-level tables that riscv_sv32_as_map() needs for a range
 *
 * The count is valid until the address space changes. A riscv_sv32_as_map() or
 * riscv_sv32_as_detach_table() in between can change it, and riscv_sv32_as_map() then rejects
 * the stale count with ESP_ERR_INVALID_ARG. Callers that change an address space from several
 * tasks must hold one lock across both calls.
 *
 * @param as Address space
 * @param vaddr Page-aligned virtual address
 * @param size Size in bytes, a nonzero multiple of RISCV_SV32_PAGE_SIZE, not wrapping
 * @return Number of tables to pass to riscv_sv32_as_map()
 */
size_t riscv_sv32_as_count_tables_needed(const riscv_sv32_as_t *as, uint32_t vaddr, size_t size);

/**
 * @brief Map whole pages at a virtual address to physical memory
 *
 * A range of which any page is already mapped is rejected. On success the address space owns
 * the tables until riscv_sv32_as_detach_table() returns them. On failure nothing is changed, and
 * the tables still belong to the caller.
 *
 * @param as Address space
 * @param vaddr Page-aligned virtual address
 * @param paddr Page-aligned physical address
 * @param size Size in bytes, a nonzero multiple of RISCV_SV32_PAGE_SIZE
 * @param permissions RISCV_SV32_PTE_R, RISCV_SV32_PTE_W and RISCV_SV32_PTE_X combined.
 *                    Not empty, and W requires R.
 * @param tables Second-level tables for the range, each zeroed by the call. May be NULL when
 *               count is 0.
 * @param count Number of tables, as returned by riscv_sv32_as_count_tables_needed()
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG for unaligned or wrapping ranges, invalid permissions, a NULL,
 *        unaligned or repeated table, the root table, or a count that is not the number of tables the range needs, for
 *        example because the address space changed since it was counted
 *      - ESP_ERR_INVALID_STATE if a page of the range is already mapped, or covered by a
 *        level-1 leaf that was not made by this API
 */
esp_err_t riscv_sv32_as_map(riscv_sv32_as_t *as, uint32_t vaddr, riscv_sv32_paddr_t paddr, size_t size,
                            uint32_t permissions, riscv_sv32_table_t *const tables[], size_t count);

/**
 * @brief Remove the mappings of whole pages
 *
 * Pages of the range that are not mapped are skipped. The tables stay in the address space.
 *
 * @param as Address space
 * @param vaddr Page-aligned virtual address
 * @param size Size in bytes, a nonzero multiple of RISCV_SV32_PAGE_SIZE
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG for unaligned or wrapping ranges
 */
esp_err_t riscv_sv32_as_unmap(riscv_sv32_as_t *as, uint32_t vaddr, size_t size);

/**
 * @brief Change the permissions of mapped pages
 *
 * Every page of the range must be mapped. On failure nothing is changed.
 *
 * @param as Address space
 * @param vaddr Page-aligned virtual address
 * @param size Size in bytes, a nonzero multiple of RISCV_SV32_PAGE_SIZE
 * @param permissions New permissions, as for riscv_sv32_as_map()
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG for unaligned or wrapping ranges, or invalid permissions
 *      - ESP_ERR_NOT_FOUND if a page of the range is not mapped
 */
esp_err_t riscv_sv32_as_protect(riscv_sv32_as_t *as, uint32_t vaddr, size_t size, uint32_t permissions);

/**
 * @brief Remove a second-level table from an address space
 *
 * Removes the mappings of the table's 4 MiB window. Call it only once no core uses the address
 * space.
 *
 * @param as Address space
 * @return The table, which the caller now owns, or NULL if the address space has no table left
 */
riscv_sv32_table_t *riscv_sv32_as_detach_table(riscv_sv32_as_t *as);

/**
 * @brief Get the satp value that selects an address space, with ASID 0
 *
 * @param as Address space
 * @return satp value to load before entering U-mode
 */
uint32_t riscv_sv32_as_satp(const riscv_sv32_as_t *as);

#ifdef __cplusplus
}
#endif
