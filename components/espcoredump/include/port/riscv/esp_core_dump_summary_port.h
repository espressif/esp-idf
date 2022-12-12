/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "sdkconfig.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

/**
 * @brief Backtrace information
 *
 * For RISCV, backtrace cannot be generated on device without including and parsing
 * DWARF sections. Including these sections would increase the binary size so provide
 * the stackdump that can be later used to generate backtrace with the help of GDB or by parsing the ELF file
 * on the host machine
 */
typedef struct {
    uint8_t stackdump[CONFIG_ESP_COREDUMP_SUMMARY_STACKDUMP_SIZE];    /*!< Stack dump of the crashing task. */
    uint32_t dump_size;                                               /*!< Size (in bytes) of the stack dump */
} esp_core_dump_bt_info_t;

/**
 * @brief RISC-V architecture specific extra information
 */
typedef struct {
    uint32_t mstatus;     /* Machine Status */
    uint32_t mtvec;       /* Machine Trap-Vector Base Address */
    uint32_t mcause;      /* Machine Trap Cause */
    uint32_t mtval;       /* Machine Trap Value */
    uint32_t ra;          /* Return Address */
    uint32_t sp;          /* Stack pointer */
    uint32_t exc_a[8];    /* A0-A7 registers when the exception caused */
} esp_core_dump_summary_extra_info_t;

#endif /* CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#ifdef __cplusplus
}
#endif
