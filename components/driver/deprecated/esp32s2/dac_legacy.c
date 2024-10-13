/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "driver/dac_types_legacy.h"
#include "hal/adc_ll.h"
#include "hal/dac_ll.h"
#include "esp_check.h"
#include "esp_pm.h"

static __attribute__((unused)) const char *TAG = "DAC";

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define DAC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_dac_digi_lock = NULL;
#endif  //CONFIG_PM_ENABLE

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

esp_err_t dac_digi_init(void)
{
    DAC_ENTER_CRITICAL();
    dac_ll_digi_clk_inv(true);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_lock) {
        esp_pm_lock_delete(s_dac_digi_lock);
        s_dac_digi_lock = NULL;
    }
#endif
    DAC_ENTER_CRITICAL();
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
    dac_ll_digi_fifo_reset();
    dac_ll_digi_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_controller_config(const dac_digi_config_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg->mode <= DAC_CONV_ALTER, ESP_ERR_INVALID_ARG, TAG, "DAC mode error");
    ESP_RETURN_ON_FALSE(cfg->interval > 0 && cfg->interval < 4096, ESP_ERR_INVALID_ARG, TAG, "DAC interval error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_num < 256, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_num error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_b > 0 && cfg->dig_clk.div_b < 64, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_b error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_a < 64, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_a error");
#ifdef CONFIG_PM_ENABLE
    esp_err_t err;
    if (s_dac_digi_lock == NULL) {
        if (cfg->dig_clk.use_apll) {
            err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "dac_dma", &s_dac_digi_lock);
        } else {
            err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_dma", &s_dac_digi_lock);
        }
        if (err != ESP_OK) {
            s_dac_digi_lock = NULL;
            ESP_LOGE(TAG, "DAC-DMA pm lock error");
            return err;
        }
    }
#endif //CONFIG_PM_ENABLE

    DAC_ENTER_CRITICAL();
    dac_ll_digi_set_convert_mode(cfg->mode == DAC_CONV_ALTER);
    dac_ll_digi_set_trigger_interval(cfg->interval);
    adc_ll_digi_controller_clk_div(cfg->dig_clk.div_num, cfg->dig_clk.div_b, cfg->dig_clk.div_a);
    adc_ll_digi_clk_sel(cfg->dig_clk.use_apll ? ADC_DIGI_CLK_SRC_APLL : ADC_DIGI_CLK_SRC_DEFAULT);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_start(void)
{
#ifdef CONFIG_PM_ENABLE
    ESP_RETURN_ON_FALSE(s_dac_digi_lock, ESP_FAIL, TAG, "Should start after call `dac_digi_controller_config`");
    esp_pm_lock_acquire(s_dac_digi_lock);
#endif
    DAC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(true);
    dac_ll_digi_trigger_output(true);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_stop(void)
{
#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_lock) {
        esp_pm_lock_release(s_dac_digi_lock);
    }
#endif
    DAC_ENTER_CRITICAL();
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_fifo_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_ll_digi_fifo_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_ll_digi_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}
