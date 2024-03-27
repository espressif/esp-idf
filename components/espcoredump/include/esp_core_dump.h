/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_H_
#define ESP_CORE_DUMP_H_

#include "sdkconfig.h"
#include <stddef.h>
#include "esp_err.h"
#include "esp_private/panic_internal.h"
#include "esp_core_dump_summary_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_ELF_SHA256_SZ (CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1)

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

/**
 * @brief Core dump summary, Most meaningful contents of the core dump
 *        are accommodated in this structure
 */
typedef struct {
    uint32_t exc_tcb;                           /*!< TCB pointer to the task causing exception */
    char exc_task[16];                          /*!< Name of the task that caused exception */
    uint32_t exc_pc;                            /*!< Program counter for exception */
    esp_core_dump_bt_info_t exc_bt_info;        /*!< Backtrace information for task causing exception */
    uint32_t core_dump_version;                 /*!< Core dump version */
    uint8_t app_elf_sha256[APP_ELF_SHA256_SZ];  /*!< Crashing application's SHA256 sum as a string */
    esp_core_dump_summary_extra_info_t ex_info; /*!< Architecture specific extra data */
} esp_core_dump_summary_t;

#endif /* CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

/**************************************************************************************/
/******************************** EXCEPTION MODE API **********************************/
/**************************************************************************************/

/**
 * @brief  Initializes core dump module internal data.
 *
 * @note  Should be called at system startup.
 */
void esp_core_dump_init(void);

/**************************************************************************************/
/*********************************** USER MODE API ************************************/
/**************************************************************************************/

/**
 * Core dump file consists of header and data (in binary or ELF format) for every task in the system at the moment of crash.
 * For the data integrity, a checksum is used at the end of core the dump data.
 * The structure of core dump file is described below in details.
 * 1) Core dump starts with header:
 * 1.1) TOTAL_LEN is total length of core dump data in flash including the checksum. Size is 4 bytes.
 * 1.2) VERSION field keeps 4 byte version of core dump.
 * 1.2) TASKS_NUM is the number of tasks for which data are stored. Size is 4 bytes. Unused in ELF format
 * 1.3) TCB_SIZE is the size of task's TCB structure. Size is 4 bytes. Unused in ELF format
 * 1.4) MEM_SEG_NUM is the number of memory segment. Size is 4 bytes. Unused in ELF format
 * 1.5) CHIP_REV is the revision of the chip. Size is 4 bytes.
 * 2) Core dump header is followed by the data for every task in the system. Data part is differs for the binary
 *  and elf formats.
 * 2.1) The core dump file uses a subset of the ELF structures to store the crash information.
 *  Loadable ELF segments and ELF notes (ELF.PT_NOTE) used with a special name and type (CORE, NT_PRSTATUS type)
 * 2.2) In Binary format task data are started with the task header:
 * 2.2.1) TCB_ADDR is the address of TCB in memory. Size is 4 bytes.
 * 2.2.2) STACK_TOP is the top of task's stack (address of the topmost stack item). Size is 4 bytes.
 * 2.2.3) STACK_END is the end of task's stack (address from which task's stack starts). Size is 4 bytes.
 * 2.2.4) Task header is followed by TCB data. Size is TCB_SIZE bytes.
 * 2.2.5) Task's stack is placed after TCB data. Size is (STACK_END - STACK_TOP) bytes.
 * 2.3) The checksum is placed at the end of the data.
 * 3) The structure of the uart data is the same as the data stored in flash
 * 3.1) Uart data is printed in base64 format surrounded with special messages to help user recognize the start and
 * end of actual data.
 *
 * For more information about the implementation please check api-guides/core_dump_internals.rst
 *
 */

/**
 * @brief  Print/store coredump data to the selected destination uart or flash.
 *
 * @param  info  Pointer to the panic information. It contains the execution frame.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
void esp_core_dump_write(panic_info_t *info);

/**
 * @brief  Check integrity of coredump data in flash.
 *         This function reads the coredump data while calculating their checksum. If it
 *         doesn't match the checksum written on flash, it means data are corrupted,
 *         an error will be returned. Else, ESP_OK is returned.
 *
 * @return `ESP_OK` if core dump is present and valid, `ESP_ERR_NOT_FOUND` if no core dump
 *         is stored in the partition, `ESP_ERR_INVALID_SIZE` or `ESP_ERR_INVALID_CRC`
 *         if the core dump is corrupted, other errors when unable to access flash, in that
 *         case please refer to \see esp_err_t
 */
esp_err_t esp_core_dump_image_check(void);

/**
 * @brief  Retrieves address and size of coredump data in flash.
 *         This function is always available, even when core dump is disabled in menuconfig.
 *
 * @param  out_addr   pointer to store image address in flash.
 * @param  out_size   pointer to store image size in flash (including checksum). In bytes.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
esp_err_t esp_core_dump_image_get(size_t* out_addr, size_t *out_size);

/**
 * @brief  Erases coredump data in flash. esp_core_dump_image_get() will then return
 *         ESP_ERR_NOT_FOUND. Can be used after a coredump has been transmitted successfully.
 *         This function is always available, even when core dump is disabled in menuconfig.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
esp_err_t esp_core_dump_image_erase(void);

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

/**
 * @brief Get panic reason from the core dump.
 *
 * This function retrieves the panic reason from the core dump data and copies it to the provided buffer.
 *
 * @param[in,out] reason_buffer Pointer to the buffer where the panic reason will be copied.
 * @param[in] buffer_size Size of the destination buffer in bytes.
 * @return
 *     - ESP_OK if the panic reason was successfully copied.
 *     - ESP_ERR_INVALID_ARG if reason_buffer is NULL or buffer_size is 0.
 *     - Other error codes indicating the outcome of the core dump retrieval.
 *     - ESP_ERR_NOT_FOUND if the panic reason is not found in the core dump.
 *
 * Example usage:
 * @code{c}
    char panic_reason[200];
    esp_err_t err = esp_core_dump_get_panic_reason(panic_reason, sizeof(panic_reason));
    if (err == ESP_OK) {
        ESP_LOGW(TAG, "%s", panic_reason);
    }
 * @endcode
 */
esp_err_t esp_core_dump_get_panic_reason(char *reason_buffer, size_t buffer_size);

/**
 * @brief  Get the summary of a core dump.
 *
 * @param  summary   Summary of the core dump
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 *
 * @note  This function works only if coredump is stored in flash and in ELF format
 *
 * Example usage:
 * @code{c}
 *  esp_core_dump_summary_t *summary = malloc(sizeof(esp_core_dump_summary_t));
 *  if (summary) {
 *      if (esp_core_dump_get_summary(summary) == ESP_OK) {
 *          // Do stuff
 *      }
 *  }
 *  free(summary);
 * @endcode
 */
esp_err_t esp_core_dump_get_summary(esp_core_dump_summary_t *summary);

#endif /* CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#ifdef __cplusplus
}
#endif

#endif
