// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <esp_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"

#include "soc/dport_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "rom/lldesc.h"

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"

#include "esp_intr.h"
#include "esp_err.h"
#include "esp_log.h"

static const char* I2S_TAG = "I2S";
#define I2S_CHECK(a, str, ret) if (!(a)) {                                              \
        ESP_LOGE(I2S_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);       \
        return (ret);                                                                   \
        }
#define I2S_BASE_CLK (2*APB_CLK_FREQ)
#define I2S_ENTER_CRITICAL_ISR()     portENTER_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL_ISR()      portEXIT_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_ENTER_CRITICAL()         portENTER_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL()          portEXIT_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_FULL_DUPLEX_SLAVE_MODE_MASK   (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_SLAVE)
#define I2S_FULL_DUPLEX_MASTER_MODE_MASK  (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_MASTER)

/**
 * @brief DMA buffer object
 *
 */
typedef struct {
    char **buf;
    int buf_size;
    int rw_pos;
    void *curr_ptr;
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
    i2s_dma_t *rx;              /*!< DMA Tx buffer*/
    i2s_dma_t *tx;              /*!< DMA Rx buffer*/
    i2s_isr_handle_t i2s_isr_handle; /*!< I2S Interrupt handle*/
    int channel_num;            /*!< Number of channels*/
    int bytes_per_sample;        /*!< Bytes per sample*/
    int bits_per_sample;        /*!< Bits per sample*/
    i2s_mode_t mode;            /*!< I2S Working mode*/
} i2s_obj_t;

static i2s_obj_t *p_i2s_obj[I2S_NUM_MAX] = {0};
static i2s_dev_t* I2S[I2S_NUM_MAX] = {&I2S0, &I2S1};
static portMUX_TYPE i2s_spinlock[I2S_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};

static i2s_dma_t *i2s_create_dma_queue(i2s_port_t i2s_num, int dma_buf_count, int dma_buf_len);
static esp_err_t i2s_destroy_dma_queue(i2s_port_t i2s_num, i2s_dma_t *dma);
static esp_err_t i2s_reset_fifo(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_ENTER_CRITICAL();
    I2S[i2s_num]->conf.rx_fifo_reset = 1;
    I2S[i2s_num]->conf.rx_fifo_reset = 0;
    I2S[i2s_num]->conf.tx_fifo_reset = 1;
    I2S[i2s_num]->conf.tx_fifo_reset = 0;
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

inline static void gpio_matrix_out_check(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv) 
{
    //if pin = -1, do not need to configure
    if (gpio != -1) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        gpio_set_direction(gpio, GPIO_MODE_DEF_OUTPUT);
        gpio_matrix_out(gpio, signal_idx, out_inv, oen_inv);
    } 
} 
inline static void gpio_matrix_in_check(uint32_t gpio, uint32_t signal_idx, bool inv) 
{
    if (gpio != -1) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        //Set direction, for some GPIOs, the input function are not enabled as default.
        gpio_set_direction(gpio, GPIO_MODE_DEF_INPUT);
        gpio_matrix_in(gpio, signal_idx, inv);
    }
}


esp_err_t i2s_clear_intr_status(i2s_port_t i2s_num, uint32_t clr_mask)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S[i2s_num]->int_clr.val = clr_mask;
    return ESP_OK;
}

esp_err_t i2s_enable_rx_intr(i2s_port_t i2s_num)
{

    I2S_ENTER_CRITICAL();
    I2S[i2s_num]->int_ena.in_suc_eof = 1;
    I2S[i2s_num]->int_ena.in_dscr_err = 1;
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_disable_rx_intr(i2s_port_t i2s_num)
{
    I2S_ENTER_CRITICAL();
    I2S[i2s_num]->int_ena.in_suc_eof = 0;
    I2S[i2s_num]->int_ena.in_dscr_err = 0;
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_disable_tx_intr(i2s_port_t i2s_num)
{
    I2S_ENTER_CRITICAL();
    I2S[i2s_num]->int_ena.out_eof = 0;
    I2S[i2s_num]->int_ena.out_dscr_err = 0;
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_enable_tx_intr(i2s_port_t i2s_num)
{
    I2S_ENTER_CRITICAL();
    I2S[i2s_num]->int_ena.out_eof = 1;
    I2S[i2s_num]->int_ena.out_dscr_err = 1;
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

static esp_err_t i2s_isr_register(i2s_port_t i2s_num, uint8_t intr_alloc_flags, void (*fn)(void*), void * arg, i2s_isr_handle_t *handle)
{
    return esp_intr_alloc(ETS_I2S0_INTR_SOURCE + i2s_num, intr_alloc_flags, fn, arg, handle);
}

esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, i2s_bits_per_sample_t bits, i2s_channel_t ch)
{
    int factor = (256%bits)? 384 : 256; // According to hardware codec requirement(supported 256fs or 384fs)
    int clkmInteger, clkmDecimals, bck = 0;
    double denom = (double)1 / 64;
    int channel = 2;
    i2s_dma_t *save_tx = NULL, *save_rx = NULL;

    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);

    if (bits % 8 != 0 || bits > I2S_BITS_PER_SAMPLE_32BIT || bits < I2S_BITS_PER_SAMPLE_16BIT) {
        ESP_LOGE(I2S_TAG, "Invalid bits per sample");
        return ESP_ERR_INVALID_ARG;
    }


    if (p_i2s_obj[i2s_num] == NULL) {
        ESP_LOGE(I2S_TAG, "Not initialized yet");
        return ESP_FAIL;
    }

    double clkmdiv = (double)I2S_BASE_CLK / (rate * factor);

    if (clkmdiv > 256) {
        ESP_LOGE(I2S_TAG, "clkmdiv is too large\r\n");
        return ESP_FAIL;
    }

    // wait all on-going writing finish
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) && p_i2s_obj[i2s_num]->tx) {
        xSemaphoreTake(p_i2s_obj[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
    }
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) && p_i2s_obj[i2s_num]->rx) {
        xSemaphoreTake(p_i2s_obj[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
    }

    i2s_stop(i2s_num);

    p_i2s_obj[i2s_num]->channel_num = (ch == 2) ? 2 : 1;

    if (bits != p_i2s_obj[i2s_num]->bits_per_sample) {

        //change fifo mode
        if (p_i2s_obj[i2s_num]->bits_per_sample <= 16 && bits > 16) {
            I2S[i2s_num]->fifo_conf.tx_fifo_mod += 2;
            I2S[i2s_num]->fifo_conf.rx_fifo_mod += 2;
        } else if (p_i2s_obj[i2s_num]->bits_per_sample > 16 && bits <= 16) {
            I2S[i2s_num]->fifo_conf.tx_fifo_mod -= 2;
            I2S[i2s_num]->fifo_conf.rx_fifo_mod -= 2;
        }

        p_i2s_obj[i2s_num]->bits_per_sample = bits;
        p_i2s_obj[i2s_num]->bytes_per_sample = p_i2s_obj[i2s_num]->bits_per_sample / 8;

        // Round bytes_per_sample up to next multiple of 16 bits
        int halfwords_per_sample = (p_i2s_obj[i2s_num]->bits_per_sample + 15) / 16;
        p_i2s_obj[i2s_num]->bytes_per_sample = halfwords_per_sample * 2;

        // Because limited of DMA buffer is 4092 bytes
        if (p_i2s_obj[i2s_num]->dma_buf_len * p_i2s_obj[i2s_num]->bytes_per_sample * p_i2s_obj[i2s_num]->channel_num > 4092) {
            p_i2s_obj[i2s_num]->dma_buf_len = 4092 / p_i2s_obj[i2s_num]->bytes_per_sample / p_i2s_obj[i2s_num]->channel_num;
        }
        // Re-create TX DMA buffer
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {

            save_tx = p_i2s_obj[i2s_num]->tx;

            p_i2s_obj[i2s_num]->tx = i2s_create_dma_queue(i2s_num, p_i2s_obj[i2s_num]->dma_buf_count, p_i2s_obj[i2s_num]->dma_buf_len);
            if (p_i2s_obj[i2s_num]->tx == NULL) {
                ESP_LOGE(I2S_TAG, "Failed to create tx dma buffer");
                i2s_driver_uninstall(i2s_num);
                return ESP_FAIL;
            }
            I2S[i2s_num]->out_link.addr = (uint32_t) p_i2s_obj[i2s_num]->tx->desc[0];

            //destroy old tx dma if exist
            if (save_tx) {
                i2s_destroy_dma_queue(i2s_num, save_tx);
            }
        }
        // Re-create RX DMA buffer
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {

            save_rx = p_i2s_obj[i2s_num]->rx;
            
            p_i2s_obj[i2s_num]->rx = i2s_create_dma_queue(i2s_num, p_i2s_obj[i2s_num]->dma_buf_count, p_i2s_obj[i2s_num]->dma_buf_len);
            if (p_i2s_obj[i2s_num]->rx == NULL){
                ESP_LOGE(I2S_TAG, "Failed to create rx dma buffer");
                i2s_driver_uninstall(i2s_num);
                return ESP_FAIL;
            }
            I2S[i2s_num]->rx_eof_num = (p_i2s_obj[i2s_num]->dma_buf_len * p_i2s_obj[i2s_num]->channel_num * p_i2s_obj[i2s_num]->bytes_per_sample)/4;
            I2S[i2s_num]->in_link.addr = (uint32_t) p_i2s_obj[i2s_num]->rx->desc[0];

            //destroy old rx dma if exist
            if (save_rx) {
                i2s_destroy_dma_queue(i2s_num, save_rx);
            }
        }
        
    }

    double mclk;
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_DAC_BUILT_IN) {
        //DAC uses bclk as sample clock, not WS. WS can be something arbitrary.
        //Rate as given to this function is the intended sample rate;
        //According to the TRM, WS clk equals to the sample rate, and bclk is double the speed of WS
        uint32_t b_clk = rate * 2;
        int factor2 = 60;
        mclk = b_clk * factor2;
        clkmdiv = ((double) I2S_BASE_CLK) / mclk;
        clkmInteger = clkmdiv;
        clkmDecimals = (clkmdiv - clkmInteger) / denom;
        bck = mclk / b_clk;
    } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_PDM) {
        uint32_t b_clk = 0;
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {
            int fp = I2S[i2s_num]->pdm_freq_conf.tx_pdm_fp;
            int fs = I2S[i2s_num]->pdm_freq_conf.tx_pdm_fs;
            b_clk = rate * 64 * (fp / fs);
        } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
            b_clk = rate * 64 * (I2S[i2s_num]->pdm_conf.rx_sinc_dsr_16_en + 1);
        }
        int factor2 = 5 ;
        mclk = b_clk * factor2;
        clkmdiv = ((double) I2S_BASE_CLK) / mclk;
        clkmInteger = clkmdiv;
        clkmDecimals = (clkmdiv - clkmInteger) / denom;
        bck = mclk / b_clk;
    } else {
        clkmInteger = clkmdiv;
        clkmDecimals = (clkmdiv - clkmInteger) / denom;
        mclk = clkmInteger + denom * clkmDecimals;
        bck = factor/(bits * channel);
    }

    I2S[i2s_num]->clkm_conf.clka_en = 0;
    I2S[i2s_num]->clkm_conf.clkm_div_a = 63;
    I2S[i2s_num]->clkm_conf.clkm_div_b = clkmDecimals;
    I2S[i2s_num]->clkm_conf.clkm_div_num = clkmInteger;
    I2S[i2s_num]->sample_rate_conf.tx_bck_div_num = bck;
    I2S[i2s_num]->sample_rate_conf.rx_bck_div_num = bck;
    I2S[i2s_num]->sample_rate_conf.tx_bits_mod = bits;
    I2S[i2s_num]->sample_rate_conf.rx_bits_mod = bits;
    double real_rate = (double) (I2S_BASE_CLK / (bck * bits * clkmInteger) / 2);
    ESP_LOGI(I2S_TAG, "Req RATE: %d, real rate: %0.3f, BITS: %u, CLKM: %u, BCK: %u, MCLK: %0.3f, SCLK: %f, diva: %d, divb: %d",
        rate, real_rate, bits, clkmInteger, bck, (double)I2S_BASE_CLK / mclk, real_rate*bits*channel, 64, clkmDecimals);

    // wait all writing on-going finish
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) && p_i2s_obj[i2s_num]->tx) {
        xSemaphoreGive(p_i2s_obj[i2s_num]->tx->mux);
    }
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) && p_i2s_obj[i2s_num]->rx) {
        xSemaphoreGive(p_i2s_obj[i2s_num]->rx->mux);
    }
    i2s_start(i2s_num);
    return ESP_OK;
}

static void IRAM_ATTR i2s_intr_handler_default(void *arg)
{
    i2s_obj_t *p_i2s = (i2s_obj_t*) arg;
    uint8_t i2s_num = p_i2s->i2s_num;
    i2s_dev_t* i2s_reg = I2S[i2s_num];
    i2s_event_t i2s_event;
    int dummy;

    portBASE_TYPE high_priority_task_awoken = 0;

    lldesc_t *finish_desc;

    if (i2s_reg->int_st.out_dscr_err || i2s_reg->int_st.in_dscr_err) {
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_DMA_ERROR;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }

    if (i2s_reg->int_st.out_eof && p_i2s->tx) {
        finish_desc = (lldesc_t*) i2s_reg->out_eof_des_addr;
        // All buffers are empty. This means we have an underflow on our hands.
        if (xQueueIsQueueFullFromISR(p_i2s->tx->queue)) {
            xQueueReceiveFromISR(p_i2s->tx->queue, &dummy, &high_priority_task_awoken);
        }
        xQueueSendFromISR(p_i2s->tx->queue, (void*)(&finish_desc->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_TX_DONE;
            if (xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }

    }

    if (i2s_reg->int_st.in_suc_eof && p_i2s->rx) {
        // All buffers are full. This means we have an overflow.
        finish_desc = (lldesc_t*) i2s_reg->in_eof_des_addr;
        if (xQueueIsQueueFullFromISR(p_i2s->rx->queue)) {
            xQueueReceiveFromISR(p_i2s->rx->queue, &dummy, &high_priority_task_awoken);
        }
        xQueueSendFromISR(p_i2s->rx->queue, (void*)(&finish_desc->buf), &high_priority_task_awoken);
        if (p_i2s->i2s_queue) {
            i2s_event.type = I2S_EVENT_RX_DONE;
            if (p_i2s->i2s_queue && xQueueIsQueueFullFromISR(p_i2s->i2s_queue)) {
                xQueueReceiveFromISR(p_i2s->i2s_queue, &dummy, &high_priority_task_awoken);
            }
            xQueueSendFromISR(p_i2s->i2s_queue, (void * )&i2s_event, &high_priority_task_awoken);
        }
    }
    if (high_priority_task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }

    i2s_reg->int_clr.val = I2S[i2s_num]->int_st.val;
}

static esp_err_t i2s_destroy_dma_queue(i2s_port_t i2s_num, i2s_dma_t *dma)
{
    int bux_idx;
    if (p_i2s_obj[i2s_num] == NULL) {
        ESP_LOGE(I2S_TAG, "Not initialized yet");
        return ESP_FAIL;
    }
    if (dma == NULL) {
        return ESP_FAIL;
    }
    for (bux_idx = 0; bux_idx < p_i2s_obj[i2s_num]->dma_buf_count; bux_idx++) {
        if (dma->desc && dma->desc[bux_idx])
            free(dma->desc[bux_idx]);
        if (dma->buf && dma->buf[bux_idx])
            free(dma->buf[bux_idx]);
    }
    if (dma->buf)
        free(dma->buf);
    if (dma->desc)
        free(dma->desc);
    vQueueDelete(dma->queue);
    vSemaphoreDelete(dma->mux);
    free(dma);
    return ESP_OK;
}

static i2s_dma_t *i2s_create_dma_queue(i2s_port_t i2s_num, int dma_buf_count, int dma_buf_len)
{
    int bux_idx;
    int sample_size = p_i2s_obj[i2s_num]->bytes_per_sample * p_i2s_obj[i2s_num]->channel_num;
    i2s_dma_t *dma = (i2s_dma_t*) malloc(sizeof(i2s_dma_t));
    if (dma == NULL) {
        ESP_LOGE(I2S_TAG, "Error malloc i2s_dma_t");
        return NULL;
    }
    memset(dma, 0, sizeof(i2s_dma_t));

    dma->buf = (char **)malloc(sizeof(char*) * dma_buf_count);
    if (dma->buf == NULL) {
        ESP_LOGE(I2S_TAG, "Error malloc dma buffer pointer");

        return NULL;
    }
    memset(dma->buf, 0, sizeof(char*) * dma_buf_count);

    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->buf[bux_idx] = (char*) malloc(dma_buf_len * sample_size);
        if (dma->buf[bux_idx] == NULL) {
            ESP_LOGE(I2S_TAG, "Error malloc dma buffer");
            i2s_destroy_dma_queue(i2s_num, dma);
            return NULL;
        }
        ESP_LOGD(I2S_TAG, "Addr[%d] = %d", bux_idx, (int)dma->buf[bux_idx]);
        memset(dma->buf[bux_idx], 0, dma_buf_len * sample_size);
    }

    dma->desc = (lldesc_t**) malloc(sizeof(lldesc_t*) * dma_buf_count);
    if (dma->desc == NULL) {
        ESP_LOGE(I2S_TAG, "Error malloc dma description");
        i2s_destroy_dma_queue(i2s_num, dma);
        return NULL;
    }
    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->desc[bux_idx] = (lldesc_t*) malloc(sizeof(lldesc_t));
        if (dma->desc[bux_idx] == NULL) {
            ESP_LOGE(I2S_TAG, "Error malloc dma description entry");
            i2s_destroy_dma_queue(i2s_num, dma);
            return NULL;
        }
    }

    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->desc[bux_idx]->owner = 1;
        dma->desc[bux_idx]->eof = 1;
        dma->desc[bux_idx]->sosf = 0;
        dma->desc[bux_idx]->length = dma_buf_len * sample_size;
        dma->desc[bux_idx]->size = dma_buf_len * sample_size;
        dma->desc[bux_idx]->buf = (uint8_t *) dma->buf[bux_idx];
        dma->desc[bux_idx]->offset = 0;
        dma->desc[bux_idx]->empty = (uint32_t)((bux_idx < (dma_buf_count - 1)) ? (dma->desc[bux_idx + 1]) : dma->desc[0]);
    }
    dma->queue = xQueueCreate(dma_buf_count - 1, sizeof(char*));
    dma->mux = xSemaphoreCreateMutex();
    dma->rw_pos = 0;
    dma->buf_size = dma_buf_len * sample_size;
    dma->curr_ptr = NULL;
    ESP_LOGI(I2S_TAG, "DMA Malloc info, datalen=blocksize=%d, dma_buf_count=%d", dma_buf_len * sample_size, dma_buf_count);
    return dma;
}


esp_err_t i2s_start(i2s_port_t i2s_num)
{
    //start DMA link
    I2S_ENTER_CRITICAL();
    esp_intr_disable(p_i2s_obj[i2s_num]->i2s_isr_handle);
    I2S[i2s_num]->int_clr.val = 0xFFFFFFFF;
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {
        i2s_enable_tx_intr(i2s_num);
        I2S[i2s_num]->out_link.start = 1;
        I2S[i2s_num]->conf.tx_start = 1;
    }
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
        i2s_enable_rx_intr(i2s_num);
        I2S[i2s_num]->in_link.start = 1;
        I2S[i2s_num]->conf.rx_start = 1;
    }
    esp_intr_enable(p_i2s_obj[i2s_num]->i2s_isr_handle);
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_stop(i2s_port_t i2s_num)
{
    I2S_ENTER_CRITICAL();
    esp_intr_disable(p_i2s_obj[i2s_num]->i2s_isr_handle);
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {
        I2S[i2s_num]->out_link.stop = 1;
        I2S[i2s_num]->conf.tx_start = 0;
        i2s_disable_tx_intr(i2s_num);
    }
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
        I2S[i2s_num]->in_link.stop = 1;
        I2S[i2s_num]->conf.rx_start = 0;
        i2s_disable_rx_intr(i2s_num);
    }
    I2S[i2s_num]->int_clr.val = I2S[i2s_num]->int_st.val; //clear pending interrupt
    i2s_reset_fifo(i2s_num);
    //reset dma
    I2S[i2s_num]->lc_conf.in_rst = 1;
    I2S[i2s_num]->lc_conf.in_rst = 0;
    I2S[i2s_num]->lc_conf.out_rst = 1;
    I2S[i2s_num]->lc_conf.out_rst = 0;
    I2S_EXIT_CRITICAL();
    return 0;
}

esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode)
{
    I2S_CHECK((dac_mode < I2S_DAC_CHANNEL_MAX), "i2s dac mode error", ESP_ERR_INVALID_ARG);
    if(dac_mode == I2S_DAC_CHANNEL_DISABLE) {
        dac_output_disable(DAC_CHANNEL_1);
        dac_output_disable(DAC_CHANNEL_1);
        dac_i2s_disable();
    } else {
        dac_i2s_enable();
    }

    if (dac_mode & I2S_DAC_CHANNEL_RIGHT_EN) {
        //DAC1, right channel, GPIO25
        dac_output_enable(DAC_CHANNEL_1);
    }
    if (dac_mode & I2S_DAC_CHANNEL_LEFT_EN) {
        //DAC2, left channel, GPIO26
        dac_output_enable(DAC_CHANNEL_2);
    }
    return ESP_OK;
}

esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (pin == NULL) {
        return i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    }
    if (pin->bck_io_num != -1 && !GPIO_IS_VALID_GPIO(pin->bck_io_num)) {
        ESP_LOGE(I2S_TAG, "bck_io_num error");
        return ESP_FAIL;
    }
    if (pin->ws_io_num != -1 && !GPIO_IS_VALID_GPIO(pin->ws_io_num)) {
        ESP_LOGE(I2S_TAG, "ws_io_num error");
        return ESP_FAIL;
    }
    if (pin->data_out_num != -1 && !GPIO_IS_VALID_GPIO(pin->data_out_num)) {
        ESP_LOGE(I2S_TAG, "data_out_num error");
        return ESP_FAIL;
    }
    if (pin->data_in_num != -1 && !GPIO_IS_VALID_GPIO(pin->data_in_num)) {
        ESP_LOGE(I2S_TAG, "data_in_num error");
        return ESP_FAIL;
    }

    int bck_sig = -1, ws_sig = -1, data_out_sig = -1, data_in_sig = -1;
    //Each IIS hw module has a RX and TX unit.
    //For TX unit, the output signal index should be I2SnO_xxx_OUT_IDX
    //For TX unit, the input signal index should be I2SnO_xxx_IN_IDX
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_MASTER) {
            if (i2s_num == I2S_NUM_0) {
                bck_sig = I2S0O_BCK_OUT_IDX;
                ws_sig = I2S0O_WS_OUT_IDX;
                data_out_sig = I2S0O_DATA_OUT23_IDX;
            } else {
                bck_sig = I2S1O_BCK_OUT_IDX;
                ws_sig = I2S1O_WS_OUT_IDX;
                data_out_sig = I2S1O_DATA_OUT23_IDX;
            }
        } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_SLAVE) {
            if (i2s_num == I2S_NUM_0) {
                bck_sig = I2S0O_BCK_IN_IDX;
                ws_sig = I2S0O_WS_IN_IDX;
                data_out_sig = I2S0O_DATA_OUT23_IDX;
            } else {
                bck_sig = I2S1O_BCK_IN_IDX;
                ws_sig = I2S1O_WS_IN_IDX;
                data_out_sig = I2S1O_DATA_OUT23_IDX;
            }
        }
    }
    //For RX unit, the output signal index should be I2SnI_xxx_OUT_IDX
    //For RX unit, the input signal index shuld be I2SnI_xxx_IN_IDX
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_MASTER) {
            if (i2s_num == I2S_NUM_0) {
                bck_sig = I2S0I_BCK_OUT_IDX;
                ws_sig = I2S0I_WS_OUT_IDX;
                data_in_sig = I2S0I_DATA_IN15_IDX;
            } else {
                bck_sig = I2S1I_BCK_OUT_IDX;
                ws_sig = I2S1I_WS_OUT_IDX;
                data_in_sig = I2S1I_DATA_IN15_IDX;
            }
        } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_SLAVE) {
            if (i2s_num == I2S_NUM_0) {
                bck_sig = I2S0I_BCK_IN_IDX;
                ws_sig = I2S0I_WS_IN_IDX;
                data_in_sig = I2S0I_DATA_IN15_IDX;
            } else {
                bck_sig = I2S1I_BCK_IN_IDX;
                ws_sig = I2S1I_WS_IN_IDX;
                data_in_sig = I2S1I_DATA_IN15_IDX;
            }
        }
    }
    //For "full-duplex + slave" mode, we should select RX signal index for ws and bck.
    //For "full-duplex + master" mode, we should select TX signal index for ws and bck.
    if ((p_i2s_obj[i2s_num]->mode & I2S_FULL_DUPLEX_SLAVE_MODE_MASK) == I2S_FULL_DUPLEX_SLAVE_MODE_MASK) {
        if (i2s_num == I2S_NUM_0) {
            bck_sig = I2S0I_BCK_IN_IDX;
            ws_sig = I2S0I_WS_IN_IDX;
        } else {
            bck_sig = I2S1I_BCK_IN_IDX;
            ws_sig = I2S1I_WS_IN_IDX;
        }
    } else if ((p_i2s_obj[i2s_num]->mode & I2S_FULL_DUPLEX_MASTER_MODE_MASK) == I2S_FULL_DUPLEX_MASTER_MODE_MASK) {
        if (i2s_num == I2S_NUM_0) {
            bck_sig = I2S0O_BCK_OUT_IDX;
            ws_sig = I2S0O_WS_OUT_IDX;
        } else {
            bck_sig = I2S1O_BCK_OUT_IDX;
            ws_sig = I2S1O_WS_OUT_IDX;
        }
    }

    gpio_matrix_out_check(pin->data_out_num, data_out_sig, 0, 0);
    gpio_matrix_in_check(pin->data_in_num, data_in_sig, 0);
    if (p_i2s_obj[i2s_num]->mode & I2S_MODE_MASTER) {
        gpio_matrix_out_check(pin->ws_io_num, ws_sig, 0, 0);
        gpio_matrix_out_check(pin->bck_io_num, bck_sig, 0, 0);
    } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_SLAVE) {
        gpio_matrix_in_check(pin->ws_io_num, ws_sig, 0);
        gpio_matrix_in_check(pin->bck_io_num, bck_sig, 0);
    }
    ESP_LOGD(I2S_TAG, "data: out %d, in: %d, ws: %d, bck: %d", data_out_sig, data_in_sig, ws_sig, bck_sig);

    return ESP_OK;
}

esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num]->bytes_per_sample > 0), "bits_per_sample not set", ESP_ERR_INVALID_ARG);
    return i2s_set_clk(i2s_num, rate, p_i2s_obj[i2s_num]->bits_per_sample, p_i2s_obj[i2s_num]->channel_num);
}
static esp_err_t i2s_param_config(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((i2s_config), "param null", ESP_ERR_INVALID_ARG);

    if (i2s_num == I2S_NUM_1) {
        periph_module_enable(PERIPH_I2S1_MODULE);
    } else {
        periph_module_enable(PERIPH_I2S0_MODULE);
    }

    // configure I2S data port interface.
    i2s_reset_fifo(i2s_num);

    //reset i2s
    I2S[i2s_num]->conf.tx_reset = 1;
    I2S[i2s_num]->conf.tx_reset = 0;
    I2S[i2s_num]->conf.rx_reset = 1;
    I2S[i2s_num]->conf.rx_reset = 0;


    //reset dma
    I2S[i2s_num]->lc_conf.in_rst = 1;
    I2S[i2s_num]->lc_conf.in_rst = 0;
    I2S[i2s_num]->lc_conf.out_rst = 1;
    I2S[i2s_num]->lc_conf.out_rst = 0;


    //Enable and configure DMA
    I2S[i2s_num]->lc_conf.check_owner = 0;
    I2S[i2s_num]->lc_conf.out_loop_test = 0;
    I2S[i2s_num]->lc_conf.out_auto_wrback = 0;
    I2S[i2s_num]->lc_conf.out_data_burst_en = 0;
    I2S[i2s_num]->lc_conf.outdscr_burst_en = 0;
    I2S[i2s_num]->lc_conf.out_no_restart_clr = 0;
    I2S[i2s_num]->lc_conf.indscr_burst_en = 0;
    I2S[i2s_num]->lc_conf.out_eof_mode = 1;


    I2S[i2s_num]->conf2.lcd_en = 0;
    I2S[i2s_num]->conf2.camera_en = 0;
    I2S[i2s_num]->pdm_conf.pcm2pdm_conv_en = 0;
    I2S[i2s_num]->pdm_conf.pdm2pcm_conv_en = 0;

    I2S[i2s_num]->fifo_conf.dscr_en = 0;

    I2S[i2s_num]->conf_chan.tx_chan_mod = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? i2s_config->channel_format : (i2s_config->channel_format >> 1); // 0-two channel;1-right;2-left;3-righ;4-left
    I2S[i2s_num]->fifo_conf.tx_fifo_mod = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? 0 : 1; // 0-right&left channel;1-one channel
    I2S[i2s_num]->conf.tx_mono = 0;

    I2S[i2s_num]->conf_chan.rx_chan_mod = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? i2s_config->channel_format : (i2s_config->channel_format >> 1); // 0-two channel;1-right;2-left;3-righ;4-left
    I2S[i2s_num]->fifo_conf.rx_fifo_mod = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? 0 : 1; // 0-right&left channel;1-one channel
    I2S[i2s_num]->conf.rx_mono = 0;

    I2S[i2s_num]->fifo_conf.dscr_en = 1;//connect dma to fifo

    I2S[i2s_num]->conf.tx_start = 0;
    I2S[i2s_num]->conf.rx_start = 0;

    if (i2s_config->mode & I2S_MODE_TX) {
        I2S[i2s_num]->conf.tx_msb_right = 0;
        I2S[i2s_num]->conf.tx_right_first = 0;

        I2S[i2s_num]->conf.tx_slave_mod = 0; // Master
        I2S[i2s_num]->fifo_conf.tx_fifo_mod_force_en = 1;

        if (i2s_config->mode & I2S_MODE_SLAVE) {
            I2S[i2s_num]->conf.tx_slave_mod = 1;//TX Slave
        }
    }

    if (i2s_config->mode & I2S_MODE_RX) {
        I2S[i2s_num]->conf.rx_msb_right = 0;
        I2S[i2s_num]->conf.rx_right_first = 0;
        I2S[i2s_num]->conf.rx_slave_mod = 0; // Master
        I2S[i2s_num]->fifo_conf.rx_fifo_mod_force_en = 1;
        
        if (i2s_config->mode & I2S_MODE_SLAVE) {
            I2S[i2s_num]->conf.rx_slave_mod = 1;//RX Slave
        }
    }

    if (i2s_config->mode & I2S_MODE_DAC_BUILT_IN) {
        I2S[i2s_num]->conf2.lcd_en = 1;
        I2S[i2s_num]->conf.tx_right_first = 1;
    }

    if (i2s_config->mode & I2S_MODE_PDM) {
        I2S[i2s_num]->fifo_conf.rx_fifo_mod_force_en = 1;
        I2S[i2s_num]->fifo_conf.tx_fifo_mod_force_en = 1;

        I2S[i2s_num]->pdm_freq_conf.tx_pdm_fp = 960;
        I2S[i2s_num]->pdm_freq_conf.tx_pdm_fs = i2s_config->sample_rate / 1000 * 10;
        I2S[i2s_num]->pdm_conf.tx_sinc_osr2 = I2S[i2s_num]->pdm_freq_conf.tx_pdm_fp / I2S[i2s_num]->pdm_freq_conf.tx_pdm_fs;

        I2S[i2s_num]->pdm_conf.rx_sinc_dsr_16_en = 0;
        I2S[i2s_num]->pdm_conf.rx_pdm_en = 1;
        I2S[i2s_num]->pdm_conf.tx_pdm_en = 1;

        I2S[i2s_num]->pdm_conf.pcm2pdm_conv_en = 1;
        I2S[i2s_num]->pdm_conf.pdm2pcm_conv_en = 1;
    } else {
        I2S[i2s_num]->pdm_conf.rx_pdm_en = 0;
        I2S[i2s_num]->pdm_conf.tx_pdm_en = 0;
    }
    if (i2s_config->communication_format & I2S_COMM_FORMAT_I2S) {
        I2S[i2s_num]->conf.tx_short_sync = 0;
        I2S[i2s_num]->conf.rx_short_sync = 0;
        I2S[i2s_num]->conf.tx_msb_shift = 1;
        I2S[i2s_num]->conf.rx_msb_shift = 1;
        if (i2s_config->communication_format & I2S_COMM_FORMAT_I2S_LSB) {
            if (i2s_config->mode & I2S_MODE_TX) {
                I2S[i2s_num]->conf.tx_msb_shift = 0;
            }
            if (i2s_config->mode & I2S_MODE_RX) {
                I2S[i2s_num]->conf.rx_msb_shift = 0;
            }
        }
    }

    if (i2s_config->communication_format & I2S_COMM_FORMAT_PCM) {
        I2S[i2s_num]->conf.tx_msb_shift = 0;
        I2S[i2s_num]->conf.rx_msb_shift = 0;
        I2S[i2s_num]->conf.tx_short_sync = 0;
        I2S[i2s_num]->conf.rx_short_sync = 0;
        if (i2s_config->communication_format & I2S_COMM_FORMAT_PCM_SHORT) {
            if (i2s_config->mode & I2S_MODE_TX) {
                I2S[i2s_num]->conf.tx_short_sync = 1;
            }
            if (i2s_config->mode & I2S_MODE_RX) {
                I2S[i2s_num]->conf.rx_short_sync = 1;
            }
        }
    }
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) &&  (p_i2s_obj[i2s_num]->mode & I2S_MODE_TX)) {
        I2S[i2s_num]->conf.sig_loopback = 1;
        if (p_i2s_obj[i2s_num]->mode & I2S_MODE_MASTER) {
            I2S[i2s_num]->conf.tx_slave_mod = 0;    //MASTER Slave
            I2S[i2s_num]->conf.rx_slave_mod = 1;    //RX Slave
        } else {
            I2S[i2s_num]->conf.tx_slave_mod = 1;    //RX Slave
            I2S[i2s_num]->conf.rx_slave_mod = 1;    //RX Slave
        }
    }
    return ESP_OK;
}

esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num]->rx && p_i2s_obj[i2s_num]->rx->buf != NULL && p_i2s_obj[i2s_num]->rx->buf_size != 0) {
        for (int i = 0; i < p_i2s_obj[i2s_num]->dma_buf_count; i++) {
            memset(p_i2s_obj[i2s_num]->rx->buf[i], 0, p_i2s_obj[i2s_num]->rx->buf_size);
        }
    }
    if (p_i2s_obj[i2s_num]->tx && p_i2s_obj[i2s_num]->tx->buf != NULL && p_i2s_obj[i2s_num]->tx->buf_size != 0) {
        for (int i = 0; i < p_i2s_obj[i2s_num]->dma_buf_count; i++) {
            memset(p_i2s_obj[i2s_num]->tx->buf[i], 0, p_i2s_obj[i2s_num]->tx->buf_size);
        }
    }
    return ESP_OK;
}

esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void* i2s_queue)
{
    esp_err_t err;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((i2s_config != NULL), "I2S configuration must not NULL", ESP_ERR_INVALID_ARG);
    I2S_CHECK((i2s_config->dma_buf_count >= 2 && i2s_config->dma_buf_count <= 128), "I2S buffer count less than 128 and more than 2", ESP_ERR_INVALID_ARG);
    I2S_CHECK((i2s_config->dma_buf_len >= 8 && i2s_config->dma_buf_len <= 1024), "I2S buffer length at most 1024 and more than 8", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num] == NULL) {
        p_i2s_obj[i2s_num] = (i2s_obj_t*) malloc(sizeof(i2s_obj_t));
        if (p_i2s_obj[i2s_num] == NULL) {
            ESP_LOGE(I2S_TAG, "Malloc I2S driver error");
            return ESP_FAIL;
        }
        memset(p_i2s_obj[i2s_num], 0, sizeof(i2s_obj_t));

        p_i2s_obj[i2s_num]->i2s_num = i2s_num;
        p_i2s_obj[i2s_num]->dma_buf_count = i2s_config->dma_buf_count;
        p_i2s_obj[i2s_num]->dma_buf_len = i2s_config->dma_buf_len;
        p_i2s_obj[i2s_num]->i2s_queue = i2s_queue;
        p_i2s_obj[i2s_num]->mode = i2s_config->mode;

        p_i2s_obj[i2s_num]->bits_per_sample = 0;
        p_i2s_obj[i2s_num]->bytes_per_sample = 0; // Not initialized yet
        p_i2s_obj[i2s_num]->channel_num = i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? 2 : 1;

        //To make sure hardware is enabled before any hardware register operations.
        if (i2s_num == I2S_NUM_1) {
            periph_module_enable(PERIPH_I2S1_MODULE);
        } else {
            periph_module_enable(PERIPH_I2S0_MODULE);
        }

        //initial interrupt
        err = i2s_isr_register(i2s_num, i2s_config->intr_alloc_flags, i2s_intr_handler_default, p_i2s_obj[i2s_num], &p_i2s_obj[i2s_num]->i2s_isr_handle);
        if (err != ESP_OK) {
            free(p_i2s_obj[i2s_num]);
            ESP_LOGE(I2S_TAG, "Register I2S Interrupt error");
            return ESP_FAIL;
        }
        i2s_stop(i2s_num);
        i2s_param_config(i2s_num, i2s_config);

        if (i2s_queue) {
            p_i2s_obj[i2s_num]->i2s_queue = xQueueCreate(queue_size, sizeof(i2s_event_t));
            *((QueueHandle_t*) i2s_queue) = p_i2s_obj[i2s_num]->i2s_queue;
            ESP_LOGI(I2S_TAG, "queue free spaces: %d", uxQueueSpacesAvailable(p_i2s_obj[i2s_num]->i2s_queue));
        } else {
            p_i2s_obj[i2s_num]->i2s_queue = NULL;
        }
        //set clock and start
        return i2s_set_clk(i2s_num, i2s_config->sample_rate, i2s_config->bits_per_sample, p_i2s_obj[i2s_num]->channel_num);
    }

    ESP_LOGE(I2S_TAG, "I2S driver already installed");
    return ESP_ERR_INVALID_STATE;
}

esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num] == NULL) {
        ESP_LOGI(I2S_TAG, "ALREADY NULL");
        return ESP_OK;
    }
    i2s_stop(i2s_num);
    esp_intr_free(p_i2s_obj[i2s_num]->i2s_isr_handle);

    if (p_i2s_obj[i2s_num]->tx != NULL && p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) {
        i2s_destroy_dma_queue(i2s_num, p_i2s_obj[i2s_num]->tx);
        p_i2s_obj[i2s_num]->tx = NULL;
    }
    if (p_i2s_obj[i2s_num]->rx != NULL && p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
        i2s_destroy_dma_queue(i2s_num, p_i2s_obj[i2s_num]->rx);
        p_i2s_obj[i2s_num]->rx = NULL;
    }

    if (p_i2s_obj[i2s_num]->i2s_queue) {
        vQueueDelete(p_i2s_obj[i2s_num]->i2s_queue);
        p_i2s_obj[i2s_num]->i2s_queue = NULL;
    }

    free(p_i2s_obj[i2s_num]);
    p_i2s_obj[i2s_num] = NULL;

    if (i2s_num == I2S_NUM_0) {
        periph_module_disable(PERIPH_I2S0_MODULE);
    } else if (i2s_num == I2S_NUM_1) {
        periph_module_disable(PERIPH_I2S1_MODULE);
    }
    return ESP_OK;
}

int i2s_write_bytes(i2s_port_t i2s_num, const char *src, size_t size, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_write, bytes_writen = 0;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num]->tx == NULL) {
        return 0;
    }
    xSemaphoreTake(p_i2s_obj[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
    while (size > 0) {
        if (p_i2s_obj[i2s_num]->tx->rw_pos == p_i2s_obj[i2s_num]->tx->buf_size || p_i2s_obj[i2s_num]->tx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s_obj[i2s_num]->tx->queue, &p_i2s_obj[i2s_num]->tx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s_obj[i2s_num]->tx->rw_pos = 0;
        }
        ESP_LOGD(I2S_TAG, "size: %d, rw_pos: %d, buf_size: %d, curr_ptr: %d", size, p_i2s_obj[i2s_num]->tx->rw_pos, p_i2s_obj[i2s_num]->tx->buf_size, (int)p_i2s_obj[i2s_num]->tx->curr_ptr);
        data_ptr = (char*)p_i2s_obj[i2s_num]->tx->curr_ptr;
        data_ptr += p_i2s_obj[i2s_num]->tx->rw_pos;
        bytes_can_write = p_i2s_obj[i2s_num]->tx->buf_size - p_i2s_obj[i2s_num]->tx->rw_pos;
        if (bytes_can_write > size) {
            bytes_can_write = size;
        }
        memcpy(data_ptr, src, bytes_can_write);
        size -= bytes_can_write;
        src += bytes_can_write;
        p_i2s_obj[i2s_num]->tx->rw_pos += bytes_can_write;
        bytes_writen += bytes_can_write;
    }
    xSemaphoreGive(p_i2s_obj[i2s_num]->tx->mux);
    return bytes_writen;
}

int i2s_read_bytes(i2s_port_t i2s_num, char* dest, size_t size, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_read, byte_read = 0;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num]->rx == NULL) {
        return 0;
    }
    xSemaphoreTake(p_i2s_obj[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
    while (size > 0) {
        if (p_i2s_obj[i2s_num]->rx->rw_pos == p_i2s_obj[i2s_num]->rx->buf_size || p_i2s_obj[i2s_num]->rx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s_obj[i2s_num]->rx->queue, &p_i2s_obj[i2s_num]->rx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s_obj[i2s_num]->rx->rw_pos = 0;
        }
        data_ptr = (char*)p_i2s_obj[i2s_num]->rx->curr_ptr;
        data_ptr += p_i2s_obj[i2s_num]->rx->rw_pos;
        bytes_can_read = p_i2s_obj[i2s_num]->rx->buf_size - p_i2s_obj[i2s_num]->rx->rw_pos;
        if (bytes_can_read > size) {
            bytes_can_read = size;
        }
        memcpy(dest, data_ptr, bytes_can_read);
        size -= bytes_can_read;
        dest += bytes_can_read;
        p_i2s_obj[i2s_num]->rx->rw_pos += bytes_can_read;
        byte_read += bytes_can_read;
    }
    xSemaphoreGive(p_i2s_obj[i2s_num]->rx->mux);
    return byte_read;
}
int i2s_push_sample(i2s_port_t i2s_num, const char *sample, TickType_t ticks_to_wait)
{
    int i, bytes_to_push = 0;
    char *data_ptr;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num]->tx->rw_pos == p_i2s_obj[i2s_num]->tx->buf_size || p_i2s_obj[i2s_num]->tx->curr_ptr == NULL) {
        if (xQueueReceive(p_i2s_obj[i2s_num]->tx->queue, &p_i2s_obj[i2s_num]->tx->curr_ptr, ticks_to_wait) == pdFALSE) {
            return 0;
        }
        ESP_LOGD(I2S_TAG, "rw_pos: %d, buf_size: %d, curr_ptr: %d", p_i2s_obj[i2s_num]->tx->rw_pos, p_i2s_obj[i2s_num]->tx->buf_size, (int)p_i2s_obj[i2s_num]->tx->curr_ptr);
        p_i2s_obj[i2s_num]->tx->rw_pos = 0;
    }
    data_ptr = (char*)p_i2s_obj[i2s_num]->tx->curr_ptr;
    data_ptr += p_i2s_obj[i2s_num]->tx->rw_pos;
    for (i = 0; i < p_i2s_obj[i2s_num]->bytes_per_sample * p_i2s_obj[i2s_num]->channel_num; i++) {
        *data_ptr++ = *sample++;
        bytes_to_push ++;
    }
    p_i2s_obj[i2s_num]->tx->rw_pos += bytes_to_push;
    return bytes_to_push;
}

int i2s_pop_sample(i2s_port_t i2s_num, char *sample, TickType_t ticks_to_wait)
{
    int  i, bytes_to_pop = 0;
    char *data_ptr;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num]->rx->rw_pos == p_i2s_obj[i2s_num]->rx->buf_size || p_i2s_obj[i2s_num]->rx->curr_ptr == NULL) {
        if (xQueueReceive(p_i2s_obj[i2s_num]->rx->queue, &p_i2s_obj[i2s_num]->rx->curr_ptr, ticks_to_wait) == pdFALSE) {
            return 0;
        }
        p_i2s_obj[i2s_num]->rx->rw_pos = 0;
    }
    data_ptr = (char*)p_i2s_obj[i2s_num]->rx->curr_ptr;
    data_ptr += p_i2s_obj[i2s_num]->rx->rw_pos;
    for (i = 0; i < p_i2s_obj[i2s_num]->bytes_per_sample; i++) {
        *sample++ = *data_ptr++;
        bytes_to_pop++;
    }
    if (p_i2s_obj[i2s_num]->channel_num == 2) {
        for (i = 0; i < p_i2s_obj[i2s_num]->bytes_per_sample; i++) {
            *sample++ = *data_ptr++;
            bytes_to_pop++;
        }
    }

    p_i2s_obj[i2s_num]->rx->rw_pos += p_i2s_obj[i2s_num]->bytes_per_sample * p_i2s_obj[i2s_num]->channel_num;
    return bytes_to_pop;
}
