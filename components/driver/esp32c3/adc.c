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
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "driver/rtc_io.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "sdkconfig.h"

#include "esp32c3/rom/ets_sys.h"
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

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ADC_ENTER_CRITICAL();
    adc_hal_digi_controller_config(config);
    ADC_EXIT_CRITICAL();
    return ret;
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
 * @param ctrl ADC controller, Refer to `adc_ll_controller_t`.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_set_controller(adc_unit_t adc_unit, adc_ll_controller_t ctrl)
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
    abort(); // TODO ESP32-C3 IDF-2528
}

esp_err_t adc_digi_filter_set_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    abort(); // TODO ESP32-C3 IDF-2528
}

esp_err_t adc_digi_filter_get_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    abort(); // TODO ESP32-C3 IDF-2528
}

esp_err_t adc_digi_filter_enable(adc_digi_filter_idx_t idx, bool enable)
{
    abort(); // TODO ESP32-C3 IDF-2528
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
    abort(); // TODO ESP32-C3 IDF-2528
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


//This feature is currently supported on ESP32C3, will be supported on other chips soon
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
#include "soc/system_reg.h"
#include "hal/dma_types.h"
#include "hal/gdma_ll.h"
#include "hal/adc_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include <string.h>

#define INTERNAL_BUF_NUM 5
#define IN_SUC_EOF_BIT GDMA_LL_EVENT_RX_SUC_EOF

typedef struct adc_digi_context_t {
    intr_handle_t           dma_intr_hdl;           //MD interrupt handle
    uint32_t                bytes_between_intr;     //bytes between in suc eof intr
    uint8_t                 *rx_dma_buf;            //dma buffer
    adc_dma_hal_context_t   hal_dma;                //dma context (hal)
    adc_dma_hal_config_t    hal_dma_config;         //dma config (hal)
    RingbufHandle_t         ringbuf_hdl;            //RX ringbuffer handler
    bool                    ringbuf_overflow_flag;  //1: ringbuffer overflow
    bool                    driver_state_flag;      //1: driver is started; 2: driver is stoped
} adc_digi_context_t;


static const char* ADC_DMA_TAG = "ADC_DMA:";
static adc_digi_context_t *adc_digi_ctx = NULL;

static void adc_dma_intr(void* arg);

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
    gpio_config(&cfg);

    return ret;
}

esp_err_t adc_digi_initialize(const adc_digi_init_config_t *init_config)
{
    esp_err_t ret = ESP_OK;

    adc_digi_ctx = calloc(1, sizeof(adc_digi_context_t));
    if (adc_digi_ctx == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    ret = esp_intr_alloc(SOC_GDMA_ADC_INTR_SOURCE, 0, adc_dma_intr, (void *)adc_digi_ctx, &adc_digi_ctx->dma_intr_hdl);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    //ringbuffer
    adc_digi_ctx->ringbuf_hdl = xRingbufferCreate(init_config->max_store_buf_size, RINGBUF_TYPE_BYTEBUF);
    if (!adc_digi_ctx->ringbuf_hdl) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc internal buffer
    adc_digi_ctx->bytes_between_intr = init_config->conv_num_each_intr;
    adc_digi_ctx->rx_dma_buf = heap_caps_calloc(1, adc_digi_ctx->bytes_between_intr * INTERNAL_BUF_NUM, MALLOC_CAP_INTERNAL);
    if (!adc_digi_ctx->rx_dma_buf) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    //malloc dma descriptor
    adc_digi_ctx->hal_dma_config.rx_desc = heap_caps_calloc(1, (sizeof(dma_descriptor_t)) * INTERNAL_BUF_NUM, MALLOC_CAP_DMA);
    if (!adc_digi_ctx->hal_dma_config.rx_desc) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    adc_digi_ctx->hal_dma_config.desc_max_num = INTERNAL_BUF_NUM;
    adc_digi_ctx->hal_dma_config.dma_chan = init_config->dma_chan;

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

    periph_module_enable(PERIPH_SARADC_MODULE);
    periph_module_enable(PERIPH_GDMA_MODULE);
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    ADC_ENTER_CRITICAL();
    adc_hal_init();
    adc_hal_arbiter_config(&config);
    adc_hal_digi_init(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);
    ADC_EXIT_CRITICAL();

    return ret;

cleanup:
    adc_digi_deinitialize();
    return ret;

}

static IRAM_ATTR void adc_dma_intr(void *arg)
{
    portBASE_TYPE taskAwoken = 0;
    BaseType_t ret;

    //clear the in suc eof interrupt
    adc_hal_digi_clr_intr(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config, IN_SUC_EOF_BIT);

    while (adc_digi_ctx->hal_dma_config.cur_desc_ptr->dw0.owner == 0) {

        dma_descriptor_t *current_desc = adc_digi_ctx->hal_dma_config.cur_desc_ptr;
        ret = xRingbufferSendFromISR(adc_digi_ctx->ringbuf_hdl, current_desc->buffer, current_desc->dw0.length, &taskAwoken);
        if (ret == pdFALSE) {
            //ringbuffer overflow
            adc_digi_ctx->ringbuf_overflow_flag = 1;
        }

        adc_digi_ctx->hal_dma_config.desc_cnt += 1;
        //cycle the dma descriptor and buffers
        adc_digi_ctx->hal_dma_config.cur_desc_ptr = adc_digi_ctx->hal_dma_config.cur_desc_ptr->next;
        if (!adc_digi_ctx->hal_dma_config.cur_desc_ptr) {
            break;
        }
    }

    if (!adc_digi_ctx->hal_dma_config.cur_desc_ptr) {

        assert(adc_digi_ctx->hal_dma_config.desc_cnt == adc_digi_ctx->hal_dma_config.desc_max_num);
        //reset the current descriptor status
        adc_digi_ctx->hal_dma_config.cur_desc_ptr = adc_digi_ctx->hal_dma_config.rx_desc;
        adc_digi_ctx->hal_dma_config.desc_cnt = 0;

        //start next turns of dma operation
        adc_hal_digi_rxdma_start(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);
    }

    if(taskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t adc_digi_start(void)
{
    assert(adc_digi_ctx->driver_state_flag == 0 && "the driver is already started");
    //reset flags
    adc_digi_ctx->ringbuf_overflow_flag = 0;
    adc_digi_ctx->driver_state_flag = 1;

    //create dma descriptors
    adc_hal_digi_dma_multi_descriptor(&adc_digi_ctx->hal_dma_config, adc_digi_ctx->rx_dma_buf, adc_digi_ctx->bytes_between_intr, adc_digi_ctx->hal_dma_config.desc_max_num);
    adc_hal_digi_set_eof_num(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config, (adc_digi_ctx->bytes_between_intr)/4);
    //set the current descriptor pointer
    adc_digi_ctx->hal_dma_config.cur_desc_ptr = adc_digi_ctx->hal_dma_config.rx_desc;
    adc_digi_ctx->hal_dma_config.desc_cnt = 0;

    //enable in suc eof intr
    adc_hal_digi_ena_intr(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config, GDMA_LL_EVENT_RX_SUC_EOF);
    //start DMA
    adc_hal_digi_rxdma_start(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);
    //start ADC
    adc_hal_digi_start(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);

    return ESP_OK;
}

esp_err_t adc_digi_stop(void)
{
    assert(adc_digi_ctx->driver_state_flag == 1 && "the driver is already stoped");
    adc_digi_ctx->driver_state_flag = 0;

    //disable the in suc eof intrrupt
    adc_hal_digi_dis_intr(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config, IN_SUC_EOF_BIT);
    //clear the in suc eof interrupt
    adc_hal_digi_clr_intr(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config, IN_SUC_EOF_BIT);
    //stop DMA
    adc_hal_digi_rxdma_stop(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);
    //stop ADC
    adc_hal_digi_stop(&adc_digi_ctx->hal_dma, &adc_digi_ctx->hal_dma_config);

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

    data = xRingbufferReceiveUpTo(adc_digi_ctx->ringbuf_hdl, &size, ticks_to_wait, length_max);
    if (!data) {
        ESP_EARLY_LOGW(ADC_DMA_TAG, "No data, increase timeout or reduce conv_num_each_intr");
        ret = ESP_ERR_TIMEOUT;
        *out_length = 0;
        return ret;
    }

    memcpy(buf, data, size);
    vRingbufferReturnItem(adc_digi_ctx->ringbuf_hdl, data);
    assert((size % 4) == 0);
    *out_length = size;

    if (adc_digi_ctx->ringbuf_overflow_flag) {
        ret = ESP_ERR_INVALID_STATE;
    }
    return ret;
}

static esp_err_t adc_digi_deinit(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_deinit();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_deinitialize(void)
{
    assert(adc_digi_ctx->driver_state_flag == 0 && "the driver is not stoped");

    if (adc_digi_ctx == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (adc_digi_ctx->dma_intr_hdl) {
        esp_intr_free(adc_digi_ctx->dma_intr_hdl);
    }

    if(adc_digi_ctx->ringbuf_hdl) {
        vRingbufferDelete(adc_digi_ctx->ringbuf_hdl);
        adc_digi_ctx->ringbuf_hdl = NULL;
    }

    if (adc_digi_ctx->hal_dma_config.rx_desc) {
        free(adc_digi_ctx->hal_dma_config.rx_desc);
    }

    free(adc_digi_ctx);
    adc_digi_ctx = NULL;

    adc_digi_deinit();
    periph_module_disable(PERIPH_SARADC_MODULE);
    periph_module_disable(PERIPH_GDMA_MODULE);

    return ESP_OK;
}
