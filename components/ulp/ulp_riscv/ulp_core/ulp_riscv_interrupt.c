/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "ulp_riscv_register_ops.h"
#include "ulp_riscv_interrupt.h"

#define ULP_RISCV_TIMER_INT                         (1 << 0U)   /* Internal Timer Interrupt */
#define ULP_RISCV_EBREAK_ECALL_ILLEGAL_INSN_INT     (1 << 1U)   /* EBREAK, ECALL or Illegal instruction */
#define ULP_RISCV_BUS_ERROR_INT                     (1 << 2U)   /* Bus Error (Unaligned Memory Access) */
#define ULP_RISCV_PERIPHERAL_INTERRUPT              (1 << 31U)  /* RTC Peripheral Interrupt */
#define ULP_RISCV_INTERNAL_INTERRUPT                (ULP_RISCV_TIMER_INT | ULP_RISCV_EBREAK_ECALL_ILLEGAL_INSN_INT | ULP_RISCV_BUS_ERROR_INT)

/* Interrupt handler structure */
typedef struct {
    intr_handler_t handler;
    void *arg;
} ulp_riscv_intr_handler_t;

/* Statically store all interrupt handlers */
static ulp_riscv_intr_handler_t s_intr_handlers[ULP_RISCV_MAX_INTR_SOURCE];

esp_err_t ulp_riscv_intr_alloc(ulp_riscv_interrupt_source_t source, intr_handler_t handler, void *arg)
{
    /* Check the validity of the interrupt source */
    if (source < 0 || source >= ULP_RISCV_MAX_INTR_SOURCE || handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Register interrupt handler */
    if (s_intr_handlers[source].handler == NULL) {
        s_intr_handlers[source].handler = handler;
        s_intr_handlers[source].arg = arg;
    } else {
        /* Error: The interrupt handler for this interrupt source has already been allocated */
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t ulp_riscv_intr_free(ulp_riscv_interrupt_source_t source)
{
    /* Check the validity of the interrupt source */
    if (source < 0 || source >= ULP_RISCV_MAX_INTR_SOURCE) {
        return ESP_ERR_INVALID_ARG;
    }

    /* De-register interrupt handler */
    if (s_intr_handlers[source].handler != NULL) {
        s_intr_handlers[source].handler = NULL;
        s_intr_handlers[source].arg = NULL;
    }

    return ESP_OK;
}

/* This is the global interrupt handler for ULP RISC-V.
 * It is called from ulp_riscv_vectors.S
 */
void __attribute__((weak)) _ulp_riscv_interrupt_handler(uint32_t q1)
{
    /* Call respective interrupt handlers based on the interrupt status in q1 */

    /* Internal Interrupts */
    if (q1 & ULP_RISCV_INTERNAL_INTERRUPT) {
        // TODO
    }

    /* External/Peripheral interrupts */
    if (q1 & ULP_RISCV_PERIPHERAL_INTERRUPT) {
        /* TODO: RTC Peripheral interrupts */

        /* TODO: RTC IO interrupts */

        /* TODO: RTC I2C interrupt */
    }
}
