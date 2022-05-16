/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/////////////////////////////////////////////////////////////////////////////////////////
// ESP Memory Protection API (PMS)
// - allows configuration and violation-interrupt handling of the PMS module operations
// - not intended for public use.

#pragma once

#include "sdkconfig.h"
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE || CONFIG_ESP_SYSTEM_MEMPROT_TEST

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_memprot_err.h"
#include "soc_memprot_types.h"
#include "esp_memprot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_MEMPROT_ERR_CHECK(retval, fnc) if ((retval=fnc) != ESP_OK) { return retval; }

/**
* @brief Basic PMS interrupt source info
*/
typedef struct {
    esp_mprot_mem_t mem_type;   /*!< Memory type containing the faulting address  */
    int core;                   /*!< CPU/Core ID running the faulting instruction  */
} esp_memp_intr_source_t;

/**
 * @brief Clears current interrupt ON flag for given Memory type and CPU/Core ID
 *
 * This operation is non-atomic for some chips by PMS module design
 * In such a case the interrupt clearing happens in two steps:
 *      1. Interrupt CLR flag is set (clears interrupt-ON status and inhibits linked interrupt processing)
 *      2. Interrupt CLR flag is reset (resumes the interrupt monitoring)
 *
 * @param mem_type Memory type (see esp_mprot_mem_t enum)
 * @param core Target CPU/Core ID (see *_CPU_NUM defs in soc.h). Can be NULL on 1-CPU systems
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on passing invalid pointer
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_monitor_clear_intr(const esp_mprot_mem_t mem_type, int const *const core);

/**
 * @brief Checks whether any of the PMS settings is locked
 *
 * @param[out] locked Any lock on? (true/false)
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on invalid locked ptr
 *         Other failures: error code of any failing esp_mprot_get_*_lock() routine (called internally)
 */
esp_err_t esp_mprot_is_conf_locked_any(bool *locked);

/**
 * @brief Checks whether any PMS violation-interrupt monitoring is enabled
 *
 * @param[out] locked Any PMS violation interrupt monitor is enabled (true/false)
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on invalid enabled ptr
 *         Other failures: error code of esp_mprot_get_monitor_en() routine (called internally for all Memory types)
 */
esp_err_t esp_mprot_is_intr_ena_any(bool *enabled);

/**
 * @brief Returns active PMS violation-interrupt Memory type if any (MEMPROT_TYPE_NONE when none detected)
 * and the CPU/CoreID which was running the faulty code (-1 when no interrupt available)
 *
 * If there are more interrupts indicated on (shouldn't happen), the order of precedence is given by 'esp_mprot_mem_t' enum definition (low->high)
 *
 * @param[out] mem_type Out-pointer for Memory type given by the faulting address (see esp_mprot_mem_t enum)
 * @param[out] core Out-pointer for CPU/Core ID (see *_CPU_NUM defs in soc.h)
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on passing invalid pointer(s)
 */
esp_err_t esp_mprot_get_active_intr(esp_memp_intr_source_t *active_memp_intr);

/**
 * @brief Returns the address which caused the violation interrupt for given Memory type and CPU/Core ID.
 * This function is to be called after a basic resolving of (current) interrupt's parameters (ie corresponding
 * Memory type and CPU ID see esp_mprot_get_active_intr()). This is to minimize processing time of actual exception
 * as this API is typicaly used in a panic-handling code.
 * If there is no active interrupt available for the Memory type/CPU ID required, fault_addr is set to NULL.
 *
 * @param mem_type memory type
 * @param[out] fault_addr Address of the operation which caused the PMS violation interrupt
 * @param core Faulting instruction CPU/Core ID (see *_CPU_NUM defs in soc.h). Can be NULL on 1-CPU systems
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARG on invalid fault_addr pointer
 */
esp_err_t esp_mprot_get_violate_addr(const esp_mprot_mem_t mem_type, void **fault_addr, int const *const core);

/**
 * @brief Returns PMS World identifier of the code causing the violation interrupt
 *
 * The value is read from appropriate PMS violation status register and thus might be 0 if the interrupt is not currently active.
 *
 * @param mem_type Memory type
 * @param[out] world PMS World type (see esp_mprot_pms_world_t)
 * @param core Faulting instruction CPU/Core ID (see *_CPU_NUM defs in soc.h). Can be NULL on 1-CPU systems
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARG on passing invalid pointer(s)
 *         ESP_ERR_MEMPROT_WORLD_INVALID on invalid World identifier fetched from the register
 */
esp_err_t esp_mprot_get_violate_world(const esp_mprot_mem_t mem_type, esp_mprot_pms_world_t *world, int const *const core);

/**
 * @brief Returns an operation type which caused the violation interrupt
 *
 * The operation resolving is processed over various PMS status register flags, according to given Memory type argument.
 * If the interrupt is not active the result returned is irrelevant (likely evaluated to MEMPROT_OP_READ).
 *
 * @param mem_type Memory type
 * @param[out] oper Operation type (see MEMPROT_OP_* defines)
 * @param core Faulting instruction CPU/Core ID (see *_CPU_NUM defs in soc.h). Can be NULL on 1-CPU systems
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARG on invalid oper pointer
 */
esp_err_t esp_mprot_get_violate_operation(const esp_mprot_mem_t mem_type, uint32_t *oper, int const *const core);

/**
 * @brief Checks whether given memory type supports byte-enables info
 *
 * Byte-enables status is available only for DMA/DRAM operations
  *
 * @param mem_type memory type
 *
 * @return byte-enables info available true/false
 */
bool esp_mprot_has_byte_enables(const esp_mprot_mem_t mem_type);

/**
 * @brief Returns byte-enables for the address which caused the violation interrupt
 *
 * The value is taken from appropriate PMS violation status register, based on given Memory type
 *
 * @param mem_type Memory type (MEMPROT_TYPE_DRAM0_SRAM)
 * @param[out] byte_en Byte-enables bits
 * @param core Faulting instruction CPU/Core ID (see *_CPU_NUM defs in soc.h). Can be NULL on 1-CPU systems
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARGUMENT on invalid byte_en pointer
 */
esp_err_t esp_mprot_get_violate_byte_enables(const esp_mprot_mem_t mem_type, uint32_t *byte_en, int const *const core);

/**
 * @brief Convenient routine for setting the PMS defaults
 *
 * Called on system startup, depending on ESP_SYSTEM_MEMPROT_FEATURE Kconfig value
 *
 * @param memp_config pointer to Memprot configuration structure (esp_memp_config_t). The structure si chip-specific,
 * for details and defaults see appropriate [target-chip]/soc_memprot_types.h
 *
 * @return ESP_OK on success
 *         Other failures: error code of the failing routine called internally. No specific error processing provided in such a case
 *         due to large number of embedded calls (ie no global unique error table is provided and thus one error code can have different meanings,
 *         depending on the routine issuing the error)
 */
esp_err_t esp_mprot_set_prot(const esp_memp_config_t *memp_config);

/**
 * @brief Generates PMS configuration string of actual device (diagnostics)
 *
 * The functions generates a string from current configuration, control and status registers of the PMS (or similar) module of actual device.
 * The values are fetched using HAL LL calls to help finding possible errors in the Memprot API implementation
 *
 * @param[out] dump_info_string configuration string buffer pointer. The string is allocated by the callee and must be freed by the caller.
 *
 * @return ESP_OK on success
 *         ESP_ERR_NO_MEM on buffer allocation failure
 *         ESP_ERR_INVALID_ARGUMENT on invalid dump_info_string pointer
 */
esp_err_t esp_mprot_dump_configuration(char **dump_info_string);

#ifdef __cplusplus
}
#endif

#endif //CONFIG_ESP_SYSTEM_MEMPROT_FEATURE || CONFIG_ESP_SYSTEM_MEMPROT_TEST
