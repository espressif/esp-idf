/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Loads U-mode programs that umode_elf_add_program() built into a domain: RISC-V ELF shared objects whose
 * only relocations are R_RISCV_RELATIVE. Every value read from the file is checked before memory is
 * allocated, so a damaged file is refused.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_privilege_umode_domain.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Most loadable segments that a program can have */
#define UMODE_ELF_MAX_SEGMENTS 8

/** A program loaded with umode_elf_load() */
typedef struct {
    uint32_t entry;                        /**< Virtual address of the entry point */
    uint32_t end;                          /**< Virtual address of the end of the last mapped page */
    void *memory[UMODE_ELF_MAX_SEGMENTS];  /**< Memory of each segment. umode_elf_free() frees it */
    size_t count;                          /**< Entries of memory in use */
} umode_elf_image_t;

/**
 * @brief Load a program into a domain
 *
 * Allocates page-aligned internal RAM for each loadable segment, copies the segment into it, applies the
 * relocations, and maps it at base plus the address of the segment in the file, with the permissions of
 * the segment. Call before esp_privilege_umode_domain_finalize().
 *
 * @param[in] domain Domain that is not finalized yet
 * @param[in] file ELF file
 * @param[in] size Size of the file in bytes
 * @param[in] base Virtual address to load the program at. A multiple of ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE.
 * @param[out] image Entry point, end and memory of the loaded program
 * @return
 *      - ESP_OK. Free the memory with umode_elf_free() after esp_privilege_umode_domain_destroy()
 *      - ESP_ERR_INVALID_ARG if a pointer is NULL or base is not page-aligned
 *      - ESP_ERR_INVALID_SIZE or ESP_ERR_NOT_SUPPORTED if the file is not a program that can be loaded
 *      - ESP_ERR_NO_MEM if the memory for a segment cannot be allocated
 *      - An error of esp_privilege_umode_domain_map(). Some segments may be mapped: destroy the domain, then
 *        call umode_elf_free()
 *
 * For every error, image holds only the memory that is mapped into the domain, so destroying the domain and
 * then calling umode_elf_free() is always correct.
 */
esp_err_t umode_elf_load(esp_privilege_umode_domain_handle_t domain, const void *file, size_t size, uint32_t base,
                         umode_elf_image_t *image);

/**
 * @brief Free the memory of a loaded program
 *
 * Call only after esp_privilege_umode_domain_destroy() returned ESP_OK for the domain that the program was loaded
 * into. Clears image.
 */
void umode_elf_free(umode_elf_image_t *image);

#ifdef __cplusplus
}
#endif
