/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "sys/lock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/ringbuf.h"
#include "esp32c3/rom/ets_sys.h"
#include "esp_private/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/dma_types.h"
#include "esp_efuse_rtc_calib.h"
#include "esp_private/gdma.h"

#define ADC_CHECK_RET(fun_ret) ({                           \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s(%d)",__FUNCTION__,__LINE__);   \
        return ESP_FAIL;                                    \
    }                                                       \
})

static const char *ADC_TAG = "ADC";

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s(%d) :%s", __FUNCTION__, __LINE__, str);                \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/**
 * 1. sar_adc1_lock: this mutex lock is to protect the SARADC1 module.
 * 2. sar_adc2_lock: this mutex lock is to protect the SARADC2 module. On C3, it is controlled by the digital controller
 *    and PWDET controller.
 * 3. adc_reg_lock:  this spin lock is to protect the shared registers used by ADC1 / ADC2 single read mode.
 */
static _lock_t sar_adc1_lock;
#define SAR_ADC1_LOCK_ACQUIRE()    _lock_acquire(&sar_adc1_lock)
#define SAR_ADC1_LOCK_RELEASE()    _lock_release(&sar_adc1_lock)
static _lock_t sar_adc2_lock;
#define SAR_ADC2_LOCK_ACQUIRE()    _lock_acquire(&sar_adc2_lock)
#define SAR_ADC2_LOCK_RELEASE()    _lock_release(&sar_adc2_lock)
portMUX_TYPE adc_reg_lock = portMUX_INITIALIZER_UNLOCKED;
#define ADC_REG_LOCK_ENTER()       portENTER_CRITICAL(&adc_reg_lock)
#define ADC_REG_LOCK_EXIT()        portEXIT_CRITICAL(&adc_reg_lock)

#define INTERNAL_BUF_NUM 5
#define IN_SUC_EOF_BIT GDMA_LL_EVENT_RX_SUC_EOF

/*---------------------------------------------------------------
                    Digital Controller Context
---------------------------------------------------------------*/
typedef struct adc_digi_context_t {
    uint8_t                 *rx_dma_buf;                //dma buffer
    adc_hal_context_t       hal;                        //hal context
    gdma_channel_handle_t   rx_dma_channel;             //dma rx channel handle
    RingbufHandle_t         ringbuf_hdl;                //RX ringbuffer handler
    intptr_t                rx_eof_desc_addr;           //eof descriptor address of RX channel
    bool                    ringbuf_overflow_flag;      //1: ringbuffer overflow
    bool                    driver_start_flag;          //1: driver is started; 0: driver is stoped
    bool                    use_adc1;                   //1: ADC unit1 will be used; 0: ADC unit1 won't be used.
    bool                    use_adc2;                   //1: ADC unit2 will be used; 0: ADC unit2 won't be used. This determines whether to acquire sar_adc2_mutex lock or not.
    adc_atten_t             adc1_atten;                 //Attenuation for ADC1. On this chip each ADC can only support one attenuation.
    adc_atten_t             adc2_atten;                 //Attenuation for ADC2. On this chip each ADC can only support one attenuation.
    adc_digi_config_t       digi_controller_config;     //Digital Controller Configuration
    esp_pm_lock_handle_t    pm_lock;                    //For power management
} adc_digi_context_t;

static adc_digi_context_t *s_adc_digi_ctx = NULL;

static uint32_t adc_get_calibration_offset(adc_ll_num_t adc_n, adc_channel_t chan, adc_atten_t atten);

/*---------------------------------------------------------------
                   ADC Continuous Read Mode (via DMA)
---------------------------------------------------------------*/
static IRAM_ATTR bool adc_dma_in_suc_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

static int8_t adc_digi_get_io_num(uint8_t adc_unit, uint8_t adc_channel)
{
    return adc_channel_io_map[adc_unit][adc_channel];
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

esp_err_t adc_digi_initialize(const adc_digi_init_config_t *init_config)
{
    esp_err_t ret = ESP_OK;

    s_adc_digi_ctx = calloc(1, sizeof(adc_digi_context_t));
    if (s_adc_digi_ctx == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //ringbuffer
    s_adc_digi_ctx->ringbuf_hdl = xRingbufferCreate(init_config->max_store_buf_size, RINGBUF_TYPE_BYTEBUF);
    if (!s_adc_digi_ctx->ringbuf_hdl) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc internal buffer used by DMA
    s_adc_digi_ctx->rx_dma_buf = heap_caps_calloc(1, init_config->conv_num_each_intr * INTERNAL_BUF_NUM, MALLOC_CAP_INTERNAL);
    if (!s_adc_digi_ctx->rx_dma_buf) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc dma descriptor
    s_adc_digi_ctx->hal.rx_desc = heap_caps_calloc(1, (sizeof(dma_descriptor_t)) * INTERNAL_BUF_NUM, MALLOC_CAP_DMA);
    if (!s_adc_digi_ctx->hal.rx_desc) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc pattern table
    s_adc_digi_ctx->digi_controller_config.adc_pattern = calloc(1, SOC_ADC_PATT_LEN_MAX * sizeof(adc_digi_pattern_table_t));
    if (!s_adc_digi_ctx->digi_controller_config.adc_pattern) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

#if CONFIG_PM_ENABLE
    ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc_dma", &s_adc_digi_ctx->pm_lock);
    if (ret != ESP_OK) {
        goto cleanup;
    }
#endif //CONFIG_PM_ENABLE

    //init gpio pins
    if (init_config->adc1_chan_mask) {
        ret = adc_digi_gpio_init(ADC_NUM_1, init_config->adc1_chan_mask);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }
    if (init_config->adc2_chan_mask) {
        ret = adc_digi_gpio_init(ADC_NUM_2, init_config->adc2_chan_mask);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }

    //alloc rx gdma channel
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };
    ret = gdma_new_channel(&rx_alloc_config, &s_adc_digi_ctx->rx_dma_channel);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    gdma_connect(s_adc_digi_ctx->rx_dma_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_ADC, 0));

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true
    };
    gdma_apply_strategy(s_adc_digi_ctx->rx_dma_channel, &strategy_config);

    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = adc_dma_in_suc_eof_callback
    };
    gdma_register_rx_event_callbacks(s_adc_digi_ctx->rx_dma_channel, &cbs, s_adc_digi_ctx);

    int dma_chan;
    gdma_get_channel_id(s_adc_digi_ctx->rx_dma_channel, &dma_chan);

    adc_hal_config_t config = {
        .desc_max_num = INTERNAL_BUF_NUM,
        .dma_chan = dma_chan,
        .eof_num = init_config->conv_num_each_intr / ADC_HAL_DATA_LEN_PER_CONV
    };
    adc_hal_context_config(&s_adc_digi_ctx->hal, &config);

    //enable SARADC module clock
    periph_module_enable(PERIPH_SARADC_MODULE);

    adc_hal_calibration_init(ADC_NUM_1);
    adc_hal_calibration_init(ADC_NUM_2);

    return ret;

cleanup:
    adc_digi_deinitialize();
    return ret;

}

static IRAM_ATTR bool adc_dma_intr(adc_digi_context_t *adc_digi_ctx);

static IRAM_ATTR bool adc_dma_in_suc_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(event_data);
    adc_digi_context_t *adc_digi_ctx = (adc_digi_context_t *)user_data;
    adc_digi_ctx->rx_eof_desc_addr = event_data->rx_eof_desc_addr;
    return adc_dma_intr(adc_digi_ctx);
}

static IRAM_ATTR bool adc_dma_intr(adc_digi_context_t *adc_digi_ctx)
{
    portBASE_TYPE taskAwoken = 0;
    BaseType_t ret;
    adc_hal_dma_desc_status_t status = false;
    dma_descriptor_t *current_desc = NULL;

    while (1) {
        status = adc_hal_get_reading_result(&adc_digi_ctx->hal, adc_digi_ctx->rx_eof_desc_addr, &current_desc);
        if (status != ADC_HAL_DMA_DESC_VALID) {
            break;
        }

        ret = xRingbufferSendFromISR(adc_digi_ctx->ringbuf_hdl, current_desc->buffer, current_desc->dw0.length, &taskAwoken);
        if (ret == pdFALSE) {
            //ringbuffer overflow
            adc_digi_ctx->ringbuf_overflow_flag = 1;
        }
    }

    if (status == ADC_HAL_DMA_DESC_NULL) {
        //start next turns of dma operation
        adc_hal_digi_rxdma_start(&adc_digi_ctx->hal, adc_digi_ctx->rx_dma_buf);
    }

    return (taskAwoken == pdTRUE);
}

esp_err_t adc_digi_start(void)
{
    if (s_adc_digi_ctx->driver_start_flag != 0) {
        ESP_LOGE(ADC_TAG, "The driver is already started");
        return ESP_ERR_INVALID_STATE;
    }
    adc_power_acquire();
    //reset flags
    s_adc_digi_ctx->ringbuf_overflow_flag = 0;
    s_adc_digi_ctx->driver_start_flag = 1;
    if (s_adc_digi_ctx->use_adc1) {
        SAR_ADC1_LOCK_ACQUIRE();
    }
    if (s_adc_digi_ctx->use_adc2) {
        SAR_ADC2_LOCK_ACQUIRE();
    }

#if CONFIG_PM_ENABLE
    // Lock APB frequency while ADC driver is in use
    esp_pm_lock_acquire(s_adc_digi_ctx->pm_lock);
#endif

    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    if (s_adc_digi_ctx->use_adc1) {
        uint32_t cal_val = adc_get_calibration_offset(ADC_NUM_1, ADC_CHANNEL_MAX, s_adc_digi_ctx->adc1_atten);
        adc_hal_set_calibration_param(ADC_NUM_1, cal_val);
    }
    if (s_adc_digi_ctx->use_adc2) {
        uint32_t cal_val = adc_get_calibration_offset(ADC_NUM_2, ADC_CHANNEL_MAX, s_adc_digi_ctx->adc2_atten);
        adc_hal_set_calibration_param(ADC_NUM_2, cal_val);
    }

    adc_hal_init();
    adc_hal_arbiter_config(&config);
    adc_hal_digi_init(&s_adc_digi_ctx->hal);
    adc_hal_digi_controller_config(&s_adc_digi_ctx->digi_controller_config);

    //reset ADC and DMA
    adc_hal_fifo_reset(&s_adc_digi_ctx->hal);
    //start DMA
    adc_hal_digi_rxdma_start(&s_adc_digi_ctx->hal, s_adc_digi_ctx->rx_dma_buf);
    //start ADC
    adc_hal_digi_start(&s_adc_digi_ctx->hal);

    return ESP_OK;
}

esp_err_t adc_digi_stop(void)
{
    if (s_adc_digi_ctx->driver_start_flag != 1) {
        ESP_LOGE(ADC_TAG, "The driver is already stopped");
        return ESP_ERR_INVALID_STATE;
    }
    s_adc_digi_ctx->driver_start_flag = 0;

    //disable the in suc eof intrrupt
    adc_hal_digi_dis_intr(&s_adc_digi_ctx->hal, IN_SUC_EOF_BIT);
    //clear the in suc eof interrupt
    adc_hal_digi_clr_intr(&s_adc_digi_ctx->hal, IN_SUC_EOF_BIT);
    //stop ADC
    adc_hal_digi_stop(&s_adc_digi_ctx->hal);
    //stop DMA
    adc_hal_digi_rxdma_stop(&s_adc_digi_ctx->hal);
    adc_hal_digi_deinit();
#if CONFIG_PM_ENABLE
    if (s_adc_digi_ctx->pm_lock) {
        esp_pm_lock_release(s_adc_digi_ctx->pm_lock);
    }
#endif  //CONFIG_PM_ENABLE

    if (s_adc_digi_ctx->use_adc1) {
        SAR_ADC1_LOCK_RELEASE();
    }
    if (s_adc_digi_ctx->use_adc2) {
        SAR_ADC2_LOCK_RELEASE();
    }
    adc_power_release();

    return ESP_OK;
}

esp_err_t adc_digi_read_bytes(uint8_t *buf, uint32_t length_max, uint32_t *out_length, uint32_t timeout_ms)
{
    TickType_t ticks_to_wait;
    esp_err_t ret = ESP_OK;
    uint8_t *data = NULL;
    size_t size = 0;

    ticks_to_wait = timeout_ms / portTICK_RATE_MS;
    if (timeout_ms == ADC_MAX_DELAY) {
        ticks_to_wait = portMAX_DELAY;
    }

    data = xRingbufferReceiveUpTo(s_adc_digi_ctx->ringbuf_hdl, &size, ticks_to_wait, length_max);
    if (!data) {
        ESP_LOGV(ADC_TAG, "No data, increase timeout or reduce conv_num_each_intr");
        ret = ESP_ERR_TIMEOUT;
        *out_length = 0;
        return ret;
    }

    memcpy(buf, data, size);
    vRingbufferReturnItem(s_adc_digi_ctx->ringbuf_hdl, data);
    assert((size % 4) == 0);
    *out_length = size;

    if (s_adc_digi_ctx->ringbuf_overflow_flag) {
        ret = ESP_ERR_INVALID_STATE;
    }

    return ret;
}

esp_err_t adc_digi_deinitialize(void)
{
    if (!s_adc_digi_ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_adc_digi_ctx->driver_start_flag != 0) {
        ESP_LOGE(ADC_TAG, "The driver is not stopped");
        return ESP_ERR_INVALID_STATE;
    }

    if (s_adc_digi_ctx->ringbuf_hdl) {
        vRingbufferDelete(s_adc_digi_ctx->ringbuf_hdl);
        s_adc_digi_ctx->ringbuf_hdl = NULL;
    }

#if CONFIG_PM_ENABLE
    if (s_adc_digi_ctx->pm_lock) {
        esp_pm_lock_delete(s_adc_digi_ctx->pm_lock);
    }
#endif  //CONFIG_PM_ENABLE

    free(s_adc_digi_ctx->rx_dma_buf);
    free(s_adc_digi_ctx->hal.rx_desc);
    free(s_adc_digi_ctx->digi_controller_config.adc_pattern);
    gdma_disconnect(s_adc_digi_ctx->rx_dma_channel);
    gdma_del_channel(s_adc_digi_ctx->rx_dma_channel);

    free(s_adc_digi_ctx);
    s_adc_digi_ctx = NULL;

    periph_module_disable(PERIPH_SARADC_MODULE);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    ADC Single Read Mode
---------------------------------------------------------------*/
static adc_atten_t s_atten1_single[ADC1_CHANNEL_MAX];    //Array saving attenuate of each channel of ADC1, used by single read API
static adc_atten_t s_atten2_single[ADC2_CHANNEL_MAX];    //Array saving attenuate of each channel of ADC2, used by single read API

esp_err_t adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio)
{
    esp_err_t ret;
    uint32_t channel = ADC2_CHANNEL_MAX;
    if (adc_unit == ADC_UNIT_2) {
        for (int i = 0; i < ADC2_CHANNEL_MAX; i++) {
            if (gpio == ADC_GET_IO_NUM(ADC_NUM_2, i)) {
                channel = i;
                break;
            }
        }
        if (channel == ADC2_CHANNEL_MAX) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    adc_power_acquire();
    if (adc_unit & ADC_UNIT_1) {
        ADC_ENTER_CRITICAL();
        adc_hal_vref_output(ADC_NUM_1, channel, true);
        ADC_EXIT_CRITICAL();
    } else if (adc_unit & ADC_UNIT_2) {
        ADC_ENTER_CRITICAL();
        adc_hal_vref_output(ADC_NUM_2, channel, true);
        ADC_EXIT_CRITICAL();
    }

    ret = adc_digi_gpio_init(ADC_NUM_2, BIT(channel));

    return ret;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    //On ESP32C3, the data width is always 12-bits.
    if (width_bit != ADC_WIDTH_BIT_12) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    ADC_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_OK;
    s_atten1_single[channel] = atten;
    ret = adc_digi_gpio_init(ADC_NUM_1, BIT(channel));

    adc_hal_calibration_init(ADC_NUM_1);

    return ret;
}

int adc1_get_raw(adc1_channel_t channel)
{
    int raw_out = 0;

    periph_module_enable(PERIPH_SARADC_MODULE);
    adc_power_acquire();

    SAR_ADC1_LOCK_ACQUIRE();

    adc_atten_t atten = s_atten1_single[channel];
    uint32_t cal_val = adc_get_calibration_offset(ADC_NUM_1, channel, atten);
    adc_hal_set_calibration_param(ADC_NUM_1, cal_val);

    ADC_REG_LOCK_ENTER();
    adc_hal_set_atten(ADC_NUM_2, channel, atten);
    adc_hal_convert(ADC_NUM_1, channel, &raw_out);
    ADC_REG_LOCK_EXIT();

    SAR_ADC1_LOCK_RELEASE();

    adc_power_release();
    periph_module_disable(PERIPH_SARADC_MODULE);

    return raw_out;
}

esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    ADC_CHECK(atten <= ADC_ATTEN_11db, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_OK;
    s_atten2_single[channel] = atten;
    ret = adc_digi_gpio_init(ADC_NUM_2, BIT(channel));

    adc_hal_calibration_init(ADC_NUM_2);

    return ret;
}

esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out)
{
    //On ESP32C3, the data width is always 12-bits.
    if (width_bit != ADC_WIDTH_BIT_12) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    periph_module_enable(PERIPH_SARADC_MODULE);
    adc_power_acquire();

    SAR_ADC2_LOCK_ACQUIRE();

    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);

    adc_atten_t atten = s_atten2_single[channel];
    uint32_t cal_val = adc_get_calibration_offset(ADC_NUM_2, channel, atten);
    adc_hal_set_calibration_param(ADC_NUM_2, cal_val);

    ADC_REG_LOCK_ENTER();
    adc_hal_set_atten(ADC_NUM_2, channel, atten);
    ret = adc_hal_convert(ADC_NUM_2, channel, raw_out);
    ADC_REG_LOCK_EXIT();

    SAR_ADC2_LOCK_RELEASE();

    adc_power_release();
    periph_module_disable(PERIPH_SARADC_MODULE);

    return ret;
}


/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
    if (!s_adc_digi_ctx) {
        return ESP_ERR_INVALID_STATE;
    }
    ADC_CHECK(config->sample_freq_hz <= SOC_ADC_SAMPLE_FREQ_THRES_HIGH && config->sample_freq_hz >= SOC_ADC_SAMPLE_FREQ_THRES_LOW, "ADC sampling frequency out of range", ESP_ERR_INVALID_ARG);

    s_adc_digi_ctx->digi_controller_config.conv_limit_en = config->conv_limit_en;
    s_adc_digi_ctx->digi_controller_config.conv_limit_num = config->conv_limit_num;
    s_adc_digi_ctx->digi_controller_config.adc_pattern_len = config->adc_pattern_len;
    s_adc_digi_ctx->digi_controller_config.sample_freq_hz = config->sample_freq_hz;
    memcpy(s_adc_digi_ctx->digi_controller_config.adc_pattern, config->adc_pattern, config->adc_pattern_len * sizeof(adc_digi_pattern_table_t));

    const int atten_uninitialised = 999;
    s_adc_digi_ctx->adc1_atten = atten_uninitialised;
    s_adc_digi_ctx->adc2_atten = atten_uninitialised;
    s_adc_digi_ctx->use_adc1 = 0;
    s_adc_digi_ctx->use_adc2 = 0;
    for (int i = 0; i < config->adc_pattern_len; i++) {
        const adc_digi_pattern_table_t *pat = &config->adc_pattern[i];
        if (pat->unit == ADC_NUM_1) {
            s_adc_digi_ctx->use_adc1 = 1;

            if (s_adc_digi_ctx->adc1_atten == atten_uninitialised) {
                s_adc_digi_ctx->adc1_atten = pat->atten;
            } else if (s_adc_digi_ctx->adc1_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        } else if (pat->unit == ADC_NUM_2) {
            //See whether ADC2 will be used or not. If yes, the ``sar_adc2_mutex`` should be acquired in the continuous read driver
            s_adc_digi_ctx->use_adc2 = 1;

            if (s_adc_digi_ctx->adc2_atten == atten_uninitialised) {
                s_adc_digi_ctx->adc2_atten = pat->atten;
            } else if (s_adc_digi_ctx->adc2_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }

    return ESP_OK;
}

/*************************************/
/* Digital controller filter setting */
/*************************************/

esp_err_t adc_digi_filter_reset(adc_digi_filter_idx_t idx)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_filter_reset(idx);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_set_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_filter_set_factor(idx, config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_get_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_filter_get_factor(idx, config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_enable(adc_digi_filter_idx_t idx, bool enable)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_filter_enable(idx, enable);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**************************************/
/* Digital controller monitor setting */
/**************************************/

esp_err_t adc_digi_monitor_set_config(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *config)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_monitor_config(idx, config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_monitor_enable(adc_digi_monitor_idx_t idx, bool enable)
{

    ADC_ENTER_CRITICAL();
    adc_hal_digi_monitor_enable(idx, enable);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/

static uint16_t s_adc_cali_param[ADC_UNIT_MAX][ADC_ATTEN_MAX] = {};

//NOTE: according to calibration version, different types of lock may be taken during the process:
//  1. Semaphore when reading efuse
//  2. Lock (Spinlock, or Mutex) if we actually do ADC calibration in the future
//This function shoudn't be called inside critical section or ISR
static uint32_t adc_get_calibration_offset(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    const bool no_cal = false;
    if (s_adc_cali_param[adc_n][atten]) {
        return (uint32_t)s_adc_cali_param[adc_n][atten];
    }

    if (no_cal) {
        return 0;   //indicating failure
    }

    // check if we can fetch the values from eFuse.
    int version = esp_efuse_rtc_calib_get_ver();

    uint32_t init_code = 0;
    if (version == 1) {
        //for calibration v1, both ADC units use the same init code (calibrated by ADC1)
        init_code = esp_efuse_rtc_calib_get_init_code(version, atten);
        ESP_LOGD(ADC_TAG, "Calib(V%d) ADC0, 1 atten=%d: %04X", version, atten, init_code);
        s_adc_cali_param[0][atten] = init_code;
        s_adc_cali_param[1][atten] = init_code;
    } else {
        adc_power_acquire();
        ADC_ENTER_CRITICAL();
        const bool internal_gnd = true;
        init_code = adc_hal_self_calibration(adc_n, channel, atten, internal_gnd);
        ADC_EXIT_CRITICAL();
        adc_power_release();

        ESP_LOGD(ADC_TAG, "Calib(V%d) ADC%d atten=%d: %04X", version, adc_n, atten, init_code);
        s_adc_cali_param[adc_n][atten] = init_code;
    }

    return init_code;
}

// Internal function to calibrate PWDET for WiFi
esp_err_t adc_cal_offset(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    adc_hal_calibration_init(adc_n);
    uint32_t cal_val = adc_get_calibration_offset(adc_n, channel, atten);
    ADC_ENTER_CRITICAL();
    adc_hal_set_calibration_param(adc_n, cal_val);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}
