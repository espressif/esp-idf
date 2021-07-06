// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "sys/lock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_pm.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "driver/rtc_io.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "sdkconfig.h"

#include "esp32s2/rom/ets_sys.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

static const char *ADC_TAG = "ADC";

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_adc_digi_arbiter_lock = NULL;
#endif  //CONFIG_PM_ENABLE

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
esp_err_t adc_digi_init(void)
{
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_init();
    adc_hal_arbiter_config(&config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
    if (s_adc_digi_arbiter_lock) {
        esp_pm_lock_delete(s_adc_digi_arbiter_lock);
        s_adc_digi_arbiter_lock = NULL;
    }
#endif
    adc_power_release();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_deinit();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
#ifdef CONFIG_PM_ENABLE
    esp_err_t err;
    if (s_adc_digi_arbiter_lock == NULL) {
        if (config->dig_clk.use_apll) {
            err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "adc_dma", &s_adc_digi_arbiter_lock);
        } else {
            err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc_dma", &s_adc_digi_arbiter_lock);
        }
        if (err != ESP_OK) {
            s_adc_digi_arbiter_lock = NULL;
            ESP_LOGE(ADC_TAG, "ADC-DMA pm lock error");
            return err;
        }
    }
#endif //CONFIG_PM_ENABLE

    /* If enable digtal controller, adc xpd should always on. */
    adc_power_acquire();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_controller_config(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_arbiter_config(adc_unit_t adc_unit, adc_arbiter_t *config)
{
    if (adc_unit & ADC_UNIT_1) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    ADC_ENTER_CRITICAL();
    adc_hal_arbiter_config(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Set ADC module controller.
 *        There are five SAR ADC controllers:
 *        Two digital controller: Continuous conversion mode (DMA). High performance with multiple channel scan modes;
 *        Two RTC controller: Single conversion modes (Polling). For low power purpose working during deep sleep;
 *        the other is dedicated for Power detect (PWDET / PKDET), Only support ADC2.
 *
 * @note  Only ADC2 support arbiter to switch controllers automatically. Access to the ADC is based on the priority of the controller.
 * @note  For ADC1, Controller access is mutually exclusive.
 *
 * @param adc_unit ADC unit.
 * @param ctrl ADC controller, Refer to `adc_controller_t`.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_set_controller(adc_unit_t adc_unit, adc_controller_t ctrl)
{
    adc_arbiter_t config = {0};
    adc_arbiter_t cfg = ADC_ARBITER_CONFIG_DEFAULT();

    if (adc_unit & ADC_UNIT_1) {
        adc_hal_set_controller(ADC_NUM_1, ctrl);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_set_controller(ADC_NUM_2, ctrl);
        switch (ctrl) {
        case ADC2_CTRL_FORCE_PWDET:
            config.pwdet_pri = 2;
            config.mode = ADC_ARB_MODE_SHIELD;
            adc_hal_arbiter_config(&config);
            adc_hal_set_controller(ADC_NUM_2, ADC2_CTRL_PWDET);
            break;
        case ADC2_CTRL_FORCE_RTC:
            config.rtc_pri = 2;
            config.mode = ADC_ARB_MODE_SHIELD;
            adc_hal_arbiter_config(&config);
            adc_hal_set_controller(ADC_NUM_2, ADC_CTRL_RTC);
            break;
        case ADC2_CTRL_FORCE_ULP:
            config.rtc_pri = 2;
            config.mode = ADC_ARB_MODE_SHIELD;
            adc_hal_arbiter_config(&config);
            adc_hal_set_controller(ADC_NUM_2, ADC_CTRL_ULP);
            break;
        case ADC2_CTRL_FORCE_DIG:
            config.dig_pri = 2;
            config.mode = ADC_ARB_MODE_SHIELD;
            adc_hal_arbiter_config(&config);
            adc_hal_set_controller(ADC_NUM_2, ADC_CTRL_DIG);
            break;
        default:
            adc_hal_arbiter_config(&cfg);
            break;
        }
    }
    return ESP_OK;
}

esp_err_t adc_digi_start(void)
{
#ifdef CONFIG_PM_ENABLE
    ADC_CHECK((s_adc_digi_arbiter_lock), "Should start after call `adc_digi_controller_config`", ESP_FAIL);
    esp_pm_lock_acquire(s_adc_digi_arbiter_lock);
#endif
    ADC_ENTER_CRITICAL();
    adc_hal_digi_enable();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_stop(void)
{
#ifdef CONFIG_PM_ENABLE
    if (s_adc_digi_arbiter_lock) {
        esp_pm_lock_release(s_adc_digi_arbiter_lock);
    }
#endif
    ADC_ENTER_CRITICAL();
    adc_hal_digi_disable();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Reset FSM of adc digital controller.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_reset(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_reset();
    adc_hal_digi_clear_pattern_table(ADC_NUM_1);
    adc_hal_digi_clear_pattern_table(ADC_NUM_2);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/*************************************/
/* Digital controller filter setting */
/*************************************/

esp_err_t adc_digi_filter_reset(adc_digi_filter_idx_t idx)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_hal_digi_filter_reset(ADC_NUM_1);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_hal_digi_filter_reset(ADC_NUM_2);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_set_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_hal_digi_filter_set_factor(ADC_NUM_1, config->mode);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_hal_digi_filter_set_factor(ADC_NUM_2, config->mode);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_get_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        config->adc_unit = ADC_UNIT_1;
        config->channel = ADC_CHANNEL_MAX;
        adc_hal_digi_filter_get_factor(ADC_NUM_1, &config->mode);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        config->adc_unit = ADC_UNIT_2;
        config->channel = ADC_CHANNEL_MAX;
        adc_hal_digi_filter_get_factor(ADC_NUM_2, &config->mode);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_enable(adc_digi_filter_idx_t idx, bool enable)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_hal_digi_filter_enable(ADC_NUM_1, enable);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_hal_digi_filter_enable(ADC_NUM_2, enable);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Get the filtered data of adc digital controller filter. For debug.
 *        The data after each measurement and filtering is updated to the DMA by the digital controller. But it can also be obtained manually through this API.
 *
 * @note For ESP32S2, The filter will filter all the enabled channel data of the each ADC unit at the same time.
 * @param idx Filter index.
 * @return Filtered data. if <0, the read data invalid.
 */
int adc_digi_filter_read_data(adc_digi_filter_idx_t idx)
{
    if (idx == ADC_DIGI_FILTER_IDX0) {
        return adc_hal_digi_filter_read_data(ADC_NUM_1);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        return adc_hal_digi_filter_read_data(ADC_NUM_2);
    } else {
        return -1;
    }
}

/**************************************/
/* Digital controller monitor setting */
/**************************************/

esp_err_t adc_digi_monitor_set_config(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        adc_hal_digi_monitor_config(ADC_NUM_1, config);
    } else if (idx == ADC_DIGI_MONITOR_IDX1) {
        adc_hal_digi_monitor_config(ADC_NUM_2, config);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_monitor_enable(adc_digi_monitor_idx_t idx, bool enable)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        adc_hal_digi_monitor_enable(ADC_NUM_1, enable);
    } else if (idx == ADC_DIGI_MONITOR_IDX1) {
        adc_hal_digi_monitor_enable(ADC_NUM_2, enable);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**************************************/
/*   Digital controller intr setting  */
/**************************************/

esp_err_t adc_digi_intr_enable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_digi_intr_enable(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_digi_intr_enable(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_intr_disable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_digi_intr_disable(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_digi_intr_disable(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_intr_clear(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_digi_intr_clear(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_digi_intr_clear(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

uint32_t adc_digi_intr_get_status(adc_unit_t adc_unit)
{
    uint32_t ret = 0;
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        ret = adc_hal_digi_get_intr_status(ADC_NUM_1);
    }
    if (adc_unit & ADC_UNIT_2) {
        ret = adc_hal_digi_get_intr_status(ADC_NUM_2);
    }
    ADC_EXIT_CRITICAL();
    return ret;
}

static uint8_t s_isr_registered = 0;
static intr_handle_t s_adc_isr_handle = NULL;

esp_err_t adc_digi_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags)
{
    ADC_CHECK((fn != NULL), "Parameter error", ESP_ERR_INVALID_ARG);
    ADC_CHECK(s_isr_registered == 0, "ADC ISR have installed, can not install again", ESP_FAIL);

    esp_err_t ret = esp_intr_alloc(ETS_APB_ADC_INTR_SOURCE, intr_alloc_flags, fn, arg, &s_adc_isr_handle);
    if (ret == ESP_OK) {
        s_isr_registered = 1;
    }
    return ret;
}

esp_err_t adc_digi_isr_deregister(void)
{
    esp_err_t ret = ESP_FAIL;
    if (s_isr_registered) {
        ret = esp_intr_free(s_adc_isr_handle);
        if (ret == ESP_OK) {
            s_isr_registered = 0;
        }
    }
    return ret;
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/
