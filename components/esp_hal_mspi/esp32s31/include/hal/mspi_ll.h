/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

/**
 * Background
 *
 * This file is for the MSPI related, but not Flash driver related registers, these registers:
 * - may influence both Flash and PSRAM
 * - not related or directly related to Flash controller driver
 *
 * Some hints for naming convention:
 * - For MSPI timing tuning related registers, the LL should start with `mspi_timing_ll_`
 * - For others, the LL should start with `mspi_ll_`
 */

#pragma once

#include <sys/param.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: ["ESP32S31"] IDF-14653

#ifdef __cplusplus
}
#endif
