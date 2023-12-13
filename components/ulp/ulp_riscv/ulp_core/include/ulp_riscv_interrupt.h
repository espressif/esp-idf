/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "riscv/interrupt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ULP RISC-V Interrupt sources */
typedef enum {
    ULP_RISCV_SW_INTR_SOURCE = 0,           /**< Interrupt triggered by SW */
    ULP_RISCV_MAX_INTR_SOURCE,              /**< Total number of ULP RISC-V interrupt sources */
} ulp_riscv_interrupt_source_t;

/**
 * @brief Allocate interrupt handler for a ULP RISC-V interrupt source
 *
 * @param source        ULP RISC-V interrupt source
 * @param handler       Interrupt handler
 * @param arg           Interrupt handler argument
 *
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_intr_alloc(ulp_riscv_interrupt_source_t source, intr_handler_t handler, void *arg);

/**
 * @brief Free ULP RISC-V interrupt handler
 *
 * @param source        ULP RISC-V interrupt source
 *
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_intr_free(ulp_riscv_interrupt_source_t source);

#ifdef __cplusplus
}
#endif
