/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "soc/i2s_periph.h"
#include "soc/soc_caps.h"
#include "soc/soc.h"
#include "hal/gpio_hal.h"
#include "hal/i2s_hal.h"

#if SOC_I2S_SUPPORTS_ADC_DAC
#include "hal/adc_ll.h"
#endif
#if SOC_I2S_SUPPORTS_APLL
#include "clk_ctrl_os.h"
#endif

#include "esp_private/i2s_platform.h"
#include "esp_private/periph_ctrl.h"

#include "driver/gpio.h"
#include "driver/i2s_common.h"
#include "i2s_private.h"

#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"
#include "esp_attr.h"

// // #include "esp_efuse.h"
#include "esp_rom_gpio.h"

#include "sdkconfig.h"

#define I2S_DMA_BUFFER_MAX_SIZE     (4092)

// If ISR handler is allowed to run whilst cache is disabled,
// Make sure all the code and related variables used by the handler are in the SRAM
#if CONFIG_I2S_ISR_IRAM_SAFE
#define I2S_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#define I2S_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define I2S_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#define I2S_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif //CONFIG_I2S_ISR_IRAM_SAFE

/**
 * @brief Static i2s platform object
 * @note  For saving all the I2S related information
 */
i2s_platform_t s_i2s = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
    .controller[0 ... (I2S_NUM_MAX - 1)] = NULL, // groups will be lazy installed
    .comp_name[0 ... (I2S_NUM_MAX - 1)] = NULL,
};

static const char *TAG = "I2S_COMMON";

/*---------------------------------------------------------------------------
                             I2S Static APIs
 ----------------------------------------------------------------------------
    Scope: This file only
 ----------------------------------------------------------------------------*/

static void i2s_tx_start(i2s_chan_handle_t handle)
{
    /* No lock here beacuse semaphore has been taken while calling this function */
    i2s_hal_tx_reset(&(handle->parent->hal));
    i2s_hal_tx_reset_fifo(&(handle->parent->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(handle->dma.dma_chan);
    gdma_start(handle->dma.dma_chan, (uint32_t) handle->dma.desc[0]);
#else
    i2s_hal_tx_reset_dma(&(handle->parent->hal));
    i2s_hal_tx_enable_intr(&(handle->parent->hal));
    i2s_hal_tx_start_link(&(handle->parent->hal), (uint32_t) handle->dma.desc[0]);
#endif
    i2s_hal_tx_start(&(handle->parent->hal));
}

static void i2s_rx_start(i2s_chan_handle_t handle)
{
    /* No lock here beacuse semaphore has been taken while calling this function */
    i2s_hal_rx_reset(&(handle->parent->hal));
    i2s_hal_rx_reset_fifo(&(handle->parent->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(handle->dma.dma_chan);
    gdma_start(handle->dma.dma_chan, (uint32_t) handle->dma.desc[0]);
#else
    i2s_hal_rx_reset_dma(&(handle->parent->hal));
    i2s_hal_rx_enable_intr(&(handle->parent->hal));
    i2s_hal_rx_start_link(&(handle->parent->hal), (uint32_t) handle->dma.desc[0]);
#endif
    i2s_hal_rx_start(&(handle->parent->hal));
}

static void i2s_tx_stop(i2s_chan_handle_t handle)
{
    /* No lock here beacuse semaphore has been taken while calling this function */
#if SOC_GDMA_SUPPORTED
    i2s_hal_tx_stop(&(handle->parent->hal));
    gdma_stop(handle->dma.dma_chan);
#else
    i2s_hal_tx_stop(&(handle->parent->hal));
    i2s_hal_tx_stop_link(&(handle->parent->hal));
    i2s_hal_tx_disable_intr(&(handle->parent->hal));
#endif
}

static void i2s_rx_stop(i2s_chan_handle_t handle)
{
    /* No lock here beacuse semaphore has been taken while calling this function */
#if SOC_GDMA_SUPPORTED
    i2s_hal_rx_stop(&(handle->parent->hal));
    gdma_stop(handle->dma.dma_chan);
#else
    i2s_hal_rx_stop(&(handle->parent->hal));
    i2s_hal_rx_stop_link(&(handle->parent->hal));
    i2s_hal_rx_disable_intr(&(handle->parent->hal));
#endif
}

static esp_err_t i2s_destroy_controller_obj(i2s_controller_t **i2s_obj)
{
    I2S_NULL_POINTER_CHECK(TAG, i2s_obj);
    I2S_NULL_POINTER_CHECK(TAG, *i2s_obj);
    ESP_RETURN_ON_FALSE(!(*i2s_obj)->rx_chan && !(*i2s_obj)->tx_chan,
                        ESP_ERR_INVALID_STATE, TAG,
                        "there still have channels under this i2s controller");
    int id = (*i2s_obj)->id;
#if SOC_I2S_HW_VERSION_1
    i2s_ll_enable_dma((*i2s_obj)->hal.dev, false);
    esp_intr_disable((*i2s_obj)->i2s_isr_handle);
    esp_intr_free((*i2s_obj)->i2s_isr_handle);
#endif
    free(*i2s_obj);
    *i2s_obj = NULL;
    return i2s_platform_release_occupation(id);
}

/**
 * @brief Acquire i2s controller object
 *
 * @param id        i2s port id
 * @param search_reverse   reverse the sequence of port acquirement
 *                  set false to acquire from I2S_NUM_0 first
 *                  set true to acquire from I2S_NUM_MAX - 1 first
 * @return
 *      - pointer of acquired i2s controller object
 */
static i2s_controller_t *i2s_acquire_controller_obj(int id)
{
    if (id < 0 || id >= I2S_NUM_MAX) {
        return NULL;
    }
    /* pre-alloc controller object */
    i2s_controller_t *pre_alloc = (i2s_controller_t *)heap_caps_calloc(1, sizeof(i2s_controller_t), I2S_MEM_ALLOC_CAPS);
    if (pre_alloc == NULL) {
        return NULL;
    }
    pre_alloc->id = id;
    i2s_hal_init(&pre_alloc->hal, id);
#if !SOC_GDMA_SUPPORTED
    pre_alloc->i2s_isr_handle = NULL;
#endif
    pre_alloc->full_duplex = false;
    pre_alloc->tx_chan = NULL;
    pre_alloc->rx_chan = NULL;
    pre_alloc->mclk = I2S_GPIO_UNUSED;

    i2s_controller_t *i2s_obj = NULL;
    if (!s_i2s.controller[id]) {
        /* Try to occupy this i2s controller
           if failed, this controller could be occupied by other components */
        if (i2s_platform_acquire_occupation(id, "i2s_driver") == ESP_OK) {
            i2s_obj = pre_alloc;
            portENTER_CRITICAL(&s_i2s.spinlock);
            s_i2s.controller[id] = i2s_obj;
            portEXIT_CRITICAL(&s_i2s.spinlock);
        #if SOC_I2S_SUPPORTS_ADC_DAC
            if (id == I2S_NUM_0) {
                adc_ll_digi_set_data_source(ADC_I2S_DATA_SRC_IO_SIG);
            }
        #endif
        } else {
            free(pre_alloc);
            ESP_LOGE(TAG, "i2s%d might be occupied by other component", id);
        }
    } else {
        i2s_obj = s_i2s.controller[id];
        free(pre_alloc);
    }

    return i2s_obj;
}

static inline bool i2s_take_available_channel(i2s_controller_t *i2s_obj, uint8_t chan_search_mask)
{
    bool is_available = false;

#if SOC_I2S_HW_VERSION_1
    /* In ESP32 and ESP32-S2, tx channel and rx channel are not totally separated
     * Take both two channels in case one channel can affect another
     */
    chan_search_mask = I2S_DIR_RX | I2S_DIR_TX;
#endif
    portENTER_CRITICAL(&s_i2s.spinlock);
    if (!(chan_search_mask & i2s_obj->chan_occupancy)) {
        i2s_obj->chan_occupancy |= chan_search_mask;
        is_available = true;
    }
    portEXIT_CRITICAL(&s_i2s.spinlock);
    return is_available;
}

static esp_err_t i2s_register_channel(i2s_controller_t *i2s_obj, i2s_dir_t dir)
{
    I2S_NULL_POINTER_CHECK(TAG, i2s_obj);

    i2s_chan_handle_t new_chan = (i2s_chan_handle_t)heap_caps_calloc(1, sizeof(struct i2s_channel_t), I2S_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(new_chan, ESP_ERR_NO_MEM, TAG, "No memory for new channel");
    new_chan->mode = I2S_COMM_MODE_NONE;
    new_chan->role = I2S_ROLE_MASTER; // Set default role to master
    new_chan->dir = dir;
    new_chan->state = I2S_CHAN_STATE_REGISTER;
#if SOC_I2S_SUPPORTS_APLL
    new_chan->apll_en = false;
#endif
    new_chan->mode_info = NULL;
    new_chan->parent = i2s_obj;
    new_chan->event_queue = NULL;
    new_chan->pm_lock = NULL; // Init in i2s_set_clock according to clock source
    new_chan->msg_queue = NULL;
    new_chan->mutex = xSemaphoreCreateMutex();
    new_chan->start = NULL;
    new_chan->stop = NULL;

    if (!new_chan->mutex) {
        ESP_LOGE(TAG, "mutex create failed");
        free(new_chan);
        return ESP_ERR_NO_MEM;
    }
    if (dir == I2S_DIR_TX) {
        if (i2s_obj->tx_chan) {
            i2s_del_channel(i2s_obj->tx_chan);
        }
        i2s_obj->tx_chan = new_chan;

    } else {
        if (i2s_obj->rx_chan) {
            i2s_del_channel(i2s_obj->rx_chan);
        }
        i2s_obj->rx_chan = new_chan;
    }
    return ESP_OK;
}

uint32_t i2s_get_buf_size(i2s_chan_handle_t handle, uint32_t data_bit_width, uint32_t dma_frame_num)
{
    uint32_t active_chan = handle->active_slot;
    uint32_t bytes_per_sample = ((data_bit_width + 15) / 16) * 2;
    uint32_t bytes_per_frame = bytes_per_sample * active_chan;
    uint32_t bufsize = dma_frame_num * bytes_per_frame;
    /* Limit DMA buffer size if it is out of range (DMA buffer limitation is 4092 bytes) */
    if (bufsize > I2S_DMA_BUFFER_MAX_SIZE) {
        uint32_t frame_num = I2S_DMA_BUFFER_MAX_SIZE / bytes_per_frame;
        bufsize = frame_num * bytes_per_frame;
        ESP_LOGW(TAG, "dma frame num is out of dma buffer size, limited to %d", frame_num);
    }
    return bufsize;
}

esp_err_t i2s_free_dma_desc(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    if (!handle->dma.desc) {
        return ESP_OK;
    }
    for (int i = 0; i < handle->dma.desc_num; i++) {
        if (handle->dma.bufs[i]) {
            free(handle->dma.bufs[i]);
        }
        if (handle->dma.desc[i]) {
            free(handle->dma.desc[i]);
        }
    }
    free(handle->dma.bufs);
    free(handle->dma.desc);
    handle->dma.desc = NULL;

    return ESP_OK;
}

esp_err_t i2s_alloc_dma_desc(i2s_chan_handle_t handle, uint32_t num, uint32_t bufsize)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(bufsize <= I2S_DMA_BUFFER_MAX_SIZE, ESP_ERR_INVALID_ARG, TAG, "dma buffer can't be bigger than %d", I2S_DMA_BUFFER_MAX_SIZE);
    handle->dma.desc_num = num;
    handle->dma.buf_size = bufsize;

    handle->msg_queue = xQueueCreate(num, sizeof(uint8_t *));
    ESP_GOTO_ON_FALSE(handle->msg_queue, ESP_ERR_NO_MEM, err, TAG, "message queue create failed");

    handle->dma.desc = (lldesc_t **)heap_caps_calloc(num, sizeof(lldesc_t *), I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.desc, ESP_ERR_NO_MEM, err, TAG, "create I2S DMA decriptor array failed");
    handle->dma.bufs = (uint8_t **)heap_caps_calloc(num, sizeof(uint8_t *), I2S_MEM_ALLOC_CAPS);
    for (int i = 0; i < num; i++) {
        /* Allocate DMA descriptor */
        handle->dma.desc[i] = (lldesc_t *) heap_caps_calloc(1, sizeof(lldesc_t), MALLOC_CAP_DMA);
        ESP_GOTO_ON_FALSE(handle->dma.desc[i], ESP_ERR_NO_MEM, err, TAG,  "allocate DMA description failed");
        handle->dma.desc[i]->owner = 1;
        handle->dma.desc[i]->eof = 1;
        handle->dma.desc[i]->sosf = 0;
        handle->dma.desc[i]->length = bufsize;
        handle->dma.desc[i]->size = bufsize;
        handle->dma.desc[i]->offset = 0;
        handle->dma.bufs[i] = (uint8_t *) heap_caps_calloc(1, bufsize * sizeof(uint8_t), MALLOC_CAP_DMA);
        handle->dma.desc[i]->buf = handle->dma.bufs[i];
        ESP_GOTO_ON_FALSE(handle->dma.desc[i]->buf, ESP_ERR_NO_MEM, err, TAG,  "allocate DMA buffer failed");
    }
    /* Connect DMA descriptor as a circle */
    for (int i = 0; i < num; i++) {
        /* Link to the next descriptor */
        handle->dma.desc[i]->empty = (uint32_t)((i < (num - 1)) ? (handle->dma.desc[i + 1]) : handle->dma.desc[0]);
    }
    if (handle->dir == I2S_DIR_RX) {
        i2s_ll_rx_set_eof_num(handle->parent->hal.dev, bufsize);
    }
    ESP_LOGD(TAG, "DMA malloc info: dma_desc_num = %d, dma_desc_buf_size = dma_frame_num * slot_num * data_bit_width = %d, ", bufsize, num);
    return ESP_OK;
err:
    i2s_free_dma_desc(handle);
    return ret;
}

#if SOC_I2S_SUPPORTS_APLL
uint32_t i2s_set_get_apll_freq(uint32_t mclk_freq)
{
    /* Calculate the expected APLL  */
    int mclk_div = (int)((SOC_APLL_MIN_HZ / mclk_freq) + 1);
    /* apll_freq = mclk * div
        * when div = 1, hardware will still divide 2
        * when div = 0, the final mclk will be unpredictable
        * So the div here should be at least 2 */
    mclk_div = mclk_div < 2 ? 2 : mclk_div;
    uint32_t expt_freq = mclk_freq * mclk_div;
    uint32_t real_freq = 0;
    esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
    if (ret == ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "set APLL freq failed due to invalid argument");
        return 0;
    }
    if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "APLL is occupied already, it is working at %d Hz while the expected frequency is %d Hz", real_freq, expt_freq);
            ESP_LOGW(TAG, "Trying to work at %d Hz...", real_freq);
    }
    ESP_LOGI(TAG, "APLL expected frequency is %d Hz, real frequency is %d Hz", expt_freq, real_freq);
    return real_freq;
}
#endif

static inline void i2s_isr_send_event_queue(QueueHandle_t event_queue, i2s_event_type_t type, portBASE_TYPE *need_yield)
{
    if (event_queue) {
        i2s_event_t i2s_event;
        portBASE_TYPE _need_yield;
        i2s_event.type = type;
        xQueueSendFromISR(event_queue, (void * )&i2s_event, &_need_yield);
        *need_yield |= _need_yield;
    }
}

#if SOC_GDMA_SUPPORTED
static bool IRAM_ATTR i2s_dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_chan_handle_t handle = (i2s_chan_handle_t)user_data;
    portBASE_TYPE need_yield = 0;
    BaseType_t ret = 0;
    lldesc_t *finish_desc;

    if (handle) {
        finish_desc = (lldesc_t *)event_data->rx_eof_desc_addr;
        ret = xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield);
        i2s_event_type_t type = (ret == pdTRUE) ? I2S_EVENT_RX_DONE : I2S_EVENT_RX_Q_OVF;
        i2s_isr_send_event_queue(handle->event_queue, type, &need_yield);
    }
    return need_yield;
}

static bool IRAM_ATTR i2s_dma_tx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_chan_handle_t handle = (i2s_chan_handle_t)user_data;
    portBASE_TYPE need_yield = 0;
    BaseType_t ret;
    lldesc_t *finish_desc;
    if (handle) {
        finish_desc = (lldesc_t *)(event_data->tx_eof_desc_addr);
        if (handle->dma.auto_clear) {
            uint8_t *sent_buf = (uint8_t *)finish_desc->buf;
            memset(sent_buf, 0, handle->dma.buf_size);
        }
        ret = xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield);
        i2s_event_type_t type = (ret == pdTRUE) ? I2S_EVENT_TX_DONE : I2S_EVENT_TX_Q_OVF;
        i2s_isr_send_event_queue(handle->event_queue, type, &need_yield);
    }
    return need_yield;
}

#else
static void IRAM_ATTR i2s_default_intr_handler(void *arg)
{
    portBASE_TYPE tx_need_yield = 0;
    portBASE_TYPE rx_need_yield = 0;
    lldesc_t *finish_desc = NULL;
    BaseType_t ret;
    i2s_controller_t *obj = (i2s_controller_t *)arg;
    uint32_t status = i2s_hal_get_intr_status(&(obj->hal));
    i2s_hal_clear_intr_status(&(obj->hal), status);
    if (!obj || !status) {
        return;
    }

    if (obj->tx_chan && (status & I2S_LL_EVENT_TX_EOF)) {
        i2s_hal_get_out_eof_des_addr(&(obj->hal), (uint32_t *)&finish_desc);
        // Auto clear the dma buffer after data sent
        if (obj->tx_chan->dma.auto_clear) {
            uint8_t *buff = (uint8_t *)finish_desc->buf;
            memset(buff, 0, obj->tx_chan->dma.buf_size);
        }
        ret = xQueueSendFromISR(obj->tx_chan->msg_queue, &(finish_desc->buf), &tx_need_yield);
        i2s_event_type_t type = (ret == pdTRUE) ? I2S_EVENT_TX_DONE : I2S_EVENT_TX_Q_OVF;
        i2s_isr_send_event_queue(obj->tx_chan->event_queue, type, &tx_need_yield);
    }

    if (obj->rx_chan && (status & I2S_LL_EVENT_RX_EOF)) {
        i2s_hal_get_in_eof_des_addr(&(obj->hal), (uint32_t *)&finish_desc);
        ret = xQueueSendFromISR(obj->rx_chan->msg_queue, &(finish_desc->buf), &rx_need_yield);
        i2s_event_type_t type = (ret == pdTRUE) ? I2S_EVENT_RX_DONE : I2S_EVENT_RX_Q_OVF;
        i2s_isr_send_event_queue(obj->rx_chan->event_queue, type, &rx_need_yield);
    }

    if (tx_need_yield || rx_need_yield) {
        portYIELD_FROM_ISR();
    }
}
#endif

/**
 * @brief   I2S DMA interrupt initialization
 * @note    I2S will use GDMA if chip supports, and the interrupt is triggered by GDMA.
 *
 * @param   handle      I2S channel handle
 * @param   intr_flag   Interrupt allocation flag
 * @return
 *      - ESP_OK                    I2S DMA interrupt initialize success
 *      - ESP_ERR_NOT_FOUND         GDMA channel not found
 *      - ESP_ERR_INVALID_ARG       Invalid arguments
 *      - ESP_ERR_INVALID_STATE     GDMA state error
 */
esp_err_t i2s_init_dma_intr(i2s_chan_handle_t handle, int intr_flag)
{
    i2s_port_t port_id = handle->parent->id;
    ESP_RETURN_ON_FALSE((port_id >= 0) && (port_id < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid handle");
#if SOC_GDMA_SUPPORTED
    /* Set GDMA trigger module */
    gdma_trigger_t trig = {.periph = GDMA_TRIG_PERIPH_I2S};

    switch (port_id) {
#if SOC_I2S_NUM > 1
    case I2S_NUM_1:
        trig.instance_id = SOC_GDMA_TRIG_PERIPH_I2S1;
        break;
#endif
    default:
        trig.instance_id = SOC_GDMA_TRIG_PERIPH_I2S0;
        break;
    }

    /* Set GDMA config */
    gdma_channel_alloc_config_t dma_cfg = {};
    if (handle->dir & I2S_DIR_TX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_TX;
        /* Register a new GDMA tx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &handle->dma.dma_chan), TAG, "Register tx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(handle->dma.dma_chan, trig), TAG, "Connect tx dma channel error");
        gdma_tx_event_callbacks_t cb = {.on_trans_eof = i2s_dma_tx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_tx_event_callbacks(handle->dma.dma_chan, &cb, handle);
    }
    if (handle->dir & I2S_DIR_RX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_RX;
        /* Register a new GDMA rx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &handle->dma.dma_chan), TAG, "Register rx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(handle->dma.dma_chan, trig), TAG, "Connect rx dma channel error");
        gdma_rx_event_callbacks_t cb = {.on_recv_eof = i2s_dma_rx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_rx_event_callbacks(handle->dma.dma_chan, &cb, handle);
    }
#else
    /* Initial I2S module interrupt */
    if (!handle->parent->i2s_isr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_alloc(i2s_periph_signal[port_id].irq, intr_flag, i2s_default_intr_handler, handle->parent, &(handle->parent->i2s_isr_handle)), TAG, "Register I2S Interrupt error");
        esp_intr_enable(handle->parent->i2s_isr_handle);
    }
    /* Start DMA */
    i2s_ll_enable_dma(handle->parent->hal.dev, true);
#endif // SOC_GDMA_SUPPORTED
    return ESP_OK;
}

void i2s_gpio_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool is_input)
{
    /* Ignore the pin if pin = -1 */
    if (gpio != I2S_GPIO_UNUSED) {
        if (is_input) {
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
            /* Set direction, for some GPIOs, the input function are not enabled as default */
            gpio_set_direction(gpio, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio, signal_idx, 0);
        } else {
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
            gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
            esp_rom_gpio_connect_out_signal(gpio, signal_idx, 0, 0);
        }
    }
}

esp_err_t i2s_check_set_mclk(i2s_port_t id, gpio_num_t gpio_num)
{
    if (gpio_num == -1) {
        return ESP_OK;
    }
#if CONFIG_IDF_TARGET_ESP32
    ESP_RETURN_ON_FALSE((gpio_num == GPIO_NUM_0 || gpio_num == GPIO_NUM_1 || gpio_num == GPIO_NUM_3),
                        ESP_ERR_INVALID_ARG, TAG,
                        "ESP32 only support to set GPIO0/GPIO1/GPIO3 as mclk signal, error GPIO number:%d", gpio_num);
    bool is_i2s0 = id == I2S_NUM_0;
    if (gpio_num == GPIO_NUM_0) {
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        WRITE_PERI_REG(PIN_CTRL, is_i2s0 ? 0xFFF0 : 0xFFFF);
    } else if (gpio_num == GPIO_NUM_1) {
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_CLK_OUT3);
        WRITE_PERI_REG(PIN_CTRL, is_i2s0 ? 0xF0F0 : 0xF0FF);
    } else {
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_CLK_OUT2);
        WRITE_PERI_REG(PIN_CTRL, is_i2s0 ? 0xFF00 : 0xFF0F);
    }
#else
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "mck_io_num invalid");
    i2s_gpio_check_and_set(gpio_num, i2s_periph_signal[id].mck_out_sig, false);
#endif
    ESP_LOGI(TAG, "MCLK is pinned to GPIO%d on I2S%d", id, gpio_num);
    return ESP_OK;
}

/*---------------------------------------------------------------------------
                            I2S bus Public APIs
 ----------------------------------------------------------------------------
    Scope: Public
 ----------------------------------------------------------------------------*/
esp_err_t i2s_new_channel(const i2s_chan_config_t *chan_cfg, i2s_chan_handle_t *tx_handle, i2s_chan_handle_t *rx_handle)
{
    /* Parameter validity check */
    I2S_NULL_POINTER_CHECK(TAG, chan_cfg);
    I2S_NULL_POINTER_CHECK(TAG, tx_handle || rx_handle);
    ESP_RETURN_ON_FALSE(chan_cfg->id < SOC_I2S_NUM || chan_cfg->id == I2S_NUM_AUTO, ESP_ERR_INVALID_ARG, TAG, "invalid I2S port id");
    ESP_RETURN_ON_FALSE(chan_cfg->dma_desc_num >= 2, ESP_ERR_INVALID_ARG, TAG, "there should be at least 2 DMA buffers");

    esp_err_t ret = ESP_OK;
    i2s_controller_t *i2s_obj = NULL;
    i2s_port_t id = chan_cfg->id;
    bool channel_found = false;
    uint8_t chan_search_mask = 0;
    chan_search_mask |= tx_handle ? I2S_DIR_TX : 0;
    chan_search_mask |= rx_handle ? I2S_DIR_RX : 0;

    /* Channel will be registered to one i2s port automatically if id is I2S_NUM_AUTO
     * Otherwise, the channel will be registered to the specific port. */
    if (id == I2S_NUM_AUTO) {
        for (int i = 0; i < I2S_NUM_MAX && !channel_found; i++) {
            i2s_obj = i2s_acquire_controller_obj(i);
            if (!i2s_obj) {
                continue;
            }
            channel_found = i2s_take_available_channel(i2s_obj, chan_search_mask);
        }
        ESP_RETURN_ON_FALSE(i2s_obj, ESP_ERR_NOT_FOUND, TAG, "get i2s object failed");
    } else {
        i2s_obj = i2s_acquire_controller_obj(id);
        ESP_RETURN_ON_FALSE(i2s_obj, ESP_ERR_NOT_FOUND, TAG, "get i2s object failed");
        channel_found = i2s_take_available_channel(i2s_obj, chan_search_mask);
    }
    ESP_GOTO_ON_FALSE(channel_found, ESP_ERR_NOT_FOUND, err, TAG, "no available channel found");
    /* Register and specify the tx handle */
    if (tx_handle) {
        ESP_GOTO_ON_ERROR(i2s_register_channel(i2s_obj, I2S_DIR_TX), err, TAG, "register I2S tx channel failed");
        i2s_obj->tx_chan->role = chan_cfg->role;
        i2s_obj->tx_chan->dma.auto_clear = chan_cfg->auto_clear;
        i2s_obj->tx_chan->dma.desc_num = chan_cfg->dma_desc_num;
        i2s_obj->tx_chan->dma.frame_num = chan_cfg->dma_frame_num;
        i2s_obj->tx_chan->start = i2s_tx_start;
        i2s_obj->tx_chan->stop = i2s_tx_stop;
        *tx_handle = i2s_obj->tx_chan;
        ESP_LOGI(TAG, "tx channel is registered on I2S%d successfully", i2s_obj->id);
    }
    /* Register and specify the rx handle */
    if (rx_handle) {
        ESP_GOTO_ON_ERROR(i2s_register_channel(i2s_obj, I2S_DIR_RX), err, TAG, "register I2S rx channel failed");
        i2s_obj->rx_chan->role = chan_cfg->role;
        i2s_obj->rx_chan->dma.desc_num = chan_cfg->dma_desc_num;
        i2s_obj->rx_chan->dma.frame_num = chan_cfg->dma_frame_num;
        i2s_obj->rx_chan->start = i2s_rx_start;
        i2s_obj->rx_chan->stop = i2s_rx_stop;
        *rx_handle = i2s_obj->rx_chan;
        ESP_LOGI(TAG, "rx channel is registered on I2S%d successfully", i2s_obj->id);
    }

    if ((tx_handle != NULL) && (rx_handle != NULL)) {
        i2s_obj->full_duplex = true;
    }

    return ESP_OK;
/* i2s_obj allocated but register channel failed */
err:
    /* if the controller object has no channel, find the corresponding global object and destroy it */
    if (i2s_obj != NULL && i2s_obj->rx_chan == NULL && i2s_obj->tx_chan == NULL) {
        for (int i = 0; i < I2S_NUM_MAX; i++) {
            if (i2s_obj == s_i2s.controller[i]) {
                i2s_destroy_controller_obj(&s_i2s.controller[i]);
                break;
            }
        }
    }
    return ret;
}

// TODO: finish delete channel
esp_err_t i2s_del_channel(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    /* Static mutex to avoid double delete */
    static SemaphoreHandle_t del_mut = NULL;
    if (del_mut == NULL) {
        del_mut = xSemaphoreCreateMutex();
    }
    i2s_controller_t *i2s_obj = handle->parent;
    int id = i2s_obj->id;
    i2s_dir_t dir = handle->dir;
    bool is_bound = true;

    xSemaphoreTake(del_mut, portMAX_DELAY);
    /* Stop the channel first */
    if (handle->state > I2S_CHAN_STATE_READY) {
        i2s_abort_reading_writing(handle);
        i2s_stop_channel(handle);
    }
#if SOC_I2S_HW_VERSION_2
    if (dir == I2S_DIR_TX) {
        i2s_ll_tx_disable_clock(handle->parent->hal.dev);
    } else {
        i2s_ll_rx_disable_clock(handle->parent->hal.dev);
    }
#endif
#if SOC_I2S_SUPPORTS_APLL
    if (handle->apll_en) {
        /* Must switch back to D2CLK on ESP32-S2,
         * because the clock of some registers are bound to APLL,
         * otherwise, once APLL is disabled, the registers can't be updated anymore */
        if (handle->dir == I2S_DIR_TX) {
            i2s_ll_tx_clk_set_src(handle->parent->hal.dev, I2S_CLK_PLL_160M);
        } else {
            i2s_ll_rx_clk_set_src(handle->parent->hal.dev, I2S_CLK_PLL_160M);
        }
        periph_rtc_apll_release();
    }
#endif
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    if (handle->dma.desc) {
        i2s_free_dma_desc(handle);
    }
    if (handle->msg_queue) {
        vQueueDelete(handle->msg_queue);
    }
    if (handle->event_queue) {
        vQueueDelete(handle->event_queue);
    }
    if (handle->mutex) {
        vSemaphoreDelete(handle->mutex);
    }
#if SOC_I2S_HW_VERSION_1
    i2s_obj->chan_occupancy = 0;
#else
    i2s_obj->chan_occupancy &= ~(uint32_t)dir;
#endif
#if SOC_GDMA_SUPPORTED
    if (handle->dma.dma_chan) {
        gdma_del_channel(handle->dma.dma_chan);
    }
#endif
    if (handle == i2s_obj->tx_chan) {
        free(i2s_obj->tx_chan);
        i2s_obj->tx_chan = NULL;
        i2s_obj->full_duplex = false;
    } else if (handle == i2s_obj->rx_chan) {
        free(i2s_obj->rx_chan);
        i2s_obj->rx_chan = NULL;
        i2s_obj->full_duplex = false;
    } else {
        /* Indicate the delete channel is an unbound free channel */
        is_bound = false;
        free(handle);
    }

    /* If the delete channel was bound to a controller before,
       we need to destroy this controller object if there is no channel any more */
    if (is_bound) {
        if (!(i2s_obj->tx_chan) && !(i2s_obj->rx_chan)) {
            i2s_destroy_controller_obj(&s_i2s.controller[i2s_obj->id]);
        }
        ESP_LOGI(TAG, "%s channel on I2S%d deleted", dir == I2S_DIR_TX ? "tx" : "rx", id);
    }
    xSemaphoreGive(del_mut);
    return ESP_OK;
}

QueueHandle_t i2s_get_event_queue(i2s_chan_handle_t handle, uint32_t que_len)
{
    QueueHandle_t que;
    if (!handle) {
        return NULL;
    }
    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    if (handle->event_queue) {
        que = handle->event_queue;
    } else {
        handle->event_queue = xQueueCreate(que_len, sizeof(int *));
        que = handle->event_queue;
    }
    xSemaphoreGive(handle->mutex);
    return que;
}


esp_err_t i2s_start_channel(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "the channel has already started or not initialized");

    handle->dma.curr_ptr = NULL;
    handle->dma.rw_pos = 0;
    handle->start(handle);
    handle->state = I2S_CHAN_STATE_IDLE;
    xSemaphoreGive(handle->mutex);

    ESP_LOGI(TAG, "i2s %s channel started", handle->dir == I2S_DIR_TX ? "tx" : "rx");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_stop_channel(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "the channel has not started yet");
    handle->stop(handle);
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    ESP_LOGI(TAG, "i2s %s channel stopped", handle->dir == I2S_DIR_TX ? "tx" : "rx");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_write_channel(i2s_chan_handle_t handle, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "this channel is not tx channel");

    esp_err_t ret = ESP_OK;
    char *data_ptr, *src_byte;
    size_t bytes_can_write;
    *bytes_written = 0;

    /* Take the semaphore brefore changing state to ensure only one writing thread running at the same time */
    xSemaphoreTake(handle->mutex, ticks_to_wait);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    ESP_GOTO_ON_FALSE(handle->state > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "this channel has not started yet");
    handle->state = I2S_CHAN_STATE_WRITING;
    src_byte = (char *)src;
    while (size > 0 && handle->state == I2S_CHAN_STATE_WRITING) {
        if (handle->dma.rw_pos == handle->dma.buf_size || handle->dma.curr_ptr == NULL) {
            if (xQueueReceive(handle->msg_queue, &(handle->dma.curr_ptr), ticks_to_wait) == pdFALSE) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
            handle->dma.rw_pos = 0;
        }
        ESP_LOGD(TAG, "size: %d, rw_pos: %d, buf_size: %d, curr_ptr: %d", size, handle->dma.rw_pos, handle->dma.buf_size, (int)handle->dma.curr_ptr);
        data_ptr = (char *)handle->dma.curr_ptr;
        data_ptr += handle->dma.rw_pos;
        bytes_can_write = handle->dma.buf_size - handle->dma.rw_pos;
        if (bytes_can_write > size) {
            bytes_can_write = size;
        }
        memcpy(data_ptr, src_byte, bytes_can_write);
        size -= bytes_can_write;
        src_byte += bytes_can_write;
        handle->dma.rw_pos += bytes_can_write;
        (*bytes_written) += bytes_can_write;
    }
    /* Need to judge the state before switch back to idle, in case the state has been changed by 'stop' or 'abort' */
    if (handle->state == I2S_CHAN_STATE_WRITING) {
        handle->state = I2S_CHAN_STATE_IDLE;
    }
err:
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    xSemaphoreGive(handle->mutex);

    return ret;
}

esp_err_t i2s_read_channel(i2s_chan_handle_t handle, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "this channel is not rx channel");

    esp_err_t ret = ESP_OK;
    uint8_t *data_ptr, *dest_byte;
    int bytes_can_read;
    *bytes_read = 0;
    dest_byte = (uint8_t *)dest;
    /* Take the semaphore brefore changing state to ensure only one reading thread running at the same time */
    xSemaphoreTake(handle->mutex, ticks_to_wait);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    ESP_GOTO_ON_FALSE(handle->state > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "this channel has not started yet");
    handle->state = I2S_CHAN_STATE_READING;
    while (size > 0 && handle->state == I2S_CHAN_STATE_READING) {
        if (handle->dma.rw_pos == handle->dma.buf_size || handle->dma.curr_ptr == NULL) {
            if (xQueueReceive(handle->msg_queue, &(handle->dma.curr_ptr), ticks_to_wait) == pdFALSE) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
            handle->dma.rw_pos = 0;
        }
        data_ptr = (uint8_t *)handle->dma.curr_ptr;
        data_ptr += handle->dma.rw_pos;
        bytes_can_read = handle->dma.buf_size - handle->dma.rw_pos;
        if (bytes_can_read > (int)size) {
            bytes_can_read = size;
        }
        memcpy(dest_byte, data_ptr, bytes_can_read);
        size -= bytes_can_read;
        dest_byte += bytes_can_read;
        handle->dma.rw_pos += bytes_can_read;
        (*bytes_read) += bytes_can_read;
    }
    /* Need to judge the state before switch back to idle, in case the state has been changed by 'stop' or 'abort' */
    if (handle->state == I2S_CHAN_STATE_READING) {
        handle->state = I2S_CHAN_STATE_IDLE;
    }
err:
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    xSemaphoreGive(handle->mutex);

    return ret;
}

esp_err_t i2s_clear_dma_buffer(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->state >= I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, TAG, "this channel has not initialized yet");
    /* Clear all the DMA buffer */
    for (int desc_num = handle->dma.desc_num; desc_num > 0; desc_num--) {
        memset(handle->dma.bufs[desc_num-1], 0, handle->dma.buf_size);
    }
    return ESP_OK;
}

esp_err_t i2s_abort_reading_writing(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->state > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, TAG, "this channel has not started yet");
    /* No lock here. Force change state to idle when writing or reading */
    handle->state = I2S_CHAN_STATE_IDLE;

    return ESP_OK;
}


/*---------------------------------------------------------------------------
                            I2S Platform APIs
 ----------------------------------------------------------------------------
    Scope: This file and ADC/DAC/LCD driver
 ----------------------------------------------------------------------------*/

esp_err_t i2s_platform_acquire_occupation(int id, const char *comp_name)
{
    esp_err_t ret = ESP_OK;
    const char *occupied_comp = NULL;
    ESP_RETURN_ON_FALSE(id < I2S_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid i2s port id");
    portENTER_CRITICAL(&s_i2s.spinlock);
    if ((!s_i2s.controller[id]) && (s_i2s.comp_name[id] == NULL)) {
        s_i2s.comp_name[id] = comp_name;
        /* Enable module clock */
        periph_module_enable(i2s_periph_signal[id].module);
        i2s_ll_enable_clock(I2S_LL_GET_HW(id));
    } else {
        occupied_comp =  s_i2s.comp_name[id];
        ret = ESP_ERR_NOT_FOUND;
    }
    portEXIT_CRITICAL(&s_i2s.spinlock);
    if (occupied_comp != NULL) {
        ESP_LOGE(TAG, "i2s controller %d has been occupied by %s", id, occupied_comp);
    }
    return ret;
}

esp_err_t i2s_platform_release_occupation(int id)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(id < I2S_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid i2s port id");
    portENTER_CRITICAL(&s_i2s.spinlock);
    if ((!s_i2s.controller[id])) {
        s_i2s.comp_name[id] = NULL;
        /* Disable module clock */
        periph_module_disable(i2s_periph_signal[id].module);
        i2s_ll_disable_clock(I2S_LL_GET_HW(id));
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    portEXIT_CRITICAL(&s_i2s.spinlock);
    return ret;
}
