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
#include <math.h>
#include <esp_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"

#include "soc/rtc_periph.h"
#include "soc/rtc.h"
#include "soc/efuse_periph.h"
#include "esp32/rom/lldesc.h"

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "adc1_i2s_private.h"

#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"

static const char* I2S_TAG = "I2S";

#define I2S_CHECK(a, str, ret) if (!(a)) {                                              \
        ESP_LOGE(I2S_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);       \
        return (ret);                                                                   \
        }
#define I2S_MAX_BUFFER_SIZE               (4 * 1024 * 1024) //the maximum RAM can be allocated
#define I2S_BASE_CLK                      (2*APB_CLK_FREQ)
#define I2S_ENTER_CRITICAL_ISR()          portENTER_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL_ISR()           portEXIT_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_ENTER_CRITICAL()              portENTER_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL()               portEXIT_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_FULL_DUPLEX_SLAVE_MODE_MASK   (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_SLAVE)
#define I2S_FULL_DUPLEX_MASTER_MODE_MASK  (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_MASTER)
#define APLL_MIN_FREQ                     (250000000)
#define APLL_MAX_FREQ                     (500000000)
#define APLL_I2S_MIN_RATE                 (10675) //in Hz, I2S Clock rate limited by hardware
#define I2S_AD_BCK_FACTOR                 (2)
#define I2S_PDM_BCK_FACTOR                (64)
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
    uint32_t sample_rate;              /*!< I2S sample rate */
    bool use_apll;               /*!< I2S use APLL clock */
    bool tx_desc_auto_clear;    /*!< I2S auto clear tx descriptor on underflow */
    int fixed_mclk;             /*!< I2S fixed MLCK clock */
    double real_rate;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} i2s_obj_t;

static i2s_obj_t *p_i2s_obj[I2S_NUM_MAX] = {0};
static i2s_dev_t* I2S[I2S_NUM_MAX] = {&I2S0, &I2S1};
static portMUX_TYPE i2s_spinlock[I2S_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};
static int _i2s_adc_unit = -1;
static int _i2s_adc_channel = -1;

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

float i2s_get_clk(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    return p_i2s_obj[i2s_num]->real_rate;
}

static esp_err_t i2s_isr_register(i2s_port_t i2s_num, int intr_alloc_flags, void (*fn)(void*), void * arg, i2s_isr_handle_t *handle)
{
    return esp_intr_alloc(ETS_I2S0_INTR_SOURCE + i2s_num, intr_alloc_flags, fn, arg, handle);
}


static float i2s_apll_get_fi2s(int bits_per_sample, int sdm0, int sdm1, int sdm2, int odir)
{
    int f_xtal = (int)rtc_clk_xtal_freq_get() * 1000000;
    uint32_t is_rev0 = (GET_PERI_REG_BITS2(EFUSE_BLK0_RDATA3_REG, 1, 15) == 0);
    if (is_rev0) {
        sdm0 = 0;
        sdm1 = 0;
    }
    float fout = f_xtal * (sdm2 + sdm1 / 256.0f + sdm0 / 65536.0f + 4);
    if (fout < APLL_MIN_FREQ || fout > APLL_MAX_FREQ) {
        return APLL_MAX_FREQ;
    }
    float fpll = fout / (2 * (odir+2)); //== fi2s (N=1, b=0, a=1)
    return fpll/2;
}

/**
 * @brief     APLL calculate function, was described by following:
 *            APLL Output frequency is given by the formula:
 *
 *            apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)/((o_div + 2) * 2)
 *            apll_freq = fout / ((o_div + 2) * 2)
 *
 *            The dividend in this expression should be in the range of 240 - 600 MHz.
 *            In rev. 0 of ESP32, sdm0 and sdm1 are unused and always set to 0.
 *            * sdm0  frequency adjustment parameter, 0..255
 *            * sdm1  frequency adjustment parameter, 0..255
 *            * sdm2  frequency adjustment parameter, 0..63
 *            * o_div  frequency divider, 0..31
 *
 *            The most accurate way to find the sdm0..2 and odir parameters is to loop through them all,
 *            then apply the above formula, finding the closest frequency to the desired one.
 *            But 256*256*64*32 = 134.217.728 loops are too slow with ESP32
 *            1. We will choose the parameters with the highest level of change,
 *               With 350MHz<fout<500MHz, we limit the sdm2 from 4 to 9,
 *               Take average frequency close to the desired frequency, and select sdm2
 *            2. Next, we look for sequences of less influential and more detailed parameters,
 *               also by taking the average of the largest and smallest frequencies closer to the desired frequency.
 *            3. And finally, loop through all the most detailed of the parameters, finding the best desired frequency
 *
 * @param[in]  rate                  The I2S Frequency (MCLK)
 * @param[in]  bits_per_sample       The bits per sample
 * @param[out]      sdm0             The sdm 0
 * @param[out]      sdm1             The sdm 1
 * @param[out]      sdm2             The sdm 2
 * @param[out]      odir             The odir
 *
 * @return     ESP_ERR_INVALID_ARG or ESP_OK
 */

static esp_err_t i2s_apll_calculate_fi2s(int rate, int bits_per_sample, int *sdm0, int *sdm1, int *sdm2, int *odir)
{
    int _odir, _sdm0, _sdm1, _sdm2;
    float avg;
    float min_rate, max_rate, min_diff;
    if (rate/bits_per_sample/2/8 < APLL_I2S_MIN_RATE) {
        return ESP_ERR_INVALID_ARG;
    }

    *sdm0 = 0;
    *sdm1 = 0;
    *sdm2 = 0;
    *odir = 0;
    min_diff = APLL_MAX_FREQ;

    for (_sdm2 = 4; _sdm2 < 9; _sdm2 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, _sdm2, 0);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, _sdm2, 31);
        avg = (max_rate + min_rate)/2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm2 = _sdm2;
        }
    }
    min_diff = APLL_MAX_FREQ;
    for (_odir = 0; _odir < 32; _odir ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, *sdm2, _odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, *sdm2, _odir);
        avg = (max_rate + min_rate)/2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *odir = _odir;
        }
    }
    min_diff = APLL_MAX_FREQ;
    for (_sdm2 = 4; _sdm2 < 9; _sdm2 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, _sdm2, *odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, _sdm2, *odir);
        avg = (max_rate + min_rate)/2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm2 = _sdm2;
        }
    }

    min_diff = APLL_MAX_FREQ;
    for (_sdm1 = 0; _sdm1 < 256; _sdm1 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, _sdm1, *sdm2, *odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, _sdm1, *sdm2, *odir);
        avg = (max_rate + min_rate)/2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm1 = _sdm1;
        }
    }

    min_diff = APLL_MAX_FREQ;
    for (_sdm0 = 0; _sdm0 < 256; _sdm0 ++) {
        avg = i2s_apll_get_fi2s(bits_per_sample, _sdm0, *sdm1, *sdm2, *odir);
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm0 = _sdm0;
        }
    }

    return ESP_OK;
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
        return ESP_ERR_INVALID_ARG;
    }
    p_i2s_obj[i2s_num]->sample_rate = rate;
    double clkmdiv = (double)I2S_BASE_CLK / (rate * factor);

    if (clkmdiv > 256) {
        ESP_LOGE(I2S_TAG, "clkmdiv is too large\r\n");
        return ESP_ERR_INVALID_ARG;
    }

    // wait all on-going writing finish
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_TX) && p_i2s_obj[i2s_num]->tx) {
        xSemaphoreTake(p_i2s_obj[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
    }
    if ((p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) && p_i2s_obj[i2s_num]->rx) {
        xSemaphoreTake(p_i2s_obj[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
    }

    i2s_stop(i2s_num);


    uint32_t cur_mode = 0;
    if (p_i2s_obj[i2s_num]->channel_num != ch) {
        p_i2s_obj[i2s_num]->channel_num = (ch == 2) ? 2 : 1;
        cur_mode = I2S[i2s_num]->fifo_conf.tx_fifo_mod;
        I2S[i2s_num]->fifo_conf.tx_fifo_mod = (ch == 2) ? cur_mode - 1 : cur_mode + 1;
        cur_mode = I2S[i2s_num]->fifo_conf.rx_fifo_mod;
        I2S[i2s_num]->fifo_conf.rx_fifo_mod = (ch == 2) ? cur_mode -1  : cur_mode + 1;
        I2S[i2s_num]->conf_chan.tx_chan_mod = (ch == 2) ? 0 : 1;
        I2S[i2s_num]->conf_chan.rx_chan_mod = (ch == 2) ? 0 : 1;
    }

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
                return ESP_ERR_NO_MEM;
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
                return ESP_ERR_NO_MEM;
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
    int sdm0, sdm1, sdm2, odir, m_scale = 8;
    int fi2s_clk = rate*channel*bits*m_scale;
    if (p_i2s_obj[i2s_num]->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        //DAC uses bclk as sample clock, not WS. WS can be something arbitrary.
        //Rate as given to this function is the intended sample rate;
        //According to the TRM, WS clk equals to the sample rate, and bclk is double the speed of WS
        uint32_t b_clk = rate * I2S_AD_BCK_FACTOR;
        fi2s_clk /= I2S_AD_BCK_FACTOR;
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
            b_clk = rate * I2S_PDM_BCK_FACTOR * (fp / fs);
            fi2s_clk /= (I2S_PDM_BCK_FACTOR * (fp / fs));
        } else if (p_i2s_obj[i2s_num]->mode & I2S_MODE_RX) {
            b_clk = rate * I2S_PDM_BCK_FACTOR * (I2S[i2s_num]->pdm_conf.rx_sinc_dsr_16_en + 1);
            fi2s_clk /= (I2S_PDM_BCK_FACTOR * (I2S[i2s_num]->pdm_conf.rx_sinc_dsr_16_en + 1));
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

    if(p_i2s_obj[i2s_num]->use_apll && p_i2s_obj[i2s_num]->fixed_mclk) {
        fi2s_clk = p_i2s_obj[i2s_num]->fixed_mclk;
        m_scale = fi2s_clk/bits/rate/channel;
    }
    if(p_i2s_obj[i2s_num]->use_apll && i2s_apll_calculate_fi2s(fi2s_clk, bits, &sdm0, &sdm1, &sdm2, &odir) == ESP_OK) {
        ESP_LOGD(I2S_TAG, "sdm0=%d, sdm1=%d, sdm2=%d, odir=%d", sdm0, sdm1, sdm2, odir);
        rtc_clk_apll_enable(1, sdm0, sdm1, sdm2, odir);
        I2S[i2s_num]->clkm_conf.clkm_div_num = 1;
        I2S[i2s_num]->clkm_conf.clkm_div_b = 0;
        I2S[i2s_num]->clkm_conf.clkm_div_a = 1;
        I2S[i2s_num]->sample_rate_conf.tx_bck_div_num = m_scale;
        I2S[i2s_num]->sample_rate_conf.rx_bck_div_num = m_scale;
        I2S[i2s_num]->clkm_conf.clka_en = 1;
        double fi2s_rate = i2s_apll_get_fi2s(bits, sdm0, sdm1, sdm2, odir);
        p_i2s_obj[i2s_num]->real_rate = fi2s_rate/bits/channel/m_scale;
        ESP_LOGI(I2S_TAG, "APLL: Req RATE: %d, real rate: %0.3f, BITS: %u, CLKM: %u, BCK_M: %u, MCLK: %0.3f, SCLK: %f, diva: %d, divb: %d",
            rate, fi2s_rate/bits/channel/m_scale, bits, 1, m_scale, fi2s_rate, fi2s_rate/8, 1, 0);
    } else {
        I2S[i2s_num]->clkm_conf.clka_en = 0;
        I2S[i2s_num]->clkm_conf.clkm_div_a = 63;
        I2S[i2s_num]->clkm_conf.clkm_div_b = clkmDecimals;
        I2S[i2s_num]->clkm_conf.clkm_div_num = clkmInteger;
        I2S[i2s_num]->sample_rate_conf.tx_bck_div_num = bck;
        I2S[i2s_num]->sample_rate_conf.rx_bck_div_num = bck;
        double real_rate = (double) (I2S_BASE_CLK / (bck * bits * clkmInteger) / 2);
        p_i2s_obj[i2s_num]->real_rate = real_rate;
        ESP_LOGI(I2S_TAG, "PLL_D2: Req RATE: %d, real rate: %0.3f, BITS: %u, CLKM: %u, BCK: %u, MCLK: %0.3f, SCLK: %f, diva: %d, divb: %d",
            rate, real_rate, bits, clkmInteger, bck, (double)I2S_BASE_CLK / mclk, real_rate*bits*channel, 64, clkmDecimals);
    }

    I2S[i2s_num]->sample_rate_conf.tx_bits_mod = bits;
    I2S[i2s_num]->sample_rate_conf.rx_bits_mod = bits;

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
        ESP_EARLY_LOGE(I2S_TAG, "dma error, interrupt status: 0x%08x", i2s_reg->int_st.val);
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
            // See if tx descriptor needs to be auto cleared:
            // This will avoid any kind of noise that may get introduced due to transmission
            // of previous data from tx descriptor on I2S line.
            if (p_i2s->tx_desc_auto_clear == true) {
                memset((void *) dummy, 0, p_i2s->tx->buf_size);
            }
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
        return ESP_ERR_INVALID_ARG;
    }
    if (dma == NULL) {
        ESP_LOGE(I2S_TAG, "dma is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    for (bux_idx = 0; bux_idx < p_i2s_obj[i2s_num]->dma_buf_count; bux_idx++) {
        if (dma->desc && dma->desc[bux_idx]) {
            free(dma->desc[bux_idx]);
        }
        if (dma->buf && dma->buf[bux_idx]) {
            free(dma->buf[bux_idx]);
        }
    }
    if (dma->buf) {
        free(dma->buf);
    }
    if (dma->desc) {
        free(dma->desc);
    }
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
        free(dma);
        return NULL;
    }
    memset(dma->buf, 0, sizeof(char*) * dma_buf_count);

    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->buf[bux_idx] = (char*) heap_caps_calloc(1, dma_buf_len * sample_size, MALLOC_CAP_DMA);
        if (dma->buf[bux_idx] == NULL) {
            ESP_LOGE(I2S_TAG, "Error malloc dma buffer");
            i2s_destroy_dma_queue(i2s_num, dma);
            return NULL;
        }
        ESP_LOGD(I2S_TAG, "Addr[%d] = %d", bux_idx, (int)dma->buf[bux_idx]);
    }

    dma->desc = (lldesc_t**) malloc(sizeof(lldesc_t*) * dma_buf_count);
    if (dma->desc == NULL) {
        ESP_LOGE(I2S_TAG, "Error malloc dma description");
        i2s_destroy_dma_queue(i2s_num, dma);
        return NULL;
    }
    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->desc[bux_idx] = (lldesc_t*) heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
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
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    //start DMA link
    I2S_ENTER_CRITICAL();
    i2s_reset_fifo(i2s_num);
    //reset dma
    I2S[i2s_num]->lc_conf.in_rst = 1;
    I2S[i2s_num]->lc_conf.in_rst = 0;
    I2S[i2s_num]->lc_conf.out_rst = 1;
    I2S[i2s_num]->lc_conf.out_rst = 0;

    I2S[i2s_num]->conf.tx_reset = 1;
    I2S[i2s_num]->conf.tx_reset = 0;
    I2S[i2s_num]->conf.rx_reset = 1;
    I2S[i2s_num]->conf.rx_reset = 0;

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
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
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
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode)
{
    I2S_CHECK((dac_mode < I2S_DAC_CHANNEL_MAX), "i2s dac mode error", ESP_ERR_INVALID_ARG);
    if (dac_mode == I2S_DAC_CHANNEL_DISABLE) {
        dac_output_disable(DAC_CHANNEL_1);
        dac_output_disable(DAC_CHANNEL_2);
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

static esp_err_t _i2s_adc_mode_recover()
{
    I2S_CHECK(((_i2s_adc_unit != -1) && (_i2s_adc_channel != -1)), "i2s ADC recover error, not initialized...", ESP_ERR_INVALID_ARG);
    return adc_i2s_mode_init(_i2s_adc_unit, _i2s_adc_channel);
}

esp_err_t i2s_set_adc_mode(adc_unit_t adc_unit, adc1_channel_t adc_channel)
{
    I2S_CHECK((adc_unit < ADC_UNIT_2), "i2s ADC unit error, only support ADC1 for now", ESP_ERR_INVALID_ARG);
    // For now, we only support SAR ADC1.
    _i2s_adc_unit = adc_unit;
    _i2s_adc_channel = adc_channel;
    return adc_i2s_mode_init(adc_unit, adc_channel);
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
    if (pin->data_out_num != -1 && !GPIO_IS_VALID_OUTPUT_GPIO(pin->data_out_num)) {
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

esp_err_t i2s_set_pdm_rx_down_sample(i2s_port_t i2s_num, i2s_pdm_dsr_t dsr)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S[i2s_num]->pdm_conf.rx_sinc_dsr_16_en = dsr;
    return i2s_set_clk(i2s_num, p_i2s_obj[i2s_num]->sample_rate, p_i2s_obj[i2s_num]->bits_per_sample, p_i2s_obj[i2s_num]->channel_num);
}

static esp_err_t i2s_param_config(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((i2s_config), "param null", ESP_ERR_INVALID_ARG);
    I2S_CHECK(!((i2s_config->mode & I2S_MODE_ADC_BUILT_IN) && (i2s_num != I2S_NUM_0)), "I2S ADC built-in only support on I2S0", ESP_ERR_INVALID_ARG);
    I2S_CHECK(!((i2s_config->mode & I2S_MODE_DAC_BUILT_IN) && (i2s_num != I2S_NUM_0)), "I2S DAC built-in only support on I2S0", ESP_ERR_INVALID_ARG);
    I2S_CHECK(!((i2s_config->mode & I2S_MODE_PDM) && (i2s_num != I2S_NUM_0)), "I2S DAC PDM only support on I2S0", ESP_ERR_INVALID_ARG);

    if(i2s_config->mode & I2S_MODE_ADC_BUILT_IN) {
        //in ADC built-in mode, we need to call i2s_set_adc_mode to
        //initialize the specific ADC channel.
        //in the current stage, we only support ADC1 and single channel mode.
        //In default data mode, the ADC data is in 12-bit resolution mode.
        adc_power_always_on();
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

    if (i2s_config->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        I2S[i2s_num]->conf2.lcd_en = 1;
        I2S[i2s_num]->conf.tx_right_first = 1;
        I2S[i2s_num]->conf2.camera_en = 0;
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

    p_i2s_obj[i2s_num]->use_apll = i2s_config->use_apll;
    p_i2s_obj[i2s_num]->tx_desc_auto_clear = i2s_config->tx_desc_auto_clear;
    p_i2s_obj[i2s_num]->fixed_mclk = i2s_config->fixed_mclk;
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
        int bytes_left = 0;
        bytes_left = (p_i2s_obj[i2s_num]->tx->buf_size - p_i2s_obj[i2s_num]->tx->rw_pos) % 4;
        if (bytes_left) {
            size_t zero_bytes = 0, bytes_written;
            i2s_write(i2s_num, (void *)&zero_bytes, bytes_left, &bytes_written, portMAX_DELAY);
        }
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
            return ESP_ERR_NO_MEM;
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

#ifdef CONFIG_PM_ENABLE
    if (i2s_config->use_apll) {
        err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "i2s_driver", &p_i2s_obj[i2s_num]->pm_lock);
    } else {
        err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "i2s_driver", &p_i2s_obj[i2s_num]->pm_lock);
    }
    if (err != ESP_OK) {
        free(p_i2s_obj[i2s_num]);
        p_i2s_obj[i2s_num] = NULL;
        ESP_LOGE(I2S_TAG, "I2S pm lock error");
        return err;
    }
#endif //CONFIG_PM_ENABLE

        //To make sure hardware is enabled before any hardware register operations.
        if (i2s_num == I2S_NUM_1) {
            periph_module_reset(PERIPH_I2S1_MODULE);
            periph_module_enable(PERIPH_I2S1_MODULE);
        } else {
            periph_module_reset(PERIPH_I2S0_MODULE);
            periph_module_enable(PERIPH_I2S0_MODULE);
        }

        //initial interrupt
        err = i2s_isr_register(i2s_num, i2s_config->intr_alloc_flags, i2s_intr_handler_default, p_i2s_obj[i2s_num], &p_i2s_obj[i2s_num]->i2s_isr_handle);
        if (err != ESP_OK) {
#ifdef CONFIG_PM_ENABLE
            if (p_i2s_obj[i2s_num]->pm_lock) {
                esp_pm_lock_delete(p_i2s_obj[i2s_num]->pm_lock);
            }
#endif
            free(p_i2s_obj[i2s_num]);
            p_i2s_obj[i2s_num] = NULL;
            ESP_LOGE(I2S_TAG, "Register I2S Interrupt error");
            return err;
        }
        i2s_stop(i2s_num);
        err = i2s_param_config(i2s_num, i2s_config);
        if (err != ESP_OK) {
            i2s_driver_uninstall(i2s_num);
            ESP_LOGE(I2S_TAG, "I2S param configure error");
            return err;
        }

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

    ESP_LOGW(I2S_TAG, "I2S driver already installed");
    return ESP_OK;
}

esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    if (p_i2s_obj[i2s_num] == NULL) {
        ESP_LOGI(I2S_TAG, "already uninstalled");
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

    if(p_i2s_obj[i2s_num]->use_apll) {
        rtc_clk_apll_enable(0, 0, 0, 0, 0);
    }
#ifdef CONFIG_PM_ENABLE
    if (p_i2s_obj[i2s_num]->pm_lock) {
        esp_pm_lock_delete(p_i2s_obj[i2s_num]->pm_lock);
    }
#endif

    free(p_i2s_obj[i2s_num]);
    p_i2s_obj[i2s_num] = NULL;

    if (i2s_num == I2S_NUM_0) {
        periph_module_disable(PERIPH_I2S0_MODULE);
    } else if (i2s_num == I2S_NUM_1) {
        periph_module_disable(PERIPH_I2S1_MODULE);
    }
    return ESP_OK;
}

esp_err_t i2s_write(i2s_port_t i2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr, *src_byte;
    int bytes_can_write;
    *bytes_written = 0;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((size < I2S_MAX_BUFFER_SIZE), "size is too large", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num]->tx), "tx NULL", ESP_ERR_INVALID_ARG);
    xSemaphoreTake(p_i2s_obj[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2s_obj[i2s_num]->pm_lock);
#endif
    src_byte = (char *)src;
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
        memcpy(data_ptr, src_byte, bytes_can_write);
        size -= bytes_can_write;
        src_byte += bytes_can_write;
        p_i2s_obj[i2s_num]->tx->rw_pos += bytes_can_write;
        (*bytes_written) += bytes_can_write;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(p_i2s_obj[i2s_num]->pm_lock);
#endif

    xSemaphoreGive(p_i2s_obj[i2s_num]->tx->mux);
    return ESP_OK;
}

esp_err_t i2s_adc_enable(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num] != NULL), "Not initialized yet", ESP_ERR_INVALID_STATE);
    I2S_CHECK((p_i2s_obj[i2s_num]->mode & I2S_MODE_ADC_BUILT_IN), "i2s built-in adc not enabled", ESP_ERR_INVALID_STATE);

    adc1_i2s_mode_acquire();
    _i2s_adc_mode_recover();
    return i2s_set_clk(i2s_num, p_i2s_obj[i2s_num]->sample_rate, p_i2s_obj[i2s_num]->bits_per_sample, p_i2s_obj[i2s_num]->channel_num);
}

esp_err_t i2s_adc_disable(i2s_port_t i2s_num)
{
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num] != NULL), "Not initialized yet", ESP_ERR_INVALID_STATE);
    I2S_CHECK((p_i2s_obj[i2s_num]->mode & I2S_MODE_ADC_BUILT_IN), "i2s built-in adc not enabled", ESP_ERR_INVALID_STATE);

    adc1_lock_release();
    return ESP_OK;
}

esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_write, tail;
    int src_bytes, aim_bytes, zero_bytes;
    *bytes_written = 0;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((size > 0), "size must greater than zero", ESP_ERR_INVALID_ARG);
    I2S_CHECK((aim_bits * size < I2S_MAX_BUFFER_SIZE), "size is too large", ESP_ERR_INVALID_ARG);
    I2S_CHECK((aim_bits >= src_bits), "aim_bits musn't less than src_bits", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num]->tx), "tx NULL", ESP_ERR_INVALID_ARG);
    if (src_bits < I2S_BITS_PER_SAMPLE_8BIT || aim_bits < I2S_BITS_PER_SAMPLE_8BIT) {
        ESP_LOGE(I2S_TAG,"bits musn't be less than 8, src_bits %d aim_bits %d", src_bits, aim_bits);
        return ESP_ERR_INVALID_ARG;
    }
    if (src_bits > I2S_BITS_PER_SAMPLE_32BIT || aim_bits > I2S_BITS_PER_SAMPLE_32BIT) {
        ESP_LOGE(I2S_TAG,"bits musn't be greater than 32, src_bits %d aim_bits %d", src_bits, aim_bits);
        return ESP_ERR_INVALID_ARG;
    }
    if ((src_bits == I2S_BITS_PER_SAMPLE_16BIT || src_bits == I2S_BITS_PER_SAMPLE_32BIT) && (size % 2 != 0)) {
        ESP_LOGE(I2S_TAG,"size must be a even number while src_bits is even, src_bits %d size %d", src_bits, size);
        return ESP_ERR_INVALID_ARG;
    }
    if (src_bits == I2S_BITS_PER_SAMPLE_24BIT && (size % 3 != 0)) {
        ESP_LOGE(I2S_TAG,"size must be a multiple of 3 while src_bits is 24, size %d", size);
        return ESP_ERR_INVALID_ARG;
    }

    src_bytes = src_bits / 8;
    aim_bytes = aim_bits / 8;
    zero_bytes = aim_bytes - src_bytes;
    xSemaphoreTake(p_i2s_obj[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
    size = size * aim_bytes / src_bytes;
    ESP_LOGD(I2S_TAG,"aim_bytes %d src_bytes %d size %d", aim_bytes, src_bytes, size);
    while (size > 0) {
        if (p_i2s_obj[i2s_num]->tx->rw_pos == p_i2s_obj[i2s_num]->tx->buf_size || p_i2s_obj[i2s_num]->tx->curr_ptr == NULL) {
            if (xQueueReceive(p_i2s_obj[i2s_num]->tx->queue, &p_i2s_obj[i2s_num]->tx->curr_ptr, ticks_to_wait) == pdFALSE) {
                break;
            }
            p_i2s_obj[i2s_num]->tx->rw_pos = 0;
        }
        data_ptr = (char*)p_i2s_obj[i2s_num]->tx->curr_ptr;
        data_ptr += p_i2s_obj[i2s_num]->tx->rw_pos;
        bytes_can_write = p_i2s_obj[i2s_num]->tx->buf_size - p_i2s_obj[i2s_num]->tx->rw_pos;
        if (bytes_can_write > size) {
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
        p_i2s_obj[i2s_num]->tx->rw_pos += bytes_can_write;
    }
    xSemaphoreGive(p_i2s_obj[i2s_num]->tx->mux);
    return ESP_OK;
}

esp_err_t i2s_read(i2s_port_t i2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    char *data_ptr, *dest_byte;
    int bytes_can_read;
    *bytes_read = 0;
    dest_byte = (char *)dest;
    I2S_CHECK((i2s_num < I2S_NUM_MAX), "i2s_num error", ESP_ERR_INVALID_ARG);
    I2S_CHECK((size < I2S_MAX_BUFFER_SIZE), "size is too large", ESP_ERR_INVALID_ARG);
    I2S_CHECK((p_i2s_obj[i2s_num]->rx), "rx NULL", ESP_ERR_INVALID_ARG);
    xSemaphoreTake(p_i2s_obj[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2s_obj[i2s_num]->pm_lock);
#endif
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
        memcpy(dest_byte, data_ptr, bytes_can_read);
        size -= bytes_can_read;
        dest_byte += bytes_can_read;
        p_i2s_obj[i2s_num]->rx->rw_pos += bytes_can_read;
        (*bytes_read) += bytes_can_read;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(p_i2s_obj[i2s_num]->pm_lock);
#endif
    xSemaphoreGive(p_i2s_obj[i2s_num]->rx->mux);
    return ESP_OK;
}
