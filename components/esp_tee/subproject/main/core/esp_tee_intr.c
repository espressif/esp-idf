/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_cpu.h"

#include "hal/cpu_ll.h"

#include "soc/periph_defs.h"
#include "soc/interrupts.h"
#include "soc/interrupt_reg.h"

#include "esp_tee.h"
#include "esp_tee_intr.h"
#include "esp_tee_intr_defs.h"
#include "esp_tee_rv_utils.h"

#define INTR_SET_SIZE       (32)
#define INTR_SET_COUNT      ((((ETS_MAX_INTR_SOURCE) + ((INTR_SET_SIZE) - 1)) & ~((INTR_SET_SIZE) - 1)) / INTR_SET_SIZE)

static const char *TAG = "esp_tee_intr";

typedef struct tee_handler_table_entry {
    intr_handler_t handler;
    void *arg;
} tee_handler_table_entry;

static tee_handler_table_entry tee_interrupt_table[ETS_MAX_INTR_SOURCE * portNUM_PROCESSORS];

static uint32_t protected_sources[INTR_SET_COUNT];

bool esp_tee_is_intr_src_protected(int source)
{
    uint32_t base = source / INTR_SET_SIZE;
    uint32_t offset = source % INTR_SET_SIZE;

    return (protected_sources[base] & (1 << offset));
}

void esp_tee_protect_intr_src(int source)
{
    uint32_t base = source / INTR_SET_SIZE;
    uint32_t offset = source % INTR_SET_SIZE;

    protected_sources[base] |= (1 << offset);
}

/* Default handler for unhandled interrupts */
void tee_unhandled_interrupt(void *arg)
{
    ESP_LOGE(TAG, "Unhandled interrupt %d on cpu %d!", (int)arg, esp_cpu_get_core_id());
}

/* Interrupt Matrix configuration API to call from non-secure world */
void esp_tee_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num)
{
    if (esp_tee_is_intr_src_protected(model_num) || intr_num == TEE_SECURE_INUM) {
        return;
    }
#if SOC_INT_CLIC_SUPPORTED
    if (intr_num == TEE_PASS_INUM) {
        return;
    }
#endif

    esp_rom_route_intr_matrix(cpu_no, model_num, intr_num);
    ESP_LOGV(TAG, "Connected src %d to int %d (cpu %d)", model_num, intr_num, cpu_no);

#if SOC_INT_CLIC_SUPPORTED
    if (intr_num != ETS_T1_WDT_INUM && intr_num != ETS_CACHEERR_INUM && intr_num != ETS_ASSIST_DEBUG_INUM) {
        /* Configure interrupts to be visible in the TEE (M-mode) */
        REG_SET_BIT(DR_REG_INTMTX_BASE + 4 * model_num, BIT(8));
        /* Configure interrupts to be serviced in the REE (U-mode) */
        rv_utils_tee_intr_set_mode(intr_num, PRV_U);
    }
#endif
}

/**
 * This function registers a handler for the specified interrupt. The "arg"
 * parameter specifies the argument to be passed to the handler when it is
 * invoked. The function returns the address of the previous handler.
 * On error, it returns 0.
 */
static intr_handler_t tee_set_interrupt_handler(void *arg)
{
    tee_handler_table_entry *entry;
    intr_handler_t old;
    vector_desc_t *vd = (vector_desc_t *)arg;
    int source = vd->source;

    if (source < 0 || source >= ETS_MAX_INTR_SOURCE) {
        return 0;    /* invalid interrupt source */
    }

    /* Convert exception number to _xt_exception_table name */
    source = source * portNUM_PROCESSORS + esp_cpu_get_core_id();

    entry = tee_interrupt_table + source;
    old   = entry->handler;

    if (vd->isr) {
        entry->handler = vd->isr;
        entry->arg     = vd->arg;
    } else {
        entry->handler = &tee_unhandled_interrupt;
        entry->arg     = (void *)source;
    }

    return ((old == &tee_unhandled_interrupt) ? 0 : old);
}

int esp_tee_intr_register(void *arg)
{
    int cpu = esp_cpu_get_core_id();
    struct vector_desc_t *vd = (struct vector_desc_t *)arg;

    tee_set_interrupt_handler(vd);
    esp_rom_route_intr_matrix(cpu, vd->source, TEE_SECURE_INUM);

    return 0;
}

int esp_tee_intr_deregister(void *arg)
{
    int cpu = esp_cpu_get_core_id();
    struct vector_desc_t *vd = (struct vector_desc_t *)arg;

    vd->isr = NULL;
    vd->arg = (void *)((int)vd->source);
    tee_set_interrupt_handler(vd);

    // Setting back the default value for interrupt pin.
    esp_rom_route_intr_matrix(cpu, vd->source, ETS_INVALID_INUM);

    return 0;
}

#define FIND_MSB_SET(n) (31 - __builtin_clz((uint32_t)(n)))

/* called from esp_tee_vectors.S */
void esp_tee_global_interrupt_handler(intptr_t sp, int mcause)
{
    uint32_t status, source;

    do {
        status = REG_READ(INTMTX_STATUS_REG_0) & TEE_SECURE_INT_MASK_0;
        if (status) {
            source = FIND_MSB_SET(status);
            break;
        }

        status = REG_READ(INTMTX_STATUS_REG_1) & TEE_SECURE_INT_MASK_1;
        if (status) {
            source = FIND_MSB_SET(status) + 32;
            break;
        }

        status = REG_READ(INTMTX_STATUS_REG_2) & TEE_SECURE_INT_MASK_2;
        if (status) {
            source = FIND_MSB_SET(status) + 64;
            break;
        }

        /* No valid interrupt found */
        ESP_LOGV(TAG, "No valid interrupt found");
        return;
    } while (0);

    ESP_LOGV(TAG, "Found intr src: %d", source);
    tee_handler_table_entry ih = tee_interrupt_table[source];
    if (ih.handler) {
        (*ih.handler)(ih.arg);
    }
}
