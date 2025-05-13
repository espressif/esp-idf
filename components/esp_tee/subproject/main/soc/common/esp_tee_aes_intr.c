/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <assert.h>

#include "hal/aes_hal.h"
#include "soc/interrupts.h"

#include "esp_attr.h"
#include "esp_tee_intr.h"

volatile DRAM_ATTR bool intr_flag;

static IRAM_ATTR void esp_tee_aes_complete_isr(void *arg)
{
    aes_hal_interrupt_clear();
    intr_flag = false;
}

void esp_tee_aes_intr_alloc(void)
{
    struct vector_desc_t aes_vd = { 0, NULL, NULL, NULL};

    aes_vd.source = ETS_AES_INTR_SOURCE;
    aes_vd.isr = esp_tee_aes_complete_isr;

    esp_tee_intr_register((void *)&aes_vd);
    intr_flag = false;
}
