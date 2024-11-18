/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "sdkconfig.h"
#if CONFIG_ADC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/ringbuf.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_private.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_clk_tree.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/dma_types.h"
#include "soc/soc_caps.h"
#include "esp_memory_utils.h"
#include "adc_continuous_internal.h"
#include "esp_private/adc_dma.h"
#include "adc_dma_internal.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#endif

static const char *ADC_TAG = "adc_continuous";

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

#define INTERNAL_BUF_NUM      5

/*---------------------------------------------------------------
                   ADC Continuous Read Mode (via DMA)
---------------------------------------------------------------*/

static IRAM_ATTR bool adc_dma_intr(adc_continuous_ctx_t *adc_digi_ctx)
{
    BaseType_t taskAwoken = 0;
    bool need_yield = false;
    BaseType_t ret;
    adc_hal_dma_desc_status_t status = false;
    uint8_t *finished_buffer = NULL;
    uint32_t finished_size = 0;

    while (1) {
        status = adc_hal_get_reading_result(&adc_digi_ctx->hal, adc_digi_ctx->rx_eof_desc_addr, &finished_buffer, &finished_size);
        if (status != ADC_HAL_DMA_DESC_VALID) {
            break;
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        else {
            esp_err_t msync_ret = esp_cache_msync((void *)finished_buffer, finished_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
            assert(msync_ret == ESP_OK);
        }
#endif

        ret = xRingbufferSendFromISR(adc_digi_ctx->ringbuf_hdl, finished_buffer, finished_size, &taskAwoken);
        need_yield |= (taskAwoken == pdTRUE);

        if (adc_digi_ctx->cbs.on_conv_done) {
            adc_continuous_evt_data_t edata = {
                .conv_frame_buffer = finished_buffer,
                .size = finished_size,
            };
            if (adc_digi_ctx->cbs.on_conv_done(adc_digi_ctx, &edata, adc_digi_ctx->user_data)) {
                need_yield |= true;
            }
        }

        if (ret == pdFALSE) {
            if (adc_digi_ctx->flags.flush_pool) {
                size_t actual_size = 0;
                uint8_t *old_data = xRingbufferReceiveUpToFromISR(adc_digi_ctx->ringbuf_hdl, &actual_size, adc_digi_ctx->ringbuf_size);
                /**
                 * Replace by ringbuffer reset API when this API is ready.
                 * Now we do manual reset.
                 * For old_data == NULL condition (equals to the future ringbuffer reset fail condition), we don't care this time data,
                 * as this only happens when the ringbuffer size is small, new data will be filled in soon.
                 */
                if (old_data) {
                    vRingbufferReturnItemFromISR(adc_digi_ctx->ringbuf_hdl, old_data, &taskAwoken);
                    xRingbufferSendFromISR(adc_digi_ctx->ringbuf_hdl, finished_buffer, finished_size, &taskAwoken);
                    if (taskAwoken == pdTRUE) {
                        need_yield |= true;
                    }
                }
            }

            //ringbuffer overflow happens before
            if (adc_digi_ctx->cbs.on_pool_ovf) {
                adc_continuous_evt_data_t edata = {};
                if (adc_digi_ctx->cbs.on_pool_ovf(adc_digi_ctx, &edata, adc_digi_ctx->user_data)) {
                    need_yield |= true;
                }
            }
        }
    }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    esp_err_t msync_ret = esp_cache_msync((void *)(adc_digi_ctx->hal.rx_desc), adc_digi_ctx->adc_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    assert(msync_ret == ESP_OK);
#endif
    return need_yield;
}

static int8_t adc_digi_get_io_num(adc_unit_t adc_unit, uint8_t adc_channel)
{
    assert(adc_unit < SOC_ADC_PERIPH_NUM);
    uint8_t adc_n = (adc_unit == ADC_UNIT_1) ? 0 : 1;
    return adc_channel_io_map[adc_n][adc_channel];
}

static esp_err_t adc_digi_gpio_init(adc_unit_t adc_unit, uint16_t channel_mask)
{
    esp_err_t ret = ESP_OK;
    uint64_t gpio_mask = 0;
    uint32_t n = 0;
    int8_t io = 0;

    while (channel_mask) {
        if (channel_mask & 0x1) {
            io = adc_digi_get_io_num(adc_unit, n);
            if (io < 0) {
                return ESP_ERR_INVALID_ARG;
            }
            gpio_mask |= BIT64(io);
        }
        channel_mask = channel_mask >> 1;
        n++;
    }

    gpio_config_t cfg = {
        .pin_bit_mask = gpio_mask,
        .mode = GPIO_MODE_DISABLE,
    };
    ret = gpio_config(&cfg);

    return ret;
}

esp_err_t adc_continuous_new_handle(const adc_continuous_handle_cfg_t *hdl_config, adc_continuous_handle_t *ret_handle)
{
#if CONFIG_ADC_ENABLE_DEBUG_LOG
    esp_log_level_set(ADC_TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((hdl_config->conv_frame_size % SOC_ADC_DIGI_DATA_BYTES_PER_CONV == 0), ESP_ERR_INVALID_ARG, ADC_TAG, "conv_frame_size should be in multiples of `SOC_ADC_DIGI_DATA_BYTES_PER_CONV`");

    adc_continuous_ctx_t *adc_ctx = heap_caps_calloc(1, sizeof(adc_continuous_ctx_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (adc_ctx == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //ringbuffer storage/struct buffer
    adc_ctx->ringbuf_size = hdl_config->max_store_buf_size;
    adc_ctx->ringbuf_storage = heap_caps_calloc(1, hdl_config->max_store_buf_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    adc_ctx->ringbuf_struct = heap_caps_calloc(1, sizeof(StaticRingbuffer_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!adc_ctx->ringbuf_storage || !adc_ctx->ringbuf_struct) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //ringbuffer
    adc_ctx->ringbuf_hdl = xRingbufferCreateStatic(hdl_config->max_store_buf_size, RINGBUF_TYPE_BYTEBUF, adc_ctx->ringbuf_storage, adc_ctx->ringbuf_struct);
    if (!adc_ctx->ringbuf_hdl) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc internal buffer used by DMA
    adc_ctx->rx_dma_buf = heap_caps_calloc(INTERNAL_BUF_NUM, hdl_config->conv_frame_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (!adc_ctx->rx_dma_buf) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc dma descriptor
    uint32_t dma_desc_num_per_frame = (hdl_config->conv_frame_size + DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
    uint32_t dma_desc_max_num = dma_desc_num_per_frame * INTERNAL_BUF_NUM;
    adc_ctx->hal.rx_desc = heap_caps_aligned_calloc(ADC_DMA_DESC_ALIGN, dma_desc_max_num, sizeof(dma_descriptor_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (!adc_ctx->hal.rx_desc) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc pattern table
    adc_ctx->hal_digi_ctrlr_cfg.adc_pattern = calloc(1, SOC_ADC_PATT_LEN_MAX * sizeof(adc_digi_pattern_config_t));
    if (!adc_ctx->hal_digi_ctrlr_cfg.adc_pattern) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

#if CONFIG_PM_ENABLE
    ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc_dma", &adc_ctx->pm_lock);
    if (ret != ESP_OK) {
        goto cleanup;
    }
#endif //CONFIG_PM_ENABLE

    ret = adc_dma_init(&adc_ctx->adc_dma);
    adc_ctx->adc_intr_func = adc_dma_intr;
    if (ret != ESP_OK) {
        goto cleanup;
    }
    ret = adc_dma_intr_event_init(adc_ctx);

    adc_hal_dma_config_t config = {
        .eof_desc_num = INTERNAL_BUF_NUM,
        .eof_step = dma_desc_num_per_frame,
        .eof_num = hdl_config->conv_frame_size / SOC_ADC_DIGI_DATA_BYTES_PER_CONV
    };
    adc_hal_dma_ctx_config(&adc_ctx->hal, &config);

    adc_ctx->flags.flush_pool = hdl_config->flags.flush_pool;
    adc_ctx->fsm = ADC_FSM_INIT;
    *ret_handle = adc_ctx;

    adc_apb_periph_claim();

    return ret;

cleanup:
    adc_continuous_deinit(adc_ctx);
    return ret;
}

esp_err_t adc_continuous_start(adc_continuous_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver isn't initialised");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, ADC_TAG, "ADC continuous mode isn't in the init state, it's started already");

    ANALOG_CLOCK_ENABLE();
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_1);
    adc_hal_calibration_init(ADC_UNIT_2);
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

    //reset ADC digital part to reset ADC sampling EOF counter
    ADC_BUS_CLK_ATOMIC() {
        adc_ll_reset_register();
    }

    if (handle->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(handle->pm_lock), ADC_TAG, "acquire pm_lock failed");
    }

    handle->fsm = ADC_FSM_STARTED;
    sar_periph_ctrl_adc_continuous_power_acquire();
    //reset flags
    if (handle->use_adc1) {
        adc_lock_acquire(ADC_UNIT_1);
    }
    if (handle->use_adc2) {
        adc_lock_acquire(ADC_UNIT_2);
    }

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    if (handle->use_adc1) {
        adc_set_hw_calibration_code(ADC_UNIT_1, handle->adc1_atten);
    }
    if (handle->use_adc2) {
        adc_set_hw_calibration_code(ADC_UNIT_2, handle->adc2_atten);
    }
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

#if SOC_ADC_ARBITER_SUPPORTED
    if (handle->use_adc2) {
        adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
        adc_hal_arbiter_config(&config);
    }
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

#if SOC_ADC_MONITOR_SUPPORTED
    adc_ll_digi_monitor_clear_intr();
    for (int i = 0; i < SOC_ADC_DIGI_MONITOR_NUM; i++) {
        adc_monitor_t *monitor_ctx = handle->adc_monitor[i];
        if (monitor_ctx) {
            // config monitor hardware
            adc_hal_digi_monitor_set_thres(monitor_ctx->monitor_id, monitor_ctx->config.adc_unit, monitor_ctx->config.channel, monitor_ctx->config.h_threshold, monitor_ctx->config.l_threshold);
            // if monitor not enabled now, just using monitor api later
            if (monitor_ctx->fsm == ADC_MONITOR_FSM_ENABLED) {
                // restore the started FSM
                adc_ll_digi_monitor_user_start(monitor_ctx->monitor_id, ((monitor_ctx->config.h_threshold >= 0) || (monitor_ctx->config.l_threshold >= 0)));
            }
        }
    }
#endif  //#if SOC_ADC_MONITOR_SUPPORTED

    if (handle->use_adc1) {
        adc_hal_set_controller(ADC_UNIT_1, ADC_HAL_CONTINUOUS_READ_MODE);
    }
    if (handle->use_adc2) {
        adc_hal_set_controller(ADC_UNIT_2, ADC_HAL_CONTINUOUS_READ_MODE);
    }

    adc_hal_digi_init(&handle->hal);
#if !CONFIG_IDF_TARGET_ESP32
    esp_clk_tree_enable_src((soc_module_clk_t)(handle->hal_digi_ctrlr_cfg.clk_src), true);
#endif
    adc_hal_digi_controller_config(&handle->hal, &handle->hal_digi_ctrlr_cfg);
    adc_hal_digi_enable(false);

    adc_dma_stop(handle->adc_dma);
    adc_hal_digi_connect(false);

    adc_dma_reset(handle->adc_dma);
    adc_hal_digi_reset();
    adc_hal_digi_dma_link(&handle->hal, handle->rx_dma_buf);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    esp_err_t ret = esp_cache_msync(handle->hal.rx_desc, handle->adc_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    assert(ret == ESP_OK);
#endif

    adc_dma_start(handle->adc_dma, handle->hal.rx_desc);
    adc_hal_digi_connect(true);
    adc_hal_digi_enable(true);

    return ESP_OK;
}

esp_err_t adc_continuous_stop(adc_continuous_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver isn't initialised");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_STARTED, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver is already stopped");

    handle->fsm = ADC_FSM_INIT;

    adc_dma_stop(handle->adc_dma);
    adc_hal_digi_enable(false);
    adc_hal_digi_connect(false);
#if ADC_LL_WORKAROUND_CLEAR_EOF_COUNTER
    periph_module_reset(PERIPH_SARADC_MODULE);
    adc_hal_digi_clr_eof();
#endif

    adc_hal_digi_deinit();

    if (handle->use_adc2) {
        adc_lock_release(ADC_UNIT_2);
    }
    if (handle->use_adc1) {
        adc_lock_release(ADC_UNIT_1);
    }
    sar_periph_ctrl_adc_continuous_power_release();

    //release power manager lock
    if (handle->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(handle->pm_lock), ADC_TAG, "release pm_lock failed");
    }

    ANALOG_CLOCK_DISABLE();

    return ESP_OK;
}

esp_err_t adc_continuous_read(adc_continuous_handle_t handle, uint8_t *buf, uint32_t length_max, uint32_t *out_length, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver isn't initialised");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_STARTED, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver is already stopped");

    TickType_t ticks_to_wait;
    esp_err_t ret = ESP_OK;
    uint8_t *data = NULL;
    size_t size = 0;

    ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
    if (timeout_ms == ADC_MAX_DELAY) {
        ticks_to_wait = portMAX_DELAY;
    }

    data = xRingbufferReceiveUpTo(handle->ringbuf_hdl, &size, ticks_to_wait, length_max);
    if (!data) {
        ESP_LOGV(ADC_TAG, "No data, increase timeout");
        ret = ESP_ERR_TIMEOUT;
        *out_length = 0;
        return ret;
    }

    memcpy(buf, data, size);
    vRingbufferReturnItem(handle->ringbuf_hdl, data);
    assert((size % 4) == 0);
    *out_length = size;

    return ret;
}

esp_err_t adc_continuous_deinit(adc_continuous_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver isn't initialised");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver is still running");

    if (handle->ringbuf_hdl) {
        vRingbufferDelete(handle->ringbuf_hdl);
        handle->ringbuf_hdl = NULL;
        free(handle->ringbuf_storage);
        free(handle->ringbuf_struct);
    }

    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
    }

    free(handle->rx_dma_buf);
    free(handle->hal.rx_desc);
    free(handle->hal_digi_ctrlr_cfg.adc_pattern);
    adc_dma_deinit(handle->adc_dma);
    free(handle);
    handle = NULL;

    adc_apb_periph_free();

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
esp_err_t adc_continuous_config(adc_continuous_handle_t handle, const adc_continuous_config_t *config)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, ADC_TAG, "The driver isn't initialised");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, ADC_TAG, "ADC continuous mode isn't in the init state, it's started already");

    //Pattern related check
    ESP_RETURN_ON_FALSE(config->pattern_num <= SOC_ADC_PATT_LEN_MAX, ESP_ERR_INVALID_ARG, ADC_TAG, "Max pattern num is %d", SOC_ADC_PATT_LEN_MAX);
    for (int i = 0; i < config->pattern_num; i++) {
        ESP_RETURN_ON_FALSE((config->adc_pattern[i].bit_width >= SOC_ADC_DIGI_MIN_BITWIDTH && config->adc_pattern->bit_width <= SOC_ADC_DIGI_MAX_BITWIDTH), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC bitwidth not supported");
    }

    for (int i = 0; i < config->pattern_num; i++) {
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
        //we add this error log to hint users what happened
        if (SOC_ADC_DIG_SUPPORTED_UNIT(config->adc_pattern[i].unit) == 0) {
            ESP_LOGE(ADC_TAG, "ADC2 continuous mode is no longer supported, please use ADC1. Search for errata on espressif website for more details. You can enable CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3 to force use ADC2");
        }
#endif  //CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3

#if !CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3
        /**
         * On all continuous mode supported chips, we will always check the unit to see if it's a continuous mode supported unit.
         * However, on ESP32C3 and ESP32S3, we will jump this check, if `CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3` is enabled.
         */
        ESP_RETURN_ON_FALSE(SOC_ADC_DIG_SUPPORTED_UNIT(config->adc_pattern[i].unit), ESP_ERR_INVALID_ARG, ADC_TAG, "Only support using ADC1 DMA mode");
#endif  //#if !CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3
    }

    ESP_RETURN_ON_FALSE(config->sample_freq_hz <= SOC_ADC_SAMPLE_FREQ_THRES_HIGH && config->sample_freq_hz >= SOC_ADC_SAMPLE_FREQ_THRES_LOW, ESP_ERR_INVALID_ARG, ADC_TAG, "ADC sampling frequency out of range");

#if CONFIG_IDF_TARGET_ESP32
    ESP_RETURN_ON_FALSE(config->format == ADC_DIGI_OUTPUT_FORMAT_TYPE1, ESP_ERR_INVALID_ARG, ADC_TAG, "Please use type1");
#elif CONFIG_IDF_TARGET_ESP32S2
    if (config->conv_mode == ADC_CONV_BOTH_UNIT || config->conv_mode == ADC_CONV_ALTER_UNIT) {
        ESP_RETURN_ON_FALSE(config->format == ADC_DIGI_OUTPUT_FORMAT_TYPE2, ESP_ERR_INVALID_ARG, ADC_TAG, "Please use type2");
    } else if (config->conv_mode == ADC_CONV_SINGLE_UNIT_1 || config->conv_mode == ADC_CONV_SINGLE_UNIT_2) {
        ESP_RETURN_ON_FALSE(config->format == ADC_DIGI_OUTPUT_FORMAT_TYPE1, ESP_ERR_INVALID_ARG, ADC_TAG, "Please use type1");
    }
#else
    ESP_RETURN_ON_FALSE(config->format == ADC_DIGI_OUTPUT_FORMAT_TYPE2, ESP_ERR_INVALID_ARG, ADC_TAG, "Please use type2");
#endif

    uint32_t clk_src_freq_hz = 0;
    esp_clk_tree_src_get_freq_hz(ADC_DIGI_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);

    handle->hal_digi_ctrlr_cfg.adc_pattern_len = config->pattern_num;
    handle->hal_digi_ctrlr_cfg.sample_freq_hz = config->sample_freq_hz;
    handle->hal_digi_ctrlr_cfg.conv_mode = config->conv_mode;
    memcpy(handle->hal_digi_ctrlr_cfg.adc_pattern, config->adc_pattern, config->pattern_num * sizeof(adc_digi_pattern_config_t));
    handle->hal_digi_ctrlr_cfg.clk_src = ADC_DIGI_CLK_SRC_DEFAULT;
    handle->hal_digi_ctrlr_cfg.clk_src_freq_hz = clk_src_freq_hz;

    const int atten_uninitialized = 999;
    handle->adc1_atten = atten_uninitialized;
    handle->adc2_atten = atten_uninitialized;
    handle->use_adc1 = 0;
    handle->use_adc2 = 0;
    uint32_t adc1_chan_mask = 0;
    uint32_t adc2_chan_mask = 0;
    for (int i = 0; i < config->pattern_num; i++) {
        const adc_digi_pattern_config_t *pat = &config->adc_pattern[i];
        if (pat->unit == ADC_UNIT_1) {
            handle->use_adc1 = 1;
            adc1_chan_mask |= BIT(pat->channel);

            if (handle->adc1_atten == atten_uninitialized) {
                handle->adc1_atten = pat->atten;
            } else if (handle->adc1_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        } else if (pat->unit == ADC_UNIT_2) {
            handle->use_adc2 = 1;
            adc2_chan_mask |= BIT(pat->channel);

            if (handle->adc2_atten == atten_uninitialized) {
                handle->adc2_atten = pat->atten;
            } else if (handle->adc2_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }

    if (handle->use_adc1) {
        adc_digi_gpio_init(ADC_UNIT_1, adc1_chan_mask);
    }
    if (handle->use_adc2) {
        adc_digi_gpio_init(ADC_UNIT_2, adc2_chan_mask);
    }

    return ESP_OK;
}

esp_err_t adc_continuous_register_event_callbacks(adc_continuous_handle_t handle, const adc_continuous_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(handle && cbs, ESP_ERR_INVALID_ARG, ADC_TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, ADC_TAG, "ADC continuous mode isn't in the init state, it's started already");

#if CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE
    if (cbs->on_conv_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_conv_done), ESP_ERR_INVALID_ARG, ADC_TAG, "on_conv_done callback not in IRAM");
    }
    if (cbs->on_pool_ovf) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_pool_ovf), ESP_ERR_INVALID_ARG, ADC_TAG, "on_pool_ovf callback not in IRAM");
    }
#endif

    handle->cbs.on_conv_done = cbs->on_conv_done;
    handle->cbs.on_pool_ovf = cbs->on_pool_ovf;
    handle->user_data = user_data;

    return ESP_OK;
}

esp_err_t adc_continuous_flush_pool(adc_continuous_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, ADC_TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, ADC_TAG, "ADC continuous mode isn't in the init state, it's started already");

    size_t actual_size = 0;
    uint8_t *old_data = NULL;

    while ((old_data = xRingbufferReceiveUpTo(handle->ringbuf_hdl, &actual_size, 0, handle->ringbuf_size))) {
        vRingbufferReturnItem(handle->ringbuf_hdl, old_data);
    }

    return ESP_OK;
}

esp_err_t adc_continuous_io_to_channel(int io_num, adc_unit_t *const unit_id, adc_channel_t *const channel)
{
    return adc_io_to_channel(io_num, unit_id, channel);
}

esp_err_t adc_continuous_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int *const io_num)
{
    return adc_channel_to_io(unit_id, channel, io_num);
}
