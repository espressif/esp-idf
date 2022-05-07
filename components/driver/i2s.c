/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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

#include "soc/lldesc.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "hal/gpio_hal.h"
#include "hal/i2s_hal.h"
#if SOC_I2S_SUPPORTS_DAC
#include "driver/dac.h"
#endif // SOC_I2S_SUPPORTS_DAC

#if SOC_I2S_SUPPORTS_ADC
#include "adc1_private.h"
#endif // SOC_I2S_SUPPORTS_ADC

#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif

#include "soc/clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_efuse.h"
#include "esp_rom_gpio.h"
#include "esp_private/i2s_platform.h"

#include "sdkconfig.h"

static const char *TAG = "I2S";

#define I2S_ENTER_CRITICAL_ISR(i2s_num)          portENTER_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL_ISR(i2s_num)           portEXIT_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_ENTER_CRITICAL(i2s_num)              portENTER_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL(i2s_num)               portEXIT_CRITICAL(&i2s_spinlock[i2s_num])

#define I2S_DMA_BUFFER_MAX_SIZE     4092

#if !SOC_GDMA_SUPPORTED
#define I2S_INTR_IN_SUC_EOF   BIT(9)
#define I2S_INTR_OUT_EOF      BIT(12)
#define I2S_INTR_IN_DSCR_ERR  BIT(13)
#define I2S_INTR_OUT_DSCR_ERR BIT(14)
#define I2S_INTR_MAX          (~0)
#endif

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
    xQueueHandle queue;
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
    int dma_buf_count;          /*!< DMA buffer count, number of buffer*/
    int dma_buf_len;            /*!< DMA buffer length, length of each buffer*/
    uint32_t last_buf_size;     /*!< DMA last buffer size */
    i2s_dma_t *tx;              /*!< DMA Tx buffer*/
    i2s_dma_t *rx;              /*!< DMA Rx buffer*/
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t rx_dma_chan;  /*!< I2S rx gDMA channel handle*/
    gdma_channel_handle_t tx_dma_chan;  /*!< I2S tx gDMA channel handle*/
#else
    i2s_isr_handle_t i2s_isr_handle; /*!< I2S Interrupt handle*/
#endif
    bool tx_desc_auto_clear;    /*!< I2S auto clear tx descriptor on underflow */
    bool use_apll;              /*!< I2S use APLL clock */
    int fixed_mclk;             /*!< I2S fixed MLCK clock */
    i2s_mclk_multiple_t mclk_multiple; /*!< The multiple of I2S master clock(MCLK) to sample rate */

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    i2s_hal_context_t hal;      /*!< I2S hal context*/
    i2s_channel_fmt_t   init_chan_fmt;  /*!< The initial channel format while installing, used for keep left or right mono when switch between mono and stereo */
    i2s_hal_config_t hal_cfg;   /*!< I2S hal configurations*/
} i2s_obj_t;

static i2s_obj_t *p_i2s[SOC_I2S_NUM];
static portMUX_TYPE i2s_platform_spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE i2s_spinlock[SOC_I2S_NUM] = {
    [0 ... SOC_I2S_NUM - 1] = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

#if SOC_I2S_SUPPORTS_ADC
static int _i2s_adc_unit = -1;
static int _i2s_adc_channel = -1;
#endif

/*
 * This block is an overview of APIs in i2s.c
 * Functions with [main] tag are summary functions that provide main i2s service
 * Functions with [helper] tag are helper functions that served for summary functions
 * Functions with [intr] tag are interrupt handling functions or interrupt callback functions
  -------------------------------------------------------------
                    I2S GPIO operation
  -------------------------------------------------------------
   - [helper]   gpio_matrix_out_check_and_set
   - [helper]   gpio_matrix_in_check_and_set
   - [helper]   i2s_check_set_mclk
   - [main]     i2s_set_pin
  -------------------------------------------------------------
                    I2S DMA operation
  -------------------------------------------------------------
   - [intr]     i2s_dma_rx_callback
   - [intr]     i2s_dma_tx_callback
   - [intr]     i2s_intr_handler_default
   - [helper]   i2s_dma_intr_init
   - [helper]   i2s_tx_reset
   - [helper]   i2s_rx_reset
   - [helper]   i2s_tx_start
   - [helper]   i2s_rx_start
   - [helper]   i2s_tx_stop
   - [helper]   i2s_rx_stop
  -------------------------------------------------------------
                   I2S buffer operation
  -------------------------------------------------------------
   - [helper]   i2s_get_buf_size
   - [helper]   i2s_delete_dma_buffer
   - [helper]   i2s_alloc_dma_buffer
   - [main]     i2s_realloc_dma_buffer
   - [main]     i2s_destroy_dma_object
   - [main]     i2s_create_dma_object
   - [main]     i2s_zero_dma_buffer
  -------------------------------------------------------------
                   I2S clock operation
  -------------------------------------------------------------
   - [helper]   i2s_config_source_clock
   - [helper]   i2s_calculate_adc_dac_clock
   - [helper]   i2s_calculate_pdm_tx_clock
   - [helper]   i2s_calculate_pdm_rx_clock
   - [helper]   i2s_calculate_common_clock
   - [main]     i2s_calculate_clock
  -------------------------------------------------------------
                   I2S configuration
  -------------------------------------------------------------
   - [helper]   i2s_get_max_channel_num
   - [helper]   i2s_get_active_channel_num
   - [helper]   i2s_set_dac_mode
   - [helper]   _i2s_adc_mode_recover
   - [main]     i2s_set_adc_mode
   - [main]     i2s_adc_enable
   - [main]     i2s_adc_disable
   - [helper]   i2s_set_sample_rates
   - [main]     i2s_pcm_config
   - [helper]   i2s_set_pdm_rx_down_sample
   - [helper]   i2s_set_pdm_tx_up_sample
   - [helper]   i2s_check_cfg_validity
   - [helper]   i2s_tx_set_clk_and_channel
   - [helper]   i2s_rx_set_clk_and_channel
   - [main]     i2s_get_clk
   - [main]     i2s_set_clk
  -------------------------------------------------------------
                   I2S driver operation
  -------------------------------------------------------------
   - [main]     i2s_start
   - [main]     i2s_stop
   - [helper]   i2s_driver_init
   - [helper]   i2s_dma_object_init
   - [main]     i2s_driver_install
   - [main]     i2s_driver_uninstall
   - [main]     i2s_write
   - [main]     i2s_write_expand
   - [main]     i2s_read
  -------------------------------------------------------------*/

/*-------------------------------------------------------------
                    I2S GPIO operation
  -------------------------------------------------------------*/
/**
 * @brief   I2S GPIO matrix set ouput
 *
 * @param   gpio          GPIO number
 * @param   singal_idx    GPIO singnal ID, refer to 'gpio_sig_map.h'
 * @param   out_inv       Output invert enable
 * @param   oen_inv       Output eanble control invert enable
 */
static void gpio_matrix_out_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    //if pin = -1, do not need to configure
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio, signal_idx, out_inv, oen_inv);
    }
}

/**
 * @brief   I2S GPIO matrix set input
 *
 * @param   gpio          GPIO number
 * @param   singal_idx    GPIO singnal ID, refer to 'gpio_sig_map.h'
 * @param   out_inv       Output invert enable
 * @param   oen_inv       Output eanble control invert enable
 */
static void gpio_matrix_in_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool inv)
{
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        /* Set direction, for some GPIOs, the input function are not enabled as default */
        gpio_set_direction(gpio, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(gpio, signal_idx, inv);
    }
}

/**
 * @brief   I2S set GPIO for mclk
 *
 * @param   i2s_num     I2S device number
 * @param   gpio_num    GPIO number for mclk
 * @return
 *      - ESP_OK                Check or set success
 *      - ESP_ERR_INVALID_ARG   GPIO is not available
 */
static esp_err_t i2s_check_set_mclk(i2s_port_t i2s_num, gpio_num_t gpio_num)
{
    if (gpio_num == -1) {
        return ESP_OK;
    }
#if CONFIG_IDF_TARGET_ESP32
    ESP_RETURN_ON_FALSE((gpio_num == GPIO_NUM_0 || gpio_num == GPIO_NUM_1 || gpio_num == GPIO_NUM_3),
                        ESP_ERR_INVALID_ARG, TAG,
                        "ESP32 only support to set GPIO0/GPIO1/GPIO3 as mclk signal, error GPIO number:%d", gpio_num);
    bool is_i2s0 = i2s_num == I2S_NUM_0;
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
    gpio_matrix_out_check_and_set(gpio_num, i2s_periph_signal[i2s_num].mck_out_sig, 0, 0);
#endif
    ESP_LOGI(TAG, "I2S%d, MCLK output by GPIO%d", i2s_num, gpio_num);
    return ESP_OK;
}

/**
 * @brief   Set gpio pins for I2S
 *
 * @param   i2s_num     I2S device number
 * @param   pin         Pin configuration
 * @return
 *      - ESP_OK                Set pin success
 *      - ESP_ERR_INVALID_ARG   GPIO is not available
 */
esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
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

    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_SLAVE) {
        /* For "tx + rx + slave" or "rx + slave" mode, we should select RX signal index for ws and bck */
        if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
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
        if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].m_tx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].m_tx_bck_sig, 0, 0);
            /* For "rx + master" mode, we should select RX signal index for ws and bck */
        } else {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].m_rx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].m_rx_bck_sig, 0, 0);
        }
    }

    /* Set data input/ouput GPIO */
    gpio_matrix_out_check_and_set(pin->data_out_num, i2s_periph_signal[i2s_num].data_out_sig, 0, 0);
    gpio_matrix_in_check_and_set(pin->data_in_num, i2s_periph_signal[i2s_num].data_in_sig, 0);
    return ESP_OK;
}

/*-------------------------------------------------------------
                    I2S DMA operation
  -------------------------------------------------------------*/
#if SOC_GDMA_SUPPORTED
/**
 * @brief   GDMA rx callback function
 * @note    This function is called by GDMA default ISR handler
 *
 * @param   dma_chan    GDMA channel handler
 * @param   event_data  GDMA rx event data
 * @param   user_data   GDMA user data
 * @return
 *      - true  need yield
 *      - false no need
 */
static bool IRAM_ATTR i2s_dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) user_data;
    portBASE_TYPE high_priority_task_awoken = 0;
    BaseType_t ret = 0;
    int dummy;
    i2s_event_t i2s_event;
    uint32_t finish_desc;

    if (p_i2s->rx) {
        finish_desc = event_data->rx_eof_desc_addr;
        if (xQueueIsQueueFullFromISR(p_i2s->rx->queue)) {
            xQueueReceiveFromISR(p_i2s->rx->queue, &dummy, &high_priority_task_awoken);
        }
        ret = xQueueSendFromISR(p_i2s->rx->queue, &(((lldesc_t *)finish_desc)->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = (ret == pdPASS) ? I2S_EVENT_RX_DONE : I2S_EVENT_RX_Q_OVF;
            if (p_i2s->i2s_queue && xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }
    return high_priority_task_awoken;
}

/**
 * @brief   GDMA tx callback function
 * @note    This function is called by GDMA default ISR handler
 *
 * @param   dma_chan    GDMA channel handler
 * @param   event_data  GDMA tx event data
 * @param   user_data   GDMA user data
 * @return
 *      - whether need yield
 */
static bool IRAM_ATTR i2s_dma_tx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) user_data;
    portBASE_TYPE high_priority_task_awoken = 0;
    BaseType_t ret;
    int dummy;
    i2s_event_t i2s_event;
    uint32_t finish_desc;
    if (p_i2s->tx) {
        finish_desc = event_data->tx_eof_desc_addr;
        if (xQueueIsQueueFullFromISR(p_i2s->tx->queue)) {
            xQueueReceiveFromISR(p_i2s->tx->queue, &dummy, &high_priority_task_awoken);
            if (p_i2s->tx_desc_auto_clear) {
                memset((void *) dummy, 0, p_i2s->tx->buf_size);
            }
        }
        ret = xQueueSendFromISR(p_i2s->tx->queue, &(((lldesc_t *)finish_desc)->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = (ret == pdPASS) ? I2S_EVENT_TX_DONE : I2S_EVENT_TX_Q_OVF;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }
    return high_priority_task_awoken;
}

#else

/**
 * @brief   I2S defalut interrupt handler
 * @note    This function is triggered by I2S dedicated DMA interrupt
 *
 * @param   arg      Argument transport to ISR, here is the pointer to I2S object
 */
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
    portBASE_TYPE high_priority_task_awoken = 0;
    uint32_t  finish_desc = 0;
    if ((status & I2S_INTR_OUT_DSCR_ERR) || (status & I2S_INTR_IN_DSCR_ERR)) {
        ESP_EARLY_LOGE(TAG, "dma error, interrupt status: 0x%08x", status);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_DMA_ERROR;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }

    if ((status & I2S_INTR_OUT_EOF) && p_i2s->tx) {
        i2s_hal_get_out_eof_des_addr(&(p_i2s->hal), &finish_desc);
        // All buffers are empty. This means we have an underflow on our hands.
        if (xQueueIsQueueFullFromISR(p_i2s->tx->queue)) {
            xQueueReceiveFromISR(p_i2s->tx->queue, &dummy, &high_priority_task_awoken);
            // See if tx descriptor needs to be auto cleared:
            // This will avoid any kind of noise that may get introduced due to transmission
            // of previous data from tx descriptor on I2S line.
            if (p_i2s->tx_desc_auto_clear == true) {
                memset((void *) dummy, 0, p_i2s->tx->buf_size);
            }
        }
        xQueueSendFromISR(p_i2s->tx->queue, &(((lldesc_t *)finish_desc)->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_TX_DONE;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }

    if ((status & I2S_INTR_IN_SUC_EOF) && p_i2s->rx) {
        // All buffers are full. This means we have an overflow.
        i2s_hal_get_in_eof_des_addr(&(p_i2s->hal), &finish_desc);
        if (xQueueIsQueueFullFromISR(p_i2s->rx->queue)) {
            xQueueReceiveFromISR(p_i2s->rx->queue, &dummy, &high_priority_task_awoken);
        }
        xQueueSendFromISR(p_i2s->rx->queue, &(((lldesc_t *)finish_desc)->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_RX_DONE;
            if (p_i2s->i2s_queue && xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }
    i2s_hal_clear_intr_status(&(p_i2s->hal), status);

    if (high_priority_task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
#endif


/**
 * @brief   I2S DMA interrupt initialization
 * @note    I2S will use GDMA if chip supports, and the interrupt is triggered by GDMA.
 *
 * @param   i2s_num     I2S device number
 * @return
 *      - ESP_OK                    I2S DMA interrupt initialize success
 *      - ESP_ERR_NOT_FOUND         GDMA channel not found
 *      - ESP_ERR_INVALID_ARG       Invalid arguments
 *      - ESP_ERR_INVALID_STATE     GDMA state error
 */
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
    if ( p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_TX;
        /* Register a new GDMA tx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &p_i2s[i2s_num]->tx_dma_chan), TAG, "Register tx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(p_i2s[i2s_num]->tx_dma_chan, trig), TAG, "Connect tx dma channel error");
        gdma_tx_event_callbacks_t cb = {.on_trans_eof = i2s_dma_tx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_tx_event_callbacks(p_i2s[i2s_num]->tx_dma_chan, &cb, p_i2s[i2s_num]);
    }
    if ( p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
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

/**
 * @brief   I2S tx reset
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_tx_reset(i2s_port_t i2s_num)
{
    p_i2s[i2s_num]->tx->curr_ptr = NULL;
    p_i2s[i2s_num]->tx->rw_pos = 0;
    i2s_hal_reset_tx(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->tx_dma_chan);
#else
    i2s_hal_reset_txdma(&(p_i2s[i2s_num]->hal));
#endif
    i2s_hal_reset_tx_fifo(&(p_i2s[i2s_num]->hal));
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
    i2s_hal_reset_rx(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->rx_dma_chan);
#else
    i2s_hal_reset_rxdma(&(p_i2s[i2s_num]->hal));
#endif
    i2s_hal_reset_rx_fifo(&(p_i2s[i2s_num]->hal));
}

/**
 * @brief   I2S tx start
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_tx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->tx_dma_chan, (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#else
    i2s_hal_enable_tx_dma(&(p_i2s[i2s_num]->hal));
    i2s_hal_enable_tx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_start_tx_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#endif
    i2s_hal_start_tx(&(p_i2s[i2s_num]->hal));
}

/**
 * @brief   I2S rx start
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_rx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->rx_dma_chan, (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#else
    i2s_hal_enable_rx_dma(&(p_i2s[i2s_num]->hal));
    i2s_hal_enable_rx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_start_rx_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#endif
    i2s_hal_start_rx(&(p_i2s[i2s_num]->hal));
}

/**
 * @brief   I2S tx stop
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_tx_stop(i2s_port_t i2s_num)
{
    i2s_hal_stop_tx(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->tx_dma_chan);
#else
    i2s_hal_stop_tx_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_tx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_tx_dma(&(p_i2s[i2s_num]->hal));
#endif
}

/**
 * @brief   I2S rx stop
 *
 * @param   i2s_num     I2S device number
 */
static void i2s_rx_stop(i2s_port_t i2s_num)
{
    i2s_hal_stop_rx(&(p_i2s[i2s_num]->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->rx_dma_chan);
#else
    i2s_hal_stop_rx_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_rx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_rx_dma(&(p_i2s[i2s_num]->hal));
#endif
}

/*-------------------------------------------------------------
                   I2S buffer operation
  -------------------------------------------------------------*/
/**
 * @brief I2S get DMA buffer size
 *
 * @param   i2s_num     I2S device number
 * @return
 *      - DMA buffer size
 */
static inline uint32_t i2s_get_buf_size(i2s_port_t i2s_num)
{
    /* Calculate bytes per sample, align to 16 bit */
    uint32_t bytes_per_sample = ((p_i2s[i2s_num]->hal_cfg.sample_bits + 15) / 16) * 2;
    /* The DMA buffer limitation is 4092 bytes */
    uint32_t bytes_per_frame = bytes_per_sample * p_i2s[i2s_num]->hal_cfg.active_chan;
    p_i2s[i2s_num]->dma_buf_len = (p_i2s[i2s_num]->dma_buf_len * bytes_per_frame > I2S_DMA_BUFFER_MAX_SIZE) ?
                                  I2S_DMA_BUFFER_MAX_SIZE / bytes_per_frame : p_i2s[i2s_num]->dma_buf_len;
    return p_i2s[i2s_num]->dma_buf_len * bytes_per_frame;
}

/**
 * @brief   Delete DMA buffer and descriptor
 *
 * @param   i2s_num     I2S device number
 * @param   dma_obj     DMA object
 * @return
 *      - ESP_OK                DMA buffer delete success
 *      - ESP_ERR_INVALID_ARG   dma_obj is NULL
 */
static esp_err_t i2s_delete_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    ESP_RETURN_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, TAG, "I2S DMA object can't be NULL");
    /* Loop to destroy every descriptor and buffer */
    for (int cnt = 0; cnt < p_i2s[i2s_num]->dma_buf_count; cnt++) {
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

/**
 * @brief   Allocate memory for DMA buffer and descriptor
 *
 * @param   i2s_num     I2S device number
 * @param   dma_obj     DMA object
 * @return
 *      - ESP_OK            Allocate success
 *      - ESP_ERR_NO_MEM    No memory for DMA buffer
 */
static esp_err_t i2s_alloc_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, err, TAG, "I2S DMA object can't be NULL");

    uint32_t buf_cnt = p_i2s[i2s_num]->dma_buf_count;
    for (int cnt = 0; cnt < buf_cnt; cnt++) {
        /* Allocate DMA buffer */
        dma_obj->buf[cnt] = (char *) heap_caps_calloc(dma_obj->buf_size, sizeof(char), MALLOC_CAP_DMA);
        ESP_GOTO_ON_FALSE(dma_obj->buf[cnt], ESP_ERR_NO_MEM, err, TAG, "Error malloc dma buffer");
        /* Initialize DMA buffer to 0 */
        memset(dma_obj->buf[cnt], 0, dma_obj->buf_size);
        ESP_LOGD(TAG, "Addr[%d] = %d", cnt, (int)dma_obj->buf[cnt]);

        /* Allocate DMA descpriptor */
        dma_obj->desc[cnt] = (lldesc_t *) heap_caps_calloc(1, sizeof(lldesc_t), MALLOC_CAP_DMA);
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
    }
    ESP_LOGI(TAG, "DMA Malloc info, datalen=blocksize=%d, dma_buf_count=%d", dma_obj->buf_size, buf_cnt);
    return ESP_OK;
err:
    /* Delete DMA buffer if failed to allocate memory */
    i2s_delete_dma_buffer(i2s_num, dma_obj);
    return ret;
}

/**
 * @brief   Realloc I2S dma buffer
 *
 * @param   i2s_num     I2S device number
 * @param   dma_obj     DMA object
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_NO_MEM      No memory for I2S tx dma buffer
 */
static esp_err_t i2s_realloc_dma_buffer(i2s_port_t i2s_num, i2s_dma_t *dma_obj)
{
    ESP_RETURN_ON_FALSE(dma_obj, ESP_ERR_INVALID_ARG, TAG, "I2S DMA object can't be NULL");

    /* Destroy old dma descriptor and buffer */
    i2s_delete_dma_buffer(i2s_num, dma_obj);
    /* Alloc new dma descriptor and buffer */
    ESP_RETURN_ON_ERROR(i2s_alloc_dma_buffer(i2s_num, dma_obj), TAG, "Failed to allocate dma buffer");

    return ESP_OK;
}

/**
 * @brief   I2S destroy the whole DMA object
 *
 * @param   i2s_num     I2S device number
 * @param   dma         Secondary pointer to the DMA object
 * @return
 *      - ESP_OK                I2S DMA buffer has been destroyed successfully
 *      - ESP_ERR_INVALID_ARG   I2S driver has not installed yet
 */
static esp_err_t i2s_destroy_dma_object(i2s_port_t i2s_num, i2s_dma_t **dma)
{
    /* Check if DMA truely need destroy */
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
    ESP_LOGI(TAG, "DMA queue destroyed");
    return ESP_OK;
}

/**
 * @brief   Create I2S DMA object
 * @note    This function only create I2S DMA object but will not allocate memory
 *          for DMA descriptor and buffer, call 'i2s_alloc_dma_buffer' additionally to
 *          allocate DMA buffer
 *
 * @param   i2s_num         I2S device number
 * @param   dma             The secondary pointer of DMA object
 * @return
 *      - ESP_OK                The pointer of DMA object
 *      - ESP_ERR_INVALID_ARG   NULL pointer error or DMA object has been created
 *      - ESP_ERR_NO_MEM        No memory for new DMA object
 */
static esp_err_t i2s_create_dma_object(i2s_port_t i2s_num, i2s_dma_t **dma)
{
    ESP_RETURN_ON_FALSE(dma, ESP_ERR_INVALID_ARG, TAG, "DMA object secondary pointer is NULL");
    ESP_RETURN_ON_FALSE((*dma == NULL), ESP_ERR_INVALID_ARG, TAG, "DMA object has been created");
    uint32_t buf_cnt = p_i2s[i2s_num]->dma_buf_count;
    /* Allocate new DMA structure */
    *dma = (i2s_dma_t *) malloc(sizeof(i2s_dma_t));
    ESP_RETURN_ON_FALSE(*dma, ESP_ERR_NO_MEM, TAG, "DMA object allocate failed");
    /* Allocate DMA buffer poiter */
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

/**
 * @brief   Zero the contents of the TX DMA buffer.
 * @note    Pushes zero-byte samples into the TX DMA buffer, until it is full.
 *
 * @param   i2s_num         I2S device number
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");

    /* Clear I2S RX DMA buffer */
    if (p_i2s[i2s_num]->rx && p_i2s[i2s_num]->rx->buf != NULL && p_i2s[i2s_num]->rx->buf_size != 0) {
        for (int i = 0; i < p_i2s[i2s_num]->dma_buf_count; i++) {
            memset(p_i2s[i2s_num]->rx->buf[i], 0, p_i2s[i2s_num]->rx->buf_size);
        }
    }

    /* Clear I2S TX DMA buffer */
    if (p_i2s[i2s_num]->tx && p_i2s[i2s_num]->tx->buf != NULL && p_i2s[i2s_num]->tx->buf_size != 0) {
        /* Finish to write all tx data */
        int bytes_left = (p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos) % 4;
        if (bytes_left) {
            size_t zero_bytes = 0, bytes_written;
            i2s_write(i2s_num, (void *)&zero_bytes, bytes_left, &bytes_written, portMAX_DELAY);
        }
        for (int i = 0; i < p_i2s[i2s_num]->dma_buf_count; i++) {
            memset(p_i2s[i2s_num]->tx->buf[i], 0, p_i2s[i2s_num]->tx->buf_size);
        }
    }
    return ESP_OK;
}

/*-------------------------------------------------------------
                   I2S clock operation
  -------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_APLL
static esp_err_t i2s_set_apll_freq(uint32_t expt_freq, uint32_t *real_freq)
{
    uint32_t rtc_xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    ESP_RETURN_ON_FALSE(rtc_xtal_freq, ESP_ERR_INVALID_STATE, TAG, "Get xtal clock frequency failed, it has not been set yet");

    /* Reference formula: apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) / ((o_div + 2) * 2)
     *                                ----------------------------------------------   -----------------
     *                                     350 MHz <= Numerator <= 500 MHz                Denominator
     */
    int o_div = 0; // range: 0~31
    int sdm0 = 0;  // range: 0~255
    int sdm1 = 0;  // range: 0~255
    int sdm2 = 0;  // range: 0~63
    /* Firstly try to satisfy the condition that the operation frequency of numerator should be greater than 350 MHz,
     * i.e. xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) >= 350 MHz, '+1' in the following code is to get the ceil value.
     * With this condition, as we know the 'o_div' can't be greater than 31, then we can calculate the APLL minimum support frequency is
     * 350 MHz / ((31 + 2) * 2) = 5303031 Hz (for ceil) */
    o_div = (int)(SOC_APLL_MULTIPLIER_OUT_MIN_HZ / (float)(expt_freq * 2) + 1) - 2;
    ESP_RETURN_ON_FALSE(o_div < 32, ESP_ERR_INVALID_ARG, TAG, "Expected frequency is too small");
    if (o_div < 0) {
        /* Try to satisfy the condition that the operation frequency of numerator should be smaller than 500 MHz,
         * i.e. xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) <= 500 MHz, we need to get the floor value in the following code.
         * With this condition, as we know the 'o_div' can't be smaller than 0, then we can calculate the APLL maximum support frequency is
         * 500 MHz / ((0 + 2) * 2) = 125000000 Hz */
        o_div = (int)(SOC_APLL_MULTIPLIER_OUT_MAX_HZ / (float)(expt_freq * 2)) - 2;
        ESP_RETURN_ON_FALSE(o_div >= 0, ESP_ERR_INVALID_ARG, TAG, "Expected frequency is too small");
    }
    // sdm2 = (int)(((o_div + 2) * 2) * apll_freq / xtal_freq) - 4
    sdm2 = (int)(((o_div + 2) * 2 * expt_freq) / (rtc_xtal_freq * 1000000)) - 4;
    // numrator = (((o_div + 2) * 2) * apll_freq / xtal_freq) - 4 - sdm2
    float numrator = (((o_div + 2) * 2 * expt_freq) / ((float)rtc_xtal_freq * 1000000)) - 4 - sdm2;
    // If numrator is bigger than 255/256 + 255/65536 + (1/65536)/2 = 1 - (1 / 65536)/2, carry bit to sdm2
    if (numrator > 1.0 - (1.0 / 65536.0) / 2.0) {
        sdm2++;
    }
    // If numrator is smaller than (1/65536)/2, keep sdm0 = sdm1 = 0, otherwise calculate sdm0 and sdm1
    else if (numrator > (1.0 / 65536.0) / 2.0) {
        // Get the closest sdm1
        sdm1 = (int)(numrator * 65536.0 + 0.5) / 256;
        // Get the closest sdm0
        sdm0 = (int)(numrator * 65536.0 + 0.5) % 256;
    }
    rtc_clk_apll_enable(true, sdm0, sdm1, sdm2, o_div);
    *real_freq = (uint32_t)(rtc_xtal_freq * 1000000 * (4 + sdm2 + (float)sdm1/256.0 + (float)sdm0/65536.0) / (((float)o_div + 2) * 2));

    return ESP_OK;
}
#endif
/**
 * @brief   Config I2S source clock and get its frequency
 *
 * @param   i2s_num         I2S device number
 * @param   use_apll        Whether use apll, only take effect when chip supports
 * @param   mclk            module clock
 *
 * @return
 *      - 0                 use I2S_CLK_APLL as clock source, no I2S system clock to set
 *      - I2S_LL_BASE_CLK   use I2S_CLK_D2CLK as clock source, return APB clock frequency
 */
static uint32_t i2s_config_source_clock(i2s_port_t i2s_num, bool use_apll, uint32_t mclk)
{
#if SOC_I2S_SUPPORTS_APLL
    if (use_apll) {
        /* Calculate the expected APLL  */
        int div = (int)((SOC_APLL_MIN_HZ / mclk) + 1);
        /* apll_freq = mclk * div
         * when div = 1, hardware will still divide 2
         * when div = 0, the final mclk will be unpredictable
         * So the div here should be at least 2 */
        div = div < 2 ? 2 : div;
        uint32_t expt_freq = mclk * div;
        /* Set APLL coefficients to the given frequency */
        uint32_t real_freq = 0;
        esp_err_t ret = i2s_set_apll_freq(expt_freq, &real_freq);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "set APLL coefficients failed");
            return 0;
        }
        ESP_LOGI(TAG, "APLL expected frequency is %d Hz, real frequency is %d Hz", expt_freq, real_freq);
        /* Set I2S_APLL as I2S module clock source */
        i2s_hal_set_clock_src(&(p_i2s[i2s_num]->hal), I2S_CLK_APLL);
        /* In APLL mode, there is no sclk but only mclk, so return 0 here to indicate APLL mode */
        return real_freq;
    }
    /* Set I2S_D2CLK (160M) as default I2S module clock source */
    i2s_hal_set_clock_src(&(p_i2s[i2s_num]->hal), I2S_CLK_D2CLK);
    return I2S_LL_BASE_CLK;
#else
    if (use_apll) {
        ESP_LOGW(TAG, "APLL not supported on current chip, use I2S_CLK_D2CLK as default clock source");
    }
    /* Set I2S_D2CLK (160M) as I2S module clock source */
    i2s_hal_set_clock_src(&(p_i2s[i2s_num]->hal), I2S_CLK_D2CLK);
    return I2S_LL_BASE_CLK;
#endif
}

#if SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC
/**
 * @brief   I2S calculate clock for built-in ADC/DAC mode
 *
 * @param   i2s_num         I2S device number
 * @param   clk_cfg         Struct to restore clock confiuration
 * @return
 *      - ESP_OK                Get clock success
 *      - ESP_ERR_INVALID_ARG   Invalid args
 */
static esp_err_t i2s_calculate_adc_dac_clock(int i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    ESP_RETURN_ON_FALSE(clk_cfg, ESP_ERR_INVALID_ARG, TAG, "input clk_cfg is NULL");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->hal_cfg.mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN), ESP_ERR_INVALID_ARG, TAG, "current mode is not built-in ADC/DAC");

    /* Set I2S bit clock */
    clk_cfg->bclk = p_i2s[i2s_num]->hal_cfg.sample_rate * I2S_LL_AD_BCK_FACTOR;
    /* Set I2S bit clock default division */
    clk_cfg->bclk_div = p_i2s[i2s_num]->hal_cfg.chan_bits;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate * multiple */
    clk_cfg->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                    p_i2s[i2s_num]->fixed_mclk : clk_cfg->bclk * clk_cfg->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_cfg->bclk_div = clk_cfg->mclk / clk_cfg->bclk;
    /* Get I2S system clock by config source clock */
    clk_cfg->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_cfg->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_cfg->mclk_div = clk_cfg->sclk / clk_cfg->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_cfg->mclk <= clk_cfg->sclk, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");

    return ESP_OK;
}
#endif // SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief   I2S calculate clock for PDM tx mode
 *
 * @param   i2s_num         I2S device number
 * @param   clk_cfg         Struct to restore clock confiuration
 * @return
 *      - ESP_OK                Get clock success
 *      - ESP_ERR_INVALID_ARG   Invalid args
 */
static esp_err_t i2s_calculate_pdm_tx_clock(int i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    ESP_RETURN_ON_FALSE(clk_cfg, ESP_ERR_INVALID_ARG, TAG, "input clk_cfg is NULL");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_PDM, ESP_ERR_INVALID_ARG, TAG, "current mode is not PDM");

    int fp = i2s_hal_get_tx_pdm_fp(&(p_i2s[i2s_num]->hal));
    int fs = i2s_hal_get_tx_pdm_fs(&(p_i2s[i2s_num]->hal));
    /* Set I2S bit clock */
    clk_cfg->bclk = p_i2s[i2s_num]->hal_cfg.sample_rate * I2S_LL_PDM_BCK_FACTOR * fp / fs;
    /* Set I2S bit clock default division */
    clk_cfg->bclk_div = 8;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate * multiple */
    clk_cfg->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                    p_i2s[i2s_num]->fixed_mclk : clk_cfg->bclk * clk_cfg->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_cfg->bclk_div = clk_cfg->mclk / clk_cfg->bclk;
    /* Get I2S system clock by config source clock */
    clk_cfg->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_cfg->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_cfg->mclk_div = clk_cfg->sclk / clk_cfg->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_cfg->mclk <= clk_cfg->sclk, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");

    return ESP_OK;
}
#endif  // SOC_I2S_SUPPORTS_PDM_TX

#if SOC_I2S_SUPPORTS_PDM_RX
/**
 * @brief   I2S calculate clock for PDM rx mode
 *
 * @param   i2s_num         I2S device number
 * @param   clk_cfg         Struct to restore clock confiuration
 * @return
 *      - ESP_OK                Get clock success
 *      - ESP_ERR_INVALID_ARG   Invalid args
 */
static esp_err_t i2s_calculate_pdm_rx_clock(int i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    ESP_RETURN_ON_FALSE(clk_cfg, ESP_ERR_INVALID_ARG, TAG, "input clk_cfg is NULL");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_PDM, ESP_ERR_INVALID_ARG, TAG, "current mode is not PDM");

    i2s_pdm_dsr_t dsr;
    i2s_hal_get_rx_pdm_dsr(&(p_i2s[i2s_num]->hal), &dsr);
    /* Set I2S bit clock */
    clk_cfg->bclk = p_i2s[i2s_num]->hal_cfg.sample_rate * I2S_LL_PDM_BCK_FACTOR * (dsr == I2S_PDM_DSR_16S ? 2 : 1);
    /* Set I2S bit clock default division */
    clk_cfg->bclk_div = 8;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate * multiple */
    clk_cfg->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                    p_i2s[i2s_num]->fixed_mclk : clk_cfg->bclk * clk_cfg->bclk_div;
    /* Calculate bclk_div = mclk / bclk */
    clk_cfg->bclk_div = clk_cfg->mclk / clk_cfg->bclk;
    /* Get I2S system clock by config source clock */
    clk_cfg->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_cfg->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_cfg->mclk_div = clk_cfg->sclk / clk_cfg->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_cfg->mclk <= clk_cfg->sclk, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");

    return ESP_OK;
}
#endif // SOC_I2S_SUPPORTS_PDM_RX

/**
 * @brief   I2S calculate clock for common mode (philip, MSB, PCM)
 *
 * @param   i2s_num         I2S device number
 * @param   clk_cfg         Struct to restore clock confiuration
 * @return
 *      - ESP_OK                Get clock success
 *      - ESP_ERR_INVALID_ARG   Invalid args
 */
static esp_err_t i2s_calculate_common_clock(int i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    ESP_RETURN_ON_FALSE(clk_cfg, ESP_ERR_INVALID_ARG, TAG, "input clk_cfg is NULL");

    uint32_t rate     = p_i2s[i2s_num]->hal_cfg.sample_rate;
    uint32_t chan_num = p_i2s[i2s_num]->hal_cfg.total_chan < 2 ? 2 : p_i2s[i2s_num]->hal_cfg.total_chan;
    uint32_t chan_bit = p_i2s[i2s_num]->hal_cfg.chan_bits;
    uint32_t multi;
    /* Calculate multiple */
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_MASTER || p_i2s[i2s_num]->use_apll) {
        multi = p_i2s[i2s_num]->mclk_multiple ? p_i2s[i2s_num]->mclk_multiple : I2S_MCLK_MULTIPLE_256;
    } else {
        /* Only need to set the multiple of mclk to sample rate for MASTER mode,
         * because BCK and WS clock are provided by the external codec in SLAVE mode.
         * The multiple should be big enough to get a high module clock which could detect the edges of externel clock more accurately,
         * otherwise the data we receive or send would get a large latency and go wrong due to the slow module clock.
         * But on ESP32 and ESP32S2, due to the different clock work mode in hardware,
         * their multiple should be set to an appropriate range according to the sample bits,
         * and this particular multiple finally aims at guaranteeing the bclk_div not smaller than 8,
         * if not, the I2S may can't send data or send wrong data.
         * Here use 'SOC_I2S_SUPPORTS_TDM' to differentialize other chips with ESP32 and ESP32S2.
         */
#if SOC_I2S_SUPPORTS_TDM
        multi = I2S_LL_BASE_CLK / rate;
#else
        multi =  64 * chan_bit;
#endif
    }
    /* Set I2S bit clock */
    clk_cfg->bclk = rate * chan_num * chan_bit;
    /* If fixed_mclk and use_apll are set, use fixed_mclk as mclk frequency, otherwise calculate by mclk = sample_rate * multiple */
    clk_cfg->mclk = (p_i2s[i2s_num]->use_apll && p_i2s[i2s_num]->fixed_mclk) ?
                    p_i2s[i2s_num]->fixed_mclk : (rate * multi);
    /* Calculate bclk_div = mclk / bclk */
    clk_cfg->bclk_div = clk_cfg->mclk / clk_cfg->bclk;
    /* Get I2S system clock by config source clock */
    clk_cfg->sclk = i2s_config_source_clock(i2s_num, p_i2s[i2s_num]->use_apll, clk_cfg->mclk);
    /* Get I2S master clock rough division, later will calculate the fine division parameters in HAL */
    clk_cfg->mclk_div = clk_cfg->sclk / clk_cfg->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_cfg->mclk <= clk_cfg->sclk, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");

    return ESP_OK;
}

/**
 * @brief   I2S calculate clocks according to the selected I2S mode
 *
 * @param   i2s_num     I2S device number
 * @param   clk_cfg         Struct to restore clock confiuration
 * @return
 *      - ESP_OK                Claculate clock success
 *      - ESP_ERR_INVALID_ARG   Invalid args
 */
static esp_err_t i2s_calculate_clock(i2s_port_t i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    /* Calculate clock for ADC mode */
#if SOC_I2S_SUPPORTS_ADC
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_ADC_BUILT_IN) {
        ESP_RETURN_ON_ERROR(i2s_calculate_adc_dac_clock(i2s_num, clk_cfg), TAG, "ADC clock calculate failed");
        return ESP_OK;
    }
#endif // SOC_I2S_SUPPORTS_ADC
    /* Calculate clock for DAC mode */
#if SOC_I2S_SUPPORTS_DAC
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_DAC_BUILT_IN) {
        ESP_RETURN_ON_ERROR(i2s_calculate_adc_dac_clock(i2s_num, clk_cfg), TAG, "DAC clock calculate failed");
        return ESP_OK;
    }
#endif // SOC_I2S_SUPPORTS_DAC

    /* Calculate clock for PDM mode */
#if SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_PDM) {
#if SOC_I2S_SUPPORTS_PDM_TX
        if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
            ESP_RETURN_ON_ERROR(i2s_calculate_pdm_tx_clock(i2s_num, clk_cfg), TAG, "PDM TX clock calculate failed");
        }
#endif // SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_SUPPORTS_PDM_RX
        if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
            ESP_RETURN_ON_ERROR(i2s_calculate_pdm_rx_clock(i2s_num, clk_cfg), TAG, "PDM RX clock calculate failed");
        }
#endif // SOC_I2S_SUPPORTS_PDM_RX
        return ESP_OK;
    }
#endif // SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX

    /* Calculate clock for common mode */
    ESP_RETURN_ON_ERROR(i2s_calculate_common_clock(i2s_num, clk_cfg), TAG, "Common clock calculate failed");
    return ESP_OK;
}

/*-------------------------------------------------------------
                   I2S configuration
  -------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_TDM
/**
 * @brief Get max actived channel number
 *
 * @param chan_mask I2S channel mask that indicates which channels are actived
 * @return
 *      - Max actived channel number
 */
static uint32_t i2s_get_max_channel_num(i2s_channel_t chan_mask)
{
    uint32_t max_chan = 0;
    uint32_t channel = chan_mask >> 16;
    for (int i = 0; channel && i < 16; i++, channel >>= 1) {
        if (channel & 0x01) {
            max_chan = i + 1;
        }
    }
    /* Can't be smaller than 2 */
    return max_chan < 2 ? 2 : max_chan;
}
#endif

/**
 * @brief Get active channel number according to channel format
 * @note  In 'I2S_CHANNEL_FMT_MULTIPLE' format, this function will check
 *        'total_chan' and fix it if it is not correct.
 *
 * @param hal_cfg   [input/output] I2S hal configuration structer
 * @return
 *      - Active channel number
 */
static uint32_t i2s_get_active_channel_num(const i2s_hal_config_t *hal_cfg)
{
    switch (hal_cfg->chan_fmt) {
    case I2S_CHANNEL_FMT_RIGHT_LEFT: //fall through
    case I2S_CHANNEL_FMT_ALL_RIGHT:  //fall through
    case I2S_CHANNEL_FMT_ALL_LEFT:
        return 2;
    case I2S_CHANNEL_FMT_ONLY_RIGHT: //fall through
    case I2S_CHANNEL_FMT_ONLY_LEFT:
        return 1;
#if SOC_I2S_SUPPORTS_TDM
    case I2S_CHANNEL_FMT_MULTIPLE: {
        uint32_t num = 0;
        uint32_t chan_mask = hal_cfg->chan_mask >> 16;
        for (int i = 0; chan_mask && i < 16; i++, chan_mask >>= 1) {
            if (chan_mask & 0x01) {
                num++;
            }
        }
        return num;
    }
#endif
    default:
        return 0;
    }
}

#if SOC_I2S_SUPPORTS_DAC
/**
 * @brief   I2S set built-in DAC mode
 *
 * @param   dac_mode    DAC mode
 * @return
 *      - ESP_OK                Set DAC success
 *      - ESP_ERR_INVALID_ARG   Wrong DAC mode
 */
esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode)
{
    ESP_RETURN_ON_FALSE((dac_mode < I2S_DAC_CHANNEL_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s dac mode error");
    if (dac_mode == I2S_DAC_CHANNEL_DISABLE) {
        dac_output_disable(DAC_CHANNEL_1);
        dac_output_disable(DAC_CHANNEL_2);
        dac_i2s_disable();
    } else {
        dac_i2s_enable();
    }

    if (dac_mode & I2S_DAC_CHANNEL_RIGHT_EN) {
        //DAC1, right channel
        dac_output_enable(DAC_CHANNEL_1);
    }
    if (dac_mode & I2S_DAC_CHANNEL_LEFT_EN) {
        //DAC2, left channel
        dac_output_enable(DAC_CHANNEL_2);
    }
    return ESP_OK;
}
#endif // SOC_I2S_SUPPORTS_DAC

#if SOC_I2S_SUPPORTS_ADC
/**
 * @brief   ADC mode recover
 *
 * @return
 *      - ESP_OK ADC            Recover success
 *      - ESP_ERR_INVALID_ARG   ADC not initialized yet
 */
static esp_err_t _i2s_adc_mode_recover(void)
{
    ESP_RETURN_ON_FALSE(((_i2s_adc_unit != -1) && (_i2s_adc_channel != -1)), ESP_ERR_INVALID_ARG, TAG, "i2s ADC recover error, not initialized...");
    return adc_i2s_mode_init(_i2s_adc_unit, _i2s_adc_channel);
}

/**
 * @brief   I2S set adc mode
 *
 * @param   adc_unit    ADC unit number
 * @param   adc_channel ADC channel
 * @return
 *      - ESP_OK ADC            Recover success
 *      - ESP_ERR_INVALID_ARG   ADC not initialized yet
 */
esp_err_t i2s_set_adc_mode(adc_unit_t adc_unit, adc1_channel_t adc_channel)
{
    ESP_RETURN_ON_FALSE((adc_unit < ADC_UNIT_2), ESP_ERR_INVALID_ARG, TAG, "i2s ADC unit error, only support ADC1 for now");
    // For now, we only support SAR ADC1.
    _i2s_adc_unit = adc_unit;
    _i2s_adc_channel = adc_channel;
    return adc_i2s_mode_init(adc_unit, adc_channel);
}

/**
 * @brief   I2S enable ADC mode
 *
 * @param   i2s_num         I2S device number
 * @return
 *      - ESP_OK                Enable ADC success
 *      - ESP_ERR_INVALID_ARG   Invalid argument
 *      - ESP_ERR_INVALID_STATE Current I2S mode is not built-in ADC
 */
esp_err_t i2s_adc_enable(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_ADC_BUILT_IN), ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    adc1_dma_mode_acquire();
    _i2s_adc_mode_recover();
    i2s_rx_reset(i2s_num);
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->hal_cfg.sample_rate, p_i2s[i2s_num]->hal_cfg.sample_bits, p_i2s[i2s_num]->hal_cfg.active_chan);
}

/**
 * @brief   I2S disable ADC
 *
 * @param   i2s_num         I2S device number
 * @return
 *      - ESP_OK        I2S ADC mode successfully disabled
 *      - ESP_ERR_INVALID_ARG   Invalid argument
 *      - ESP_ERR_INVALID_STATE Current I2S mode is not built-in ADC
 */
esp_err_t i2s_adc_disable(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_ADC_BUILT_IN), ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    i2s_hal_stop_rx(&(p_i2s[i2s_num]->hal));
    adc1_lock_release();
    return ESP_OK;
}
#endif

/**
 * @brief   Set sample rate used for I2S RX and TX.
 * @note    The bit clock rate is determined by the sample rate and i2s_config_t configuration parameters (number of channels, bits_per_sample).
 *          `bit_clock = rate * (number of channels) * bits_per_sample`
 *
 * @param   i2s_num  I2S device number
 * @param   rate I2S sample rate (ex: 8000, 44100...)
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.sample_bits > 0), ESP_ERR_INVALID_ARG, TAG, "sample bits not set");
    return i2s_set_clk(i2s_num, rate, p_i2s[i2s_num]->hal_cfg.sample_bits, p_i2s[i2s_num]->hal_cfg.active_chan);
}

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief   Configure I2S a/u-law decompress or compress
 * @note    This function should be called after i2s driver installed
 *          Only take effect when the i2s 'communication_format' is set to 'I2S_COMM_FORMAT_STAND_PCM_SHORT' or 'I2S_COMM_FORMAT_STAND_PCM_LONG'
 *
 * @param   i2s_num  I2S_NUM_0
 * @param   pcm_cfg  Including mode selection and a/u-law decompress or compress configuration paramater
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_pcm_config(i2s_port_t i2s_num, const i2s_pcm_cfg_t *pcm_cfg)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.comm_fmt & I2S_COMM_FORMAT_STAND_PCM_SHORT),
                        ESP_ERR_INVALID_ARG, TAG, "i2s communication mode is not PCM mode");
    i2s_stop(i2s_num);
    I2S_ENTER_CRITICAL(i2s_num);
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        i2s_hal_tx_pcm_cfg(&(p_i2s[i2s_num]->hal), pcm_cfg->pcm_type);
    } else if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
        i2s_hal_rx_pcm_cfg(&(p_i2s[i2s_num]->hal), pcm_cfg->pcm_type);
    }
    I2S_EXIT_CRITICAL(i2s_num);
    i2s_start(i2s_num);
    return ESP_OK;
}
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
/**
 * @brief   Set PDM mode down-sample rate
 *          In PDM RX mode, there would be 2 rounds of downsample process in hardware.
 *          In the first downsample process, the sampling number can be 16 or 8.
 *          In the second downsample process, the sampling number is fixed as 8.
 *          So the clock frequency in PDM RX mode would be (fpcm * 64) or (fpcm * 128) accordingly.
 * @note    After calling this function, it would call i2s_set_clk inside to update the clock frequency.
 *          Please call this function after I2S driver has been initialized.
 *
 * @param   i2s_num I2S device number
 * @param   downsample i2s RX down sample rate for PDM mode.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_pdm_rx_down_sample(i2s_port_t i2s_num, i2s_pdm_dsr_t downsample)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_PDM), ESP_ERR_INVALID_ARG, TAG, "i2s mode is not PDM mode");
    i2s_stop(i2s_num);
    i2s_hal_set_rx_pdm_dsr(&(p_i2s[i2s_num]->hal), downsample);
    // i2s will start in 'i2s_set_clk'
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->hal_cfg.sample_rate, p_i2s[i2s_num]->hal_cfg.sample_bits, p_i2s[i2s_num]->hal_cfg.active_chan);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief   Set TX PDM mode up-sample rate
 * @note    If you have set PDM mode while calling 'i2s_driver_install',
 *          default PDM TX upsample parameters have already been set,
 *          no need to call this function again if you don't have to change the default configuration
 *
 * @param   i2s_num         I2S device number
 * @param   upsample_cfg    Set I2S PDM up-sample rate configuration
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_pdm_tx_up_sample(i2s_port_t i2s_num, const i2s_pdm_tx_upsample_cfg_t *upsample_cfg)
{
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_FAIL, TAG, "i2s has not installed yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_PDM), ESP_ERR_INVALID_ARG, TAG, "i2s mode is not PDM mode");
    i2s_stop(i2s_num);
    i2s_hal_set_tx_pdm_fpfs(&(p_i2s[i2s_num]->hal), upsample_cfg->fp, upsample_cfg->fs);
    // i2s will start in 'i2s_set_clk'
    return i2s_set_clk(i2s_num, upsample_cfg->sample_rate, p_i2s[i2s_num]->hal_cfg.sample_bits, p_i2s[i2s_num]->hal_cfg.active_chan);
}
#endif

/**
 * @brief   I2S check the validity of configuration
 *
 * @param   i2s_num     I2S device number
 * @param   cfg         I2S HAL configuration
 * @return
 *      - ESP_OK                I2S configuration is valid
 *      - ESP_ERR_INVALID_ARG   I2S configuration is invalid
 */
static esp_err_t i2s_check_cfg_validity(i2s_port_t i2s_num, i2s_hal_config_t *cfg)
{
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
    ESP_RETURN_ON_FALSE(cfg->comm_fmt && (cfg->comm_fmt < I2S_COMM_FORMAT_STAND_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid communication formats");
#endif // SOC_I2S_SUPPORTS_ADC || SOC_I2S_SUPPORTS_DAC

    return ESP_OK;
}

static void i2s_tx_set_clk_and_channel(i2s_port_t i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    i2s_hal_tx_clock_config(&(p_i2s[i2s_num]->hal), clk_cfg);
    i2s_hal_set_tx_sample_bit(&(p_i2s[i2s_num]->hal), p_i2s[i2s_num]->hal_cfg.chan_bits, p_i2s[i2s_num]->hal_cfg.sample_bits);
    i2s_hal_tx_set_channel_style(&(p_i2s[i2s_num]->hal), &(p_i2s[i2s_num]->hal_cfg));
}

static void i2s_rx_set_clk_and_channel(i2s_port_t i2s_num, i2s_hal_clock_cfg_t *clk_cfg)
{
    i2s_hal_rx_clock_config(&(p_i2s[i2s_num]->hal), clk_cfg);
    i2s_hal_set_rx_sample_bit(&(p_i2s[i2s_num]->hal), p_i2s[i2s_num]->hal_cfg.chan_bits, p_i2s[i2s_num]->hal_cfg.sample_bits);
    i2s_hal_rx_set_channel_style(&(p_i2s[i2s_num]->hal), &(p_i2s[i2s_num]->hal_cfg));
}

/**
 * @brief   Get clock set on particular port number.
 *
 * @param   i2s_num         I2S device number
 * @return
 *      - sample rate
 */
float i2s_get_clk(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    return (float)p_i2s[i2s_num]->hal_cfg.sample_rate;
}

/**
 * @brief   Set clock & bit width used for I2S RX and TX.
 *          Similar to i2s_set_sample_rates(), but also sets bit width.
 *
 * 1. stop i2s
 * 2. calculate mclk, bck, bck_factor
 * 3. set clock configurations
 * 4. realloc dma buffer if DMA buffer size changed
 * 5. start i2s
 *
 * @param   i2s_num     I2S device number
 * @param   rate        I2S sample rate (ex: 8000, 44100...)
 * @param   bits_cfg    I2S bits configuration
 *             the low 16 bits is for data bits per sample in one channel (see 'i2s_bits_per_sample_t')
 *             the high 16 bits is for total bits in one channel (see 'i2s_bits_per_chan_t')
 *             high 16bits =0 means same as the bits per sample.
 * @param   ch          I2S channel, (I2S_CHANNEL_MONO, I2S_CHANNEL_STEREO or specific channel in TDM mode)
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, uint32_t bits_cfg, i2s_channel_t ch)
{
    esp_err_t ret = ESP_OK;

    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_ERR_INVALID_ARG, TAG, "I2S%d has not installed yet", i2s_num);

    i2s_hal_config_t *cfg = &p_i2s[i2s_num]->hal_cfg;

    /* Stop I2S */
    i2s_stop(i2s_num);

    /* If not the first time, update configuration */
    if (p_i2s[i2s_num]->last_buf_size) {
        cfg->sample_rate = rate;
        cfg->sample_bits = bits_cfg & 0xFFFF;
        cfg->chan_bits = (bits_cfg >> 16) > cfg->sample_bits ? (bits_cfg >> 16) : cfg->sample_bits;
#if SOC_I2S_SUPPORTS_TDM
        if (ch & I2S_CHANNEL_MONO) {
            if (ch >> 16) {
                cfg->chan_fmt = I2S_CHANNEL_FMT_MULTIPLE;
                cfg->chan_mask = 1 << __builtin_ctz(ch & 0xFFFF0000); // mono the minimun actived slot
                cfg->total_chan = i2s_get_max_channel_num(ch);
            } else {
                if (p_i2s[i2s_num]->init_chan_fmt == I2S_CHANNEL_FMT_ONLY_LEFT) {
                    cfg->chan_mask = I2S_TDM_ACTIVE_CH1; // left slot mono
                    cfg->chan_fmt = I2S_CHANNEL_FMT_ONLY_LEFT;
                } else {
                    cfg->chan_mask = I2S_TDM_ACTIVE_CH0; // right slot mono
                    cfg->chan_fmt = I2S_CHANNEL_FMT_ONLY_RIGHT;
                }
                cfg->total_chan = 2;
            }
        } else {
            if (ch >> 16) {
                cfg->chan_fmt = I2S_CHANNEL_FMT_MULTIPLE;
                cfg->chan_mask = ch & 0xFFFF0000;
                cfg->total_chan = i2s_get_max_channel_num(cfg->chan_mask);
            } else {
                /* If no TDM channels activated, use 2 channels as defualt */
                cfg->chan_fmt = I2S_CHANNEL_FMT_RIGHT_LEFT;
                cfg->chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1;
                cfg->total_chan = 2;
            }
        }
#else
        /* Default */
        if (ch & I2S_CHANNEL_MONO) {
            if (p_i2s[i2s_num]->init_chan_fmt == I2S_CHANNEL_FMT_ONLY_LEFT) {
                cfg->chan_fmt = I2S_CHANNEL_FMT_ONLY_LEFT;
            } else {
                cfg->chan_fmt = I2S_CHANNEL_FMT_ONLY_RIGHT;
            }
        } else {
            cfg->chan_fmt = I2S_CHANNEL_FMT_RIGHT_LEFT;
        }
        cfg->active_chan = i2s_get_active_channel_num(cfg);
        cfg->total_chan = 2;
#endif
        if (cfg->mode & I2S_MODE_TX) {
            xSemaphoreTake(p_i2s[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
            i2s_hal_tx_set_channel_style(&(p_i2s[i2s_num]->hal), cfg);
            xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
        }
        if (cfg->mode & I2S_MODE_RX) {
            xSemaphoreTake(p_i2s[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
            i2s_hal_rx_set_channel_style(&(p_i2s[i2s_num]->hal), cfg);
            xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
        }
    }
    uint32_t data_bits = cfg->sample_bits;

    /* Check the validity of sample bits */
    ESP_RETURN_ON_FALSE((data_bits % 8 == 0), ESP_ERR_INVALID_ARG, TAG, "Invalid bits per sample");
    ESP_RETURN_ON_FALSE((data_bits <= I2S_BITS_PER_SAMPLE_32BIT), ESP_ERR_INVALID_ARG, TAG, "Invalid bits per sample");

    i2s_hal_clock_cfg_t clk_cfg;
    /* To get sclk, mclk, mclk_div bclk and bclk_div */
    i2s_calculate_clock(i2s_num, &clk_cfg);

    uint32_t buf_size = i2s_get_buf_size(i2s_num);
    bool need_realloc = p_i2s[i2s_num]->last_buf_size != buf_size;

    /* TX mode clock reset */
    if (cfg->mode & I2S_MODE_TX) {
        ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->tx, ESP_ERR_INVALID_ARG, TAG, "I2S TX DMA object has not initialized yet");
        /* Waiting for transmit finish */
        xSemaphoreTake(p_i2s[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
        i2s_tx_set_clk_and_channel(i2s_num, &clk_cfg);
        /* If buffer size changed, the DMA buffer need realloc */
        if (need_realloc) {
            p_i2s[i2s_num]->tx->buf_size = buf_size;
            ret = i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->tx);
        }
        /* If not the first time, update I2S tx channel style */
        if (p_i2s[i2s_num]->last_buf_size) {
            i2s_hal_tx_set_channel_style(&(p_i2s[i2s_num]->hal), &(p_i2s[i2s_num]->hal_cfg));
        }
        /* Reset the queue to avoid receive invalid data */
        xQueueReset(p_i2s[i2s_num]->tx->queue);
        xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
        ESP_RETURN_ON_ERROR(ret, TAG, "I2S%d tx DMA buffer malloc failed", i2s_num);
    }
    /* RX mode clock reset */
    if (cfg->mode & I2S_MODE_RX) {
        ESP_RETURN_ON_FALSE(p_i2s[i2s_num]->rx, ESP_ERR_INVALID_ARG, TAG, "I2S TX DMA object has not initialized yet");
        /* Waiting for receive finish */
        xSemaphoreTake(p_i2s[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
        i2s_rx_set_clk_and_channel(i2s_num, &clk_cfg);
        /* If buffer size changed, the DMA buffer need realloc */
        if (need_realloc) {
            p_i2s[i2s_num]->rx->buf_size = buf_size;
            ret = i2s_realloc_dma_buffer(i2s_num, p_i2s[i2s_num]->rx);
            /* Reset the end-of-frame number */
            i2s_hal_set_rx_eof_num(&(p_i2s[i2s_num]->hal), buf_size);
        }
        /* If not the first time, update I2S rx channel style */
        if (p_i2s[i2s_num]->last_buf_size) {
            i2s_hal_rx_set_channel_style(&(p_i2s[i2s_num]->hal), &(p_i2s[i2s_num]->hal_cfg));
        }
        /* Reset the queue to avoid receiving invalid data */
        xQueueReset(p_i2s[i2s_num]->rx->queue);
        xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
        ESP_RETURN_ON_ERROR(ret, TAG, "I2S%d rx DMA buffer malloc failed", i2s_num);
    }
    /* Update last buffer size */
    p_i2s[i2s_num]->last_buf_size = buf_size;

    /* I2S start */
    i2s_start(i2s_num);

    return ESP_OK;
}

/*-------------------------------------------------------------
                   I2S driver operation
  -------------------------------------------------------------*/
/**
 * @brief   Start I2S driver
 * @note    It is not necessary to call this function after i2s_driver_install() (it is started automatically), however it is necessary to call it after i2s_stop().
 *
 * @param   i2s_num  I2S device number
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_start(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    //start DMA link
    I2S_ENTER_CRITICAL(i2s_num);

    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        i2s_tx_reset(i2s_num);
        i2s_tx_start(i2s_num);
    }
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
        i2s_rx_reset(i2s_num);
        i2s_rx_start(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    esp_intr_enable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    I2S_EXIT_CRITICAL(i2s_num);
    return ESP_OK;
}

/**
 * @brief   Stop I2S driver
 * @note    There is no need to call i2s_stop() before calling i2s_driver_uninstall().
 *          Disables I2S TX/RX, until i2s_start() is called.
 *
 * @param   i2s_num     I2S device number
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_stop(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    I2S_ENTER_CRITICAL(i2s_num);
#if !SOC_GDMA_SUPPORTED
    esp_intr_disable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        i2s_tx_stop(i2s_num);
    }
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
        i2s_rx_stop(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    i2s_hal_clear_intr_status(&(p_i2s[i2s_num]->hal), I2S_INTR_MAX);
#endif
    I2S_EXIT_CRITICAL(i2s_num);
    return ESP_OK;
}

/**
 * @brief   Initialize I2S driver configurations
 *
 * @param   i2s_num     I2S device number
 * @param   i2s_config  I2S configurations - see i2s_config_t struct
 * @return
 *      - ESP_OK        I2S initialize success
 *      - ESP_ERR_INVALID_ARG   No channel enabled in multiple channel format
 */
static esp_err_t i2s_driver_init(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{
    ESP_RETURN_ON_FALSE(i2s_config, ESP_ERR_INVALID_ARG, TAG, "The pointer of I2S configuration structure is NULL");

    /* I2S driver configuration assignment */
    p_i2s[i2s_num]->i2s_num = i2s_num;
    p_i2s[i2s_num]->dma_buf_count = i2s_config->dma_buf_count;
    p_i2s[i2s_num]->dma_buf_len = i2s_config->dma_buf_len;
    p_i2s[i2s_num]->last_buf_size = 0;
    p_i2s[i2s_num]->use_apll = i2s_config->use_apll;
    p_i2s[i2s_num]->fixed_mclk = i2s_config->fixed_mclk;
    p_i2s[i2s_num]->mclk_multiple = i2s_config->mclk_multiple;
    p_i2s[i2s_num]->tx_desc_auto_clear = i2s_config->tx_desc_auto_clear;
    p_i2s[i2s_num]->init_chan_fmt = i2s_config->channel_format;

    /* I2S HAL configuration assignment */
    p_i2s[i2s_num]->hal_cfg.mode = i2s_config->mode;
    p_i2s[i2s_num]->hal_cfg.sample_rate = i2s_config->sample_rate;
    p_i2s[i2s_num]->hal_cfg.comm_fmt = i2s_config->communication_format;
    p_i2s[i2s_num]->hal_cfg.chan_fmt = i2s_config->channel_format;
    p_i2s[i2s_num]->hal_cfg.sample_bits = i2s_config->bits_per_sample;
    p_i2s[i2s_num]->hal_cfg.chan_bits = (uint32_t)i2s_config->bits_per_chan < (uint32_t)i2s_config->bits_per_sample ?
                                        (uint32_t)i2s_config->bits_per_sample : (uint32_t)i2s_config->bits_per_chan;

#if SOC_I2S_SUPPORTS_TDM
    /* I2S HAL TDM configuration assignment */
    p_i2s[i2s_num]->hal_cfg.left_align    = i2s_config->left_align;
    p_i2s[i2s_num]->hal_cfg.big_edin      = i2s_config->big_edin;
    p_i2s[i2s_num]->hal_cfg.bit_order_msb = i2s_config->bit_order_msb;
    p_i2s[i2s_num]->hal_cfg.skip_msk      = i2s_config->skip_msk;

    /* Set chan_mask according to channel format */
    switch (i2s_config->channel_format) {
    case I2S_CHANNEL_FMT_RIGHT_LEFT:    // fall through
    case I2S_CHANNEL_FMT_ALL_RIGHT:     // fall through
    case I2S_CHANNEL_FMT_ALL_LEFT:
        p_i2s[i2s_num]->hal_cfg.chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1;
        p_i2s[i2s_num]->hal_cfg.total_chan = 2;
        break;
    case I2S_CHANNEL_FMT_ONLY_RIGHT:
        p_i2s[i2s_num]->hal_cfg.chan_mask = I2S_TDM_ACTIVE_CH1;
        p_i2s[i2s_num]->hal_cfg.total_chan = 2;
        break;
    case I2S_CHANNEL_FMT_ONLY_LEFT:
        p_i2s[i2s_num]->hal_cfg.chan_mask = I2S_TDM_ACTIVE_CH0;
        p_i2s[i2s_num]->hal_cfg.total_chan = 2;
        break;
    case I2S_CHANNEL_FMT_MULTIPLE:
        ESP_RETURN_ON_FALSE((i2s_config->chan_mask >> 16), ESP_ERR_INVALID_ARG, TAG, "i2s all channel are disabled");
        p_i2s[i2s_num]->hal_cfg.chan_mask = i2s_config->chan_mask & 0xFFFF0000;
        /* Get the max actived channel number */
        uint32_t max_channel = i2s_get_max_channel_num(p_i2s[i2s_num]->hal_cfg.chan_mask);
        /* If total channel is smaller than max actived channel number then set it to the max active channel number */
        p_i2s[i2s_num]->hal_cfg.total_chan    = p_i2s[i2s_num]->hal_cfg.total_chan < max_channel ? max_channel :
                                                p_i2s[i2s_num]->hal_cfg.total_chan;
        break;
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "wrong i2s channel format, going to uninstall i2s");
    }

    /* Calculate actived channel number in channel mask */
    p_i2s[i2s_num]->hal_cfg.active_chan   = i2s_get_active_channel_num(&p_i2s[i2s_num]->hal_cfg);

#else
    /* Calculate actived channel number in channel mask */
    p_i2s[i2s_num]->hal_cfg.active_chan   = i2s_get_active_channel_num(&p_i2s[i2s_num]->hal_cfg);
    /* Total channel number is equal to the actived channel number in non-TDM mode */
    p_i2s[i2s_num]->hal_cfg.total_chan    = 2;
#endif
    return ESP_OK;
}

/**
 * @brief   Initialize I2S DMA object
 *
 * @param   i2s_num     I2S device number
 * @return
 *      - ESP_OK            DMA object initialize success
 *      - ESP_ERR_NO_MEM    No memory for DMA object
 */
static esp_err_t i2s_dma_object_init(i2s_port_t i2s_num)
{
    uint32_t buf_size = i2s_get_buf_size(i2s_num);
    /* Create DMA object */
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        ESP_RETURN_ON_ERROR(i2s_create_dma_object(i2s_num, &p_i2s[i2s_num]->tx), TAG, "I2S TX DMA object create failed");
        p_i2s[i2s_num]->tx->buf_size = buf_size;
    }
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
        ESP_RETURN_ON_ERROR(i2s_create_dma_object(i2s_num, &p_i2s[i2s_num]->rx), TAG, "I2S RX DMA object create failed");
        p_i2s[i2s_num]->rx->buf_size = buf_size;
    }
    return ESP_OK;
}

/**
 * @brief   Install and start I2S driver.
 * @note    This function must be called before any I2S driver read/write operations.
 *
 *
 * @param   i2s_num         I2S device number
 * @param   i2s_config      I2S configurations - see i2s_config_t struct
 * @param   queue_size      I2S event queue size/depth.
 * @param   i2s_queue       I2S event queue handle, if set NULL, driver will not use an event queue.
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 *     - ESP_ERR_INVALID_STATE  Current I2S port is in use
 */
esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void *i2s_queue)
{
    esp_err_t ret = ESP_OK;

    /* Step 1: Check the validity of input parameters */
    /* Check the validity of i2s device number */
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(i2s_config, ESP_ERR_INVALID_ARG, TAG, "I2S configuration must not be NULL");
    /* Check the size of DMA buffer */
    ESP_RETURN_ON_FALSE((i2s_config->dma_buf_count >= 2 && i2s_config->dma_buf_count <= 128), ESP_ERR_INVALID_ARG, TAG, "I2S buffer count less than 128 and more than 2");
    ESP_RETURN_ON_FALSE((i2s_config->dma_buf_len >= 8 && i2s_config->dma_buf_len <= 1024), ESP_ERR_INVALID_ARG, TAG, "I2S buffer length at most 1024 and more than 8");

    /* Step 2: Allocate driver object and register to platform */
    i2s_obj_t *pre_alloc_i2s_obj = calloc(1, sizeof(i2s_obj_t));
    ESP_RETURN_ON_FALSE(pre_alloc_i2s_obj, ESP_ERR_NO_MEM, TAG, "no mem for I2S driver");
    ret = i2s_priv_register_object(pre_alloc_i2s_obj, i2s_num);
    if (ret != ESP_OK) {
        free(pre_alloc_i2s_obj);
        ESP_LOGE(TAG, "register I2S object to platform failed");
        return ESP_ERR_INVALID_STATE;
    }

    /* Step 3: Initialize I2S object, assign configarations */
    ESP_GOTO_ON_ERROR(i2s_driver_init(i2s_num, i2s_config), err, TAG, "I2S init failed");
    /* Check the validity of I2S configuration */
    ESP_GOTO_ON_ERROR(i2s_check_cfg_validity(i2s_num, &(pre_alloc_i2s_obj->hal_cfg)), err, TAG, "I2S configuration is invalid");

    /* Get device instance */
    i2s_hal_init(&(pre_alloc_i2s_obj->hal), i2s_num);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
    if (i2s_config->use_apll) {
        pm_lock = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2S_SUPPORTS_APLL
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_lock, 0, "i2s_driver", &pre_alloc_i2s_obj->pm_lock), err, TAG, "I2S pm lock error");
#endif //CONFIG_PM_ENABLE

    /* Step 4: Initialize I2S DMA interrupt and DMA object */
    ESP_GOTO_ON_ERROR(i2s_dma_intr_init(i2s_num, i2s_config->intr_alloc_flags), err, TAG, "I2S interrupt initailze failed");
    /* Initialize I2S DMA object */
    ESP_GOTO_ON_ERROR(i2s_dma_object_init(i2s_num), err, TAG,  "I2S dma object create failed");

#if SOC_I2S_SUPPORTS_ADC
    /* If using built-in ADC, we need to enable ADC power manerge*/
    if (pre_alloc_i2s_obj->hal_cfg.mode & I2S_MODE_ADC_BUILT_IN) {
        adc_power_acquire();
    }
#endif
    /* Enable module clock */
    i2s_hal_enable_module_clock(&p_i2s[i2s_num]->hal);
#if SOC_I2S_SUPPORTS_TDM
    if (i2s_config->mode & I2S_MODE_TX) {
        i2s_ll_tx_enable_clock(p_i2s[i2s_num]->hal.dev);
    }
    if (i2s_config->mode & I2S_MODE_RX) {
        i2s_ll_rx_enable_clock(p_i2s[i2s_num]->hal.dev);
    }
#endif

    /* Step 5: Initialize I2S configuration and set the configurations to register */
    i2s_hal_config_param(&(pre_alloc_i2s_obj->hal), &pre_alloc_i2s_obj->hal_cfg);

    /* Step 6: Initialise i2s event queue if user needs */
    if (i2s_queue) {
        pre_alloc_i2s_obj->i2s_queue = xQueueCreate(queue_size, sizeof(i2s_event_t));
        ESP_GOTO_ON_FALSE(pre_alloc_i2s_obj->i2s_queue, ESP_ERR_NO_MEM, err, TAG, "I2S queue create failed");
        *((QueueHandle_t *) i2s_queue) = pre_alloc_i2s_obj->i2s_queue;
        ESP_LOGI(TAG, "queue free spaces: %d", uxQueueSpacesAvailable(pre_alloc_i2s_obj->i2s_queue));
    } else {
        pre_alloc_i2s_obj->i2s_queue = NULL;
    }

    /* Step 7: Set I2S clocks and start. No need to give parameters since configurations has been set in 'i2s_driver_init' */
    ESP_GOTO_ON_ERROR(i2s_set_clk(i2s_num, 0, 0, 0), err, TAG, "I2S set clock failed");

    return ESP_OK;

err:
    /* I2S install failed, prepare to uninstall */
    i2s_driver_uninstall(i2s_num);
    return ret;
}

/**
 * @brief Uninstall I2S driver.
 *
 * @param   i2s_num     I2S device number
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE(i2s_num < I2S_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE(p_i2s[i2s_num], ESP_ERR_INVALID_STATE, TAG, "I2S port %d has not installed", i2s_num);
    i2s_obj_t *obj = p_i2s[i2s_num];
    i2s_stop(i2s_num);
#if SOC_I2S_SUPPORTS_DAC
    i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE);
#endif
#if SOC_GDMA_SUPPORTED
    if (p_i2s[i2s_num]->tx_dma_chan) {
        gdma_disconnect(p_i2s[i2s_num]->tx_dma_chan);
        gdma_del_channel(p_i2s[i2s_num]->tx_dma_chan);
    }
    if (p_i2s[i2s_num]->rx_dma_chan) {
        gdma_disconnect(p_i2s[i2s_num]->rx_dma_chan);
        gdma_del_channel(p_i2s[i2s_num]->rx_dma_chan);
    }
#else
    if (p_i2s[i2s_num]->i2s_isr_handle) {
        esp_intr_free(p_i2s[i2s_num]->i2s_isr_handle);
    }
#endif
    /* Destroy dma object if exist */
    i2s_destroy_dma_object(i2s_num, &p_i2s[i2s_num]->tx);
    i2s_destroy_dma_object(i2s_num, &p_i2s[i2s_num]->rx);

    if (p_i2s[i2s_num]->i2s_queue) {
        vQueueDelete(p_i2s[i2s_num]->i2s_queue);
        p_i2s[i2s_num]->i2s_queue = NULL;
    }

#if SOC_I2S_SUPPORTS_APLL
    if (p_i2s[i2s_num]->use_apll) {
        // switch back to PLL clock source
        i2s_hal_set_clock_src(&(p_i2s[i2s_num]->hal), I2S_CLK_D2CLK);
        rtc_clk_apll_enable(0, 0, 0, 0, 0);
    }
#endif

#ifdef CONFIG_PM_ENABLE
    if (p_i2s[i2s_num]->pm_lock) {
        esp_pm_lock_delete(p_i2s[i2s_num]->pm_lock);
        p_i2s[i2s_num]->pm_lock = NULL;
    }
#endif
#if SOC_I2S_SUPPORTS_TDM
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_TX) {
        i2s_ll_tx_disable_clock(p_i2s[i2s_num]->hal.dev);
    }
    if (p_i2s[i2s_num]->hal_cfg.mode & I2S_MODE_RX) {
        i2s_ll_rx_disable_clock(p_i2s[i2s_num]->hal.dev);
    }
#endif
    /* Disable module clock */
    i2s_hal_disable_module_clock(&p_i2s[i2s_num]->hal);
    i2s_priv_deregister_object(i2s_num);
    free(obj);
    return ESP_OK;
}

/**
 * @brief   Write data to I2S DMA transmit buffer.
 * @note    Many ticks pass without space becoming available in the DMA
 *          transmit buffer, then the function will return (note that if the
 *          data is written to the DMA buffer in pieces, the overall operation
 *          may still take longer than this timeout.) Pass portMAX_DELAY for no
 *          timeout.
 *
 * @param       i2s_num             I2S device number
 * @param       src                 Source address to write from
 * @param       size                Size of data in bytes
 * @param[out]  bytes_written       Number of bytes written, if timeout, the result will be less than the size passed in.
 * @param       ticks_to_wait       TX buffer wait timeout in RTOS ticks. If this
 * @return
 *     - ESP_OK               Success
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t i2s_write(i2s_port_t i2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr, *src_byte;
    size_t bytes_can_write;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->tx), ESP_ERR_INVALID_ARG, TAG, "TX mode is not enabled");
    xSemaphoreTake(p_i2s[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
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
        ESP_LOGD(TAG, "size: %d, rw_pos: %d, buf_size: %d, curr_ptr: %d", size, p_i2s[i2s_num]->tx->rw_pos, p_i2s[i2s_num]->tx->buf_size, (int)p_i2s[i2s_num]->tx->curr_ptr);
        data_ptr = (char *)p_i2s[i2s_num]->tx->curr_ptr;
        data_ptr += p_i2s[i2s_num]->tx->rw_pos;
        bytes_can_write = p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos;
        if (bytes_can_write > size) {
            bytes_can_write = size;
        }
        memcpy(data_ptr, src_byte, bytes_can_write);
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

/**
 * @brief   Write data to I2S DMA transmit buffer while expanding the number of bits per sample. For example, expanding 16-bit PCM to 32-bit PCM.
 * @note    Many ticks pass without space becoming available in the DMA
 *          transmit buffer, then the function will return (note that if the
 *          data is written to the DMA buffer in pieces, the overall operation
 *          may still take longer than this timeout.) Pass portMAX_DELAY for no
 *          timeout.
 *          Format of the data in source buffer is determined by the I2S configuration (see i2s_config_t).
 *
 * @param       i2s_num             I2S device number
 * @param       src                 Source address to write from
 * @param       size                Size of data in bytes
 * @param       src_bits            Source audio bit
 * @param       aim_bits            Bit wanted, no more than 32, and must be greater than src_bits
 * @param[out]  bytes_written       Number of bytes written, if timeout, the result will be less than the size passed in.
 * @param       ticks_to_wait       TX buffer wait timeout in RTOS ticks. If this
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_write, tail;
    int src_bytes, aim_bytes, zero_bytes;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
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
    xSemaphoreTake(p_i2s[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
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
        size -= bytes_can_write;
        p_i2s[i2s_num]->tx->rw_pos += bytes_can_write;
    }
    xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
    return ESP_OK;
}

/**
 * @brief   Read data from I2S DMA receive buffer
 * @note    If the built-in ADC mode is enabled, we should call i2s_adc_enable and i2s_adc_disable around the whole reading process,
 *          to prevent the data getting corrupted.
 *
 * @param       i2s_num         I2S device number
 * @param       dest            Destination address to read into
 * @param       size            Size of data in bytes
 * @param[out]  bytes_read      Number of bytes read, if timeout, bytes read will be less than the size passed in.
 * @param       ticks_to_wait   RX buffer wait timeout in RTOS ticks. If this many ticks pass without bytes becoming available in the DMA receive buffer, then the function will return (note that if data is read from the DMA buffer in pieces, the overall operation may still take longer than this timeout.) Pass portMAX_DELAY for no timeout.
 * @return
 *     - ESP_OK               Success
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t i2s_read(i2s_port_t i2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    char *data_ptr, *dest_byte;
    int bytes_can_read;
    *bytes_read = 0;
    dest_byte = (char *)dest;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->rx), ESP_ERR_INVALID_ARG, TAG, "RX mode is not enabled");
    xSemaphoreTake(p_i2s[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
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

esp_err_t i2s_priv_register_object(void *driver_obj, int port_id)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    ESP_RETURN_ON_FALSE(driver_obj && (port_id < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid arguments");
    portENTER_CRITICAL(&i2s_platform_spinlock);
    if (!p_i2s[port_id]) {
        ret = ESP_OK;
        p_i2s[port_id] = driver_obj;
        periph_module_enable(i2s_periph_signal[port_id].module);
    }
    portEXIT_CRITICAL(&i2s_platform_spinlock);
    return ret;
}

esp_err_t i2s_priv_deregister_object(int port_id)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    ESP_RETURN_ON_FALSE(port_id < SOC_I2S_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid arguments");
    portENTER_CRITICAL(&i2s_platform_spinlock);
    if (p_i2s[port_id]) {
        ret = ESP_OK;
        p_i2s[port_id] = NULL;
        periph_module_disable(i2s_periph_signal[port_id].module);
    }
    portEXIT_CRITICAL(&i2s_platform_spinlock);
    return ret;
}
