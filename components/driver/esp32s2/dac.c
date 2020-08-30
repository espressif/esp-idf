// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "soc/dac_periph.h"
#include "hal/dac_hal.h"

static const char *DAC_TAG = "DAC";

#define DAC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(DAC_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})

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
    dac_hal_digi_init();
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
    dac_hal_digi_deinit();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_controller_config(const dac_digi_config_t *cfg)
{
    DAC_CHECK(cfg->mode < DAC_CONV_MAX, "DAC mode error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(cfg->interval > 0 && cfg->interval < 4096, "DAC interval error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(cfg->dig_clk.div_num < 256, "DAC clk div_num error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(cfg->dig_clk.div_b > 0 && cfg->dig_clk.div_b < 64, "DAC clk div_b error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(cfg->dig_clk.div_a < 64, "DAC clk div_a error", ESP_ERR_INVALID_ARG);
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
            ESP_LOGE(DAC_TAG, "DAC-DMA pm lock error");
            return err;
        }
    }
#endif //CONFIG_PM_ENABLE

    DAC_ENTER_CRITICAL();
    dac_hal_digi_controller_config(cfg);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_start(void)
{
#ifdef CONFIG_PM_ENABLE
    DAC_CHECK((s_dac_digi_lock), "Should start after call `dac_digi_controller_config`", ESP_FAIL);
    esp_pm_lock_acquire(s_dac_digi_lock);
#endif
    DAC_ENTER_CRITICAL();
    dac_hal_digi_start();
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
    dac_hal_digi_stop();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_fifo_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_fifo_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}
