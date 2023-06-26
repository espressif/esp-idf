/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* This file contains definitions that are common between esp32/ulp.h,
   esp32s2/ulp.h and esp32s3/ulp.h
*/

#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@{*/
#define ESP_ERR_ULP_BASE                0x1200                  /*!< Offset for ULP-related error codes */
#define ESP_ERR_ULP_SIZE_TOO_BIG        (ESP_ERR_ULP_BASE + 1)  /*!< Program doesn't fit into RTC memory reserved for the ULP */
#define ESP_ERR_ULP_INVALID_LOAD_ADDR   (ESP_ERR_ULP_BASE + 2)  /*!< Load address is outside of RTC memory reserved for the ULP */
#define ESP_ERR_ULP_DUPLICATE_LABEL     (ESP_ERR_ULP_BASE + 3)  /*!< More than one label with the same number was defined */
#define ESP_ERR_ULP_UNDEFINED_LABEL     (ESP_ERR_ULP_BASE + 4)  /*!< Branch instructions references an undefined label */
#define ESP_ERR_ULP_BRANCH_OUT_OF_RANGE (ESP_ERR_ULP_BASE + 5)  /*!< Branch target is out of range of B instruction (try replacing with BX) */
/**@}*/

union ulp_insn;  // Declared in the chip-specific ulp.h header

typedef union ulp_insn ulp_insn_t;

/**
 * @brief Register ULP wakeup signal ISR
 *
 * @note The ISR routine will only be active if the main CPU is not in deepsleep
 *
 * @param fn    ISR callback function
 * @param arg   ISR callback function arguments
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if callback function is NULL
 *      - ESP_ERR_NO_MEM if heap memory cannot be allocated for the interrupt
 */
esp_err_t ulp_isr_register(intr_handler_t fn, void *arg);

/**
 * @brief Deregister ULP wakeup signal ISR
 *
 * @param fn    ISR callback function
 * @param arg   ISR callback function arguments
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if callback function is NULL
 *      - ESP_ERR_INVALID_STATE if a handler matching both callback function and its arguments isn't registered
 */
esp_err_t ulp_isr_deregister(intr_handler_t fn, void *arg);

/**
 * @brief Resolve all macro references in a program and load it into RTC memory
 * @param load_addr  address where the program should be loaded, expressed in 32-bit words
 * @param program  ulp_insn_t array with the program
 * @param psize  size of the program, expressed in 32-bit words
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if auxiliary temporary structure can not be allocated
 *      - one of ESP_ERR_ULP_xxx if program is not valid or can not be loaded
 */
esp_err_t ulp_process_macros_and_load(uint32_t load_addr, const ulp_insn_t* program, size_t* psize);

/**
 * @brief Load ULP program binary into RTC memory
 *
 * ULP program binary should have the following format (all values little-endian):
 *
 * 1. MAGIC, (value 0x00706c75, 4 bytes)
 * 2. TEXT_OFFSET, offset of .text section from binary start (2 bytes)
 * 3. TEXT_SIZE, size of .text section (2 bytes)
 * 4. DATA_SIZE, size of .data section (2 bytes)
 * 5. BSS_SIZE, size of .bss section (2 bytes)
 * 6. (TEXT_OFFSET - 12) bytes of arbitrary data (will not be loaded into RTC memory)
 * 7. .text section
 * 8. .data section
 *
 * Linker script in components/ulp/ld/esp32.ulp.ld produces ELF files which
 * correspond to this format. This linker script produces binaries with load_addr == 0.
 *
 * @param load_addr address where the program should be loaded, expressed in 32-bit words
 * @param program_binary pointer to program binary
 * @param program_size size of the program binary
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if load_addr is out of range
 *      - ESP_ERR_INVALID_SIZE if program_size doesn't match (TEXT_OFFSET + TEXT_SIZE + DATA_SIZE)
 *      - ESP_ERR_NOT_SUPPORTED if the magic number is incorrect
 */
esp_err_t ulp_load_binary(uint32_t load_addr, const uint8_t* program_binary, size_t program_size);

/**
 * @brief Run the program loaded into RTC memory
 * @param entry_point entry point, expressed in 32-bit words
 * @return  ESP_OK on success
 */
esp_err_t ulp_run(uint32_t entry_point);

#ifdef __cplusplus
}
#endif
