/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * System level OPI Flash APIs (private)
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register ROM functions and init flash device registers to make use of octal flash
 *
 * @param chip_id Full device ID read via RDID command
 */
esp_err_t esp_opiflash_init(uint32_t chip_id);

/**
 * @brief Set Octal Flash chip specifically required MSPI register settings here
 */
void esp_opiflash_set_required_regs(void);

#ifdef __cplusplus
}
#endif
