/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/sv32.h"
#endif /* CONFIG_IDF_TARGET_ARCH_RISCV */

/**
 * @file
 * @brief U-mode domains, which run code in RISC-V user mode (U-mode)
 *
 * Runs code in U-mode, in a domain of its own.
 *
 * U-mode domain functions must be called from a task context.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief U-mode domain handle
 *
 * A domain runs code in U-mode. Its page tables map virtual addresses to the physical
 * memory that the domain code can access. The application creates a domain, maps memory into it region by
 * region, finalizes it, and then runs it from a task.
 */
typedef struct esp_privilege_umode_domain *esp_privilege_umode_domain_handle_t;

/** The domain code can read the mapped memory. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_READ    1u
/** The domain code can write the mapped memory. Requires ESP_PRIVILEGE_UMODE_DOMAIN_READ. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_WRITE   2u
/** The domain code can execute the mapped memory. Cannot be combined with ESP_PRIVILEGE_UMODE_DOMAIN_WRITE. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE 4u

#if CONFIG_IDF_TARGET_ARCH_RISCV
/** Page size. Virtual addresses, and the addresses and sizes of mapped memory, are multiples of it. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE RISCV_SV32_PAGE_SIZE

/** Highest end of mapped memory, as a virtual address, which is 0xfffff000. The page of
 *  ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE bytes at this address is reserved, because the entry function returns to it. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT (0u - ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE)
#endif /* CONFIG_IDF_TARGET_ARCH_RISCV */

/** Entry point and initial register values of the domain code, in virtual addresses */
typedef struct {
    uint32_t entry;          /**< Address of the entry function. Must be executable and 2-byte aligned, as RISC-V instructions are. */
    uint32_t stack;          /**< Lowest address of the stack. Must be 16-byte aligned, as the RISC-V calling convention requires. */
    uint32_t stack_size;     /**< Stack size in bytes. Must be a nonzero multiple of 16, so that the top of the stack is 16-byte aligned, as the RISC-V calling convention requires. */
    uint32_t global_pointer; /**< Initial gp value. */
    uint32_t thread_pointer; /**< Initial tp value. */
} esp_privilege_umode_domain_entry_t;

/** Reason why a run of the domain code ended */
typedef enum {
    ESP_PRIVILEGE_UMODE_DOMAIN_EXIT_RETURN, /**< The entry function returned. */
} esp_privilege_umode_domain_exit_reason_t;

/**
 * @brief Result of a run of the domain code
 *
 * esp_privilege_umode_domain_run() fills it in. It tells why the run ended and gives the data for that reason,
 * such as the value that the entry function returned.
 */
typedef struct {
    esp_privilege_umode_domain_exit_reason_t reason; /**< Exit reason of the run. */
    int32_t value;                      /**< Value returned by the entry function, for ESP_PRIVILEGE_UMODE_DOMAIN_EXIT_RETURN. */
} esp_privilege_umode_domain_result_t;

/**
 * @brief Create a domain
 *
 * Allocates the handle and the page tables of a new domain. esp_privilege_umode_domain_map() adds memory to it.
 *
 * @param[out] domain Handle of the new domain
 * @return
 *      - ESP_OK. The domain was created
 *      - ESP_ERR_INVALID_ARG. The pointer or the calling context is invalid
 *      - ESP_ERR_NO_MEM. Out of memory for the page tables
 */
esp_err_t esp_privilege_umode_domain_create(esp_privilege_umode_domain_handle_t *domain);

/**
 * @brief Map memory provided by the caller into the address space of a domain
 *
 * Makes physical memory accessible to the domain code at a virtual address, with the given permissions.
 * Call this function once for each region of the domain code, such as its code, its data and its stack,
 * before esp_privilege_umode_domain_finalize().
 *
 * The caller does four things with the memory it provides.
 *
 * - Allocates the memory page-aligned, for example with heap_caps_aligned_calloc(), which also clears it.
 * - Writes the code and data into it before esp_privilege_umode_domain_finalize().
 * - Uses each page for one region of one domain.
 * - Frees the memory after esp_privilege_umode_domain_destroy() returns ESP_OK.
 *
 * @note The memory is in internal RAM. Memory mapped with ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE is in internal RAM
 *       that the CPU can execute from.
 * @note Virtual address, memory and size are multiples of ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE, and a region is at most
 *       4 MiB. The region ends at or below ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT and is separate from the other regions
 *       of the domain. If the mapping fails, the domain is unchanged.
 *
 * @param[in] domain Domain handle
 * @param[in] virtual_address Virtual address at which the domain code accesses the physical memory
 * @param[in] memory Start of the physical memory
 * @param[in] size Size of the region in bytes
 * @param[in] permissions Combination of ESP_PRIVILEGE_UMODE_DOMAIN_READ, ESP_PRIVILEGE_UMODE_DOMAIN_WRITE and ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE.
 *            ESP_PRIVILEGE_UMODE_DOMAIN_WRITE requires ESP_PRIVILEGE_UMODE_DOMAIN_READ and cannot be combined with
 *            ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE.
 * @return
 *      - ESP_OK. The region was mapped successfully
 *      - ESP_ERR_INVALID_ARG. An address, the size, the memory, the permissions, an overlap or the calling
 *        context is invalid
 *      - ESP_ERR_INVALID_SIZE. The region is larger than 4 MiB
 *      - ESP_ERR_INVALID_STATE. The domain is already finalized
 *      - ESP_ERR_NO_MEM. Out of memory for the page tables
 */
esp_err_t esp_privilege_umode_domain_map(esp_privilege_umode_domain_handle_t domain, uint32_t virtual_address, void *memory,
                                         size_t size, uint32_t permissions);

/**
 * @brief Finish mapping a domain and set its entry point and stack
 *
 * Call after every region is mapped with esp_privilege_umode_domain_map(). It checks these conditions.
 *
 * - The entry point is executable.
 * - The stack is readable, writable and not executable.
 * - The page right below the stack is not writable, so that a stack overflow raises an exception.
 *
 * Afterwards the memory map of the domain is fixed, and the domain can run.
 *
 * @param[in] domain Domain handle
 * @param[in] start Entry point, stack and initial registers
 * @return
 *      - ESP_OK. The domain is ready to run
 *      - ESP_ERR_INVALID_ARG. The entry point, the stack, a mapping, a pointer or the calling context is invalid
 *      - ESP_ERR_INVALID_STATE. The domain is already finalized
 */
esp_err_t esp_privilege_umode_domain_finalize(esp_privilege_umode_domain_handle_t domain, const esp_privilege_umode_domain_entry_t *start);

/**
 * @brief Run the domain code on the calling task
 *
 * Calls the entry function with two arguments, and returns when the entry function returns.
 *
 * @note Do not delete the calling task until this function returns.
 *
 * @param[in] domain Finalized domain handle
 * @param[in] arg0 First argument of the entry function
 * @param[in] arg1 Second argument of the entry function
 * @param[out] result Result of the run. Must stay valid until this function returns.
 *
 * @return
 *      - ESP_OK. The entry function returned
 *      - ESP_ERR_INVALID_ARG. A pointer or the calling context is invalid
 *      - ESP_ERR_INVALID_STATE. The domain cannot run
 */
esp_err_t esp_privilege_umode_domain_run(esp_privilege_umode_domain_handle_t domain, uint32_t arg0, uint32_t arg1,
                                         esp_privilege_umode_domain_result_t *result);

/**
 * @brief Destroy a domain
 *
 * Removes the address translations of the domain and frees its handle and page tables. The caller must free
 * the mapped memory only after this function returns ESP_OK.
 *
 * @param[in] domain Domain handle. Invalid after ESP_OK.
 * @return
 *      - ESP_OK. The domain was destroyed
 *      - ESP_ERR_INVALID_ARG. The pointer or the calling context is invalid
 *      - Other. Flushing translations failed. The domain still exists and destruction can be retried.
 */
esp_err_t esp_privilege_umode_domain_destroy(esp_privilege_umode_domain_handle_t domain);

#ifdef __cplusplus
}
#endif
