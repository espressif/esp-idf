/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <esp_err.h>
#include <esp_spi_flash.h> /* including in bootloader for error values */
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_CACHE_SUPPORT_WRAP
/**
 * @brief Set the burst mode setting command for specified wrap mode.
 *
 * @param mode The specified warp mode.
 * @return always ESP_OK
 */
esp_err_t bootloader_flash_wrap_set(spi_flash_wrap_mode_t mode);
#endif
