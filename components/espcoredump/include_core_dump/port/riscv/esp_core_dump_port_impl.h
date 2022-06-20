/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_PORT_IMPL_H_
#define ESP_CORE_DUMP_PORT_IMPL_H_

/**
 * @file
 * @brief Core dump port interface implementation for RISC-V.
 */

#include "esp_core_dump_types.h"
#include "esp_app_format.h"

/**
 * @brief Chip ID associated to this implementation.
 */
#define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32C3

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the stack pointer to the address passed as a parameter.
 * @note This function must be inlined.
 *
 * @param new_sp New stack pointer to set in sp register.
 *
 * @return Former stack pointer address (sp register value).
 */
FORCE_INLINE_ATTR void* esp_core_dump_replace_sp(void* new_sp)
{
    void* current_sp = NULL;
    asm volatile ("mv %0, sp \n\t\
                   mv sp, %1 \n\t\
                  "
                   : "=&r"(current_sp)
                   : "r"(new_sp));
    return current_sp;
}

#ifdef __cplusplus
}
#endif

#endif
