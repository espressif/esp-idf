/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Page tables that give a U-mode domain its own address space */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "riscv/sv32_as.h"

/** Sv32 page tables of a domain, in internal RAM. The memory they map is provided by the caller that mapped it. */
typedef riscv_sv32_as_t privilege_domain_address_space_t;

/** Free the page tables. */
void privilege_domain_address_space_delete(privilege_domain_address_space_t *as);

/**
 * @brief Get the satp value activating an address space
 *
 * @param as Address space, or NULL
 * @return satp value, 0 (Bare) if as is NULL
 */
uint32_t privilege_domain_address_space_satp(const privilege_domain_address_space_t *as);

/** Create an empty address space. */
esp_err_t privilege_domain_address_space_create(privilege_domain_address_space_t **out);

/**
 * @brief Map whole pages at va to the memory at backing
 *
 * The address space must not be in use by any core. On failure nothing is mapped, and the tables
 * that the call added are freed.
 *
 * @param as Address space
 * @param va Page-aligned user address
 * @param size Size in bytes, a nonzero multiple of the page size
 * @param backing Page-aligned address of the memory
 * @param flags Combination of ESP_PRIVILEGE_UMODE_DOMAIN_READ, ESP_PRIVILEGE_UMODE_DOMAIN_WRITE and ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE
 * @return
 *      - ESP_OK. The range is mapped
 *      - ESP_ERR_INVALID_ARG or ESP_ERR_INVALID_STATE. The range is invalid or already mapped
 *      - ESP_ERR_INVALID_SIZE. The range needs more than two page tables
 *      - ESP_ERR_NO_MEM. A page table could not be allocated
 */
esp_err_t privilege_domain_address_space_map(privilege_domain_address_space_t *as, uint32_t va, size_t size,
                                             uintptr_t backing, uint32_t flags);

/**
 * @brief Check that a range of user addresses is mapped with at least the given permissions
 *
 * Reads the page tables, so it also finds memory mapped by several calls of
 * privilege_domain_address_space_map().
 *
 * @param as Address space
 * @param address First user address of the range
 * @param size Size of the range in bytes, at least 1
 * @param flags Combination of ESP_PRIVILEGE_UMODE_DOMAIN_READ, ESP_PRIVILEGE_UMODE_DOMAIN_WRITE and ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE, or 0 for
 *        mapped with any permissions
 * @return true if every page of the range is mapped for U-mode with the permissions
 */
bool privilege_domain_address_space_check(const privilege_domain_address_space_t *as, uint32_t address, size_t size,
                                          uint32_t flags);

/**
 * @brief Flush address translations and instruction fetch on all cores
 *
 * Call after the page tables or the executable memory of an address space changed, before U-mode
 * code runs with it. Call it with released before freeing the page tables or the mapped memory of
 * an address space that no core runs any more. Then a core whose satp still points to its page
 * tables gets satp 0. On failure, keep both allocated.
 *
 * @param released Address space that is about to be freed, or NULL
 */
esp_err_t privilege_domain_address_space_flush_all_cores(const privilege_domain_address_space_t *released);
