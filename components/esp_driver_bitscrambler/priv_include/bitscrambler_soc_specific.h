/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_private/gdma.h"
#include "soc/bitscrambler_peri_select.h"

typedef struct {
    gdma_trigger_t dma_trigger;
    int bus;
} bitscrambler_periph_desc_t;

extern const bitscrambler_periph_desc_t g_bitscrambler_periph_desc[];
