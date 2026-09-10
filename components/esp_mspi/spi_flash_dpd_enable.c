/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "hal/spi_flash_hal.h"
#include "esp_private/spi_flash_os.h"

/*******************************************************************************
 * Flash deep power-down mode.
 * DPD: Deep power-down mode.
 * TDP: CS high to deep power-down mode duration.
 * TRES1: CS high to standby mode without ID read duration.
 *
 * Different flash chips might have different deep power-down strategy.
 * 1. Most flash chips send B9H to enter DPD and send ABH to exist DPD.
 * 2. Some flash chips send ABH followed by 3-dummy bytes to get device ID.
 * 3. Some flash chips send B9H to enter PD(power-down) → send 79H to enter UDPD(ultra-deep power-down mode); send FFH to exit UDPD → send ABH to exit PD (ABH).
 * 4. Some flash chips do nothing.
 ******************************************************************************/

__attribute__((unused)) const static char *DPD_TAG = "flash DPD";

#ifdef CONFIG_ESP_SLEEP_SPI_FLASH_ENTER_DPD_MODE_DELAY
#define SPI_FLASH_TDP_SAFE_VAL_US CONFIG_ESP_SLEEP_SPI_FLASH_ENTER_DPD_MODE_DELAY
#else
#define SPI_FLASH_TDP_SAFE_VAL_US (25)
#endif

#ifdef CONFIG_ESP_SLEEP_SPI_FLASH_EXIT_DPD_MODE_DELAY
#define SPI_FLASH_TRES1_SAFE_VAL_US CONFIG_ESP_SLEEP_SPI_FLASH_EXIT_DPD_MODE_DELAY
#else
#define SPI_FLASH_TRES1_SAFE_VAL_US (40)
#endif

/*
 * Note: This file should only be compiled when DPD_ON, which is only available when (!APP_BUILD_TYPE_PURE_RAM_APP && !ESP_SLEEP_POWER_DOWN_FLASH).
 * However when DPD_ON, there are still some cases this file is not actually used:
 * TODO: PM-623
 */

static spi_flash_dpd_funcs_t s_dpd_funcs = {
    .enter_dpd = NULL,
    .exit_dpd = NULL,
};

esp_err_t spi_flash_dpd_register_funcs(spi_flash_dpd_funcs_t *dpd_funcs)
{
    if (dpd_funcs == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    s_dpd_funcs = *dpd_funcs;
    return ESP_OK;
}

uint32_t spi_flash_dpd_get_enter_duration(void)
{
#ifndef CONFIG_ESP_SLEEP_SPI_FLASH_ENTER_DPD_MODE_DELAY
    ESP_EARLY_LOGW(DPD_TAG, "No DPD enter delay value defined. Using default safe delay value. Verify with your flash datasheet.");
#endif
    return SPI_FLASH_TDP_SAFE_VAL_US;
}

uint32_t spi_flash_dpd_get_exit_duration(void)
{
#ifndef CONFIG_ESP_SLEEP_SPI_FLASH_EXIT_DPD_MODE_DELAY
    ESP_EARLY_LOGW(DPD_TAG, "No DPD exit delay value defined. Using default safe delay value. Verify with your flash datasheet.");
#endif
    return SPI_FLASH_TRES1_SAFE_VAL_US;
}

static esp_err_t spi_flash_enter_dpd(bool wait_delay)
{
    if (s_dpd_funcs.enter_dpd == NULL) {
        ESP_EARLY_LOGE(DPD_TAG, "DPD enter function not registered");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = s_dpd_funcs.enter_dpd();
    if (wait_delay) {
        esp_rom_delay_us(spi_flash_dpd_get_enter_duration());
    }

    return ret;
}

static esp_err_t spi_flash_exit_dpd(bool wait_delay)
{
    if (s_dpd_funcs.exit_dpd == NULL) {
        ESP_EARLY_LOGE(DPD_TAG, "DPD exit function not registered");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = s_dpd_funcs.exit_dpd();
    if (wait_delay) {
        esp_rom_delay_us(spi_flash_dpd_get_exit_duration());
    }

    return ret;
}

esp_err_t spi_flash_enable_deep_power_down_mode(bool enable, bool wait_delay)
{
    if (enable) {
        return spi_flash_enter_dpd(wait_delay);
    }
    return spi_flash_exit_dpd(wait_delay);
}
