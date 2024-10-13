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

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE

/* ULP RISC-V Interrupt sources */
typedef enum {
    ULP_RISCV_SW_INTR_SOURCE = 0,           /**< Interrupt triggered by SW */
    ULP_RISCV_RTCIO0_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 0 */
    ULP_RISCV_RTCIO1_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 1 */
    ULP_RISCV_RTCIO2_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 2 */
    ULP_RISCV_RTCIO3_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 3 */
    ULP_RISCV_RTCIO4_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 4 */
    ULP_RISCV_RTCIO5_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 5 */
    ULP_RISCV_RTCIO6_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 6 */
    ULP_RISCV_RTCIO7_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 7 */
    ULP_RISCV_RTCIO8_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 8 */
    ULP_RISCV_RTCIO9_INTR_SOURCE,           /**< Interrupt triggered by RTCIO 9 */
    ULP_RISCV_RTCIO10_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 10 */
    ULP_RISCV_RTCIO11_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 11 */
    ULP_RISCV_RTCIO12_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 12 */
    ULP_RISCV_RTCIO13_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 13 */
    ULP_RISCV_RTCIO14_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 14 */
    ULP_RISCV_RTCIO15_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 15 */
    ULP_RISCV_RTCIO16_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 16 */
    ULP_RISCV_RTCIO17_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 17 */
    ULP_RISCV_RTCIO18_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 18 */
    ULP_RISCV_RTCIO19_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 19 */
    ULP_RISCV_RTCIO20_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 20 */
    ULP_RISCV_RTCIO21_INTR_SOURCE,          /**< Interrupt triggered by RTCIO 21 */
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

#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

#ifdef __cplusplus
}
#endif
