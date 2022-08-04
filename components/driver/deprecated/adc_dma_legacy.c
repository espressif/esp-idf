/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp_check.h"
#include "sys/lock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/ringbuf.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/dma_types.h"
#include "hal/adc_hal_common.h"

#include "driver/gpio.h"
#include "driver/adc_types_legacy.h"

//For calibration
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_efuse_rtc_table.h"
#elif SOC_ADC_CALIBRATION_V1_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#endif
//For DMA
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "hal/spi_types.h"
#include "esp_private/spi_common_internal.h"
#elif CONFIG_IDF_TARGET_ESP32
#include "driver/i2s_types.h"
#include "soc/i2s_periph.h"
#include "esp_private/i2s_platform.h"
#endif

static const char *ADC_TAG = "ADC";

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

#define INTERNAL_BUF_NUM 5

/*---------------------------------------------------------------
                    Digital Controller Context
---------------------------------------------------------------*/
typedef struct adc_digi_context_t {
    uint8_t                         *rx_dma_buf;                //dma buffer
    adc_hal_dma_ctx_t               hal;                        //hal context
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t           rx_dma_channel;             //dma rx channel handle
#elif CONFIG_IDF_TARGET_ESP32S2
    spi_host_device_t               spi_host;                   //ADC uses this SPI DMA
    intr_handle_t                   intr_hdl;                   //Interrupt handler
#elif CONFIG_IDF_TARGET_ESP32
    i2s_port_t                      i2s_host;                   //ADC uses this I2S DMA
    intr_handle_t                   intr_hdl;                   //Interrupt handler
#endif
    RingbufHandle_t                 ringbuf_hdl;                //RX ringbuffer handler
    intptr_t                        rx_eof_desc_addr;           //eof descriptor address of RX channel
    bool                            ringbuf_overflow_flag;      //1: ringbuffer overflow
    bool                            driver_start_flag;          //1: driver is started; 0: driver is stoped
    bool                            use_adc1;                   //1: ADC unit1 will be used; 0: ADC unit1 won't be used.
    bool                            use_adc2;                   //1: ADC unit2 will be used; 0: ADC unit2 won't be used. This determines whether to acquire sar_adc2_mutex lock or not.
    adc_atten_t                     adc1_atten;                 //Attenuation for ADC1. On this chip each ADC can only support one attenuation.
    adc_atten_t                     adc2_atten;                 //Attenuation for ADC2. On this chip each ADC can only support one attenuation.
    adc_hal_digi_ctrlr_cfg_t             hal_digi_ctrlr_cfg;         //Hal digital controller configuration
    esp_pm_lock_handle_t            pm_lock;                    //For power management
} adc_digi_context_t;

static adc_digi_context_t *s_adc_digi_ctx = NULL;
#ifdef CONFIG_PM_ENABLE
//Only for deprecated API
extern esp_pm_lock_handle_t adc_digi_arbiter_lock;
#endif  //CONFIG_PM_ENABLE

/*---------------------------------------------------------------
                   ADC Continuous Read Mode (via DMA)
---------------------------------------------------------------*/
//Function to address transaction
static bool s_adc_dma_intr(adc_digi_context_t *adc_digi_ctx);

#if SOC_GDMA_SUPPORTED
static bool adc_dma_in_suc_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);
#else
static void adc_dma_intr_handler(void *arg);
#endif

static int8_t adc_digi_get_io_num(adc_unit_t adc_unit, uint8_t adc_channel)
{
    assert(adc_unit <= SOC_ADC_PERIPH_NUM);
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
    free(s_adc_digi_ctx->hal_digi_ctrlr_cfg.adc_pattern);
#if SOC_GDMA_SUPPORTED
    gdma_disconnect(s_adc_digi_ctx->rx_dma_channel);
    gdma_del_channel(s_adc_digi_ctx->rx_dma_channel);
#elif CONFIG_IDF_TARGET_ESP32S2
    esp_intr_free(s_adc_digi_ctx->intr_hdl);
    spicommon_dma_chan_free(s_adc_digi_ctx->spi_host);
    spicommon_periph_free(s_adc_digi_ctx->spi_host);
#elif CONFIG_IDF_TARGET_ESP32
    esp_intr_free(s_adc_digi_ctx->intr_hdl);
    i2s_platform_release_occupation(s_adc_digi_ctx->i2s_host);
#endif
    free(s_adc_digi_ctx);
    s_adc_digi_ctx = NULL;

    periph_module_disable(PERIPH_SARADC_MODULE);

    return ESP_OK;
}

esp_err_t adc_digi_initialize(const adc_digi_init_config_t *init_config)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((init_config->conv_num_each_intr % SOC_ADC_DIGI_DATA_BYTES_PER_CONV == 0), ESP_ERR_INVALID_ARG, ADC_TAG, "conv_frame_size should be in multiples of `SOC_ADC_DIGI_DATA_BYTES_PER_CONV`");

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
    s_adc_digi_ctx->rx_dma_buf = heap_caps_calloc(1, init_config->conv_num_each_intr * INTERNAL_BUF_NUM, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
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
    s_adc_digi_ctx->hal_digi_ctrlr_cfg.adc_pattern = calloc(1, SOC_ADC_PATT_LEN_MAX * sizeof(adc_digi_pattern_config_t));
    if (!s_adc_digi_ctx->hal_digi_ctrlr_cfg.adc_pattern) {
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
        ret = adc_digi_gpio_init(ADC_UNIT_1, init_config->adc1_chan_mask);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }
    if (init_config->adc2_chan_mask) {
        ret = adc_digi_gpio_init(ADC_UNIT_2, init_config->adc2_chan_mask);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }

#if SOC_GDMA_SUPPORTED
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

#elif CONFIG_IDF_TARGET_ESP32S2
    //ADC utilises SPI3 DMA on ESP32S2
    bool spi_success = false;
    uint32_t dma_chan = 0;

    spi_success = spicommon_periph_claim(SPI3_HOST, "adc");
    ret = spicommon_dma_chan_alloc(SPI3_HOST, SPI_DMA_CH_AUTO, &dma_chan, &dma_chan);
    if (ret == ESP_OK) {
        s_adc_digi_ctx->spi_host = SPI3_HOST;
    }
    if (!spi_success || (s_adc_digi_ctx->spi_host != SPI3_HOST)) {
        goto cleanup;
    }

    ret = esp_intr_alloc(spicommon_irqdma_source_for_host(s_adc_digi_ctx->spi_host), 0, adc_dma_intr_handler,
                        (void *)s_adc_digi_ctx, &s_adc_digi_ctx->intr_hdl);
    if (ret != ESP_OK) {
        goto cleanup;
    }

#elif CONFIG_IDF_TARGET_ESP32
    //ADC utilises I2S0 DMA on ESP32
    uint32_t dma_chan = 0;
    ret = i2s_platform_acquire_occupation(I2S_NUM_0, "adc");
    if (ret != ESP_OK) {
        goto cleanup;
    }

    s_adc_digi_ctx->i2s_host = I2S_NUM_0;
    ret = esp_intr_alloc(i2s_periph_signal[s_adc_digi_ctx->i2s_host].irq, 0, adc_dma_intr_handler,
                        (void *)s_adc_digi_ctx, &s_adc_digi_ctx->intr_hdl);
    if (ret != ESP_OK) {
        goto cleanup;
    }
#endif

    adc_hal_dma_config_t config = {
#if SOC_GDMA_SUPPORTED
        .dev = (void *)GDMA_LL_GET_HW(0),
#elif CONFIG_IDF_TARGET_ESP32S2
        .dev = (void *)SPI_LL_GET_HW(s_adc_digi_ctx->spi_host),
#elif CONFIG_IDF_TARGET_ESP32
        .dev = (void *)I2S_LL_GET_HW(s_adc_digi_ctx->i2s_host),
#endif
        .desc_max_num = INTERNAL_BUF_NUM,
        .dma_chan = dma_chan,
        .eof_num = init_config->conv_num_each_intr / SOC_ADC_DIGI_DATA_BYTES_PER_CONV
    };
    adc_hal_dma_ctx_config(&s_adc_digi_ctx->hal, &config);

    //enable ADC digital part
    periph_module_enable(PERIPH_SARADC_MODULE);
    //reset ADC digital part
    periph_module_reset(PERIPH_SARADC_MODULE);

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_1);
    adc_hal_calibration_init(ADC_UNIT_2);
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

    return ret;

cleanup:
    adc_digi_deinitialize();
    return ret;
}

#if SOC_GDMA_SUPPORTED
static IRAM_ATTR bool adc_dma_in_suc_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(event_data);
    s_adc_digi_ctx->rx_eof_desc_addr = event_data->rx_eof_desc_addr;
    return s_adc_dma_intr(user_data);
}
#else
static IRAM_ATTR void adc_dma_intr_handler(void *arg)
{
    adc_digi_context_t *ctx = (adc_digi_context_t *)arg;
    bool need_yield = false;

    bool conversion_finish = adc_hal_check_event(&ctx->hal, ADC_HAL_DMA_INTR_MASK);
    if (conversion_finish) {
        adc_hal_digi_clr_intr(&s_adc_digi_ctx->hal, ADC_HAL_DMA_INTR_MASK);

        intptr_t desc_addr = adc_hal_get_desc_addr(&ctx->hal);

        ctx->rx_eof_desc_addr = desc_addr;
        need_yield = s_adc_dma_intr(ctx);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
#endif

static IRAM_ATTR bool s_adc_dma_intr(adc_digi_context_t *adc_digi_ctx)
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
        adc_hal_digi_start(&adc_digi_ctx->hal, adc_digi_ctx->rx_dma_buf);
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
        adc_lock_acquire(ADC_UNIT_1);
    }
    if (s_adc_digi_ctx->use_adc2) {
        adc_lock_acquire(ADC_UNIT_2);
    }

#if CONFIG_PM_ENABLE
    // Lock APB frequency while ADC driver is in use
    esp_pm_lock_acquire(s_adc_digi_ctx->pm_lock);
#endif

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    if (s_adc_digi_ctx->use_adc1) {
        adc_set_hw_calibration_code(ADC_UNIT_1, s_adc_digi_ctx->adc1_atten);
    }
    if (s_adc_digi_ctx->use_adc2) {
        adc_set_hw_calibration_code(ADC_UNIT_2, s_adc_digi_ctx->adc2_atten);
    }
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

#if SOC_ADC_ARBITER_SUPPORTED
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

    adc_hal_set_controller(ADC_UNIT_1, ADC_HAL_CONTINUOUS_READ_MODE);
    adc_hal_set_controller(ADC_UNIT_2, ADC_HAL_CONTINUOUS_READ_MODE);

    adc_hal_digi_init(&s_adc_digi_ctx->hal);
    adc_hal_digi_controller_config(&s_adc_digi_ctx->hal, &s_adc_digi_ctx->hal_digi_ctrlr_cfg);

    //start conversion
    adc_hal_digi_start(&s_adc_digi_ctx->hal, s_adc_digi_ctx->rx_dma_buf);

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
    adc_hal_digi_dis_intr(&s_adc_digi_ctx->hal, ADC_HAL_DMA_INTR_MASK);
    //clear the in suc eof interrupt
    adc_hal_digi_clr_intr(&s_adc_digi_ctx->hal, ADC_HAL_DMA_INTR_MASK);
    //stop ADC
    adc_hal_digi_stop(&s_adc_digi_ctx->hal);

    adc_hal_digi_deinit(&s_adc_digi_ctx->hal);
#if CONFIG_PM_ENABLE
    if (s_adc_digi_ctx->pm_lock) {
        esp_pm_lock_release(s_adc_digi_ctx->pm_lock);
    }
#endif  //CONFIG_PM_ENABLE

    if (s_adc_digi_ctx->use_adc2) {
        adc_lock_release(ADC_UNIT_2);
    }
    if (s_adc_digi_ctx->use_adc1) {
        adc_lock_release(ADC_UNIT_1);
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

    ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
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

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
esp_err_t adc_digi_controller_configure(const adc_digi_configuration_t *config)
{
    if (!s_adc_digi_ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    //Pattern related check
    ESP_RETURN_ON_FALSE(config->pattern_num <= SOC_ADC_PATT_LEN_MAX, ESP_ERR_INVALID_ARG, ADC_TAG, "Max pattern num is %d", SOC_ADC_PATT_LEN_MAX);
#if CONFIG_IDF_TARGET_ESP32
    for (int i = 0; i < config->pattern_num; i++) {
        ESP_RETURN_ON_FALSE((config->adc_pattern[i].bit_width >= SOC_ADC_DIGI_MIN_BITWIDTH && config->adc_pattern->bit_width <= SOC_ADC_DIGI_MAX_BITWIDTH), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC bitwidth not supported");
        ESP_RETURN_ON_FALSE(config->adc_pattern[i].unit == 0, ESP_ERR_INVALID_ARG, ADC_TAG, "Only support using ADC1 DMA mode");
    }
#else
    for (int i = 0; i < config->pattern_num; i++) {
        ESP_RETURN_ON_FALSE((config->adc_pattern[i].bit_width == SOC_ADC_DIGI_MAX_BITWIDTH), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC bitwidth not supported");
    }
#endif
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

    s_adc_digi_ctx->hal_digi_ctrlr_cfg.adc_pattern_len = config->pattern_num;
    s_adc_digi_ctx->hal_digi_ctrlr_cfg.sample_freq_hz = config->sample_freq_hz;
    s_adc_digi_ctx->hal_digi_ctrlr_cfg.conv_mode = config->conv_mode;
    memcpy(s_adc_digi_ctx->hal_digi_ctrlr_cfg.adc_pattern, config->adc_pattern, config->pattern_num * sizeof(adc_digi_pattern_config_t));

    const int atten_uninitialized = 999;
    s_adc_digi_ctx->adc1_atten = atten_uninitialized;
    s_adc_digi_ctx->adc2_atten = atten_uninitialized;
    s_adc_digi_ctx->use_adc1 = 0;
    s_adc_digi_ctx->use_adc2 = 0;
    for (int i = 0; i < config->pattern_num; i++) {
        const adc_digi_pattern_config_t *pat = &config->adc_pattern[i];
        if (pat->unit == ADC_UNIT_1) {
            s_adc_digi_ctx->use_adc1 = 1;

            if (s_adc_digi_ctx->adc1_atten == atten_uninitialized) {
                s_adc_digi_ctx->adc1_atten = pat->atten;
            } else if (s_adc_digi_ctx->adc1_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        } else if (pat->unit == ADC_UNIT_2) {
            //See whether ADC2 will be used or not. If yes, the ``sar_adc2_mutex`` should be acquired in the continuous read driver
            s_adc_digi_ctx->use_adc2 = 1;

            if (s_adc_digi_ctx->adc2_atten == atten_uninitialized) {
                s_adc_digi_ctx->adc2_atten = pat->atten;
            } else if (s_adc_digi_ctx->adc2_atten != pat->atten) {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }

    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that adc_continuous driver is not running along with the legacy adc_continuous driver
 */
__attribute__((constructor))
static void check_adc_continuous_driver_conflict(void)
{
    // This function was declared as weak here. adc_continuous driver has one implementation.
    // So if adc_continuous driver is not linked in, then `adc_continuous_new_handle` should be NULL at runtime.
    extern __attribute__((weak)) esp_err_t adc_continuous_new_handle(const void *init_config, void **ret_handle);
    if ((void *)adc_continuous_new_handle != NULL) {
        ESP_EARLY_LOGE(ADC_TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(ADC_TAG, "legacy driver is deprecated, please migrate to `esp_adc/adc_continuous.h`");
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
static __attribute__((constructor)) void adc_hw_calibration(void)
{
    //Calculate all ICode
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        adc_hal_calibration_init(i);
        for (int j = 0; j < SOC_ADC_ATTEN_NUM; j++) {
            /**
             * This may get wrong when attenuations are NOT consecutive on some chips,
             * update this when bringing up the calibration on that chip
             */
            adc_calc_hw_calibration_code(i, j);
        }
    }
}
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED
