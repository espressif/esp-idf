/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* INTERNAL API
 * implementation of generic interface to MMU memory protection features
 */

#include <stdbool.h>
#include "esp32s3/memprot.h"

mem_type_prot_t IRAM_ATTR esp_memprot_get_active_intr_memtype(void)
{
    return MEMPROT_NONE;
}
