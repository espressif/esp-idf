/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <esp_types.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "sdkconfig.h"

#if CONFIG_I2S_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif

#include "soc/lldesc.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "driver/i2s_types_legacy.h"
#include "hal/i2s_hal.h"
#if SOC_I2S_SUPPORTS_APLL
#include "hal/clk_tree_ll.h"
#endif

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#endif

#if SOC_I2S_SUPPORTS_DAC
#include "hal/dac_ll.h"
#include "hal/dac_types.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "adc1_private.h"
#include "driver/adc_i2s_legacy.h"
#include "driver/adc_types_legacy.h"
#endif // SOC_I2S_SUPPORTS_ADC

#if CONFIG_IDF_TARGET_ESP32
#include "esp_clock_output.h"
#endif

#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif

#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_efuse.h"
#include "esp_rom_gpio.h"
#include "esp_dma_utils.h"

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif

#include "esp_private/i2s_platform.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"

static const char *TAG = "i2s(legacy)";

#define I2S_ENTER_CRITICAL_ISR(i2s_num)          portENTER_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL_ISR(i2s_num)           portEXIT_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_ENTER_CRITICAL(i2s_num)              portENTER_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL(i2s_num)               portEXIT_CRITICAL(&i2s_spinlock[i2s_num])

#if SOC_PERIPH_CLK_CTRL_SHARED
#define I2S_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2S_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define I2S_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2S_RCC_ATOMIC()
#endif

#define I2S_DMA_BUFFER_MAX_SIZE     4092

#if SOC_I2S_SUPPORTS_ADC_DAC
#define    I2S_COMM_MODE_ADC_DAC    -1
#endif

/**
 * @brief General clock configuration information
 * @note It is a general purpose struct, not supposed to be used directly by user
 */
typedef struct {
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate */
#if SOC_I2S_SUPPORTS_PDM_TX
    uint32_t                up_sample_fp;       /*!< Up-sampling param fp */
    uint32_t                up_sample_fs;       /*!< Up-sampling param fs */
#endif
#if SOC_I2S_SUPPORTS_PDM_RX
    i2s_pdm_dsr_t           dn_sample_mode;     /*!< Down-sampling rate mode */
#endif
} i2s_clk_config_t;

/**
 * @brief DMA buffer object
 *
 */
typedef struct {
    char **buf;
    int buf_size;
    volatile int rw_pos;
    volatile void *curr_ptr;
    SemaphoreHandle_t mux;
    QueueHandle_t queue;
    lldesc_t **desc;
} i2s_dma_t;

/**
 * @brief I2S object instance
 *
 */
typedef struct {
    i2s_port_t i2s_num;         /*!< I2S port number*/
    int queue_size;             /*!< I2S event queue size*/
    QueueHandle_t i2s_queue;    /*!< I2S queue handler*/
    uint32_t last_buf_size;     /*!< DMA last buffer size */
    i2s_dma_t *tx;              /*!< DMA Tx buffer*/
    i2s_dma_t *rx;              /*!< DMA Rx buffer*/
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t rx_dma_chan;  /*!< I2S rx gDMA channel handle*/
    gdma_channel_handle_t tx_dma_chan;  /*!< I2S tx gDMA channel handle*/
#else
    intr_handle_t i2s_isr_handle; /*!< I2S Interrupt handle*/
#endif
    uint32_t dma_desc_num;
    uint32_t dma_frame_num;
    bool tx_desc_auto_clear;    /*!< I2S auto clear tx descriptor on underflow */
    bool use_apll;              /*!< I2S use APLL clock */
    int fixed_mclk;             /*!< I2S fixed MLCK clock */
    i2s_mclk_multiple_t mclk_multiple; /*!< The multiple of I2S master clock(MCLK) to sample rate */
#if CONFIG_IDF_TARGET_ESP32
    esp_clock_output_mapping_handle_t mclk_out_hdl;
#endif
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    i2s_hal_context_t hal;       /*!< I2S hal context*/

    /* New config */
    i2s_dir_t         dir;
    i2s_role_t        role;
    i2s_comm_mode_t   mode;
    i2s_hal_slot_config_t slot_cfg;
    i2s_clk_config_t      clk_cfg;
    uint32_t          active_slot;    /*!< Active slot number */
    uint32_t          total_slot;     /*!< Total slot number */
} i2s_obj_t;

// Global I2S object pointer
static i2s_obj_t *p_i2s[SOC_I2S_NUM] = {
    [0 ... SOC_I2S_NUM - 1] = NULL,
};

// Global spin lock for all i2s controllers
static portMUX_TYPE i2s_spinlock[SOC_I2S_NUM] = {
    [0 ... SOC_I2S_NUM - 1] = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

/*-------------------------------------------------------------
                    I2S DMA operation
  -------------------------------------------------------------*/
#if SOC_GDMA_SUPPORTED
static bool IRAM_ATTR i2s_dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) user_data;
    BaseType_t need_awoke = 0;
    BaseType_t tmp = 0;
    int dummy;
    i2s_event_t i2s_event;
    lldesc_t *finish_desc;

    if (p_i2s->rx) {
        finish_desc = (lldesc_t *)event_data->rx_eof_desc_addr;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync((void *)finish_desc->buf, p_i2s->rx->buf_size, ESP_CACHE_MSYNC_FLAG_INVALIDATE);
#endif
        i2s_event.size = finish_desc->size;
        if (xQueueIsQueueFullFromISR(p_i2s->rx->queue)) {
            xQueueReceiveFromISR(p_i2s->rx->queue, &dummy, &tmp);
            need_awoke |= tmp;
            if (p_i2s->i2s_queue) {
                i2s_event.type = I2S_EVENT_RX_Q_OVF;
                xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
                need_awoke |= tmp;
            }
        }
        xQueueSendFromISR(p_i2s->rx->queue, &finish_desc->buf, &tmp);
        need_awoke |= tmp;
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_RX_DONE;
            xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
            need_awoke |= tmp;
        }
    }
    return need_awoke;
}

static bool IRAM_ATTR i2s_dma_tx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) user_data;
    BaseType_t need_awoke = 0;
    BaseType_t tmp = 0;
    int dummy;
    i2s_event_t i2s_event;
    lldesc_t *finish_desc;
    if (p_i2s->tx) {
        finish_desc = (lldesc_t *)event_data->tx_eof_desc_addr;
        i2s_event.size = finish_desc->size;
        if (xQueueIsQueueFullFromISR(p_i2s->tx->queue)) {
            xQueueReceiveFromISR(p_i2s->tx->queue, &dummy, &tmp);
            need_awoke |= tmp;
            if (p_i2s->i2s_queue) {
                i2s_event.type = I2S_EVENT_TX_Q_OVF;
                i2s_event.size = p_i2s->tx->buf_size;
                xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
                need_awoke |= tmp;
            }
        }
        if (p_i2s->tx_desc_auto_clear) {
            uint8_t *sent_buf = (uint8_t *)finish_desc->buf;
            memset(sent_buf, 0, p_i2s->tx->buf_size);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            esp_cache_msync(sent_buf, p_i2s->tx->buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        }
        xQueueSendFromISR(p_i2s->tx->queue, &finish_desc->buf, &tmp);
        need_awoke |= tmp;
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_TX_DONE;
            xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
            need_awoke |= tmp;
        }
    }
    return need_awoke;
}

#else
static void IRAM_ATTR i2s_intr_handler_default(void *arg)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) arg;
    uint32_t status = i2s_hal_get_intr_status(&(p_i2s->hal));
    if (status == 0) {
        //Avoid spurious interrupt
        return;
    }

    i2s_event_t i2s_event;
    int dummy;
    BaseType_t need_awoke = 0;
    BaseType_t tmp = 0;
    uint32_t  finish_desc = 0;
    if ((status & I2S_LL_EVENT_TX_DSCR_ERR) || (status & I2S_LL_EVENT_RX_DSCR_ERR)) {
        ESP_EARLY_LOGE(TAG, "dma error, interrupt status: 0x%08" PRIx32, status);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_DMA_ERROR;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &tmp);
                need_awoke |= tmp;
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
            need_awoke |= tmp;
        }
    }

    if ((status & I2S_LL_EVENT_TX_EOF) && p_i2s->tx) {
        i2s_hal_get_out_eof_des_addr(&(p_i2s->hal), &finish_desc);
        i2s_event.size = ((lldesc_t *)finish_desc)->size;
        // All buffers are empty. This means we have an underflow on our hands.
        if (xQueueIsQueueFullFromISR(p_i2s->tx->queue)) {
            xQueueReceiveFromISR(p_i2s->tx->queue, &dummy, &tmp);
            need_awoke |= tmp;
            if (p_i2s->i2s_queue) {
                i2s_event.type = I2S_EVENT_TX_Q_OVF;
                xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
                need_awoke |= tmp;
            }
        }
        // See if tx descriptor needs to be auto cleared:
        // This will avoid any kind of noise that may get introduced due to transmission
        // of previous data from tx descriptor on I2S line.
        if (p_i2s->tx_desc_auto_clear == true) {
            memset((void *)(((lldesc_t *)finish_desc)->buf), 0, p_i2s->tx->buf_size);
        }
        xQueueSendFromISR(p_i2s->tx->queue, &(((lldesc_t *)finish_desc)->buf), &tmp);
        need_awoke |= tmp;
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_TX_DONE;
            xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
            need_awoke |= tmp;
        }
    }

    if ((status & I2S_LL_EVENT_RX_EOF) && p_i2s->rx) {
        // All buffers are full. This means we have an overflow.
        i2s_hal_get_in_eof_des_addr(&(p_i2s->hal), &finish_desc);
        i2s_event.size = ((lldesc_t *)finish_desc)->size;
        if (xQueueIsQueueFullFromISR(p_i2s->rx->queue)) {
            xQueueReceiveFromISR(p_i2s->rx->queue, &dummy, &tmp);
            need_awoke |= tmp;
            if (p_i2s->i2s_queue) {
                i2s_event.type = I2S_EVENT_RX_Q_OVF;
                xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
                need_awoke |= tmp;
            }
        }
        xQueueSendFromISR(p_i2s->rx->queue, &(((lldesc_t *)finish_desc)->buf), &tmp);
        need_awoke |= tmp;
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_RX_DONE;
            xQueueSendFromISR(p_i2s->i2s_queue, (void *)&i2s_event, &tmp);
            need_awoke |= tmp;
        }
    }
    i2s_hal_clear_intr_status(&(p_i2s->hal), status);

    if (need_awoke == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
#endif

static esp_err_t i2s_dma_intr_init(i2s_port_t i2s_num, int intr_flag)
{
#if SOC_GDMA_SUPPORTED
    /* Set GDMA trigger module */
    gdma_trigger_t trig = {.periph = GDMA_TRIG_PERIPH_I2S};

    switch (i2s_num) {
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
    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_TX;
        /* Register a new GDMA tx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &p_i2s[i2s_num]->tx_dma_chan), TAG, "Register tx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(p_i2s[i2s_num]->tx_dma_chan, trig), TAG, "Connect tx dma channel error");
        gdma_tx_event_callbacks_t cb = {.on_trans_eof = i2s_dma_tx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_tx_event_callbacks(p_i2s[i2s_num]->tx_dma_chan, &cb, p_i2s[i2s_num]);
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_RX;
        /* Register a new GDMA rx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &p_i2s[i2s_num]->rx_dma_chan), TAG, "Register rx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(p_i2s[i2s_num]->rx_dma_chan, trig), TAG, "Connect rx dma channel error");
        gdma_rx_event_callbacks_t cb = {.on_recv_eof = i2s_dma_rx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_rx_event_callbacks(p_i2s[i2s_num]->rx_dma_chan, &cb, p_i2s[i2s_num]);
    }
#else
    /* Initial I2S module interrupt */
    ESP_RETURN_ON_ERROR(esp_intr_alloc(i2s_periph_signal[i2s_num].irq, intr_flag, i2s_intr_handler_default, p_i2s[i2s_num], &p_i2s[i2s_num]->i2s_isr_handle), TAG, "Register I2S Interrupt error");
#endif // SOC_GDMA_SUPPORTED
    return ESP_OK;
}

static void i2s_tx_reset(i2s_port_t i2s_num)
{
    p_i2s[i2s_num]->tx->curr_ptr = NULL;
    p_i2s[i2s_num]->tx->rw_pos = 0;
    i2s_hal_tx_reset(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->tx_dma_chan);
#else
    i2s_hal_tx_reset_dma(&(p_i2s[i2s_num]->hal));
#endif
    i2s_hal_tx_reset_fifo(&(p_i2s[i2s_num]->hal));
}

/**
 * @brief   I2S rx reset
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_rx_reset(i2s_port_t i2s_num)
{
    p_i2s[i2s_num]->rx->curr_ptr = NULL;
    p_i2s[i2s_num]->rx->rw_pos = 0;
    i2s_hal_rx_reset(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->rx_dma_chan);
#else
    i2s_hal_rx_reset_dma(&(p_i2s[i2s_num]->hal));
#endif
    i2s_hal_rx_reset_fifo(&(p_i2s[i2s_num]->hal));
}

static void i2s_tx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->tx_dma_chan, (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#else
    i2s_hal_tx_enable_dma(&(p_i2s[i2s_num]->hal));
    i2s_hal_tx_enable_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_tx_start_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#endif
    i2s_hal_tx_start(&(p_i2s[i2s_num]->hal));
}

static void i2s_rx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->rx_dma_chan, (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#else
    i2s_hal_rx_enable_dma(&(p_i2s[i2s_num]->hal));
    i2s_hal_rx_enable_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_rx_start_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#endif
    i2s_hal_rx_start(&(p_i2s[i2s_num]->hal));
}

static void i2s_tx_stop(i2s_port_t i2s_num)
{
    i2s_hal_tx_stop(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->tx_dma_chan);
#else
    i2s_hal_tx_stop_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_tx_disable_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_tx_disable_dma(&(p_i2s[i2s_num]->hal));
#endif
}

static void i2s_rx_stop(i2s_port_t i2s_num)
{
    i2s_hal_rx_stop(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->rx_dma_chan);
#else
    i2s_hal_rx_stop_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_rx_disable_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_rx_disable_dma(&(p_i2s[i2s_num]->hal));
#endif
}

esp_err_t i2s_start(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    //start DMA link
    I2S_ENTER_CRITICAL(i2s_num);

    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        i2s_tx_reset(i2s_num);
        i2s_tx_start(i2s_num);
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        i2s_rx_reset(i2s_num);
        i2s_rx_start(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    esp_intr_enable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    I2S_EXIT_CRITICAL(i2s_num);
    return ESP_OK;
}

esp_err_t i2s_stop(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    I2S_ENTER_CRITICAL(i2s_num);
#if !SOC_GDMA_SUPPORTED
    esp_intr_disable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        i2s_tx_stop(i2s_num);
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        i2s_rx_stop(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    i2s_hal_clear_intr_status(&(p_i2s[i2s_num]->hal), I2S_INTR_MAX);
#endif
    I2S_EXIT_CRITICAL(i2s_num);
    return ESP_OK;
}

/*-------------------------------------------------------------
                   I2S buffer operation
  -------------------------------------------------------------*/
static inline uint32_t i2s_get_buf_size(i2s_port_t i2s_num)
{
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[i2s_num]->slot_cfg;
    /* Calculate bytes per sample, align to 16 bit */
    uint32_t bytes_per_sample = ((slot_cfg->data_bit_width + 15) / 16) * 2;
    /* The DMA buffer limitation is 4092 bytes */
    uint32_t bytes_per_frame = bytes_per_sample * p_i2s[i2s_num]->active_slot;
    p_i2s[i2s_num]->dma_frame_num = (p_i2s[i2s_num]->dma_frame_num * bytes_per_frame > I2S_DMA_BUFFER_MAX_SIZE) ?
                                    I2S_DMA_BUFFER_MAX_SIZE / bytes_per_frame : p_i2s[i2s_num]->dma_frame_num;
    uint32_t bufsize = p_i2s[i2s_num]->dma_frame_num * bytes_per_frame;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* bufsize need to align with cache line size */
    uint32_t alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    uint32_t aligned_frame_num = p_i2s[i2s_num]->dma_frame_num;
    /* To make the buffer aligned with the cache line size, search for the ceil aligned size first,
       If the buffer size exceed the max DMA buffer size, toggle the sign to search for the floor aligned size */
    for (int sign = 1; bufsize % alignment != 0; aligned_frame_num += sign) {
        bufsize = aligned_frame_num * bytes_per_frame;
        /* If the buffer size exceed the max dma size */
        if (bufsize > I2S_DMA_BUFFER_MAX_SIZE) {
            sign = -1; // toggle the search sign
            aligned_frame_num = p_i2s[i2s_num]->dma_frame_num;              // Reset the frame num
            bufsize = aligned_frame_num * bytes_per_frame;  // Reset the bufsize
        }
    }
    if (bufsize / bytes_per_frame != p_i2s[i2s_num]->dma_frame_num) {
        ESP_LOGW(TAG, "dma frame num is adjusted to %"PRIu32" to align the dma buffer with %"PRIu32
                 ", bufsize = %"PRIu32, bufsize / bytes_per_frame, alignment, bufsize);
    }
#else
    /* Limit DMA buffer size if it is out of range (DMA buffer limitation is 4092 bytes) */
    if (bufsize > I2S_DMA_BUFFER_MAX_SIZE) {
        uint32_t frame_num = I2S_DMA_BUFFER_MAX_SIZE / bytes_per_frame;
        bufsize = frame_num * bytes_per_frame;
        ESP_LOGW(TAG, "dma frame num is out of dma buffer size, limited to %"PRIu32, frame_num);
    }
#endif
    return bufsize;
}

static esp_err_t i2s_delete_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    ESP_RETURN_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, TAG, "I2S DMA object can't be NULL");
    uint32_t buf_cnt = p_i2s[i2s_num]->dma_desc_num;
    /* Loop to destroy every descriptor and buffer */
    for (int cnt = 0; cnt < buf_cnt; cnt++) {
        if (dma_obj->desc && dma_obj->desc[cnt]) {
            free(dma_obj->desc[cnt]);
            dma_obj->desc[cnt] = NULL;
        }
        if (dma_obj->buf && dma_obj->buf[cnt]) {
            free(dma_obj->buf[cnt]);
            dma_obj->buf[cnt] = NULL;
        }
    }
    return ESP_OK;
}

static esp_err_t i2s_alloc_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, err, TAG, "I2S DMA object can't be NULL");

    uint32_t buf_cnt = p_i2s[i2s_num]->dma_desc_num;
    size_t desc_size = 0;
    for (int cnt = 0; cnt < buf_cnt; cnt++) {
        /* Allocate DMA buffer */
        esp_dma_mem_info_t dma_mem_info = {
            .extra_heap_caps = MALLOC_CAP_INTERNAL,
            .dma_alignment_bytes = 4,
        };
        //TODO: IDF-9636
        esp_dma_capable_calloc(1, sizeof(char) * dma_obj->buf_size, &dma_mem_info, (void **)&dma_obj->buf[cnt], NULL);
        ESP_GOTO_ON_FALSE(dma_obj->buf[cnt], ESP_ERR_NO_MEM, err, TAG, "Error malloc dma buffer");
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(dma_obj->buf[cnt], dma_obj->buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif

        /* Allocate DMA descriptor */
        esp_dma_capable_calloc(1, sizeof(lldesc_t), &dma_mem_info, (void **)&dma_obj->desc[cnt], &desc_size);
        ESP_GOTO_ON_FALSE(dma_obj->desc[cnt], ESP_ERR_NO_MEM, err, TAG,  "Error malloc dma description entry");
    }
    /* DMA descriptor must be initialize after all descriptor has been created, otherwise they can't be linked together as a chain */
    for (int cnt = 0; cnt < buf_cnt; cnt++) {
        /* Initialize DMA descriptor */
        dma_obj->desc[cnt]->owner = 1;
        dma_obj->desc[cnt]->eof = 1;
        dma_obj->desc[cnt]->sosf = 0;
        dma_obj->desc[cnt]->length = dma_obj->buf_size;
        dma_obj->desc[cnt]->size = dma_obj->buf_size;
        dma_obj->desc[cnt]->buf = (uint8_t *) dma_obj->buf[cnt];
        dma_obj->desc[cnt]->offset = 0;
        /* Link to the next descriptor */
        dma_obj->desc[cnt]->empty = (uint32_t)((cnt < (buf_cnt - 1)) ? (dma_obj->desc[cnt + 1]) : dma_obj->desc[0]);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(dma_obj->desc[cnt], desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        i2s_ll_rx_set_eof_num(p_i2s[i2s_num]->hal.dev, dma_obj->buf_size);
    }
    ESP_LOGD(TAG, "DMA Malloc info, datalen=blocksize=%d, dma_desc_num=%"PRIu32, dma_obj->buf_size, buf_cnt);
    return ESP_OK;
err:
    /* Delete DMA buffer if failed to allocate memory */
    i2s_delete_dma_buffer(i2s_num, dma_obj);
    return ret;
}

static esp_err_t i2s_realloc_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    ESP_RETURN_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, TAG, "I2S DMA object can't be NULL");

    /* Destroy old dma descriptor and buffer */
    i2s_delete_dma_buffer(i2s_num, dma_obj);
    /* Alloc new dma descriptor and buffer */
    ESP_RETURN_ON_ERROR(i2s_alloc_dma_buffer(i2s_num, dma_obj), TAG, "Failed to allocate dma buffer");

    return ESP_OK;
}

static esp_err_t i2s_destroy_dma_object(i2s_port_t i2s_num, i2s_dma_t **dma)
{
    /* Check if DMA truly need destroy */
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_ERR_INVALID_ARG, TAG, "I2S not initialized yet");
    if (!(*dma)) {
        return ESP_OK;
    }
    /* Destroy every descriptor and buffer */
    i2s_delete_dma_buffer(i2s_num, (*dma));
    /* Destroy descriptor pointer */
    if ((*dma)->desc) {
        free((*dma)->desc);
        (*dma)->desc = NULL;
    }
    /* Destroy buffer pointer */
    if ((*dma)->buf) {
        free((*dma)->buf);
        (*dma)->buf = NULL;
    }
    /* Delete DMA mux */
    vSemaphoreDelete((*dma)->mux);
    /* Delete DMA queue */
    vQueueDelete((*dma)->queue);
    /* Free DMA structure */
    free(*dma);
    *dma = NULL;
    ESP_LOGD(TAG, "DMA queue destroyed");
    return ESP_OK;
}

static esp_err_t i2s_create_dma_object(i2s_port_t i2s_num, i2s_dma_t **dma)
{
    ESP_RETURN_ON_FALSE(dma, ESP_ERR_INVALID_ARG, TAG, "DMA object secondary pointer is NULL");
    ESP_RETURN_ON_FALSE((*dma == NULL), ESP_ERR_INVALID_ARG, TAG, "DMA object has been created");
    uint32_t buf_cnt = p_i2s[i2s_num]->dma_desc_num;
    /* Allocate new DMA structure */
    *dma = (i2s_dma_t *) calloc(1, sizeof(i2s_dma_t));
    ESP_RETURN_ON_FALSE(*dma, ESP_ERR_NO_MEM, TAG, "DMA object allocate failed");
    /* Allocate DMA buffer pointer */
    (*dma)->buf = (char **)heap_caps_calloc(buf_cnt, sizeof(char *), MALLOC_CAP_DMA);
    if (!(*dma)->buf) {
        goto err;
    }
    /* Allocate secondary pointer of DMA descriptor chain */
    (*dma)->desc = (lldesc_t **)heap_caps_calloc(buf_cnt, sizeof(lldesc_t *), MALLOC_CAP_DMA);
    if (!(*dma)->desc) {
        goto err;
    }
    /* Create queue and mutex */
    (*dma)->queue = xQueueCreate(buf_cnt - 1, sizeof(char *));
    if (!(*dma)->queue) {
        goto err;
    }
    (*dma)->mux = xSemaphoreCreateMutex();
    if (!(*dma)->mux) {
        goto err;
    }

    return ESP_OK;
err:
    ESP_LOGE(TAG, "I2S DMA object create failed, preparing to uninstall");
    /* Destroy DMA queue if failed to allocate memory */
    i2s_destroy_dma_object(i2s_num, dma);
    return ESP_ERR_NO_MEM;
}

/*-------------------------------------------------------------
                   I2S clock operation
  -------------------------------------------------------------*/
// [clk_tree] TODO: replace the following switch table by clk_tree API
static uint32_t i2s_config_source_clock(i2s_port_t i2s_num, bool use_apll, uint32_t mclk)
{
#if SOC_I2S_SUPPORTS_APLL
    if (use_apll) {
        /* Calculate the expected APLL  */
        int div = (int)((CLK_LL_APLL_MIN_HZ / mclk) + 1);
        /* apll_freq = mclk * div
         * when div = 1, hardware will still divide 2
         * when div = 0, the final mclk will be unpredictable
         * So the div here should be at least 2 */
        div = div < 2 ? 2 : div;
        uint32_t expt_freq = mclk * div;
        /* Set APLL coefficients to the given frequency */
        uint32_t real_freq = 0;
        esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
        if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGE(TAG, "set APLL coefficients failed");
            return 0;
        }
        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "APLL is occupied already, it is working at %"PRIu32" Hz", real_freq);
        }
        ESP_LOGD(TAG, "APLL expected frequency is %"PRIu32" Hz, real frequency is %"PRIu32" Hz", expt_freq, real_freq);
        /* In APLL mode, there is no sclk but only mclk, so return 0 here to indicate APLL mode */
        return real_freq;
    }
    return I2S_LL_DEFAULT_CLK_FREQ;
#else
    if (use_apll) {
        ESP_LOGW(TAG, "APLL not supported on current chip, use I2S_CLK_SRC_DEFAULT as default clock source");
    }
    return I2S_LL_DEFAULT_CLK_FREQ;
#endif
}

#if SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC
static esp_err_t i2s_calculate_adc_dac_clock(int i2s_num, i2s_hal_clock_info_t *clk_info)
{
    /* For ADC/DAC mode, the built-in ADC/DAC is driven by 'mclk' instead of 'bclk'
     * 'bclk' should be fixed to the double of sample rate
     * 'bclk_div' is the real coefficient that affects the slot bit */
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[i2s_num]->slot_cfg;
    uint32_t slot_bits = slot_cfg->slot_bit_width;
    /* Set I2S bit clock */
    clk_info->bclk = clk_cfg->sample_rate_hz * I2S_LL_AD_BCK_FACTOR;
    /* Set I2S bit clock default division */
    clk_info->bclk_div = slot_bits;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = bclk * bclk_div */
    clk_info->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                     p_i2s[i2s_num]->fixed_mclk : clk_info->bclk * clk_info->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
    /* Get I2S system clock by config source clock */
    clk_info->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_info->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->sclk / (float)clk_info->mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(clk_info->mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "sample rate is too small, the mclk division exceed the maximum value 255");

    return ESP_OK;
}
#endif // SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC

#if SOC_I2S_SUPPORTS_PDM_TX
static esp_err_t i2s_calculate_pdm_tx_clock(int i2s_num, i2s_hal_clock_info_t *clk_info)
{
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;

    int fp = clk_cfg->up_sample_fp;
    int fs = clk_cfg->up_sample_fs;
    /* Set I2S bit clock */
    clk_info->bclk = clk_cfg->sample_rate_hz * I2S_LL_PDM_BCK_FACTOR * fp / fs;
    /* Set I2S bit clock default division */
    clk_info->bclk_div = 8;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate_hz * multiple */
    clk_info->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                     p_i2s[i2s_num]->fixed_mclk : clk_info->bclk * clk_info->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
    /* Get I2S system clock by config source clock */
    clk_info->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_info->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->sclk / (float)clk_info->mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(clk_info->mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "sample rate is too small, the mclk division exceed the maximum value 255");

    return ESP_OK;
}
#endif  // SOC_I2S_SUPPORTS_PDM_TX

#if SOC_I2S_SUPPORTS_PDM_RX
static esp_err_t i2s_calculate_pdm_rx_clock(int i2s_num, i2s_hal_clock_info_t *clk_info)
{
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    i2s_pdm_dsr_t dsr = clk_cfg->dn_sample_mode;
    /* Set I2S bit clock */
    clk_info->bclk = clk_cfg->sample_rate_hz * I2S_LL_PDM_BCK_FACTOR * (dsr == I2S_PDM_DSR_16S ? 2 : 1);
    /* Set I2S bit clock default division */
    clk_info->bclk_div = 8;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate_hz * multiple */
    clk_info->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                     p_i2s[i2s_num]->fixed_mclk : clk_info->bclk * clk_info->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
    /* Get I2S system clock by config source clock */
    clk_info->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_info->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->sclk / (float)clk_info->mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(clk_info->mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "sample rate is too small, the mclk division exceed the maximum value 255");

    return ESP_OK;
}
#endif // SOC_I2S_SUPPORTS_PDM_RX
static esp_err_t i2s_calculate_common_clock(int i2s_num, i2s_hal_clock_info_t *clk_info)
{
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[i2s_num]->slot_cfg;
    uint32_t rate     = clk_cfg->sample_rate_hz;
    uint32_t slot_num = p_i2s[i2s_num]->total_slot < 2 ? 2 : p_i2s[i2s_num]->total_slot;
    uint32_t slot_bits = slot_cfg->slot_bit_width;
    /* Calculate multiple */
    if (p_i2s[i2s_num]->role == I2S_ROLE_MASTER) {
        clk_info->bclk = rate * slot_num * slot_bits;
        clk_info->mclk = rate * clk_cfg->mclk_multiple;
        clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
    } else {
        /* For slave mode, mclk >= bclk * 8, so fix bclk_div to 8 first */
        clk_info->bclk_div = 8;
        clk_info->bclk = rate * slot_num * slot_bits;
        clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
    }
    /* Get I2S system clock by config source clock */
    clk_info->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_info->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk <= clk_info->sclk, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");

    return ESP_OK;
}

static esp_err_t i2s_calculate_clock(i2s_port_t i2s_num, i2s_hal_clock_info_t *clk_info)
{
    /* Calculate clock for ADC/DAC mode */
#if SOC_I2S_SUPPORTS_ADC_DAC
    if ((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) {
        ESP_RETURN_ON_ERROR(i2s_calculate_adc_dac_clock(i2s_num, clk_info), TAG, "ADC/DAC clock calculate failed");
        return ESP_OK;
    }
#endif // SOC_I2S_SUPPORTS_ADC

    /* Calculate clock for PDM mode */
#if SOC_I2S_SUPPORTS_PDM
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) {
#if SOC_I2S_SUPPORTS_PDM_TX
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            ESP_RETURN_ON_ERROR(i2s_calculate_pdm_tx_clock(i2s_num, clk_info), TAG, "PDM TX clock calculate failed");
        }
#endif // SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_SUPPORTS_PDM_RX
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            ESP_RETURN_ON_ERROR(i2s_calculate_pdm_rx_clock(i2s_num, clk_info), TAG, "PDM RX clock calculate failed");
        }
#endif // SOC_I2S_SUPPORTS_PDM_RX
        return ESP_OK;
    }
#endif // SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX

    /* Calculate clock for common mode */
    ESP_RETURN_ON_ERROR(i2s_calculate_common_clock(i2s_num, clk_info), TAG, "Common clock calculate failed");
    ESP_LOGD(TAG, "[sclk] %"PRIu32" [mclk] %"PRIu32" [mclk_div] %d [bclk] %"PRIu32" [bclk_div] %d",
             clk_info->sclk, clk_info->mclk, clk_info->mclk_div, clk_info->bclk, clk_info->bclk_div);
    return ESP_OK;
}

/*-------------------------------------------------------------
                   I2S configuration
  -------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_ADC_DAC
static void i2s_dac_set_slot_legacy(void)
{
    i2s_dev_t *dev = p_i2s[0]->hal.dev;
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[0]->slot_cfg;

    i2s_ll_tx_reset(dev);
    i2s_ll_tx_set_slave_mod(dev, false);
    i2s_ll_tx_set_sample_bit(dev, slot_cfg->slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_tx_enable_msb_shift(dev, false);
    i2s_ll_tx_set_ws_width(dev, slot_cfg->slot_bit_width);
    i2s_ll_tx_enable_msb_right(dev, false);
    i2s_ll_tx_enable_right_first(dev, true);
    /* Should always enable fifo */
    i2s_ll_tx_force_enable_fifo_mod(dev, true);
}

esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode)
{
    ESP_RETURN_ON_FALSE((dac_mode < I2S_DAC_CHANNEL_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s dac mode error");
    if (dac_mode == I2S_DAC_CHANNEL_DISABLE) {
        dac_ll_power_down(DAC_CHAN_0);
        dac_ll_power_down(DAC_CHAN_1);
        dac_ll_digi_enable_dma(false);
    } else {
        dac_ll_digi_enable_dma(true);
    }

    if (dac_mode & I2S_DAC_CHANNEL_RIGHT_EN) {
        //DAC1, right channel
        dac_ll_power_on(DAC_CHAN_0);
        dac_ll_rtc_sync_by_adc(false);
    }
    if (dac_mode & I2S_DAC_CHANNEL_LEFT_EN) {
        //DAC2, left channel
        dac_ll_power_on(DAC_CHAN_1);
        dac_ll_rtc_sync_by_adc(false);
    }
    return ESP_OK;
}

static void i2s_adc_set_slot_legacy(void)
{
    i2s_dev_t *dev = p_i2s[0]->hal.dev;
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[0]->slot_cfg;
    // When ADC/DAC are installed as duplex mode, ADC will share the WS and BCLK clock by working in slave mode
    i2s_ll_rx_set_slave_mod(dev, false);
    i2s_ll_rx_set_sample_bit(dev, slot_cfg->slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(dev, true); // ADC should use mono mode to meet the sample rate
    i2s_ll_rx_enable_msb_shift(dev, false);
    i2s_ll_rx_set_ws_width(dev, slot_cfg->slot_bit_width);
    i2s_ll_rx_enable_msb_right(dev, false);
    i2s_ll_rx_enable_right_first(dev, false);
    i2s_ll_rx_select_std_slot(dev, I2S_STD_SLOT_LEFT, false);
    /* Should always enable fifo */
    i2s_ll_rx_force_enable_fifo_mod(dev, true);
}

static int _i2s_adc_unit = -1;
static int _i2s_adc_channel = -1;

static esp_err_t _i2s_adc_mode_recover(void)
{
    ESP_RETURN_ON_FALSE(((_i2s_adc_unit != -1) && (_i2s_adc_channel != -1)), ESP_ERR_INVALID_ARG, TAG, "i2s ADC recover error, not initialized...");
    return adc_i2s_mode_init(_i2s_adc_unit, _i2s_adc_channel);
}

esp_err_t i2s_set_adc_mode(adc_unit_t adc_unit, adc1_channel_t adc_channel)
{
    ESP_RETURN_ON_FALSE((adc_unit < ADC_UNIT_2), ESP_ERR_INVALID_ARG, TAG, "i2s ADC unit error, only support ADC1 for now");
    // For now, we only support SAR ADC1.
    _i2s_adc_unit = adc_unit;
    _i2s_adc_channel = adc_channel;
    return adc_i2s_mode_init(adc_unit, adc_channel);
}

esp_err_t i2s_adc_enable(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE(((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) && (p_i2s[i2s_num]->dir & I2S_DIR_RX),
                        ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    adc1_dma_mode_acquire();
    _i2s_adc_mode_recover();
    i2s_rx_reset(i2s_num);
    return i2s_start(i2s_num);
}

esp_err_t i2s_adc_disable(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE(((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) && (p_i2s[i2s_num]->dir & I2S_DIR_RX),
                        ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    i2s_hal_rx_stop(&(p_i2s[i2s_num]->hal));
    adc1_lock_release();
    return ESP_OK;
}
#endif

static esp_err_t i2s_check_cfg_validity(i2s_port_t i2s_num, const i2s_config_t *cfg)
{
    /* Step 1: Check the validity of input parameters */
    /* Check the validity of i2s device number */
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num] == NULL, ESP_ERR_INVALID_STATE, TAG, "this i2s port is in use");
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "I2S configuration must not be NULL");
    /* Check the size of DMA buffer */
    ESP_RETURN_ON_FALSE((cfg->dma_desc_num >= 2 && cfg->dma_desc_num <= 128), ESP_ERR_INVALID_ARG, TAG, "I2S buffer count less than 128 and more than 2");
    ESP_RETURN_ON_FALSE((cfg->dma_frame_num >= 8 && cfg->dma_frame_num <= 1024), ESP_ERR_INVALID_ARG, TAG, "I2S buffer length at most 1024 and more than 8");

#if SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX
    /* Check PDM mode */
    if (cfg->mode & I2S_MODE_PDM) {
        ESP_RETURN_ON_FALSE(i2s_num == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "I2S PDM mode only support on I2S0");
#if !SOC_I2S_SUPPORTS_PDM_TX
        ESP_RETURN_ON_FALSE(!(cfg->mode & I2S_MODE_TX), ESP_ERR_INVALID_ARG, TAG, "PDM does not support TX on this chip");
#endif // SOC_I2S_SUPPORTS_PDM_TX
#if !SOC_I2S_SUPPORTS_PDM_RX
        ESP_RETURN_ON_FALSE(!(cfg->mode & I2S_MODE_RX), ESP_ERR_INVALID_ARG, TAG, "PDM does not support RX on this chip");
#endif // SOC_I2S_SUPPORTS_PDM_RX
    }
#else
    ESP_RETURN_ON_FALSE(!(cfg->mode & I2S_MODE_PDM), ESP_ERR_INVALID_ARG, TAG, "I2S PDM mode not supported on current chip");
#endif // SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX

#if SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC
    /* Check built-in ADC/DAC mode */
    if (cfg->mode & (I2S_MODE_ADC_BUILT_IN | I2S_MODE_DAC_BUILT_IN)) {
        ESP_RETURN_ON_FALSE(i2s_num == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "I2S built-in ADC/DAC only support on I2S0");
    }
#else
    /* Check the transmit/receive mode */
    ESP_RETURN_ON_FALSE((cfg->mode & I2S_MODE_TX) || (cfg->mode & I2S_MODE_RX), ESP_ERR_INVALID_ARG, TAG, "I2S no TX/RX mode selected");
    /* Check communication format */
    ESP_RETURN_ON_FALSE(cfg->communication_format && (cfg->communication_format < I2S_COMM_FORMAT_STAND_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid communication formats");
#endif // SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC

    return ESP_OK;
}

static void i2s_set_slot_legacy(i2s_port_t i2s_num)
{
    bool is_tx_slave = p_i2s[i2s_num]->role == I2S_ROLE_SLAVE;
    bool is_rx_slave = is_tx_slave;
    if (p_i2s[i2s_num]->dir == (I2S_DIR_TX | I2S_DIR_RX)) {
        i2s_ll_share_bck_ws(p_i2s[i2s_num]->hal.dev, true);
        /* Since bck and ws are shared, only tx or rx can be master
           Force to set rx as slave to avoid conflict of clock signal */
        is_rx_slave = true;
    } else {
        i2s_ll_share_bck_ws(p_i2s[i2s_num]->hal.dev, false);
    }
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_STD) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_std_set_tx_slot(&(p_i2s[i2s_num]->hal), is_tx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_std_set_rx_slot(&(p_i2s[i2s_num]->hal), is_rx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
    }
#if SOC_I2S_SUPPORTS_PDM
    else if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) {
#if SOC_I2S_SUPPORTS_PDM_TX
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_pdm_set_tx_slot(&(p_i2s[i2s_num]->hal), is_tx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
#endif
#if SOC_I2S_SUPPORTS_PDM_RX
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_pdm_set_rx_slot(&(p_i2s[i2s_num]->hal), is_rx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
#endif
    }
#endif
#if SOC_I2S_SUPPORTS_TDM
    else if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_TDM) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_tdm_set_tx_slot(&(p_i2s[i2s_num]->hal), is_tx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_tdm_set_rx_slot(&(p_i2s[i2s_num]->hal), is_rx_slave, (i2s_hal_slot_config_t *)(&p_i2s[i2s_num]->slot_cfg));
        }
    }
#endif
#if SOC_I2S_SUPPORTS_ADC_DAC
    else if ((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_dac_set_slot_legacy();
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_adc_set_slot_legacy();
        }
    }
#endif
}

static void i2s_set_clock_legacy(i2s_port_t i2s_num)
{
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    i2s_hal_clock_info_t clk_info;
    i2s_calculate_clock(i2s_num, &clk_info);
    I2S_CLOCK_SRC_ATOMIC() {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_set_tx_clock(&(p_i2s[i2s_num]->hal), &clk_info, clk_cfg->clk_src);
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_set_rx_clock(&(p_i2s[i2s_num]->hal), &clk_info, clk_cfg->clk_src);
        }
    }
}

float i2s_get_clk(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    return (float)clk_cfg->sample_rate_hz;
}

esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, uint32_t bits_cfg, i2s_channel_t ch)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_ERR_INVALID_ARG, TAG, "I2S%d has not installed yet", i2s_num);

    /* Acquire the lock before stop i2s, otherwise reading/writing operation will stuck on receiving the message queue from interrupt */
    if (p_i2s[i2s_num]->dir & I2S_MODE_TX) {
        xSemaphoreTake(p_i2s[i2s_num]->tx->mux, portMAX_DELAY);
    }
    if (p_i2s[i2s_num]->dir & I2S_MODE_RX) {
        xSemaphoreTake(p_i2s[i2s_num]->rx->mux, portMAX_DELAY);
    }

    /* Stop I2S */
    i2s_stop(i2s_num);

    i2s_clk_config_t *clk_cfg = &p_i2s[i2s_num]->clk_cfg;
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[i2s_num]->slot_cfg;

    clk_cfg->sample_rate_hz = rate;
    slot_cfg->data_bit_width = bits_cfg & 0xFFFF;
    ESP_RETURN_ON_FALSE((slot_cfg->data_bit_width % 8 == 0), ESP_ERR_INVALID_ARG, TAG, "Invalid bits per sample");
    slot_cfg->slot_bit_width = (bits_cfg >> 16) > slot_cfg->data_bit_width ?
                               (bits_cfg >> 16) : slot_cfg->data_bit_width;
    ESP_RETURN_ON_FALSE((slot_cfg->slot_bit_width % 8 == 0), ESP_ERR_INVALID_ARG, TAG, "Invalid bits per channel");
    ESP_RETURN_ON_FALSE(((int)slot_cfg->slot_bit_width <= (int)I2S_BITS_PER_SAMPLE_32BIT), ESP_ERR_INVALID_ARG, TAG, "Invalid bits per sample");
    slot_cfg->slot_mode = ((ch & 0xFFFF) == I2S_CHANNEL_MONO) ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO;
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_STD) {
        if (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) {
            if (slot_cfg->std.slot_mask == I2S_STD_SLOT_BOTH) {
                slot_cfg->std.slot_mask = I2S_STD_SLOT_LEFT;
#if SOC_I2S_HW_VERSION_1
                // Enable right first to get correct data sequence
                slot_cfg->std.ws_pol = !slot_cfg->std.ws_pol;
#endif
            }
        } else {
            slot_cfg->std.slot_mask = I2S_STD_SLOT_BOTH;
        }
    }
#if SOC_I2S_SUPPORTS_TDM
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_TDM) {
        uint32_t slot_mask = ch >> 16;
        if (slot_mask == 0) {
            slot_mask = (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ? 1 : 2;
        }
        ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->total_slot >= (32 - __builtin_clz(slot_mask)), ESP_ERR_INVALID_ARG, TAG,
                            "The max channel number can't be greater than CH%"PRIu32, p_i2s[i2s_num]->total_slot);
        p_i2s[i2s_num]->active_slot = __builtin_popcount(slot_mask);
    } else
#endif
    {
        p_i2s[i2s_num]->active_slot = (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ? 1 : 2;
    }

    i2s_set_slot_legacy(i2s_num);
    i2s_set_clock_legacy(i2s_num);

    uint32_t buf_size = i2s_get_buf_size(i2s_num);
    bool need_realloc = buf_size != p_i2s[i2s_num]->last_buf_size;

    if (need_realloc) {
        esp_err_t ret = ESP_OK;
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            p_i2s[i2s_num]->tx->buf_size = buf_size;
            ret = i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->tx);
            xQueueReset(p_i2s[i2s_num]->tx->queue);
            ESP_RETURN_ON_ERROR(ret, TAG, "I2S%d tx DMA buffer malloc failed", i2s_num);
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            p_i2s[i2s_num]->rx->buf_size = buf_size;
            ret = i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->rx);
            xQueueReset(p_i2s[i2s_num]->rx->queue);
            ESP_RETURN_ON_ERROR(ret, TAG, "I2S%d rx DMA buffer malloc failed", i2s_num);
        }
    }
    /* Update last buffer size */
    p_i2s[i2s_num]->last_buf_size = buf_size;

    /* I2S start */
    i2s_start(i2s_num);

    if (p_i2s[i2s_num]->dir & I2S_MODE_TX) {
        xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    }
    if (p_i2s[i2s_num]->dir & I2S_MODE_RX) {
        xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
    }

    return ESP_OK;
}

esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    i2s_hal_slot_config_t *slot_cfg = &p_i2s[i2s_num]->slot_cfg;
    uint32_t mask = 0;
#if SOC_I2S_SUPPORTS_TDM
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_TDM) {
        mask = slot_cfg->tdm.slot_mask;
    }
#endif
    return i2s_set_clk(i2s_num, rate, slot_cfg->data_bit_width, slot_cfg->slot_mode | (mask << 16));
}

#if SOC_I2S_SUPPORTS_PCM

esp_err_t i2s_pcm_config(i2s_port_t i2s_num, const i2s_pcm_cfg_t *pcm_cfg)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");

    if (p_i2s[i2s_num]->dir & I2S_MODE_TX) {
        xSemaphoreTake(p_i2s[i2s_num]->tx->mux, portMAX_DELAY);
    }
    if (p_i2s[i2s_num]->dir & I2S_MODE_RX) {
        xSemaphoreTake(p_i2s[i2s_num]->rx->mux, portMAX_DELAY);
    }

    i2s_stop(i2s_num);
    I2S_ENTER_CRITICAL(i2s_num);
    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        i2s_ll_tx_set_pcm_type(p_i2s[i2s_num]->hal.dev, pcm_cfg->pcm_type);
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        i2s_ll_rx_set_pcm_type(p_i2s[i2s_num]->hal.dev, pcm_cfg->pcm_type);
    }
    I2S_EXIT_CRITICAL(i2s_num);
    i2s_start(i2s_num);

    if (p_i2s[i2s_num]->dir & I2S_MODE_TX) {
        xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    }
    if (p_i2s[i2s_num]->dir & I2S_MODE_RX) {
        xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
    }

    return ESP_OK;
}
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
esp_err_t i2s_set_pdm_rx_down_sample(i2s_port_t i2s_num, i2s_pdm_dsr_t downsample)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM), ESP_ERR_INVALID_ARG, TAG, "i2s mode is not PDM mode");
    xSemaphoreTake(p_i2s[i2s_num]->rx->mux, portMAX_DELAY);
    i2s_stop(i2s_num);
    p_i2s[i2s_num]->clk_cfg.dn_sample_mode = downsample;
    i2s_ll_rx_set_pdm_dsr(p_i2s[i2s_num]->hal.dev, downsample);
    i2s_start(i2s_num);
    xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->clk_cfg.sample_rate_hz, p_i2s[i2s_num]->slot_cfg.data_bit_width, p_i2s[i2s_num]->slot_cfg.slot_mode);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
esp_err_t i2s_set_pdm_tx_up_sample(i2s_port_t i2s_num, const i2s_pdm_tx_upsample_cfg_t *upsample_cfg)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) && (p_i2s[i2s_num]->dir & I2S_DIR_TX),
                        ESP_ERR_INVALID_ARG, TAG, "i2s mode is not PDM mode");
    xSemaphoreTake(p_i2s[i2s_num]->tx->mux, portMAX_DELAY);
    i2s_stop(i2s_num);
    p_i2s[i2s_num]->clk_cfg.up_sample_fp = upsample_cfg->fp;
    p_i2s[i2s_num]->clk_cfg.up_sample_fs = upsample_cfg->fs;
    i2s_ll_tx_set_pdm_fpfs(p_i2s[i2s_num]->hal.dev, upsample_cfg->fp, upsample_cfg->fs);
    i2s_ll_tx_set_pdm_over_sample_ratio(p_i2s[i2s_num]->hal.dev, upsample_cfg->fp / upsample_cfg->fs);
    i2s_start(i2s_num);
    xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->clk_cfg.sample_rate_hz, p_i2s[i2s_num]->slot_cfg.data_bit_width, p_i2s[i2s_num]->slot_cfg.slot_mode);
}
#endif

static esp_err_t i2s_dma_object_init(i2s_port_t i2s_num)
{
    uint32_t buf_size = i2s_get_buf_size(i2s_num);
    p_i2s[i2s_num]->last_buf_size = buf_size;
    /* Create DMA object */
    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        ESP_RETURN_ON_ERROR(i2s_create_dma_object(i2s_num, &p_i2s[i2s_num]->tx), TAG, "I2S TX DMA object create failed");
        p_i2s[i2s_num]->tx->buf_size = buf_size;
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        ESP_RETURN_ON_ERROR(i2s_create_dma_object(i2s_num, &p_i2s[i2s_num]->rx), TAG, "I2S RX DMA object create failed");
        p_i2s[i2s_num]->rx->buf_size = buf_size;
    }
    return ESP_OK;
}

static void i2s_mode_identify(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{

    p_i2s[i2s_num]->mode = I2S_COMM_MODE_STD;

    if (i2s_config->mode & I2S_MODE_MASTER) {
        p_i2s[i2s_num]->role = I2S_ROLE_MASTER;
    } else if (i2s_config->mode & I2S_MODE_SLAVE) {
        p_i2s[i2s_num]->role = I2S_ROLE_SLAVE;
    }
    if (i2s_config->mode & I2S_MODE_TX) {
        p_i2s[i2s_num]->dir |= I2S_DIR_TX;
    }
    if (i2s_config->mode & I2S_MODE_RX) {
        p_i2s[i2s_num]->dir |= I2S_DIR_RX;
    }
#if SOC_I2S_SUPPORTS_PDM
    if (i2s_config->mode & I2S_MODE_PDM) {
        p_i2s[i2s_num]->mode = I2S_COMM_MODE_PDM;
    }
#endif // SOC_I2S_SUPPORTS_PDM

#if SOC_I2S_SUPPORTS_TDM
    if (i2s_config->channel_format == I2S_CHANNEL_FMT_MULTIPLE) {
        p_i2s[i2s_num]->mode = I2S_COMM_MODE_TDM;
    }
#endif // SOC_I2S_SUPPORTS_TDM

#if SOC_I2S_SUPPORTS_ADC_DAC
    if ((i2s_config->mode & I2S_MODE_DAC_BUILT_IN) ||
            (i2s_config->mode & I2S_MODE_ADC_BUILT_IN)) {
        p_i2s[i2s_num]->mode = (i2s_comm_mode_t)I2S_COMM_MODE_ADC_DAC;
    }
#endif // SOC_I2S_SUPPORTS_ADC_DAC
}

static esp_err_t i2s_config_transfer(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{
#define SLOT_CFG(m)     p_i2s[i2s_num]->slot_cfg.m
#define CLK_CFG()       p_i2s[i2s_num]->clk_cfg
    /* Convert legacy configuration into general part of slot and clock configuration */
    p_i2s[i2s_num]->slot_cfg.data_bit_width = i2s_config->bits_per_sample;
    p_i2s[i2s_num]->slot_cfg.slot_bit_width = (int)i2s_config->bits_per_chan < (int)i2s_config->bits_per_sample ?
                                              i2s_config->bits_per_sample : i2s_config->bits_per_chan;

    p_i2s[i2s_num]->slot_cfg.slot_mode = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ?
                                         I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
    CLK_CFG().sample_rate_hz = i2s_config->sample_rate;
    CLK_CFG().mclk_multiple = i2s_config->mclk_multiple == 0 ? I2S_MCLK_MULTIPLE_256 : i2s_config->mclk_multiple;
    CLK_CFG().clk_src = I2S_CLK_SRC_DEFAULT;
    p_i2s[i2s_num]->fixed_mclk = i2s_config->fixed_mclk;
    p_i2s[i2s_num]->use_apll = false;
#if SOC_I2S_SUPPORTS_APLL
    CLK_CFG().clk_src = i2s_config->use_apll ? I2S_CLK_SRC_APLL : I2S_CLK_SRC_DEFAULT;
    p_i2s[i2s_num]->use_apll = i2s_config->use_apll;
#endif // SOC_I2S_SUPPORTS_APLL

    /* Convert legacy configuration into particular part of slot and clock configuration */
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_STD) {
        /* Generate STD slot configuration */
        SLOT_CFG(std).ws_width = i2s_config->bits_per_sample;
        SLOT_CFG(std).ws_pol = false;
        if (i2s_config->channel_format == I2S_CHANNEL_FMT_RIGHT_LEFT) {
            SLOT_CFG(std).slot_mask = I2S_STD_SLOT_BOTH;
        } else if (i2s_config->channel_format == I2S_CHANNEL_FMT_ALL_LEFT ||
                   i2s_config->channel_format == I2S_CHANNEL_FMT_ONLY_LEFT) {
            SLOT_CFG(std).slot_mask = I2S_STD_SLOT_LEFT;
        } else {
            SLOT_CFG(std).slot_mask = I2S_STD_SLOT_RIGHT;
        }
        if (i2s_config->communication_format == I2S_COMM_FORMAT_STAND_I2S) {
            SLOT_CFG(std).bit_shift = true;
        }
        if (i2s_config->communication_format & I2S_COMM_FORMAT_STAND_PCM_SHORT) {
            SLOT_CFG(std).bit_shift = true;
            SLOT_CFG(std).ws_width = 1;
            SLOT_CFG(std).ws_pol = true;
        }
#if SOC_I2S_HW_VERSION_1
        SLOT_CFG(std).msb_right = true;
#elif SOC_I2S_HW_VERSION_2
        SLOT_CFG(std).left_align = i2s_config->left_align;
        SLOT_CFG(std).big_endian = i2s_config->big_edin;
        SLOT_CFG(std).bit_order_lsb = i2s_config->bit_order_msb; // The old name is incorrect
#endif // SOC_I2S_HW_VERSION_1

        p_i2s[i2s_num]->active_slot = (int)p_i2s[i2s_num]->slot_cfg.slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;
        p_i2s[i2s_num]->total_slot = 2;
        goto finish;
    }
#if SOC_I2S_SUPPORTS_PDM_TX
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) {
        /* Generate PDM TX slot configuration */
        SLOT_CFG(pdm_tx).sd_prescale = 0;
        SLOT_CFG(pdm_tx).sd_scale = I2S_PDM_SIG_SCALING_MUL_1;
        SLOT_CFG(pdm_tx).hp_scale = I2S_PDM_SIG_SCALING_MUL_1;
        SLOT_CFG(pdm_tx).lp_scale = I2S_PDM_SIG_SCALING_MUL_1;
        SLOT_CFG(pdm_tx).sinc_scale = I2S_PDM_SIG_SCALING_MUL_1;
#if SOC_I2S_HW_VERSION_2
        SLOT_CFG(pdm_tx).line_mode = I2S_PDM_TX_ONE_LINE_CODEC;
        SLOT_CFG(pdm_tx).hp_en = true;
        SLOT_CFG(pdm_tx).hp_cut_off_freq_hzx10 = 490;
        SLOT_CFG(pdm_tx).sd_dither = 0;
        SLOT_CFG(pdm_tx).sd_dither2 = 1;
#endif // SOC_I2S_HW_VERSION_2

        /* Generate PDM TX clock configuration */
        CLK_CFG().up_sample_fp = 960;
        CLK_CFG().up_sample_fs = i2s_config->sample_rate / 100;
        p_i2s[i2s_num]->active_slot = (int)p_i2s[i2s_num]->slot_cfg.slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;
        p_i2s[i2s_num]->total_slot = 2;
        goto finish;
    }
#endif // SOC_I2S_SUPPORTS_PDM_TX

#if SOC_I2S_SUPPORTS_PDM_RX
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) {
        /* Generate PDM RX clock configuration */
        CLK_CFG().dn_sample_mode = I2S_PDM_DSR_8S;
        p_i2s[i2s_num]->active_slot = (int)p_i2s[i2s_num]->slot_cfg.slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;
        p_i2s[i2s_num]->total_slot = 2;
        goto finish;
    }
#endif // SOC_I2S_SUPPOTYS_PDM_RX

#if SOC_I2S_SUPPORTS_TDM
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_TDM) {
        /* Generate TDM slot configuration */
        SLOT_CFG(tdm).slot_mask = i2s_config->chan_mask >> 16;

        SLOT_CFG(tdm).ws_width = 0; // I2S_TDM_AUTO_WS_WIDTH
        p_i2s[i2s_num]->slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
        SLOT_CFG(tdm).ws_pol = false;
        if (i2s_config->communication_format == I2S_COMM_FORMAT_STAND_I2S) {
            SLOT_CFG(tdm).bit_shift = true;
        } else if (i2s_config->communication_format == I2S_COMM_FORMAT_STAND_PCM_SHORT) {
            SLOT_CFG(tdm).bit_shift = true;
            SLOT_CFG(tdm).ws_width = 1;
            SLOT_CFG(tdm).ws_pol = true;
        } else if (i2s_config->communication_format == I2S_COMM_FORMAT_STAND_PCM_LONG) {
            SLOT_CFG(tdm).bit_shift = true;
            SLOT_CFG(tdm).ws_width = p_i2s[i2s_num]->slot_cfg.slot_bit_width;
            SLOT_CFG(tdm).ws_pol = true;
        }
        SLOT_CFG(tdm).left_align = i2s_config->left_align;
        SLOT_CFG(tdm).big_endian = i2s_config->big_edin;
        SLOT_CFG(tdm).bit_order_lsb = i2s_config->bit_order_msb; // The old name is incorrect
        SLOT_CFG(tdm).skip_mask = i2s_config->skip_msk;

        /* Generate TDM clock configuration */
        p_i2s[i2s_num]->active_slot = __builtin_popcount(SLOT_CFG(tdm).slot_mask);
        uint32_t mx_slot = 32 - __builtin_clz(SLOT_CFG(tdm).slot_mask);
        mx_slot = mx_slot < 2 ? 2 : mx_slot;
        p_i2s[i2s_num]->total_slot = mx_slot < i2s_config->total_chan ? mx_slot : i2s_config->total_chan;
        goto finish;
    }
#endif // SOC_I2S_SUPPORTS_TDM

#if SOC_I2S_SUPPORTS_ADC_DAC
    if ((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) {
        p_i2s[i2s_num]->slot_cfg.slot_mode = (p_i2s[i2s_num]->dir & I2S_DIR_TX) ?
                                             I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
        p_i2s[i2s_num]->active_slot = (p_i2s[i2s_num]->dir & I2S_DIR_TX) ? 2 : 1;
        p_i2s[i2s_num]->total_slot = 2;
    }
#endif // SOC_I2S_SUPPORTS_ADC_DAC

#undef SLOT_CFG
#undef CLK_CFG

finish:
    return ESP_OK;
}

static esp_err_t i2s_init_legacy(i2s_port_t i2s_num, int intr_alloc_flag)
{
    /* Create power management lock */
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
    if (p_i2s[i2s_num]->use_apll) {
        pm_lock = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2S_SUPPORTS_APLL
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_lock, 0, "i2s_driver", &p_i2s[i2s_num]->pm_lock), TAG, "I2S pm lock error");
#endif //CONFIG_PM_ENABLE

#if SOC_I2S_SUPPORTS_APLL
    if (p_i2s[i2s_num]->use_apll) {
        periph_rtc_apll_acquire();
    }
#endif

    /* Enable communicaiton mode */
    if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_STD) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_std_enable_tx_channel(&(p_i2s[i2s_num]->hal));
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_std_enable_rx_channel(&(p_i2s[i2s_num]->hal));
        }
    }
#if SOC_I2S_SUPPORTS_PDM
    else if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_PDM) {
#if SOC_I2S_SUPPORTS_PDM_TX
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_pdm_enable_tx_channel(&(p_i2s[i2s_num]->hal));
        }
#endif
#if SOC_I2S_SUPPORTS_PDM_RX
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_pdm_enable_rx_channel(&(p_i2s[i2s_num]->hal));
        }
#endif
    }
#endif
#if SOC_I2S_SUPPORTS_TDM
    else if (p_i2s[i2s_num]->mode == I2S_COMM_MODE_TDM) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_hal_tdm_enable_tx_channel(&(p_i2s[i2s_num]->hal));
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            i2s_hal_tdm_enable_rx_channel(&(p_i2s[i2s_num]->hal));
        }
    }
#endif
#if SOC_I2S_SUPPORTS_ADC_DAC
    if ((int)p_i2s[i2s_num]->mode == I2S_COMM_MODE_ADC_DAC) {
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            sar_periph_ctrl_adc_continuous_power_acquire();
            adc_set_i2s_data_source(ADC_I2S_DATA_SRC_ADC);
            i2s_ll_enable_builtin_adc_dac(p_i2s[i2s_num]->hal.dev, true);
        }
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            i2s_ll_enable_builtin_adc_dac(p_i2s[i2s_num]->hal.dev, true);
        }
    } else {
        adc_set_i2s_data_source(ADC_I2S_DATA_SRC_IO_SIG);
        i2s_ll_enable_builtin_adc_dac(p_i2s[i2s_num]->hal.dev, false);
    }
#endif

    i2s_set_slot_legacy(i2s_num);
    i2s_set_clock_legacy(i2s_num);
    ESP_RETURN_ON_ERROR(i2s_dma_intr_init(i2s_num, intr_alloc_flag), TAG, "I2S interrupt initialize failed");
    ESP_RETURN_ON_ERROR(i2s_dma_object_init(i2s_num), TAG, "I2S dma object create failed");
    if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
        ESP_RETURN_ON_ERROR(i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->tx), TAG, "Allocate I2S dma tx buffer failed");
    }
    if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
        ESP_RETURN_ON_ERROR(i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->rx), TAG, "Allocate I2S dma rx buffer failed");
    }

    return ESP_OK;
}

esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE(i2s_num < SOC_I2S_NUM, ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_ERR_INVALID_STATE, TAG, "I2S port %d has not installed", i2s_num);
    i2s_obj_t *obj = p_i2s[i2s_num];
    i2s_stop(i2s_num);

#if CONFIG_IDF_TARGET_ESP32
    if (obj->mclk_out_hdl) {
        esp_clock_output_stop(obj->mclk_out_hdl);
    }
#endif

#if SOC_I2S_SUPPORTS_ADC_DAC
    if ((int)(obj->mode) == I2S_COMM_MODE_ADC_DAC) {
        if (obj->dir & I2S_DIR_TX) {
            // Deinit DAC
            i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE);
        }
        if (obj->dir & I2S_DIR_RX) {
            // Deinit ADC
            adc_set_i2s_data_source(ADC_I2S_DATA_SRC_IO_SIG);
            sar_periph_ctrl_adc_continuous_power_release();
        }
    }
#endif
#if SOC_GDMA_SUPPORTED
    if (obj->tx_dma_chan) {
        gdma_disconnect(obj->tx_dma_chan);
        gdma_del_channel(obj->tx_dma_chan);
    }
    if (obj->rx_dma_chan) {
        gdma_disconnect(obj->rx_dma_chan);
        gdma_del_channel(obj->rx_dma_chan);
    }
#else
    if (obj->i2s_isr_handle) {
        esp_intr_free(obj->i2s_isr_handle);
    }
#endif
    /* Destroy dma object if exist */
    i2s_destroy_dma_object(i2s_num, &obj->tx);
    i2s_destroy_dma_object(i2s_num, &obj->rx);

    if (obj->i2s_queue) {
        vQueueDelete(obj->i2s_queue);
        obj->i2s_queue = NULL;
    }

#if SOC_I2S_SUPPORTS_APLL
    if (obj->use_apll) {
        I2S_CLOCK_SRC_ATOMIC() {
            // switch back to PLL clock source
            if (obj->dir & I2S_DIR_TX) {
                i2s_hal_set_tx_clock(&obj->hal, NULL, I2S_CLK_SRC_DEFAULT);
            }
            if (obj->dir & I2S_DIR_RX) {
                i2s_hal_set_rx_clock(&obj->hal, NULL, I2S_CLK_SRC_DEFAULT);
            }
        }
        periph_rtc_apll_release();
    }
#endif

#ifdef CONFIG_PM_ENABLE
    if (obj->pm_lock) {
        esp_pm_lock_delete(obj->pm_lock);
        obj->pm_lock = NULL;
    }
#endif
#if SOC_I2S_HW_VERSION_2
    I2S_CLOCK_SRC_ATOMIC() {
        if (obj->dir & I2S_DIR_TX) {
            i2s_ll_tx_disable_clock(obj->hal.dev);
        }
        if (obj->dir & I2S_DIR_RX) {
            i2s_ll_rx_disable_clock(obj->hal.dev);
        }
    }
#endif
    /* Disable module clock */
    i2s_platform_release_occupation(i2s_num);
    free(obj);
    p_i2s[i2s_num] = NULL;
    return ESP_OK;
}

esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void *i2s_queue)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;

    /* Step 1: Check the validity of input parameters */
    ESP_RETURN_ON_ERROR(i2s_check_cfg_validity(i2s_num, i2s_config), TAG, "I2S configuration is invalid");

    /* Step 2: Allocate driver object and register to platform */
    i2s_obj_t *i2s_obj = calloc(1, sizeof(i2s_obj_t));
    ESP_RETURN_ON_FALSE(i2s_obj, ESP_ERR_NO_MEM, TAG, "no mem for I2S driver");
    if (i2s_platform_acquire_occupation(i2s_num, "i2s_legacy") != ESP_OK) {
        free(i2s_obj);
        ESP_LOGE(TAG, "register I2S object to platform failed");
        return ESP_ERR_INVALID_STATE;
    }
    p_i2s[i2s_num] = i2s_obj;
    i2s_hal_init(&i2s_obj->hal, i2s_num);

    /* Step 3: Store and assign configurations */
    i2s_mode_identify(i2s_num, i2s_config);
    ESP_GOTO_ON_ERROR(i2s_config_transfer(i2s_num, i2s_config), err, TAG, "I2S install failed");
    i2s_obj->dma_desc_num = i2s_config->dma_desc_num;
    i2s_obj->dma_frame_num = i2s_config->dma_frame_num;
    i2s_obj->tx_desc_auto_clear = i2s_config->tx_desc_auto_clear;

    /* Step 4: Apply configurations and init hardware */
    ESP_GOTO_ON_ERROR(i2s_init_legacy(i2s_num, i2s_config->intr_alloc_flags), err, TAG, "I2S init failed");

    /* Step 5: Initialise i2s event queue if user needs */
    if (i2s_queue) {
        i2s_obj->i2s_queue = xQueueCreate(queue_size, sizeof(i2s_event_t));
        ESP_GOTO_ON_FALSE(i2s_obj->i2s_queue, ESP_ERR_NO_MEM, err, TAG, "I2S queue create failed");
        *((QueueHandle_t *) i2s_queue) = i2s_obj->i2s_queue;
        ESP_LOGD(TAG, "queue free spaces: %" PRIu32, (uint32_t)uxQueueSpacesAvailable(i2s_obj->i2s_queue));
    } else {
        i2s_obj->i2s_queue = NULL;
    }

    /* Step 6: Start I2S for backward compatibility */
    ESP_GOTO_ON_ERROR(i2s_start(i2s_num), err, TAG, "I2S start failed");

    return ESP_OK;

err:
    /* I2S install failed, prepare to uninstall */
    i2s_driver_uninstall(i2s_num);
    return ret;
}

esp_err_t i2s_write(i2s_port_t i2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr;
    char *src_byte;
    size_t bytes_can_write;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->tx), ESP_ERR_INVALID_ARG, TAG, "TX mode is not enabled");
    xSemaphoreTake(p_i2s[i2s_num]->tx->mux, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2s[i2s_num]->pm_lock);
#endif
    src_byte = (char *)src;
    while (size > 0) {
        if (p_i2s[i2s_num]->tx->rw_pos == p_i2s[i2s_num]->tx->buf_size || p_i2s[i2s_num]->tx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s[i2s_num]->tx->queue, &p_i2s[i2s_num]->tx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s[i2s_num]->tx->rw_pos = 0;
        }
        ESP_LOGD(TAG, "size: %d, rw_pos: %d, buf_size: %d, curr_ptr: %p", size, p_i2s[i2s_num]->tx->rw_pos, p_i2s[i2s_num]->tx->buf_size, p_i2s[i2s_num]->tx->curr_ptr);
        data_ptr = (char *)p_i2s[i2s_num]->tx->curr_ptr;
        data_ptr += p_i2s[i2s_num]->tx->rw_pos;
        bytes_can_write = p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos;
        if (bytes_can_write > size) {
            bytes_can_write = size;
        }
        memcpy(data_ptr, src_byte, bytes_can_write);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync((void *)(p_i2s[i2s_num]->tx->curr_ptr), p_i2s[i2s_num]->tx->buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        size -= bytes_can_write;
        src_byte += bytes_can_write;
        p_i2s[i2s_num]->tx->rw_pos += bytes_can_write;
        (*bytes_written) += bytes_can_write;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(p_i2s[i2s_num]->pm_lock);
#endif
    xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    return ESP_OK;
}

esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_write;
    int tail;
    int src_bytes;
    int aim_bytes;
    int zero_bytes;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((size > 0), ESP_ERR_INVALID_ARG, TAG, "size must greater than zero");
    ESP_RETURN_ON_FALSE((aim_bits >= src_bits), ESP_ERR_INVALID_ARG, TAG, "aim_bits mustn't be less than src_bits");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->tx), ESP_ERR_INVALID_ARG, TAG, "TX mode is not enabled");
    if (src_bits < I2S_BITS_PER_SAMPLE_8BIT || aim_bits < I2S_BITS_PER_SAMPLE_8BIT) {
        ESP_LOGE(TAG, "bits mustn't be less than 8, src_bits %d aim_bits %d", src_bits, aim_bits);
        return ESP_ERR_INVALID_ARG;
    }
    if (src_bits > I2S_BITS_PER_SAMPLE_32BIT || aim_bits > I2S_BITS_PER_SAMPLE_32BIT) {
        ESP_LOGE(TAG, "bits mustn't be greater than 32, src_bits %d aim_bits %d", src_bits, aim_bits);
        return ESP_ERR_INVALID_ARG;
    }
    if ((src_bits == I2S_BITS_PER_SAMPLE_16BIT || src_bits == I2S_BITS_PER_SAMPLE_32BIT) && (size % 2 != 0)) {
        ESP_LOGE(TAG, "size must be a even number while src_bits is even, src_bits %d size %d", src_bits, size);
        return ESP_ERR_INVALID_ARG;
    }
    if (src_bits == I2S_BITS_PER_SAMPLE_24BIT && (size % 3 != 0)) {
        ESP_LOGE(TAG, "size must be a multiple of 3 while src_bits is 24, size %d", size);
        return ESP_ERR_INVALID_ARG;
    }

    src_bytes = src_bits / 8;
    aim_bytes = aim_bits / 8;
    zero_bytes = aim_bytes - src_bytes;
    xSemaphoreTake(p_i2s[i2s_num]->tx->mux, portMAX_DELAY);
    size = size * aim_bytes / src_bytes;
    ESP_LOGD(TAG, "aim_bytes %d src_bytes %d size %d", aim_bytes, src_bytes, size);
    while (size > 0) {
        if (p_i2s[i2s_num]->tx->rw_pos == p_i2s[i2s_num]->tx->buf_size || p_i2s[i2s_num]->tx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s[i2s_num]->tx->queue, &p_i2s[i2s_num]->tx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s[i2s_num]->tx->rw_pos = 0;
        }
        data_ptr = (char *)p_i2s[i2s_num]->tx->curr_ptr;
        data_ptr += p_i2s[i2s_num]->tx->rw_pos;
        bytes_can_write = p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos;
        if (bytes_can_write > (int)size) {
            bytes_can_write = size;
        }
        tail = bytes_can_write % aim_bytes;
        bytes_can_write = bytes_can_write - tail;

        memset(data_ptr, 0, bytes_can_write);
        for (int j = 0; j < bytes_can_write; j += (aim_bytes - zero_bytes)) {
            j += zero_bytes;
            memcpy(&data_ptr[j], (const char *)(src + *bytes_written), aim_bytes - zero_bytes);
            (*bytes_written) += (aim_bytes - zero_bytes);
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync((void *)p_i2s[i2s_num]->tx->curr_ptr, p_i2s[i2s_num]->tx->buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        size -= bytes_can_write;
        p_i2s[i2s_num]->tx->rw_pos += bytes_can_write;
    }
    xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    return ESP_OK;
}

esp_err_t i2s_read(i2s_port_t i2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    char *data_ptr;
    char *dest_byte;
    int bytes_can_read;
    *bytes_read = 0;
    dest_byte = (char *)dest;
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->rx), ESP_ERR_INVALID_ARG, TAG, "RX mode is not enabled");
    xSemaphoreTake(p_i2s[i2s_num]->rx->mux, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2s[i2s_num]->pm_lock);
#endif
    while (size > 0) {
        if (p_i2s[i2s_num]->rx->rw_pos == p_i2s[i2s_num]->rx->buf_size || p_i2s[i2s_num]->rx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s[i2s_num]->rx->queue, &p_i2s[i2s_num]->rx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s[i2s_num]->rx->rw_pos = 0;
        }
        data_ptr = (char *)p_i2s[i2s_num]->rx->curr_ptr;
        data_ptr += p_i2s[i2s_num]->rx->rw_pos;
        bytes_can_read = p_i2s[i2s_num]->rx->buf_size - p_i2s[i2s_num]->rx->rw_pos;
        if (bytes_can_read > (int)size) {
            bytes_can_read = size;
        }
        memcpy(dest_byte, data_ptr, bytes_can_read);
        size -= bytes_can_read;
        dest_byte += bytes_can_read;
        p_i2s[i2s_num]->rx->rw_pos += bytes_can_read;
        (*bytes_read) += bytes_can_read;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(p_i2s[i2s_num]->pm_lock);
#endif
    xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
    return ESP_OK;
}

/*-------------------------------------------------------------
                    I2S GPIO operation
  -------------------------------------------------------------*/
static void gpio_matrix_out_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    //if pin = -1, do not need to configure
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio, signal_idx, out_inv, oen_inv);
    }
}

static void gpio_matrix_in_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool inv)
{
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        /* Set direction, for some GPIOs, the input function are not enabled as default */
        gpio_set_direction(gpio, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(gpio, signal_idx, inv);
    }
}

static esp_err_t i2s_check_set_mclk(i2s_port_t i2s_num, gpio_num_t gpio_num)
{
    if (gpio_num == -1) {
        return ESP_OK;
    }
#if CONFIG_IDF_TARGET_ESP32
    soc_clkout_sig_id_t clkout_sig = (i2s_num == I2S_NUM_0) ? CLKOUT_SIG_I2S0 : CLKOUT_SIG_I2S1;
    ESP_RETURN_ON_ERROR(esp_clock_output_start(clkout_sig, gpio_num, &p_i2s[i2s_num]->mclk_out_hdl), TAG, "mclk configure failed");
#else
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "mck_io_num invalid");
    gpio_matrix_out_check_and_set(gpio_num, i2s_periph_signal[i2s_num].mck_out_sig, 0, 0);
#endif
    ESP_LOGD(TAG, "I2S%d, MCLK output by GPIO%d", i2s_num, gpio_num);
    return ESP_OK;
}

esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    uint32_t buf_cnt = p_i2s[i2s_num]->dma_desc_num;

    /* Clear I2S RX DMA buffer */
    if (p_i2s[i2s_num]->rx && p_i2s[i2s_num]->rx->buf != NULL && p_i2s[i2s_num]->rx->buf_size != 0) {
        for (int i = 0; i < buf_cnt; i++) {
            memset(p_i2s[i2s_num]->rx->buf[i], 0, p_i2s[i2s_num]->rx->buf_size);
        }
    }

    /* Clear I2S TX DMA buffer */
    if (p_i2s[i2s_num]->tx && p_i2s[i2s_num]->tx->buf != NULL && p_i2s[i2s_num]->tx->buf_size != 0) {
        /* Finish to write all tx data */
        int bytes_left = (p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos) % 4;
        if (bytes_left) {
            size_t zero_bytes = 0;
            size_t bytes_written;
            i2s_write(i2s_num, (void *)&zero_bytes, bytes_left, &bytes_written, portMAX_DELAY);
        }
        for (int i = 0; i < buf_cnt; i++) {
            memset(p_i2s[i2s_num]->tx->buf[i], 0, p_i2s[i2s_num]->tx->buf_size);
        }
    }
    return ESP_OK;
}

esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin)
{
    ESP_RETURN_ON_FALSE((i2s_num < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    if (pin == NULL) {
#if SOC_I2S_SUPPORTS_DAC
        return i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#else
        return ESP_ERR_INVALID_ARG;
#endif
    }
    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((pin->bck_io_num == -1 || GPIO_IS_VALID_GPIO(pin->bck_io_num)),
                        ESP_ERR_INVALID_ARG, TAG, "bck_io_num invalid");
    ESP_RETURN_ON_FALSE((pin->ws_io_num == -1 || GPIO_IS_VALID_GPIO(pin->ws_io_num)),
                        ESP_ERR_INVALID_ARG, TAG, "ws_io_num invalid");
    ESP_RETURN_ON_FALSE((pin->data_out_num == -1 || GPIO_IS_VALID_GPIO(pin->data_out_num)),
                        ESP_ERR_INVALID_ARG, TAG, "data_out_num invalid");
    ESP_RETURN_ON_FALSE((pin->data_in_num == -1 || GPIO_IS_VALID_GPIO(pin->data_in_num)),
                        ESP_ERR_INVALID_ARG, TAG, "data_in_num invalid");

    if (p_i2s[i2s_num]->role == I2S_ROLE_SLAVE) {
        /* For "tx + rx + slave" or "rx + slave" mode, we should select RX signal index for ws and bck */
        if (p_i2s[i2s_num]->dir & I2S_DIR_RX) {
            gpio_matrix_in_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].s_rx_ws_sig, 0);
            gpio_matrix_in_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].s_rx_bck_sig, 0);
            /* For "tx + slave" mode, we should select TX signal index for ws and bck */
        } else {
            gpio_matrix_in_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].s_tx_ws_sig, 0);
            gpio_matrix_in_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].s_tx_bck_sig, 0);
        }
    } else {
        /* mclk only available in master mode */
        ESP_RETURN_ON_ERROR(i2s_check_set_mclk(i2s_num, pin->mck_io_num), TAG, "mclk config failed");
        /* For "tx + rx + master" or "tx + master" mode, we should select TX signal index for ws and bck */
        if (p_i2s[i2s_num]->dir & I2S_DIR_TX) {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].m_tx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].m_tx_bck_sig, 0, 0);
            /* For "rx + master" mode, we should select RX signal index for ws and bck */
        } else {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].m_rx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].m_rx_bck_sig, 0, 0);
        }
    }

    /* Set data input/output GPIO */
    gpio_matrix_out_check_and_set(pin->data_out_num, i2s_periph_signal[i2s_num].data_out_sig, 0, 0);
    gpio_matrix_in_check_and_set(pin->data_in_num, i2s_periph_signal[i2s_num].data_in_sig, 0);
    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that the new i2s driver is not running along with the legacy i2s driver
 */
static __attribute__((constructor)) void check_i2s_driver_conflict(void)
{
    extern __attribute__((weak)) esp_err_t i2s_del_channel(void *handle);
    /* If the new I2S driver is linked, the weak function will point to the actual function in the new driver, otherwise it is NULL*/
    if ((void *)i2s_del_channel != NULL) {
        ESP_EARLY_LOGE(TAG, "CONFLICT! The new i2s driver can't work along with the legacy i2s driver");
        abort();
    }
    ESP_EARLY_LOGW(TAG, "legacy i2s driver is deprecated, please migrate to use driver/i2s_std.h, driver/i2s_pdm.h or driver/i2s_tdm.h");
}
