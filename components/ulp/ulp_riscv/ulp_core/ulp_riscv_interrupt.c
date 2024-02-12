/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "include/ulp_riscv_interrupt.h"
#include "ulp_riscv_register_ops.h"
#include "ulp_riscv_interrupt.h"
#include "ulp_riscv_gpio.h"
#include "soc/sens_reg.h"

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE

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

/* This function -
 * - Checks RTC peripheral interrupt status bit
 * - Calls interrupt handler if it is registered
 * - Clears interrupt bit
 */
static inline void ulp_riscv_handle_rtc_periph_intr(uint32_t status)
{
    /* SW interrupt */
    if (status & SENS_COCPU_SW_INT_ST) {
        const ulp_riscv_intr_handler_t* entry = &s_intr_handlers[ULP_RISCV_SW_INTR_SOURCE];

        if (entry->handler) {
            entry->handler(entry->arg);
        }

        SET_PERI_REG_MASK(SENS_SAR_COCPU_INT_CLR_REG, SENS_COCPU_SW_INT_CLR);
    }
}

/* This function -
 * - Checks if one or more RTC IO interrupt status bits are set
 * - Calls the interrupt handler for the RTC IO if it is registered
 * - Clears all interrupt bits
 */
static inline void ulp_riscv_handle_rtc_io_intr(uint32_t status)
{
    uint32_t handler_idx = 0;
    for (int i = 0; i < GPIO_NUM_MAX; i++) {
        if (status & (1U << i)) {
            handler_idx = ULP_RISCV_RTCIO0_INTR_SOURCE + i;
            ulp_riscv_intr_handler_t* entry = &s_intr_handlers[handler_idx];

            if (entry->handler) {
                entry->handler(entry->arg);
            }
        }
    }

    REG_SET_FIELD(RTC_GPIO_STATUS_W1TC_REG, RTC_GPIO_STATUS_INT_W1TC, status);
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
        /* RTC Peripheral interrupts */
        uint32_t cocpu_int_st = READ_PERI_REG(SENS_SAR_COCPU_INT_ST_REG);
        if (cocpu_int_st) {
            ulp_riscv_handle_rtc_periph_intr(cocpu_int_st);
        }

        /* RTC IO interrupts */
        uint32_t rtcio_int_st = REG_GET_FIELD(RTC_GPIO_STATUS_REG, RTC_GPIO_STATUS_INT);
        if (rtcio_int_st) {
            ulp_riscv_handle_rtc_io_intr(rtcio_int_st);
        }

        /* TODO: RTC I2C interrupt */
    }
}

#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */
