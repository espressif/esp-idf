/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief get psram CS IO
 *
 * This interface should be called after PSRAM is enabled, otherwise it will
 * return an invalid value -1/0xff.
 *
 * @return psram CS IO or -1/0xff if psram not enabled
 */
uint8_t esp_psram_io_get_cs_io(void);


#ifdef __cplusplus
}
#endif
