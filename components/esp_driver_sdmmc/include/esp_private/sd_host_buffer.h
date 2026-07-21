/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sd_host_sdmmc_slot_t sd_host_sdmmc_slot_t;

bool sd_host_check_buffer_alignment(sd_host_sdmmc_slot_t *slot, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif
