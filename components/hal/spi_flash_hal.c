/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// HAL for SPI Flash (non-IRAM part)
// The IRAM part is in spi_flash_hal_iram.c, spi_flash_hal_gpspi.c, spi_flash_hal_common.inc.

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "soc/soc_caps.h"
#include "hal/spi_flash_hal.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "hal/spi_flash_types.h"

#define APB_CYCLE_NS   (1000*1000*1000LL/APB_CLK_FREQ)

static const char *TAG = "flash_hal";

static uint32_t get_flash_clock_divider(const spi_flash_hal_config_t *cfg)
{
    const int clk_source   = cfg->clock_src_freq;
    const int clk_freq_mhz = cfg->freq_mhz;
    // On ESP32, ESP32-S2, ESP32-C3, we allow specific frequency 26.666MHz
    // If user passes freq_mhz like 26 or 27, it's allowed to use integer divider 3.
    // However on other chips or on other frequency, we only allow user pass frequency which
    // can be integer divided. If no, the following strategy is round up the division and
    // round down flash frequency to keep it safe.
    int best_div = 0;
    if (clk_source < clk_freq_mhz) {
        HAL_LOGE(TAG, "Target frequency %dMHz higher than supported.", clk_freq_mhz);
        abort();
    }
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
    if (clk_freq_mhz == 26 || clk_freq_mhz == 27) {
        best_div = 3;
    } else
#endif
    {
        /* Do not use float/double as the FPU may not have been initialized yet on startup.
         * The values are in MHz, so for sure we won't have an overflow by adding them. */
        best_div = (clk_source + clk_freq_mhz - 1) / clk_freq_mhz;
        /* Perform a division that returns both quotient and remainder */
        const div_t res = div(clk_source, clk_freq_mhz);
        if (res.rem != 0) {
            HAL_LOGW(TAG, "Flash clock frequency round down to %d", res.quot);
        }
    }

    return best_div;
}

static uint32_t spi_flash_cal_clock(const spi_flash_hal_config_t *cfg)
{
    uint32_t div_parameter = spi_flash_ll_calculate_clock_reg(cfg->host_id, get_flash_clock_divider(cfg));
    return div_parameter;
}

static inline int get_dummy_n(bool gpio_is_used, int input_delay_ns, int eff_clk)
{
    const int apbclk_kHz = APB_CLK_FREQ / 1000;
    //calculate how many apb clocks a period has
    const int apbclk_n = APB_CLK_FREQ / eff_clk;
    const int gpio_delay_ns = gpio_is_used ? GPIO_MATRIX_DELAY_NS : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (apb_period_n < 0) {
        apb_period_n = 0;
    }

    return apb_period_n / apbclk_n;
}

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
static inline int extra_dummy_under_timing_tuning(const spi_flash_hal_config_t *cfg)
{
    bool main_flash = (cfg->host_id == SPI1_HOST && cfg->cs_num == 0);
    int extra_dummy = 0;
    if (main_flash) {
        /**
         * For Octal Flash, the dummy is `usr_dummy` + `extra_dummy`, they are in two different regs, we don't touch `extra_dummy` here, so set extra_dummy 0.
         * Instead, for both Quad and Octal Flash, we use `usr_dummy` and set the whole dummy length (usr_dummy + extra_dummy) to this register.
         */
        extra_dummy = cfg->extra_dummy;
    } else {
        // TODO: for other flash chips, dummy get logic implement here. Currently, still calculate extra dummy by itself.
        abort();
    }

    return extra_dummy;
}
#endif //SOC_SPI_MEM_SUPPORT_TIMING_TUNING

esp_err_t spi_flash_hal_init(spi_flash_hal_context_t *data_out, const spi_flash_hal_config_t *cfg)
{
    if (cfg->cs_num >= SOC_SPI_PERIPH_CS_NUM(cfg->host_id)) {
        return ESP_ERR_INVALID_ARG;
    }

    *data_out = (spi_flash_hal_context_t) {
        .inst = data_out->inst, // Keeps the function pointer table
        .spi = spi_flash_ll_get_hw(cfg->host_id),
        .cs_num = cfg->cs_num,
        .cs_hold = cfg->cs_hold,
        .cs_setup = cfg->cs_setup,
        .base_io_mode = cfg->default_io_mode,
        .freq_mhz = cfg->freq_mhz,
    };
#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    if (cfg->using_timing_tuning) {
        data_out->extra_dummy = extra_dummy_under_timing_tuning(cfg);
        data_out->clock_conf = cfg->clock_config;
    } else
#endif // SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    {
        data_out->extra_dummy = get_dummy_n(!cfg->iomux, cfg->input_delay_ns, APB_CLK_FREQ/get_flash_clock_divider(cfg));
        data_out->clock_conf = (spi_flash_ll_clock_reg_t)spi_flash_cal_clock(cfg);
    }


    if (cfg->auto_sus_en) {
        data_out->flags |= SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_SUSPEND;
        data_out->flags |= SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_RESUME;
    }

#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    if (cfg->octal_mode_en) {
        data_out->flags |= SPI_FLASH_HOST_CONTEXT_FLAG_OCTAL_MODE;
    }

    if (cfg->default_io_mode == SPI_FLASH_OPI_DTR) {
        data_out->slicer_flags |= SPI_FLASH_HOST_CONTEXT_SLICER_FLAG_DTR;
    }
#endif

    return ESP_OK;
}

bool spi_flash_hal_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    (void)p;
    bool direct_write = (((spi_flash_hal_context_t *)host)->spi != spi_flash_ll_get_hw(SPI1_HOST));
    return direct_write;
}


bool spi_flash_hal_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    (void)p;
    //currently the host doesn't support to read through dma, no word-aligned requirements
    bool direct_read = ( ((spi_flash_hal_context_t *)host)->spi != spi_flash_ll_get_hw(SPI1_HOST));
    return direct_read;
}
