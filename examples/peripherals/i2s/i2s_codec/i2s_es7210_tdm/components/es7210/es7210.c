/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "es7210.h"
#include "esp_log.h"
#include "esp_check.h"

const static char *TAG = "ES7210";

#define IS_ES7210_I2S_FMT(val) (((val)==ES7210_I2S_FMT_I2S) || ((val)==ES7210_I2S_FMT_LJ) || \
        ((val)==ES7210_I2S_FMT_DSP_A) || ((val)==ES7210_I2S_FMT_DSP_B))

#define IS_ES7210_I2S_BITS(val) (((val)==ES7210_I2S_BITS_24B) || ((val)==ES7210_I2S_BITS_20B) || \
        ((val)==ES7210_I2S_BITS_18B) || ((val)==ES7210_I2S_BITS_16B) || ((val)==ES7210_I2S_BITS_32B))

#define IS_ES7210_MIC_GAIN(val) (((val) >= ES7210_MIC_GAIN_0DB) && ((val) <= ES7210_MIC_GAIN_37_5DB))

#define IS_ES7210_MIC_BIAS(val) (((val)==ES7210_MIC_BIAS_2V18) || ((val)==ES7210_MIC_BIAS_2V26) || \
        ((val)==ES7210_MIC_BIAS_2V36) || ((val)==ES7210_MIC_BIAS_2V45) || ((val)==ES7210_MIC_BIAS_2V55) || \
        ((val)==ES7210_MIC_BIAS_2V66) || ((val)==ES7210_MIC_BIAS_2V78) || ((val)==ES7210_MIC_BIAS_2V87))

#define ES7210_WRITE_REG(reg_addr, reg_value) do { \
    ESP_RETURN_ON_ERROR(es7210_write_reg(handle, (reg_addr), (reg_value)), \
                        TAG, "i2c communication error while writing "#reg_addr); \
} while(0)

struct es7210_dev_t {
    i2c_port_t i2c_port; // TODO: update to i2c handle in future driver-NG
    uint8_t i2c_addr;
};

/**
 * @brief Clock coefficient structure
 *
 */
typedef struct {
    uint32_t mclk;            /*!< mclk frequency */
    uint32_t lrck;            /*!< lrck */
    uint8_t  ss_ds;           /*!< not used */
    uint8_t  adc_div;         /*!< adcclk divider */
    uint8_t  dll;             /*!< dll_bypass */
    uint8_t  doubler;         /*!< doubler enable */
    uint8_t  osr;             /*!< adc osr */
    uint8_t  mclk_src;        /*!< select mclk  source */
    uint32_t lrck_h;          /*!< The high 4 bits of lrck */
    uint32_t lrck_l;          /*!< The low 8 bits of lrck */
} coeff_div_t;

/**
 * @brief ES7210 clock coefficient lookup table
 *
 */
static const coeff_div_t es7210_coeff_div[] = {
//    mclk      lrck    ss_ds adc_div  dll  doubler osr  mclk_src  lrckh   lrckl
    /* 8k */
    {12288000,  8000 ,  0x00,  0x03,  0x01,  0x00,  0x20,  0x00,    0x06,  0x00},
    {16384000,  8000 ,  0x00,  0x04,  0x01,  0x00,  0x20,  0x00,    0x08,  0x00},
    {19200000,  8000 ,  0x00,  0x1e,  0x00,  0x01,  0x28,  0x00,    0x09,  0x60},
    {4096000,   8000 ,  0x00,  0x01,  0x01,  0x00,  0x20,  0x00,    0x02,  0x00},

    /* 11.025k */
    {11289600,  11025,  0x00,  0x02,  0x01,  0x00,  0x20,  0x00,    0x01,  0x00},

    /* 12k */
    {12288000,  12000,  0x00,  0x02,  0x01,  0x00,  0x20,  0x00,    0x04,  0x00},
    {19200000,  12000,  0x00,  0x14,  0x00,  0x01,  0x28,  0x00,    0x06,  0x40},

    /* 16k */
    {4096000,   16000,  0x00,  0x01,  0x01,  0x01,  0x20,  0x00,    0x01,  0x00},
    {19200000,  16000,  0x00,  0x0a,  0x00,  0x00,  0x1e,  0x00,    0x04,  0x80},
    {16384000,  16000,  0x00,  0x02,  0x01,  0x00,  0x20,  0x00,    0x04,  0x00},
    {12288000,  16000,  0x00,  0x03,  0x01,  0x01,  0x20,  0x00,    0x03,  0x00},

    /* 22.05k */
    {11289600,  22050,  0x00,  0x01,  0x01,  0x00,  0x20,  0x00,    0x02,  0x00},

    /* 24k */
    {12288000,  24000,  0x00,  0x01,  0x01,  0x00,  0x20,  0x00,    0x02,  0x00},
    {19200000,  24000,  0x00,  0x0a,  0x00,  0x01,  0x28,  0x00,    0x03,  0x20},

    /* 32k */
    {12288000,  32000,  0x00,  0x03,  0x00,  0x00,  0x20,  0x00,    0x01,  0x80},
    {16384000,  32000,  0x00,  0x01,  0x01,  0x00,  0x20,  0x00,    0x02,  0x00},
    {19200000,  32000,  0x00,  0x05,  0x00,  0x00,  0x1e,  0x00,    0x02,  0x58},

    /* 44.1k */
    {11289600,  44100,  0x00,  0x01,  0x01,  0x01,  0x20,  0x00,    0x01,  0x00},

    /* 48k */
    {12288000,  48000,  0x00,  0x01,  0x01,  0x01,  0x20,  0x00,    0x01,  0x00},
    {19200000,  48000,  0x00,  0x05,  0x00,  0x01,  0x28,  0x00,    0x01,  0x90},

    /* 64k */
    {16384000,  64000,  0x01,  0x01,  0x01,  0x00,  0x20,  0x00,    0x01,  0x00},
    {19200000,  64000,  0x00,  0x05,  0x00,  0x01,  0x1e,  0x00,    0x01,  0x2c},

    /* 88.2k */
    {11289600,  88200,  0x01,  0x01,  0x01,  0x01,  0x20,  0x00,    0x00,  0x80},

    /* 96k */
    {12288000,  96000,  0x01,  0x01,  0x01,  0x01,  0x20,  0x00,    0x00,  0x80},
    {19200000,  96000,  0x01,  0x05,  0x00,  0x01,  0x28,  0x00,    0x00,  0xc8},
};

/**
 * @brief Get coefficient from coefficient table
 *
 * @param mclk Desired MCLK value
 * @param lrck Desired LRCK vaule
 * @return Coefficient struct, NULL if desired value cannot be achieved
 */
static const coeff_div_t *es7210_get_coeff(uint32_t mclk, uint32_t lrck)
{
    for (int i = 0; i < sizeof(es7210_coeff_div) / sizeof(coeff_div_t); i++) {
        if (es7210_coeff_div[i].lrck == lrck && es7210_coeff_div[i].mclk == mclk)
            return &es7210_coeff_div[i];
    }
    return NULL;
}

static esp_err_t es7210_write_reg(es7210_dev_handle_t handle, uint8_t reg_addr, uint8_t reg_val)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle");
    esp_err_t ret = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_GOTO_ON_FALSE(cmd, ESP_ERR_NO_MEM, err, TAG, "memory allocation for i2c cmd handle failed");

    ESP_GOTO_ON_ERROR(i2c_master_start(cmd), err, TAG, "error while appending i2c command");
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd, handle->i2c_addr<<1 | I2C_MASTER_WRITE, true),
                        err, TAG, "error while appending i2c command");
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd, reg_addr, true), err,
                      TAG, "error while appending i2c command");
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd, reg_val, true), err,
                      TAG, "error while appending i2c command");
    ESP_GOTO_ON_ERROR(i2c_master_stop(cmd), err, TAG, "error while appending i2c command");

    ESP_GOTO_ON_ERROR(i2c_master_cmd_begin(handle->i2c_port, cmd, pdMS_TO_TICKS(1000)),
                        err, TAG, "error while writing register");
err:
    if(cmd) {
        i2c_cmd_link_delete(cmd);
    }
    return ret;
}

static esp_err_t es7210_set_i2s_format(es7210_dev_handle_t handle, es7210_i2s_fmt_t i2s_format,
                                       es7210_i2s_bits_t bit_width, bool tdm_enable)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");
    ESP_RETURN_ON_FALSE(IS_ES7210_I2S_FMT(i2s_format), ESP_ERR_INVALID_ARG, TAG, "invalid i2s format argument");
    ESP_RETURN_ON_FALSE(IS_ES7210_I2S_BITS(bit_width), ESP_ERR_INVALID_ARG, TAG, "invalid i2s bit width argument");

    uint8_t reg_val = 0;

    switch (bit_width) {
    case ES7210_I2S_BITS_16B:
        reg_val = 0x60;
        break;
    case ES7210_I2S_BITS_18B:
        reg_val = 0x40;
        break;
    case ES7210_I2S_BITS_20B:
        reg_val = 0x20;
        break;
    case ES7210_I2S_BITS_24B:
        reg_val = 0x00;
        break;
    case ES7210_I2S_BITS_32B:
        reg_val = 0x80;
        break;
    default:
        abort();
    }
    ES7210_WRITE_REG(ES7210_SDP_INTERFACE1_REG11, i2s_format | reg_val);

    const char *mode_str = NULL;
    switch (i2s_format) {
    case ES7210_I2S_FMT_I2S:
        reg_val = 0x02;
        mode_str = "standard i2s";
        break;
    case ES7210_I2S_FMT_LJ:
        reg_val = 0x02;
        mode_str = "left justify";
        break;
    case ES7210_I2S_FMT_DSP_A:
        reg_val = 0x01;
        mode_str = "DSP-A";
        break;
    case ES7210_I2S_FMT_DSP_B:
        reg_val = 0x01;
        mode_str = "DSP-B";
        break;
    default:
        abort();
    }

    if (tdm_enable) { // enable 1xFS TDM
        ES7210_WRITE_REG(ES7210_SDP_INTERFACE2_REG12, reg_val);
    } else {
        ES7210_WRITE_REG(ES7210_SDP_INTERFACE2_REG12, 0x00);
    }

    ESP_LOGI(TAG, "format: %s, bit width: %d, tdm mode %s", mode_str, bit_width, tdm_enable ? "enabled" : "disabled");
    return ESP_OK;
}

static esp_err_t es7210_set_i2s_sample_rate(es7210_dev_handle_t handle, uint32_t sample_rate_hz, uint32_t mclk_ratio)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

    uint32_t mclk_freq_hz = sample_rate_hz * mclk_ratio;
    const coeff_div_t *coeff_div = es7210_get_coeff(mclk_freq_hz, sample_rate_hz);
    ESP_RETURN_ON_FALSE(coeff_div, ESP_ERR_NOT_SUPPORTED, TAG,
                        "unable to set %"PRIu32"Hz sample rate with %"PRIu32"Hz MCLK", sample_rate_hz, mclk_freq_hz);
    /* Set osr */
    ES7210_WRITE_REG(ES7210_OSR_REG07, coeff_div->osr);
    /* Set adc_div & doubler & dll */
    ES7210_WRITE_REG(ES7210_MAINCLK_REG02, (coeff_div->adc_div) | (coeff_div->doubler << 6) | (coeff_div->dll << 7));
    /* Set lrck */
    ES7210_WRITE_REG(ES7210_LRCK_DIVH_REG04, coeff_div->lrck_h);
    ES7210_WRITE_REG(ES7210_LRCK_DIVL_REG05, coeff_div->lrck_l);

    ESP_LOGI(TAG, "sample rate: %"PRIu32"Hz, mclk frequency: %"PRIu32"Hz", sample_rate_hz, mclk_freq_hz);
    return ESP_OK;
}

static esp_err_t es7210_set_mic_gain(es7210_dev_handle_t handle, es7210_mic_gain_t mic_gain)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");
    ESP_RETURN_ON_FALSE(IS_ES7210_MIC_GAIN(mic_gain), ESP_ERR_INVALID_ARG, TAG, "invalid mic gain value");

    ES7210_WRITE_REG(ES7210_MIC1_GAIN_REG43, mic_gain | 0x10);
    ES7210_WRITE_REG(ES7210_MIC2_GAIN_REG44, mic_gain | 0x10);
    ES7210_WRITE_REG(ES7210_MIC3_GAIN_REG45, mic_gain | 0x10);
    ES7210_WRITE_REG(ES7210_MIC4_GAIN_REG46, mic_gain | 0x10);

    return ESP_OK;
}

static esp_err_t es7210_set_mic_bias(es7210_dev_handle_t handle, es7210_mic_bias_t mic_bias)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");
    ESP_RETURN_ON_FALSE(IS_ES7210_MIC_BIAS(mic_bias), ESP_ERR_INVALID_ARG, TAG, "invalid mic bias value");

    ES7210_WRITE_REG(ES7210_MIC12_BIAS_REG41, mic_bias);
    ES7210_WRITE_REG(ES7210_MIC34_BIAS_REG42, mic_bias);

    return ESP_OK;
}

esp_err_t es7210_new_codec(const es7210_i2c_config_t *i2c_conf, es7210_dev_handle_t *handle_out)
{
    ESP_RETURN_ON_FALSE(i2c_conf, ESP_ERR_INVALID_ARG, TAG, "invalid device config pointer");
    ESP_RETURN_ON_FALSE(handle_out, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

    struct es7210_dev_t *handle = calloc(1, sizeof(struct es7210_dev_t));
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "memory allocation for device handler failed");

    handle->i2c_port = i2c_conf->i2c_port; // TODO: check i2c handle in future driver-NG
    handle->i2c_addr = i2c_conf->i2c_addr;

    *handle_out = handle;
    return ESP_OK;
}

esp_err_t es7210_del_codec(es7210_dev_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");

    free(handle);

    return ESP_OK;
}

esp_err_t es7210_config_codec(es7210_dev_handle_t handle, const es7210_codec_config_t *codec_conf)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");
    ESP_RETURN_ON_FALSE(codec_conf, ESP_ERR_INVALID_ARG, TAG, "invalid codec config pointer");

    /* Perform software reset */
    ES7210_WRITE_REG(ES7210_RESET_REG00, 0xFF);
    ES7210_WRITE_REG(ES7210_RESET_REG00, 0x32);
    /* Set the initialization time when device powers up */
    ES7210_WRITE_REG(ES7210_TIME_CONTROL0_REG09, 0x30);
    ES7210_WRITE_REG(ES7210_TIME_CONTROL1_REG0A, 0x30);
    /* Configure HPF for ADC1-4 */
    ES7210_WRITE_REG(ES7210_ADC12_HPF1_REG23, 0x2A);
    ES7210_WRITE_REG(ES7210_ADC12_HPF2_REG22, 0x0A);
    ES7210_WRITE_REG(ES7210_ADC34_HPF1_REG21, 0x2A);
    ES7210_WRITE_REG(ES7210_ADC34_HPF2_REG20, 0x0A);
    /* Set bits per sample to 16, data protocal to I2S, enable 1xFS TDM */
    ESP_RETURN_ON_ERROR(es7210_set_i2s_format(handle, codec_conf->i2s_format, codec_conf->bit_width,
                        codec_conf->flags.tdm_enable), TAG, "error while setting i2s format");
    /* Configure analog power and VMID voltage */
    ES7210_WRITE_REG(ES7210_ANALOG_REG40, 0xC3);
    /* Set MIC14 bias to 2.87V */
    ESP_RETURN_ON_ERROR(es7210_set_mic_bias(handle, codec_conf->mic_bias), TAG, "error while setting mic bias");
    /* Set MIC1-4 gain to 30dB */
    ESP_RETURN_ON_ERROR(es7210_set_mic_gain(handle, codec_conf->mic_gain), TAG, "error while setting mic gain");
    /* Power on MIC1-4 */
    ES7210_WRITE_REG(ES7210_MIC1_POWER_REG47, 0x08);
    ES7210_WRITE_REG(ES7210_MIC2_POWER_REG48, 0x08);
    ES7210_WRITE_REG(ES7210_MIC3_POWER_REG49, 0x08);
    ES7210_WRITE_REG(ES7210_MIC4_POWER_REG4A, 0x08);
    /* Set ADC sample rate to 48kHz */
    ESP_RETURN_ON_ERROR(es7210_set_i2s_sample_rate(handle, codec_conf->sample_rate_hz, codec_conf->mclk_ratio),
                        TAG, "error while setting sample rate");
    /* Power down DLL */
    ES7210_WRITE_REG(ES7210_POWER_DOWN_REG06, 0x04);
    /* Power on MIC1-4 bias & ADC1-4 & PGA1-4 Power */
    ES7210_WRITE_REG(ES7210_MIC12_POWER_REG4B, 0x0F);
    ES7210_WRITE_REG(ES7210_MIC34_POWER_REG4C, 0x0F);
    /* Enable device */
    ES7210_WRITE_REG(ES7210_RESET_REG00, 0x71);
    ES7210_WRITE_REG(ES7210_RESET_REG00, 0x41);

    return ESP_OK;
}

esp_err_t es7210_config_volume(es7210_dev_handle_t handle, int8_t volume_db)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle pointer");
    ESP_RETURN_ON_FALSE(volume_db >= -95 && volume_db <= 32, ESP_ERR_INVALID_ARG, TAG, "invalid volume range");

    /*
     * reg_val: 0x00 represents -95.5dB, 0xBF represents 0dB (default after reset),
     * and 0xFF represents +32dB, with a 0.5dB step
     */
    uint8_t reg_val = 191 + volume_db * 2;

    ES7210_WRITE_REG(ES7210_ADC1_DIRECT_DB_REG1B, reg_val);
    ES7210_WRITE_REG(ES7210_ADC2_DIRECT_DB_REG1C, reg_val);
    ES7210_WRITE_REG(ES7210_ADC3_DIRECT_DB_REG1D, reg_val);
    ES7210_WRITE_REG(ES7210_ADC4_DIRECT_DB_REG1E, reg_val);

    return ESP_OK;
}
