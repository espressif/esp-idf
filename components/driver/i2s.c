/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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
#if SOC_I2S_SUPPORTS_ADC_DAC
#include "driver/dac.h"
#include "hal/i2s_hal.h"
#include "adc1_private.h"
#endif

#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif

#include "soc/rtc.h"

#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_efuse.h"
#include "esp_rom_gpio.h"

#include "sdkconfig.h"

static const char *TAG = "I2S";

#define I2S_ENTER_CRITICAL_ISR()          portENTER_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL_ISR()           portEXIT_CRITICAL_ISR(&i2s_spinlock[i2s_num])
#define I2S_ENTER_CRITICAL()              portENTER_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_EXIT_CRITICAL()               portEXIT_CRITICAL(&i2s_spinlock[i2s_num])
#define I2S_FULL_DUPLEX_SLAVE_MODE_MASK   (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_SLAVE)
#define I2S_FULL_DUPLEX_MASTER_MODE_MASK  (I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_MASTER)

#define I2S_MAX_BUFFER_SIZE               (4*1024*1024) //the maximum RAM can be allocated

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
    i2s_dma_t *tx;              /*!< DMA Tx buffer*/
    i2s_dma_t *rx;              /*!< DMA Rx buffer*/
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t rx_dma_chan;  /*!< I2S rx gDMA channel handle*/
    gdma_channel_handle_t tx_dma_chan;  /*!< I2S tx gDMA channel handle*/
#else
    i2s_isr_handle_t i2s_isr_handle; /*!< I2S Interrupt handle*/
#endif
    int channel_num;            /*!< Number of channels*/
    int bytes_per_sample;       /*!< Bytes per sample*/
    int bits_per_sample;        /*!< Bits per sample*/
    i2s_comm_format_t communication_format; /*!<communication standard format*/
    i2s_mode_t mode;            /*!< I2S Working mode*/
    uint32_t sample_rate;       /*!< I2S sample rate */
    bool tx_desc_auto_clear;    /*!< I2S auto clear tx descriptor on underflow */
    bool use_apll;              /*!< I2S use APLL clock */
    int fixed_mclk;             /*!< I2S fixed MLCK clock */
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    i2s_hal_context_t hal;       /*!< I2S hal context*/
    i2s_hal_config_t hal_cfg; /*!< I2S hal configurations*/
} i2s_obj_t;

static i2s_obj_t *p_i2s[I2S_NUM_MAX] = {0};

static portMUX_TYPE i2s_spinlock[I2S_NUM_MAX];
#if SOC_I2S_SUPPORTS_ADC_DAC
static int _i2s_adc_unit = -1;
static int _i2s_adc_channel = -1;
#endif

static i2s_dma_t *i2s_create_dma_queue(i2s_port_t i2s_num, int dma_buf_count, int dma_buf_len);
static esp_err_t i2s_destroy_dma_queue(i2s_port_t i2s_num, i2s_dma_t *dma);

static void gpio_matrix_out_check_and_set(int gpio, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    //if pin = -1, do not need to configure
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio, signal_idx, out_inv, oen_inv);
    }
}

static void gpio_matrix_in_check_and_set(int gpio, uint32_t signal_idx, bool inv)
{
    if (gpio != -1) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        //Set direction, for some GPIOs, the input function are not enabled as default.
        gpio_set_direction(gpio, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(gpio, signal_idx, inv);
    }
}

#if SOC_I2S_SUPPORTS_PCM
esp_err_t i2s_pcm_config(i2s_port_t i2s_num, i2s_mode_t mode, i2s_pcm_mode_t pcm_cfg)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");

    if (mode & I2S_MODE_TX) {
        i2s_hal_tx_pcm_cfg(&(p_i2s[i2s_num]->hal), pcm_cfg);
    } else if (mode & I2S_MODE_RX) {
        i2s_hal_rx_pcm_cfg(&(p_i2s[i2s_num]->hal), pcm_cfg);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
#endif

float i2s_get_clk(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    return (float)p_i2s[i2s_num]->sample_rate;
}

static void i2s_tx_reset(i2s_port_t i2s_num)
{
    p_i2s[i2s_num]->tx->curr_ptr = NULL;
    p_i2s[i2s_num]->tx->rw_pos = 0;
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->tx_dma_chan);
#else
    //attach DMA
    i2s_hal_attach_tx_dma(&(p_i2s[i2s_num]->hal));
    // Reset I2S TX module first, and then, reset DMA and FIFO.
    i2s_hal_reset_tx(&(p_i2s[i2s_num]->hal));
    i2s_hal_reset_txdma(&(p_i2s[i2s_num]->hal));
    i2s_hal_reset_tx_fifo(&(p_i2s[i2s_num]->hal));
#endif
}

static void i2s_rx_reset(i2s_port_t i2s_num)
{
    p_i2s[i2s_num]->rx->curr_ptr = NULL;
    p_i2s[i2s_num]->rx->rw_pos = 0;
#if SOC_GDMA_SUPPORTED
    gdma_reset(p_i2s[i2s_num]->rx_dma_chan);
#else
    //attach DMA
    i2s_hal_attach_rx_dma(&(p_i2s[i2s_num]->hal));
    // Reset I2S RX module first, and then, reset DMA and FIFO.
    i2s_hal_reset_rx(&(p_i2s[i2s_num]->hal));
    i2s_hal_reset_rxdma(&(p_i2s[i2s_num]->hal));
    i2s_hal_reset_rx_fifo(&(p_i2s[i2s_num]->hal));
#endif
}

static void i2s_tx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->tx_dma_chan, (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#else
    i2s_hal_enable_tx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_start_tx_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->tx->desc[0]);
#endif
    i2s_hal_start_tx(&(p_i2s[i2s_num]->hal));
}

static void i2s_rx_start(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_start(p_i2s[i2s_num]->rx_dma_chan, (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#else
    i2s_hal_enable_rx_intr(&(p_i2s[i2s_num]->hal));
    i2s_hal_start_rx_link(&(p_i2s[i2s_num]->hal), (uint32_t) p_i2s[i2s_num]->rx->desc[0]);
#endif
    i2s_hal_start_rx(&(p_i2s[i2s_num]->hal));
}

static void i2s_tx_stop(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->tx_dma_chan);
#else
    i2s_hal_stop_tx_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_stop_tx(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_tx_intr(&(p_i2s[i2s_num]->hal));
#endif
}

static void i2s_rx_stop(i2s_port_t i2s_num)
{
#if SOC_GDMA_SUPPORTED
    gdma_stop(p_i2s[i2s_num]->rx_dma_chan);
#else
    i2s_hal_stop_rx_link(&(p_i2s[i2s_num]->hal));
    i2s_hal_stop_rx(&(p_i2s[i2s_num]->hal));
    i2s_hal_disable_rx_intr(&(p_i2s[i2s_num]->hal));
#endif
}

#if SOC_I2S_SUPPORTS_APLL
static float i2s_apll_get_fi2s(int bits_per_sample, int sdm0, int sdm1, int sdm2, int odir)
{
    int f_xtal = (int)rtc_clk_xtal_freq_get() * 1000000;

#if CONFIG_IDF_TARGET_ESP32
    /* ESP32 rev0 silicon issue for APLL range/accuracy, please see ESP32 ECO document for more information on this */
    if (esp_efuse_get_chip_ver() == 0) {
        sdm0 = 0;
        sdm1 = 0;
    }
#endif
    float fout = f_xtal * (sdm2 + sdm1 / 256.0f + sdm0 / 65536.0f + 4);
    if (fout < SOC_I2S_APLL_MIN_FREQ || fout > SOC_I2S_APLL_MAX_FREQ) {
        return SOC_I2S_APLL_MAX_FREQ;
    }
    float fpll = fout / (2 * (odir + 2)); //== fi2s (N=1, b=0, a=1)
    return fpll / 2;
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
    if (rate / bits_per_sample / 2 / 8 < SOC_I2S_APLL_MIN_RATE) {
        return ESP_ERR_INVALID_ARG;
    }

    *sdm0 = 0;
    *sdm1 = 0;
    *sdm2 = 0;
    *odir = 0;
    min_diff = SOC_I2S_APLL_MAX_FREQ;

    for (_sdm2 = 4; _sdm2 < 9; _sdm2 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, _sdm2, 0);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, _sdm2, 31);
        avg = (max_rate + min_rate) / 2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm2 = _sdm2;
        }
    }
    min_diff = SOC_I2S_APLL_MAX_FREQ;
    for (_odir = 0; _odir < 32; _odir ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, *sdm2, _odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, *sdm2, _odir);
        avg = (max_rate + min_rate) / 2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *odir = _odir;
        }
    }
    min_diff = SOC_I2S_APLL_MAX_FREQ;
    for (_sdm2 = 4; _sdm2 < 9; _sdm2 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, 255, _sdm2, *odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, 0, _sdm2, *odir);
        avg = (max_rate + min_rate) / 2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm2 = _sdm2;
        }
    }

    min_diff = SOC_I2S_APLL_MAX_FREQ;
    for (_sdm1 = 0; _sdm1 < 256; _sdm1 ++) {
        max_rate = i2s_apll_get_fi2s(bits_per_sample, 255, _sdm1, *sdm2, *odir);
        min_rate = i2s_apll_get_fi2s(bits_per_sample, 0, _sdm1, *sdm2, *odir);
        avg = (max_rate + min_rate) / 2;
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm1 = _sdm1;
        }
    }

    min_diff = SOC_I2S_APLL_MAX_FREQ;
    for (_sdm0 = 0; _sdm0 < 256; _sdm0 ++) {
        avg = i2s_apll_get_fi2s(bits_per_sample, _sdm0, *sdm1, *sdm2, *odir);
        if (abs(avg - rate) < min_diff) {
            min_diff = abs(avg - rate);
            *sdm0 = _sdm0;
        }
    }

    return ESP_OK;
}
#endif

static esp_err_t i2s_alloc_dma_buffer(i2s_port_t i2s_num, int data_bits, int ch)
{
    if (p_i2s[i2s_num]->channel_num != ch) {
        p_i2s[i2s_num]->channel_num = (ch == 2) ? 2 : 1;
    }

    i2s_dma_t *save_tx = NULL, *save_rx = NULL;

    if (data_bits != p_i2s[i2s_num]->bits_per_sample) {
        p_i2s[i2s_num]->bits_per_sample = data_bits;

        // Round bytes_per_sample up to next multiple of 16 bits
        int halfwords_per_sample = (data_bits + 15) / 16;
        p_i2s[i2s_num]->bytes_per_sample = halfwords_per_sample * 2;

        // Because limited of DMA buffer is 4092 bytes
        if (p_i2s[i2s_num]->dma_buf_len * p_i2s[i2s_num]->bytes_per_sample * p_i2s[i2s_num]->channel_num > 4092) {
            p_i2s[i2s_num]->dma_buf_len = 4092 / p_i2s[i2s_num]->bytes_per_sample / p_i2s[i2s_num]->channel_num;
        }

        // Re-create TX DMA buffer
        if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
            save_tx = p_i2s[i2s_num]->tx;
            //destroy old tx dma if exist
            if (save_tx) {
                i2s_destroy_dma_queue(i2s_num, save_tx);
            }
            p_i2s[i2s_num]->tx = i2s_create_dma_queue(i2s_num, p_i2s[i2s_num]->dma_buf_count, p_i2s[i2s_num]->dma_buf_len);
            if (p_i2s[i2s_num]->tx == NULL) {
                ESP_LOGE(TAG, "Failed to create tx dma buffer");
                i2s_driver_uninstall(i2s_num);
                return ESP_ERR_NO_MEM;
            }
        }
        // Re-create RX DMA buffer
        if (p_i2s[i2s_num]->mode & I2S_MODE_RX) {
            save_rx = p_i2s[i2s_num]->rx;
            //destroy old rx dma if exist
            if (save_rx) {
                i2s_destroy_dma_queue(i2s_num, save_rx);
            }
            p_i2s[i2s_num]->rx = i2s_create_dma_queue(i2s_num, p_i2s[i2s_num]->dma_buf_count, p_i2s[i2s_num]->dma_buf_len);
            if (p_i2s[i2s_num]->rx == NULL) {
                ESP_LOGE(TAG, "Failed to create rx dma buffer");
                i2s_driver_uninstall(i2s_num);
                return ESP_ERR_NO_MEM;
            }
            i2s_hal_set_rx_eof_num(&(p_i2s[i2s_num]->hal), p_i2s[i2s_num]->dma_buf_len * p_i2s[i2s_num]->channel_num * p_i2s[i2s_num]->bytes_per_sample);
        }
    }
    return ESP_OK;
}

static esp_err_t i2s_fbclk_cal(int i2s_num, uint32_t rate, int channel, int channel_bit, uint32_t *sclk, uint32_t *fbck, uint32_t *bck_div)
{
    //Default select I2S_D2CLK (160M)
    uint32_t _sclk = I2S_LL_BASE_CLK;
    uint32_t _fbck = rate * channel * channel_bit;
    uint32_t _bck_div = (256 % channel_bit) ? 12 : 8;
    i2s_clock_src_t clk_src = I2S_CLK_D2CLK;

//ADC mode only support on ESP32,
#if SOC_I2S_SUPPORTS_ADC_DAC
    if ( p_i2s[i2s_num]->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        _fbck = rate * I2S_LL_AD_BCK_FACTOR * 2;
        _bck_div = I2S_LL_AD_BCK_FACTOR;
    }
#endif // SOC_I2S_SUPPORTS_ADC_DAC

#if SOC_I2S_SUPPORTS_PDM
    if ( p_i2s[i2s_num]->mode & I2S_MODE_PDM) {
#if SOC_I2S_SUPPORTS_PDM_TX
        if ( p_i2s[i2s_num]->mode & I2S_MODE_TX) {
            int fp = 1;
            int fs = 1;
            i2s_hal_get_tx_pdm_fpfs(&(p_i2s[i2s_num]->hal), &fp, &fs);
            _fbck = rate * I2S_LL_PDM_BCK_FACTOR * fp / fs;
        }
#endif //SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_SUPPORTS_PDM_RX
        if ( p_i2s[i2s_num]->mode & I2S_MODE_RX) {
            i2s_pdm_dsr_t dsr;
            i2s_hal_get_rx_pdm_dsr(&(p_i2s[i2s_num]->hal), &dsr);
            _fbck = rate * I2S_LL_PDM_BCK_FACTOR * (dsr == I2S_PDM_DSR_16S ? 2 : 1);
        }
#endif // SOC_I2S_SUPPORTS_PDM_RX
        _bck_div = 8;
    }
#endif // SOC_I2S_SUPPORTS_PDM

#if SOC_I2S_SUPPORTS_APLL
    int sdm0 = 0;
    int sdm1 = 0;
    int sdm2 = 0;
    int odir = 0;
    //If APLL is specified, try to calculate in APLL
    if (p_i2s[i2s_num]->use_apll && i2s_apll_calculate_fi2s(p_i2s[i2s_num]->fixed_mclk, channel_bit, &sdm0, &sdm1, &sdm2, &odir) == ESP_OK) {
        _sclk = p_i2s[i2s_num]->fixed_mclk;
        clk_src = I2S_CLK_APLL;
        ESP_LOGD(TAG, "sdm0=%d, sdm1=%d, sdm2=%d, odir=%d", sdm0, sdm1, sdm2, odir);
        rtc_clk_apll_enable(1, sdm0, sdm1, sdm2, odir);
    }
#endif // SOC_I2S_SUPPORTS_APLL
    if ((_fbck * _bck_div) > _sclk) {
        ESP_LOGE(TAG, "sample rate is too large\r\n");
        return ESP_ERR_INVALID_ARG;
    }
    i2s_hal_set_clock_src(&(p_i2s[i2s_num]->hal), clk_src);
    *sclk = _sclk;
    *fbck = _fbck;
    *bck_div = _bck_div;
    return ESP_OK;
}

static uint32_t i2s_get_active_chan_num(i2s_hal_config_t *hal_cfg)
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
    case I2S_CHANNEL_FMT_TDM: {
        uint32_t num = 0;
        uint32_t max_chan = 0;
        uint32_t chan_mask = hal_cfg->chan_cfg.chan_mask;

        for (int i = 0; chan_mask && i < 16; i++, chan_mask >>= 1) {
            if ((chan_mask & 0x01) == 1) {
                num++;
                max_chan = i + 1;
            }
        }
        if (max_chan > hal_cfg->chan_cfg.total_chan) {
            hal_cfg->chan_cfg.total_chan = max_chan;
        }
        hal_cfg->chan_cfg.active_chan = num;
        return num;
    }
#endif
    default:
        return 0;
    }
}

esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, uint32_t bits_cfg, i2s_channel_t ch)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_ARG, TAG, "Not initialized yet");

    i2s_hal_config_t *cfg = &p_i2s[i2s_num]->hal_cfg;
    int data_bits = 0;
    int slot_bits = 0;
    int active_slot_num = 0;
    int slot_num = 0;

    cfg->ch = ch;
    cfg->sample_rate = rate;
    cfg->bits_cfg.val = bits_cfg;

    cfg->bits_cfg.chan_bits = cfg->bits_cfg.chan_bits < cfg->bits_cfg.sample_bits ?
                              cfg->bits_cfg.sample_bits : cfg->bits_cfg.chan_bits;
    slot_bits = cfg->bits_cfg.chan_bits;
    data_bits = cfg->bits_cfg.sample_bits;

#if SOC_I2S_SUPPORTS_TDM
    cfg->chan_cfg.chan_mask = ch & 0xFFFF;
    active_slot_num = i2s_get_active_chan_num(cfg);
    slot_num = cfg->chan_cfg.total_chan;
#else
    active_slot_num = i2s_get_active_chan_num(cfg);
    slot_num = ch == I2S_CHANNEL_MONO ? 2 : active_slot_num;
#endif
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");

    if ((data_bits % 8 != 0) || (data_bits > I2S_BITS_PER_SAMPLE_32BIT)) {
        ESP_LOGE(TAG, "Invalid bits per sample");
        return ESP_ERR_INVALID_ARG;
    }
    //Stop I2S
    i2s_stop(i2s_num);
    // wait all on-going writing finish
    if ((p_i2s[i2s_num]->mode & I2S_MODE_TX) && p_i2s[i2s_num]->tx) {
        xSemaphoreTake(p_i2s[i2s_num]->tx->mux, (portTickType)portMAX_DELAY);
    }
    if ((p_i2s[i2s_num]->mode & I2S_MODE_RX) && p_i2s[i2s_num]->rx) {
        xSemaphoreTake(p_i2s[i2s_num]->rx->mux, (portTickType)portMAX_DELAY);
    }
    //malloc DMA buffer
    if (i2s_alloc_dma_buffer(i2s_num, data_bits, active_slot_num) != ESP_OK ) {
        return ESP_ERR_NO_MEM;
    }

    uint32_t i2s_clk = 0; // I2S source clock
    uint32_t i2s_bck = 0; // I2S back clock
    uint32_t bck_div = 0; // I2S bck div
    //calculate bck_div, f_bck and select source clock
    if (i2s_fbclk_cal(i2s_num, rate, slot_num, slot_bits, &i2s_clk, &i2s_bck, &bck_div) != ESP_OK) {
        return ESP_FAIL;
    }
    //configure i2s clock
    if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        i2s_hal_tx_clock_config(&(p_i2s[i2s_num]->hal), i2s_clk, i2s_bck, bck_div);
        i2s_hal_set_tx_sample_bit(&(p_i2s[i2s_num]->hal), slot_bits, data_bits);
        // wait all writing on-going finish
        if (p_i2s[i2s_num]->tx) {
            xSemaphoreGive(p_i2s[i2s_num]->tx->mux);
        }
    }
    if (p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        i2s_hal_rx_clock_config(&(p_i2s[i2s_num]->hal), i2s_clk, i2s_bck, bck_div);
        i2s_hal_set_rx_sample_bit(&(p_i2s[i2s_num]->hal), slot_bits, data_bits);
        // wait all writing on-going finish
        if (p_i2s[i2s_num]->rx) {
            xSemaphoreGive(p_i2s[i2s_num]->rx->mux);
        }
    }
    i2s_hal_samples_config(&(p_i2s[i2s_num]->hal), &(p_i2s[i2s_num]->hal_cfg));
    // Reset message queue to avoid receiving unavailable values because the old dma queque has been destroyed
    if (p_i2s[i2s_num]->tx) {
        xQueueReset(p_i2s[i2s_num]->tx->queue);
    }
    if (p_i2s[i2s_num]->rx) {
        xQueueReset(p_i2s[i2s_num]->rx->queue);
    }

    //I2S start
    i2s_start(i2s_num);
    p_i2s[i2s_num]->sample_rate = rate;
    return ESP_OK;
}

#if SOC_GDMA_SUPPORTED
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
static void IRAM_ATTR i2s_intr_handler_default(void *arg)
{
    i2s_obj_t *p_i2s = (i2s_obj_t *) arg;
    uint32_t status;
    i2s_hal_get_intr_status(&(p_i2s->hal), &status);
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

static esp_err_t i2s_destroy_dma_queue(i2s_port_t i2s_num, i2s_dma_t *dma)
{
    int bux_idx;
    if (p_i2s[i2s_num] == NULL) {
        ESP_LOGE(TAG, "Not initialized yet");
        return ESP_ERR_INVALID_ARG;
    }
    if (dma == NULL) {
        ESP_LOGE(TAG, "dma is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    for (bux_idx = 0; bux_idx < p_i2s[i2s_num]->dma_buf_count; bux_idx++) {
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
    ESP_LOGI(TAG, "DMA queue destroyed");
    vQueueDelete(dma->queue);
    vSemaphoreDelete(dma->mux);
    free(dma);
    return ESP_OK;
}

static i2s_dma_t *i2s_create_dma_queue(i2s_port_t i2s_num, int dma_buf_count, int dma_buf_len)
{
    int bux_idx;
    int sample_size = p_i2s[i2s_num]->bytes_per_sample * p_i2s[i2s_num]->channel_num;
    i2s_dma_t *dma = (i2s_dma_t *) malloc(sizeof(i2s_dma_t));
    if (dma == NULL) {
        ESP_LOGE(TAG, "Error malloc i2s_dma_t");
        return NULL;
    }
    memset(dma, 0, sizeof(i2s_dma_t));

    dma->buf = (char **)malloc(sizeof(char *) * dma_buf_count);
    if (dma->buf == NULL) {
        ESP_LOGE(TAG, "Error malloc dma buffer pointer");
        free(dma);
        return NULL;
    }
    memset(dma->buf, 0, sizeof(char *) * dma_buf_count);

    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->buf[bux_idx] = (char *) heap_caps_calloc(1, dma_buf_len * sample_size, MALLOC_CAP_DMA);
        if (dma->buf[bux_idx] == NULL) {
            ESP_LOGE(TAG, "Error malloc dma buffer");
            i2s_destroy_dma_queue(i2s_num, dma);
            return NULL;
        }
        ESP_LOGD(TAG, "Addr[%d] = %d", bux_idx, (int)dma->buf[bux_idx]);
    }

    dma->desc = (lldesc_t **) malloc(sizeof(lldesc_t *) * dma_buf_count);
    if (dma->desc == NULL) {
        ESP_LOGE(TAG, "Error malloc dma description");
        i2s_destroy_dma_queue(i2s_num, dma);
        return NULL;
    }
    for (bux_idx = 0; bux_idx < dma_buf_count; bux_idx++) {
        dma->desc[bux_idx] = (lldesc_t *) heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
        if (dma->desc[bux_idx] == NULL) {
            ESP_LOGE(TAG, "Error malloc dma description entry");
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
    dma->queue = xQueueCreate(dma_buf_count - 1, sizeof(char *));
    dma->mux = xSemaphoreCreateMutex();
    dma->buf_size = dma_buf_len * sample_size;
    ESP_LOGI(TAG, "DMA Malloc info, datalen=blocksize=%d, dma_buf_count=%d", dma_buf_len * sample_size, dma_buf_count);
    return dma;
}

esp_err_t i2s_start(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    //start DMA link
    I2S_ENTER_CRITICAL();

#if !SOC_GDMA_SUPPORTED
    esp_intr_disable(p_i2s[i2s_num]->i2s_isr_handle);
    i2s_hal_clear_intr_status(&(p_i2s[i2s_num]->hal), I2S_INTR_MAX);
#endif
    if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        i2s_tx_reset(i2s_num);
        i2s_tx_start(i2s_num);
    }
    if (p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        i2s_rx_reset(i2s_num);
        i2s_rx_start(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    esp_intr_enable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t i2s_stop(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    I2S_ENTER_CRITICAL();
#if !SOC_GDMA_SUPPORTED
    esp_intr_disable(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        i2s_tx_stop(i2s_num);
    }
    if (p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        i2s_rx_stop(i2s_num);
    }
#if !SOC_GDMA_SUPPORTED
    i2s_hal_clear_intr_status(&(p_i2s[i2s_num]->hal), I2S_INTR_MAX);
#endif
    I2S_EXIT_CRITICAL();
    return ESP_OK;
}

#if SOC_I2S_SUPPORTS_ADC_DAC
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
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->mode & I2S_MODE_ADC_BUILT_IN), ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    adc1_dma_mode_acquire();
    _i2s_adc_mode_recover();
    i2s_rx_reset(i2s_num);
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->sample_rate, p_i2s[i2s_num]->bits_per_sample, p_i2s[i2s_num]->channel_num);
}

esp_err_t i2s_adc_disable(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num] != NULL), ESP_ERR_INVALID_STATE, TAG, "Not initialized yet");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->mode & I2S_MODE_ADC_BUILT_IN), ESP_ERR_INVALID_STATE, TAG, "i2s built-in adc not enabled");

    i2s_hal_stop_rx(&(p_i2s[i2s_num]->hal));
    adc1_lock_release();
    return ESP_OK;
}
#endif

esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    if (pin == NULL) {
#if SOC_I2S_SUPPORTS_ADC_DAC
        return i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#else
        return ESP_ERR_INVALID_ARG;
#endif
    }
    if (pin->bck_io_num != -1 && !GPIO_IS_VALID_GPIO(pin->bck_io_num)) {
        ESP_LOGE(TAG, "bck_io_num error");
        return ESP_FAIL;
    }
    if (pin->ws_io_num != -1 && !GPIO_IS_VALID_GPIO(pin->ws_io_num)) {
        ESP_LOGE(TAG, "ws_io_num error");
        return ESP_FAIL;
    }
    if (pin->data_out_num != -1 && !GPIO_IS_VALID_OUTPUT_GPIO(pin->data_out_num)) {
        ESP_LOGE(TAG, "data_out_num error");
        return ESP_FAIL;
    }
    if (pin->data_in_num != -1 && !GPIO_IS_VALID_GPIO(pin->data_in_num)) {
        ESP_LOGE(TAG, "data_in_num error");
        return ESP_FAIL;
    }
    if (p_i2s[i2s_num]->mode & I2S_MODE_SLAVE) {
        if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
            gpio_matrix_in_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].tx_ws_sig, 0);
            gpio_matrix_in_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].tx_bck_sig, 0);
        } else {
            gpio_matrix_in_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].rx_ws_sig, 0);
            gpio_matrix_in_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].rx_bck_sig, 0);
        }
    } else {
        if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].tx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].tx_bck_sig, 0, 0);
        } else {
            gpio_matrix_out_check_and_set(pin->ws_io_num, i2s_periph_signal[i2s_num].rx_ws_sig, 0, 0);
            gpio_matrix_out_check_and_set(pin->bck_io_num, i2s_periph_signal[i2s_num].rx_bck_sig, 0, 0);
        }
    }
    gpio_matrix_out_check_and_set(pin->data_out_num, i2s_periph_signal[i2s_num].data_out_sig, 0, 0);
    gpio_matrix_in_check_and_set(pin->data_in_num, i2s_periph_signal[i2s_num].data_in_sig, 0);
    return ESP_OK;
}

esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->bytes_per_sample > 0), ESP_ERR_INVALID_ARG, TAG, "bits_per_sample not set");
    return i2s_set_clk(i2s_num, rate, p_i2s[i2s_num]->bits_per_sample, p_i2s[i2s_num]->channel_num);
}

#if SOC_I2S_SUPPORTS_PDM_RX
esp_err_t i2s_set_pdm_rx_down_sample(i2s_port_t i2s_num, i2s_pdm_dsr_t dsr)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    i2s_hal_set_rx_pdm_dsr(&(p_i2s[i2s_num]->hal), dsr);
    return i2s_set_clk(i2s_num, p_i2s[i2s_num]->sample_rate, p_i2s[i2s_num]->bits_per_sample, p_i2s[i2s_num]->channel_num);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
esp_err_t i2s_set_pdm_tx_up_sample(i2s_port_t i2s_num, int sample_rate, int fp, int fs)
{
    I2S_ENTER_CRITICAL();
    i2s_hal_set_tx_pdm_fpfs(&(p_i2s[i2s_num]->hal), fp, fs);
    I2S_EXIT_CRITICAL();
    return i2s_set_clk(i2s_num, sample_rate, p_i2s[i2s_num]->bits_per_sample, p_i2s[i2s_num]->channel_num);
}
#endif

static esp_err_t i2s_check_cfg_static(i2s_port_t i2s_num)
{
    i2s_hal_config_t *cfg = &p_i2s[i2s_num]->hal_cfg;
#if SOC_I2S_SUPPORTS_ADC_DAC
    //We only check if the I2S number is invalid when set to build in ADC and DAC mode.
    ESP_RETURN_ON_FALSE(!((cfg->mode & I2S_MODE_ADC_BUILT_IN) && (i2s_num != I2S_NUM_0)), ESP_ERR_INVALID_ARG, TAG, "I2S ADC built-in only support on I2S0");
    ESP_RETURN_ON_FALSE(!((cfg->mode & I2S_MODE_DAC_BUILT_IN) && (i2s_num != I2S_NUM_0)), ESP_ERR_INVALID_ARG, TAG, "I2S DAC built-in only support on I2S0");
    return ESP_OK;
#endif
#if SOC_I2S_SUPPORTS_PDM
    //We only check if the I2S number is invalid when set to PDM mode.
    ESP_RETURN_ON_FALSE(!((cfg->mode & I2S_MODE_PDM) && (i2s_num != I2S_NUM_0)), ESP_ERR_INVALID_ARG, TAG, "I2S DAC PDM only support on I2S0");
    return ESP_OK;
#endif

    ESP_RETURN_ON_FALSE(cfg->comm_fmt && (cfg->comm_fmt < I2S_COMM_FORMAT_STAND_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid communication formats");
    ESP_RETURN_ON_FALSE(!((cfg->comm_fmt & I2S_COMM_FORMAT_STAND_MSB) && (cfg->comm_fmt & I2S_COMM_FORMAT_STAND_PCM_LONG)), ESP_ERR_INVALID_ARG, TAG, "multiple communication formats specified");
    return ESP_OK;
}

static esp_err_t i2s_param_config(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((i2s_check_cfg_static(i2s_num) == ESP_OK), ESP_ERR_INVALID_ARG, TAG, "param check error");

    i2s_hal_config_t *cfg = &p_i2s[i2s_num]->hal_cfg;
#if SOC_I2S_SUPPORTS_ADC_DAC
    if (cfg->mode & I2S_MODE_ADC_BUILT_IN) {
        //in ADC built-in mode, we need to call i2s_set_adc_mode to
        //initialize the specific ADC channel.
        //in the current stage, we only support ADC1 and single channel mode.
        //In default data mode, the ADC data is in 12-bit resolution mode.
        adc_power_acquire();
    }
#endif
    p_i2s[i2s_num]->communication_format = cfg->comm_fmt;
    // configure I2S data port interface.
    i2s_hal_config_param(&(p_i2s[i2s_num]->hal), cfg);
    if ((p_i2s[i2s_num]->mode & I2S_MODE_RX) &&  (p_i2s[i2s_num]->mode & I2S_MODE_TX)) {
        i2s_hal_enable_sig_loopback(&(p_i2s[i2s_num]->hal));
        if (p_i2s[i2s_num]->mode & I2S_MODE_MASTER) {
            i2s_hal_enable_master_fd_mode(&(p_i2s[i2s_num]->hal));
        } else {
            i2s_hal_enable_slave_fd_mode(&(p_i2s[i2s_num]->hal));
        }
    }
    return ESP_OK;
}

esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    if (p_i2s[i2s_num]->rx && p_i2s[i2s_num]->rx->buf != NULL && p_i2s[i2s_num]->rx->buf_size != 0) {
        for (int i = 0; i < p_i2s[i2s_num]->dma_buf_count; i++) {
            memset(p_i2s[i2s_num]->rx->buf[i], 0, p_i2s[i2s_num]->rx->buf_size);
        }
    }
    if (p_i2s[i2s_num]->tx && p_i2s[i2s_num]->tx->buf != NULL && p_i2s[i2s_num]->tx->buf_size != 0) {
        int bytes_left = 0;
        bytes_left = (p_i2s[i2s_num]->tx->buf_size - p_i2s[i2s_num]->tx->rw_pos) % 4;
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

esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void *i2s_queue)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((i2s_config != NULL), ESP_ERR_INVALID_ARG, TAG, "I2S configuration must not NULL");
    ESP_RETURN_ON_FALSE((i2s_config->dma_buf_count >= 2 && i2s_config->dma_buf_count <= 128), ESP_ERR_INVALID_ARG, TAG, "I2S buffer count less than 128 and more than 2");
    ESP_RETURN_ON_FALSE((i2s_config->dma_buf_len >= 8 && i2s_config->dma_buf_len <= 1024), ESP_ERR_INVALID_ARG, TAG, "I2S buffer length at most 1024 and more than 8");
    if (p_i2s[i2s_num] != NULL) {
        ESP_LOGW(TAG, "I2S driver already installed");
        return ESP_OK;
    }

    p_i2s[i2s_num] = (i2s_obj_t *) calloc(1, sizeof(i2s_obj_t));
    if (p_i2s[i2s_num] == NULL) {
        ESP_LOGE(TAG, "Malloc I2S driver error");
        return ESP_ERR_NO_MEM;
    }

    portMUX_TYPE i2s_spinlock_unlocked[1] = {portMUX_INITIALIZER_UNLOCKED};
    for (int x = 0; x < I2S_NUM_MAX; x++) {
        i2s_spinlock[x] = i2s_spinlock_unlocked[0];
    }
    //To make sure hardware is enabled before any hardware register operations.
    periph_module_enable(i2s_periph_signal[i2s_num].module);
    i2s_hal_init(&(p_i2s[i2s_num]->hal), i2s_num);

    // Set I2S HAL configurations
    p_i2s[i2s_num]->hal_cfg.mode = i2s_config->mode;
    p_i2s[i2s_num]->hal_cfg.sample_rate = i2s_config->sample_rate;
    p_i2s[i2s_num]->hal_cfg.comm_fmt = i2s_config->communication_format;
    p_i2s[i2s_num]->hal_cfg.chan_fmt = i2s_config->channel_format;
    p_i2s[i2s_num]->hal_cfg.bits_cfg.sample_bits = i2s_config->bits_per_sample;
    p_i2s[i2s_num]->hal_cfg.bits_cfg.chan_bits = i2s_config->bits_per_slot;
#if SOC_I2S_SUPPORTS_TDM
    switch (i2s_config->channel_format) {
    case I2S_CHANNEL_FMT_RIGHT_LEFT:
    case I2S_CHANNEL_FMT_ALL_RIGHT:
    case I2S_CHANNEL_FMT_ALL_LEFT:
        p_i2s[i2s_num]->hal_cfg.chan_cfg.chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.active_chan = 2;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.total_chan = 2;
        break;
    case I2S_CHANNEL_FMT_ONLY_RIGHT:
        p_i2s[i2s_num]->hal_cfg.chan_cfg.chan_mask = i2s_config->tdm_flags.left_align_en ? I2S_TDM_ACTIVE_CH1 : I2S_TDM_ACTIVE_CH0;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.active_chan = 1;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.total_chan = 1;
        break;
    case I2S_CHANNEL_FMT_ONLY_LEFT:
        p_i2s[i2s_num]->hal_cfg.chan_cfg.chan_mask = i2s_config->tdm_flags.left_align_en ? I2S_TDM_ACTIVE_CH0 : I2S_TDM_ACTIVE_CH1;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.active_chan = 1;
        p_i2s[i2s_num]->hal_cfg.chan_cfg.total_chan = 1;
        break;
    case I2S_CHANNEL_FMT_TDM:
        ESP_RETURN_ON_FALSE((i2s_config->tdm_chan_cfg.chan_mask != 0), ESP_ERR_INVALID_ARG, TAG, "i2s all channel are disabled");
        p_i2s[i2s_num]->hal_cfg.chan_cfg.chan_mask = i2s_config->tdm_chan_cfg.chan_mask;
        i2s_get_active_chan_num(&p_i2s[i2s_num]->hal_cfg);
        break;
    default:
        ESP_LOGE(TAG, "wrong i2s channel format, uninstalled i2s.");
        goto err;
    }
    p_i2s[i2s_num]->hal_cfg.flags.val = i2s_config->tdm_flags.val;
#endif

    // Set I2S driver configurations
    p_i2s[i2s_num]->i2s_num = i2s_num;
    p_i2s[i2s_num]->mode = i2s_config->mode;
    p_i2s[i2s_num]->channel_num = i2s_get_active_chan_num(&p_i2s[i2s_num]->hal_cfg);
    p_i2s[i2s_num]->i2s_queue = i2s_queue;
    p_i2s[i2s_num]->bits_per_sample = 0;
    p_i2s[i2s_num]->bytes_per_sample = 0; // Not initialized yet
    p_i2s[i2s_num]->dma_buf_count = i2s_config->dma_buf_count;
    p_i2s[i2s_num]->dma_buf_len = i2s_config->dma_buf_len;

#ifdef CONFIG_PM_ENABLE
    if (i2s_config->use_apll) {
        ret = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "i2s_driver", &p_i2s[i2s_num]->pm_lock);
    } else {
        ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "i2s_driver", &p_i2s[i2s_num]->pm_lock);
    }
    if (ret != ESP_OK) {
        free(p_i2s[i2s_num]);
        p_i2s[i2s_num] = NULL;
        ESP_LOGE(TAG, "I2S pm lock error");
        return ret;
    }
#endif //CONFIG_PM_ENABLE
#if SOC_GDMA_SUPPORTED
    ret = ESP_OK;
    gdma_trigger_t trig = {.periph = GDMA_TRIG_PERIPH_I2S};
#if SOC_I2S_NUM > 1
    trig.instance_id = (i2s_num == I2S_NUM_0) ? SOC_GDMA_TRIG_PERIPH_I2S0 : SOC_GDMA_TRIG_PERIPH_I2S1;
#else
    trig.instance_id = SOC_GDMA_TRIG_PERIPH_I2S0;
#endif
    gdma_channel_alloc_config_t dma_cfg = {.flags.reserve_sibling = 1};
    if ( p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_RX;
        ESP_GOTO_ON_ERROR(gdma_new_channel(&dma_cfg, &p_i2s[i2s_num]->rx_dma_chan), err, TAG, "Register rx dma channel error");
        ESP_GOTO_ON_ERROR(gdma_connect(p_i2s[i2s_num]->rx_dma_chan, trig), err, TAG, "Connect rx dma channel error");
        gdma_rx_event_callbacks_t cb = {.on_recv_eof = i2s_dma_rx_callback};
        gdma_register_rx_event_callbacks(p_i2s[i2s_num]->rx_dma_chan, &cb, p_i2s[i2s_num]);
    }
    if ( p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_TX;
        ESP_GOTO_ON_ERROR(gdma_new_channel(&dma_cfg, &p_i2s[i2s_num]->tx_dma_chan), err, TAG, "Register tx dma channel error");
        ESP_GOTO_ON_ERROR(gdma_connect(p_i2s[i2s_num]->tx_dma_chan, trig), err, TAG, "Connect tx dma channel error");
        gdma_tx_event_callbacks_t cb = {.on_trans_eof = i2s_dma_tx_callback};
        gdma_register_tx_event_callbacks(p_i2s[i2s_num]->tx_dma_chan, &cb, p_i2s[i2s_num]);
    }
#else
    //initial interrupt
    ret = esp_intr_alloc(i2s_periph_signal[i2s_num].irq, i2s_config->intr_alloc_flags, i2s_intr_handler_default, p_i2s[i2s_num], &p_i2s[i2s_num]->i2s_isr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Register I2S Interrupt error");
#endif // SOC_GDMA_SUPPORTED
    i2s_stop(i2s_num);
    p_i2s[i2s_num]->use_apll = i2s_config->use_apll;
    p_i2s[i2s_num]->fixed_mclk = i2s_config->fixed_mclk;
    p_i2s[i2s_num]->tx_desc_auto_clear = i2s_config->tx_desc_auto_clear;
    ret = i2s_param_config(i2s_num);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "I2S param configure error");
    if (i2s_queue) {
        p_i2s[i2s_num]->i2s_queue = xQueueCreate(queue_size, sizeof(i2s_event_t));
        ESP_GOTO_ON_ERROR(ret, err, TAG, "I2S queue create failed");
        *((QueueHandle_t *) i2s_queue) = p_i2s[i2s_num]->i2s_queue;
        ESP_LOGI(TAG, "queue free spaces: %d", uxQueueSpacesAvailable(p_i2s[i2s_num]->i2s_queue));
    } else {
        p_i2s[i2s_num]->i2s_queue = NULL;
    }

    //set clock and start
#if SOC_I2S_SUPPORTS_TDM
    ret = i2s_set_clk(i2s_num, i2s_config->sample_rate,
                      p_i2s[i2s_num]->hal_cfg.bits_cfg.val,
                      (i2s_channel_t)p_i2s[i2s_num]->hal_cfg.chan_cfg.active_chan);
#else
    ret = i2s_set_clk(i2s_num, i2s_config->sample_rate,
                      p_i2s[i2s_num]->hal_cfg.bits_cfg.val,
                      I2S_CHANNEL_STEREO);
#endif
    ESP_GOTO_ON_ERROR(ret, err, TAG, "I2S set clock failed");
    return ret;

err:
#ifdef CONFIG_PM_ENABLE
    if (p_i2s[i2s_num]->pm_lock) {
        esp_pm_lock_delete(p_i2s[i2s_num]->pm_lock);
    }
#endif
    i2s_driver_uninstall(i2s_num);
    return ret;
}

esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)
{
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    if (p_i2s[i2s_num] == NULL) {
        ESP_LOGI(TAG, "already uninstalled");
        return ESP_OK;
    }
    i2s_stop(i2s_num);
#if SOC_I2S_SUPPORTS_ADC_DAC
    i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE);
#endif
#if SOC_GDMA_SUPPORTED
    if (p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        gdma_disconnect(p_i2s[i2s_num]->tx_dma_chan);
        gdma_del_channel(p_i2s[i2s_num]->tx_dma_chan);
    }
    if (p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        gdma_disconnect(p_i2s[i2s_num]->rx_dma_chan);
        gdma_del_channel(p_i2s[i2s_num]->rx_dma_chan);
    }
#else
    esp_intr_free(p_i2s[i2s_num]->i2s_isr_handle);
#endif
    if (p_i2s[i2s_num]->tx != NULL && p_i2s[i2s_num]->mode & I2S_MODE_TX) {
        i2s_destroy_dma_queue(i2s_num, p_i2s[i2s_num]->tx);
        p_i2s[i2s_num]->tx = NULL;
    }
    if (p_i2s[i2s_num]->rx != NULL && p_i2s[i2s_num]->mode & I2S_MODE_RX) {
        i2s_destroy_dma_queue(i2s_num, p_i2s[i2s_num]->rx);
        p_i2s[i2s_num]->rx = NULL;
    }

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
    }
#endif

    free(p_i2s[i2s_num]);
    p_i2s[i2s_num] = NULL;
#if !SOC_GDMA_SUPPORTED
    periph_module_disable(i2s_periph_signal[i2s_num].module);
#endif
    return ESP_OK;
}

esp_err_t i2s_write(i2s_port_t i2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr, *src_byte;
    size_t bytes_can_write;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((size < I2S_MAX_BUFFER_SIZE), ESP_ERR_INVALID_ARG, TAG, "size is too large");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->tx), ESP_ERR_INVALID_ARG, TAG, "tx NULL");
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

esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait)
{
    char *data_ptr;
    int bytes_can_write, tail;
    int src_bytes, aim_bytes, zero_bytes;
    *bytes_written = 0;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((size > 0), ESP_ERR_INVALID_ARG, TAG, "size must greater than zero");
    ESP_RETURN_ON_FALSE((aim_bits * size < I2S_MAX_BUFFER_SIZE), ESP_ERR_INVALID_ARG, TAG, "size is too large");
    ESP_RETURN_ON_FALSE((aim_bits >= src_bits), ESP_ERR_INVALID_ARG, TAG, "aim_bits mustn't be less than src_bits");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->tx), ESP_ERR_INVALID_ARG, TAG, "tx NULL");
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

esp_err_t i2s_read(i2s_port_t i2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    char *data_ptr, *dest_byte;
    int bytes_can_read;
    *bytes_read = 0;
    dest_byte = (char *)dest;
    ESP_RETURN_ON_FALSE((i2s_num < I2S_NUM_MAX), ESP_ERR_INVALID_ARG, TAG, "i2s_num error");
    ESP_RETURN_ON_FALSE((size < I2S_MAX_BUFFER_SIZE), ESP_ERR_INVALID_ARG, TAG, "size is too large");
    ESP_RETURN_ON_FALSE((p_i2s[i2s_num]->rx), ESP_ERR_INVALID_ARG, TAG, "rx NULL");
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
