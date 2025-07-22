/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_ADC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "stdatomic.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_clk_tree.h"
#include "esp_private/adc_private.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/esp_sleep_internal.h"
#include "hal/adc_types.h"
#include "hal/adc_oneshot_hal.h"
#include "hal/adc_ll.h"
#include "soc/adc_periph.h"
#include "soc/soc_caps.h"

#if CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM
#define ADC_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ADC_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

extern portMUX_TYPE rtc_spinlock;
static const char *TAG = "adc_oneshot";

typedef struct adc_oneshot_unit_ctx_t {
    adc_oneshot_hal_ctx_t hal;
    uint32_t unit_id;
    adc_ulp_mode_t ulp_mode;
} adc_oneshot_unit_ctx_t;

typedef struct adc_oneshot_ctx_t {
    _lock_t mutex;
    adc_oneshot_unit_ctx_t *units[SOC_ADC_PERIPH_NUM];
    int apb_periph_ref_cnts;       //For the chips that ADC oneshot mode using APB_SARADC periph
} adc_oneshot_ctx_t;

static adc_oneshot_ctx_t s_ctx;    //ADC oneshot mode context
static atomic_bool s_adc_unit_claimed[SOC_ADC_PERIPH_NUM] = {ATOMIC_VAR_INIT(false),
#if (SOC_ADC_PERIPH_NUM >= 2)
                                                             ATOMIC_VAR_INIT(false)
#endif
                                                            };

static bool s_adc_unit_claim(adc_unit_t unit);
static bool s_adc_unit_free(adc_unit_t unit);
static esp_err_t s_adc_io_init(adc_unit_t unit, adc_channel_t channel);

esp_err_t adc_oneshot_io_to_channel(int io_num, adc_unit_t * const unit_id, adc_channel_t * const channel)
{
    return adc_io_to_channel(io_num, unit_id, channel);
}

esp_err_t adc_oneshot_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int * const io_num)
{
    return adc_channel_to_io(unit_id, channel, io_num);
}

esp_err_t adc_oneshot_new_unit(const adc_oneshot_unit_init_cfg_t *init_config, adc_oneshot_unit_handle_t *ret_unit)
{
#if CONFIG_ADC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    adc_oneshot_unit_ctx_t *unit = NULL;
    ESP_GOTO_ON_FALSE(init_config && ret_unit, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument: null pointer");
    ESP_GOTO_ON_FALSE(init_config->unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, err, TAG, "invalid unit");
#if CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3
    /**
     * We only check this on ESP32C3, because other adc units are no longer supported on later chips
     * If CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3 is enabled, we jump this check
     */
    ESP_GOTO_ON_FALSE(SOC_ADC_DIG_SUPPORTED_UNIT(init_config->unit_id), ESP_ERR_INVALID_ARG, err, TAG, "adc unit not supported");
#endif

    unit = heap_caps_calloc(1, sizeof(adc_oneshot_unit_ctx_t), ADC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(unit, ESP_ERR_NO_MEM, err, TAG, "no mem for unit");

    bool success_claim = s_adc_unit_claim(init_config->unit_id);
    ESP_GOTO_ON_FALSE(success_claim, ESP_ERR_NOT_FOUND, err, TAG, "adc%d is already in use", init_config->unit_id + 1);
    _lock_acquire(&s_ctx.mutex);
    s_ctx.units[init_config->unit_id] = unit;
    _lock_release(&s_ctx.mutex);
    unit->unit_id = init_config->unit_id;
    unit->ulp_mode = init_config->ulp_mode;

    adc_oneshot_clk_src_t clk_src;
#if SOC_LP_ADC_SUPPORTED
    if (init_config->ulp_mode != ADC_ULP_MODE_DISABLE) {
        clk_src = LP_ADC_CLK_SRC_LP_DYN_FAST;
    } else
#endif /* SOC_LP_ADC_SUPPORTED */
    {
        clk_src = ADC_DIGI_CLK_SRC_DEFAULT;
        if (init_config->clk_src) {
            clk_src = init_config->clk_src;
        }
    }
    uint32_t clk_src_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz), err, TAG, "clock source not supported");

    adc_oneshot_hal_cfg_t config = {
        .unit = init_config->unit_id,
        .clk_src = clk_src,
        .clk_src_freq_hz = clk_src_freq_hz,
    };

    switch (init_config->ulp_mode) {
    case ADC_ULP_MODE_FSM:
        config.work_mode = ADC_HAL_LP_MODE;  // esp32 ulp-fsm mode
        break;
    case ADC_ULP_MODE_RISCV:
        config.work_mode = ADC_HAL_SINGLE_READ_MODE;  // esp32s2, esp32s3 ulp-riscv mode
        break;
#if SOC_LP_ADC_SUPPORTED
    case ADC_ULP_MODE_LP_CORE:
        config.work_mode = ADC_HAL_LP_MODE;  // lp core mode
        break;
#endif /* SOC_LP_ADC_SUPPORTED */
    case ADC_ULP_MODE_DISABLE:
    default:
        config.work_mode = ADC_HAL_SINGLE_READ_MODE;  // oneshot read mode
        break;
    }

    adc_oneshot_hal_init(&(unit->hal), &config);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    //To enable the APB_SARADC periph if needed
    _lock_acquire(&s_ctx.mutex);
    s_ctx.apb_periph_ref_cnts++;
    if (s_ctx.apb_periph_ref_cnts == 1) {
        adc_apb_periph_claim();
    }
    _lock_release(&s_ctx.mutex);
#endif

    if (init_config->ulp_mode == ADC_ULP_MODE_DISABLE) {
        sar_periph_ctrl_adc_oneshot_power_acquire();
    } else {
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
        ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)(unit->hal.clk_src), true), err, TAG, "clock source enable failed");
#endif
#if SOC_LIGHT_SLEEP_SUPPORTED || SOC_DEEP_SLEEP_SUPPORTED
        esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, true);
#endif
    }

    ESP_LOGD(TAG, "new adc unit%"PRId32" is created", unit->unit_id);
    *ret_unit = unit;
    return ESP_OK;

err:
    if (unit) {
        free(unit);
    }
    return ret;
}

esp_err_t adc_oneshot_config_channel(adc_oneshot_unit_handle_t handle, adc_channel_t channel, const adc_oneshot_chan_cfg_t *config)
{
    ESP_RETURN_ON_FALSE(handle && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(config->atten < SOC_ADC_ATTEN_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid attenuation");
    ESP_RETURN_ON_FALSE(((config->bitwidth >= SOC_ADC_RTC_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_RTC_MAX_BITWIDTH) || config->bitwidth == ADC_BITWIDTH_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "invalid bitwidth");
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(handle->unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");

    s_adc_io_init(handle->unit_id, channel);

    adc_oneshot_hal_ctx_t *hal = &(handle->hal);
    adc_oneshot_hal_chan_cfg_t cfg = {
        .atten = config->atten,
        .bitwidth = (config->bitwidth == ADC_BITWIDTH_DEFAULT) ? SOC_ADC_RTC_MAX_BITWIDTH : config->bitwidth,
    };
    portENTER_CRITICAL(&rtc_spinlock);
    adc_oneshot_hal_channel_config(hal, &cfg, channel);
    if (handle->ulp_mode) {
        adc_oneshot_hal_setup(hal, channel);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t adc_oneshot_read(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw)
{
    ESP_RETURN_ON_FALSE(handle && out_raw, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan < SOC_ADC_CHANNEL_NUM(handle->unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");

    if (adc_lock_try_acquire(handle->unit_id) != ESP_OK) {
        return ESP_ERR_TIMEOUT;
    }
    portENTER_CRITICAL(&rtc_spinlock);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal.clk_src), true));
#endif
    ANALOG_CLOCK_ENABLE();
    adc_oneshot_hal_setup(&(handle->hal), chan);
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_atten_t atten = adc_ll_get_atten(handle->unit_id, chan);
    adc_hal_calibration_init(handle->unit_id);
    adc_set_hw_calibration_code(handle->unit_id, atten);
#endif  // SOC_ADC_CALIBRATION_V1_SUPPORTED
    bool valid = false;
    valid = adc_oneshot_hal_convert(&(handle->hal), out_raw);
    ANALOG_CLOCK_DISABLE();
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal.clk_src), false));
#endif

    portEXIT_CRITICAL(&rtc_spinlock);
    adc_lock_release(handle->unit_id);

    return valid ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t adc_oneshot_read_isr(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw)
{
    ESP_RETURN_ON_FALSE_ISR(handle && out_raw, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(out_raw, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(chan < SOC_ADC_CHANNEL_NUM(handle->unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");

    portENTER_CRITICAL_SAFE(&rtc_spinlock);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal.clk_src), true));
#endif
    ANALOG_CLOCK_ENABLE();
    adc_oneshot_hal_setup(&(handle->hal), chan);
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_atten_t atten = adc_ll_get_atten(handle->unit_id, chan);
    adc_hal_calibration_init(handle->unit_id);
    adc_set_hw_calibration_code(handle->unit_id, atten);
#endif
    adc_oneshot_hal_convert(&(handle->hal), out_raw);
    ANALOG_CLOCK_DISABLE();
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal.clk_src), false));
#endif

    portEXIT_CRITICAL_SAFE(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t adc_oneshot_del_unit(adc_oneshot_unit_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    adc_ulp_mode_t ulp_mode = handle->ulp_mode;
    bool success_free = s_adc_unit_free(handle->unit_id);
    ESP_RETURN_ON_FALSE(success_free, ESP_ERR_NOT_FOUND, TAG, "adc%"PRId32" isn't in use", handle->unit_id + 1);

#if ADC_LL_POWER_MANAGE_SUPPORTED
    adc_ll_set_power_manage(handle->unit_id, ADC_LL_POWER_SW_OFF);
#endif

    _lock_acquire(&s_ctx.mutex);
    s_ctx.units[handle->unit_id] = NULL;
    _lock_release(&s_ctx.mutex);

    ESP_LOGD(TAG, "adc unit%"PRId32" is deleted", handle->unit_id);

    if (ulp_mode == ADC_ULP_MODE_DISABLE) {
        sar_periph_ctrl_adc_oneshot_power_release();
    } else {
#if SOC_LIGHT_SLEEP_SUPPORTED || SOC_DEEP_SLEEP_SUPPORTED
        esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, false);
#endif
        ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal.clk_src), false));
    }
    free(handle);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    //To free the APB_SARADC periph if needed
    _lock_acquire(&s_ctx.mutex);
    s_ctx.apb_periph_ref_cnts--;
    assert(s_ctx.apb_periph_ref_cnts >= 0);
    if (s_ctx.apb_periph_ref_cnts == 0) {
        adc_apb_periph_free();
    }
    _lock_release(&s_ctx.mutex);
#endif

    return ESP_OK;
}

esp_err_t adc_oneshot_get_calibrated_result(adc_oneshot_unit_handle_t handle, adc_cali_handle_t cali_handle, adc_channel_t chan, int *cali_result)
{
    int raw = 0;
    ESP_RETURN_ON_ERROR(adc_oneshot_read(handle, chan, &raw), TAG, "adc oneshot read fail");
    ESP_LOGD(TAG, "raw: 0d%d", raw);
    ESP_RETURN_ON_ERROR(adc_cali_raw_to_voltage(cali_handle, raw, cali_result), TAG, "adc calibration fail");

    return ESP_OK;
}

#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

static esp_err_t s_adc_io_init(adc_unit_t unit, adc_channel_t channel)
{
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(unit), ESP_ERR_INVALID_ARG, TAG, "invalid channel");
    return gpio_config_as_analog(ADC_GET_IO_NUM(unit, channel));
}

static bool s_adc_unit_claim(adc_unit_t unit)
{
    bool false_var = false;
    return atomic_compare_exchange_strong(&s_adc_unit_claimed[unit], &false_var, true);
}

static bool s_adc_unit_free(adc_unit_t unit)
{
    bool true_var = true;
    return atomic_compare_exchange_strong(&s_adc_unit_claimed[unit], &true_var, false);
}
