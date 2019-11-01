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

#include "media_hal_config.h"
#include "es8388_interface.h"
#include "media_hal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "esp_log.h"


//Macro define
#define HAL_TAG "MEDIA_HAL"

#define MEDIA_HAL_CHECK(a, str, ret_val)                           \
    if (!(a)) {                                                     \
        ESP_LOGE(HAL_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str);   \
        return ret_val;                                                 \
    }

//type define
typedef struct media_hal {
    media_hal_state_t state;
    codec_mode_t current_mode;
    xSemaphoreHandle lock;
} media_hal_env_t;

//local variable
static char MUSIC_BITS = 16;                //for re-bits feature, but only for 16 to 32
static int AMPLIFIER = 1 << 8;              //amplify the volume, fixed point

static const media_hal_config_t media_hal_cfg = {
    .role = CODEC_ROLE_SLAVE,
    .i2s_port = MHC_I2S_PORT,
    .i2s_pin = {
        .bck_io_num = MHC_I2S_SCLK,
        .ws_io_num = MHC_I2S_WS,
        .data_out_num = MHC_I2S_DATA_IN,
        .data_in_num = MHC_I2S_DATA_OUT,
    },
    .i2s_cfg = {
#if MHC_I2S_DAC_EN == 1
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
#else
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
#endif
        .sample_rate = 48000,
        .bits_per_sample = MHC_I2S_SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if MHC_I2S_DAC_EN == 1
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
#else
        .communication_format = I2S_COMM_FORMAT_I2S,
#endif
        //when dma_buf_count = 3 and dma_buf_len = 300, then 3 * 4 * 300 * 2 Bytes internal RAM will be used. The multiplier 2 is for Rx buffer and Tx buffer together.
        .dma_buf_count = 3,                             // amount of the DMA buffer sectors
        .dma_buf_len = 300,                             // DMA buffer size of each sector (word, i.e. 4 Bytes)
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
#if MHC_I2S_DAC_EN == 0
        .use_apll = 1,
#endif
    },
    .i2c_port = MHC_I2C_PORT,
    .i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = MHC_I2C_CLK_DATA,
        .scl_io_num = MHC_I2C_CLK_PORT,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    },
    .adc_input = ADC_INPUT_LINPUT1_RINPUT1,
    .dac_output = DAC_OUTPUT_LOUT1 | DAC_OUTPUT_LOUT2 | DAC_OUTPUT_ROUT1 | DAC_OUTPUT_ROUT2,
};

static media_hal_env_t media_hal_env = {
    .state = MEDIA_HAL_STATE_UNKNOWN,
    .current_mode = CODEC_MODE_UNKNOWN,
    .lock = NULL,
};

static media_hal_codec_ops_t media_hal_codec_ops;

//functions definition
esp_err_t media_hal_init(const media_hal_codec_ops_t *codec_ops)
{
    esp_err_t ret = 0;
#ifdef CONFIG_PA_EN_GPIO_ENABLE
    gpio_config_t  io_conf;
#endif
    if (media_hal_env.state != MEDIA_HAL_STATE_UNKNOWN) {
        return ESP_ERR_INVALID_STATE;
    }

    if (codec_ops == NULL) {
        ESP_LOGE(HAL_TAG, "Codec operations point is NULL");
        return ESP_FAIL;
    }

    if (media_hal_env.lock) {
        vSemaphoreDelete(media_hal_env.lock);
    }
    media_hal_env.lock = xSemaphoreCreateMutex();
    if (media_hal_env.lock == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Set I2S pins
    ret = i2s_driver_install(MHC_I2S_PORT, &media_hal_cfg.i2s_cfg, 0, NULL);
    if (MHC_I2S_PORT > 1 || MHC_I2S_PORT < 0) {
        ESP_LOGE(HAL_TAG, "Must set MHC_I2S_PORT as 0 or 1");
        goto _env_err;
    }

#if MHC_I2S_DAC_EN == 1
    //init DAC pad
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#else
    ret = i2s_set_pin(MHC_I2S_PORT, &media_hal_cfg.i2s_pin);
    if (ret != ESP_OK) {
        goto _i2s_err;
    }

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO35_U, FUNC_GPIO35_GPIO35);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO35_U, FUN_IE | FUN_PU);
    PIN_INPUT_ENABLE(PERIPHS_IO_MUX_GPIO35_U);

    memcpy(&media_hal_codec_ops, codec_ops, sizeof(media_hal_codec_ops_t));

    ret = media_hal_codec_ops.codec_init(&media_hal_cfg);
    if (ret != ESP_OK) {
        goto _i2s_err;
    }

    ret = media_hal_codec_ops.codec_config_fmt(CODEC_MODULE_ADC_DAC, CODEC_I2S_NORMAL);
    if (ret != ESP_OK) {
        goto _codec_err;
    }

    ret = media_hal_codec_ops.codec_set_bit(CODEC_MODULE_ADC_DAC, BIT_LENGTH_16BITS);
    if (ret != ESP_OK) {
        goto _codec_err;
    }

    ret = media_hal_codec_ops.codec_set_adc_input(ADC_INPUT_LINPUT2_RINPUT2);
    if (ret != ESP_OK) {
        goto _codec_err;
    }

    ret = media_hal_codec_ops.codec_start(CODEC_MODULE_ADC_DAC);
    if (ret != ESP_OK) {
        goto _codec_err;
    }

#if MHC_I2S_MCLK_OUT_GPIO0_EN
    SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, MHC_I2S_PORT, CLK_OUT1_S);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
#endif

#endif

    media_hal_env.current_mode = CODEC_MODE_UNKNOWN;

#if MHC_PA_EN_GPIO_EN
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = MHC_GPIO_SEL_PA_EN;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(MHC_GPIO_PA_EN, 1);
#endif

    ret = media_hal_codec_ops.codec_set_vol(MHC_I2S_OUT_VOL_DEFAULT);
    if (ret != ESP_OK) {
        goto _codec_err;
    }

    ESP_LOGI(HAL_TAG, "MHC_I2S_OUT_VOL_DEFAULT[%d]", MHC_I2S_OUT_VOL_DEFAULT);

    media_hal_env.state = MEDIA_HAL_STATE_INIT;

    return ESP_OK;

_codec_err:
    media_hal_codec_ops.codec_uninit(&media_hal_cfg);
_i2s_err:
    i2s_driver_uninstall(MHC_I2S_PORT);
_env_err:
    vSemaphoreDelete(media_hal_env.lock);
    media_hal_env.lock = NULL;

    return ret;
}

void media_hal_deinit(void)
{
    if (media_hal_env.state == MEDIA_HAL_STATE_UNKNOWN) {
        return;
    }

    vSemaphoreDelete(media_hal_env.lock);
    media_hal_env.lock = NULL;
#if MHC_I2S_DAC_EN == 0
    media_hal_codec_ops.codec_uninit(&media_hal_cfg);
#endif
    media_hal_env.lock = NULL;
    MUSIC_BITS = 0;
    media_hal_env.current_mode = CODEC_MODE_UNKNOWN;
    media_hal_env.state = MEDIA_HAL_STATE_UNKNOWN;
}

esp_err_t media_hal_start(codec_mode_t mode)
{
    int input_config;
    esp_err_t ret = ESP_OK;
#if MHC_I2S_DAC_EN == 0
    int module = 0;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    switch (mode) {
    case CODEC_MODE_ENCODE:
        module  = CODEC_MODULE_ADC;
        break;
    case CODEC_MODE_LINE_IN:
        module  = CODEC_MODULE_LINE;
        break;
    case CODEC_MODE_DECODE:
        module  = CODEC_MODULE_DAC;
        break;
    case CODEC_MODE_DECODE_ENCODE:
        module  = CODEC_MODULE_ADC_DAC;
        break;
    default:
        module = CODEC_MODULE_DAC;
        ESP_LOGW(HAL_TAG, "Codec mode not support, default is decode mode");
        break;
    }
    ESP_LOGI(HAL_TAG, "Codec module is %d", module);

    if (module == CODEC_MODULE_LINE) {
        input_config = ADC_INPUT_LINPUT2_RINPUT2;
    } else {
        input_config = ADC_INPUT_LINPUT1_RINPUT1;
    }

    ret = media_hal_codec_ops.codec_set_adc_input(input_config);
    if (ret != ESP_OK) {
        xSemaphoreGive(media_hal_env.lock);
        return ret;
    }

    ret = media_hal_codec_ops.codec_start(module);
    if (ret != ESP_OK) {
        xSemaphoreGive(media_hal_env.lock);
        return ret;
    }

    ESP_LOGI(HAL_TAG, "Codec input_config is %2X", input_config);

    media_hal_env.current_mode = mode;
    xSemaphoreGive(media_hal_env.lock);
#endif /* MHC_I2S_DAC_EN */
    return ret;
}

esp_err_t media_hal_stop(codec_mode_t mode)
{
    esp_err_t ret = ESP_OK;
#if MHC_I2S_DAC_EN == 0
    int module = 0;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    switch (mode) {
    case CODEC_MODE_ENCODE:
        module = CODEC_MODULE_ADC;
        break;
    case CODEC_MODE_LINE_IN:
        module = CODEC_MODULE_LINE;
        break;
    case CODEC_MODE_DECODE:
        module = CODEC_MODULE_DAC;
        break;
    default:
        module = CODEC_MODULE_DAC;
        ESP_LOGI(HAL_TAG, "Codec mode not support");
        break;
    }

    ret = media_hal_codec_ops.codec_stop(module);
    media_hal_env.current_mode = CODEC_MODE_UNKNOWN;
    xSemaphoreGive(media_hal_env.lock);
#endif
    return ret;
}

esp_err_t media_hal_get_current_mode(codec_mode_t *mode)
{
    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    MEDIA_HAL_CHECK(mode != NULL, "Get current mode para is null", ESP_ERR_INVALID_ARG);
    *mode = media_hal_env.current_mode;
    return ESP_OK;
}

esp_err_t media_hal_set_volume(int volume)
{
    esp_err_t ret = ESP_OK;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
#if MHC_I2S_DAC_EN == 0
    int mute;
    media_hal_codec_ops.codec_get_mute(&mute);
    if (volume < 3 ) {
        if (0 == mute) {
            media_hal_codec_ops.codec_set_mute(CODEC_MUTE_ENABLE);
        }
    } else {
        if ((1 == mute)) {
            media_hal_codec_ops.codec_set_mute(CODEC_MUTE_DISABLE);
        }
    }
    ret = media_hal_codec_ops.codec_set_vol(volume);

    xSemaphoreGive(media_hal_env.lock);
#endif
    return ret;
}

int media_hal_get_volume(int *volume)
{
    esp_err_t ret = ESP_OK;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    MEDIA_HAL_CHECK(volume != NULL, "Get volume para is null", ESP_ERR_INVALID_ARG);
#if MHC_I2S_DAC_EN == 0
    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    ret = media_hal_codec_ops.codec_get_vol(volume);
    xSemaphoreGive(media_hal_env.lock);
#endif
    return ret;
}

esp_err_t media_hal_set_volume_amplify(float scale)
{
    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    AMPLIFIER = scale * (1 << 8);
    xSemaphoreGive(media_hal_env.lock);

    return ESP_OK;
}

int media_hal_get_volume_amplify()
{
    return AMPLIFIER;
}

esp_err_t media_hal_set_mute(codec_mute_t mute)
{
    esp_err_t ret = ESP_OK;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

#if MHC_I2S_DAC_EN == 0
    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    ret = media_hal_codec_ops.codec_set_mute(mute);
    xSemaphoreGive(media_hal_env.lock);
#endif
    return ret;
}

int media_hal_get_mute(void)
{
    int mute = 0;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return -1;
    }

#if MHC_I2S_DAC_EN == 0
    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    media_hal_codec_ops.codec_get_mute(&mute);
    xSemaphoreGive(media_hal_env.lock);
#endif
    return mute;
}

esp_err_t media_hal_set_bits(int bit_per_sample)
{
    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    if (bit_per_sample <= BIT_LENGTH_MIN || bit_per_sample >= BIT_LENGTH_MAX) {
        ESP_LOGE(HAL_TAG, "bt_per_sample: wrong param");
        return ESP_ERR_INVALID_ARG;
    }
#if MHC_I2S_DAC_EN == 0
    xSemaphoreTake(media_hal_env.lock, portMAX_DELAY);
    media_hal_codec_ops.codec_set_bit(CODEC_MODULE_ADC_DAC, (bits_length_t)bit_per_sample);
    xSemaphoreGive(media_hal_env.lock);
#endif
    return ESP_OK;
}

esp_err_t media_hal_set_clk(uint32_t rate, uint8_t bits, uint32_t ch)
{
    int ret;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    if (bits != 16 && bits != 32) {
        ESP_LOGE(HAL_TAG, "bit should be 16 or 32, Bit:%d", bits);
        return ESP_ERR_INVALID_ARG;
    }
    if (ch != 1 && ch != 2) {
        ESP_LOGE(HAL_TAG, "channel should be 1 or 2 %d", ch);
        return ESP_ERR_INVALID_ARG;
    }
    if (bits > MHC_I2S_SAMPLE_BITS) {
        ESP_LOGE(HAL_TAG, "Bits:%d, bits must be smaller than %d", bits, MHC_I2S_SAMPLE_BITS);
        return ESP_ERR_INVALID_ARG;
    }

    MUSIC_BITS = bits;
    ret = i2s_set_clk((i2s_port_t)MHC_I2S_PORT, rate, MHC_I2S_SAMPLE_BITS, ch);

    return ret;
}

esp_err_t media_hal_get_i2s_config(void *info)
{
    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    MEDIA_HAL_CHECK(info != NULL, "Get I2S config info NULL", ESP_ERR_INVALID_ARG);

    if (info) {
        memcpy(info, &media_hal_cfg.i2s_cfg, sizeof(i2s_config_t));
    }

    return ESP_OK;
}

int media_hal_write_data(const uint8_t *data, int data_len)
{
    esp_err_t ret;
    size_t bytes_written = data_len;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return -1;
    }

    ret = i2s_write(MHC_I2C_PORT, data, data_len, &bytes_written, portMAX_DELAY);

    return (ret == ESP_OK ? bytes_written : -1);
}

esp_err_t media_hal_clear_data(void)
{
    esp_err_t ret;

    ret = i2s_zero_dma_buffer(MHC_I2S_PORT);

    return ret;
}

int media_hal_get_i2s_num(void)
{
    return MHC_I2S_PORT;
}

int media_hal_get_i2s_bits(void)
{
    return MHC_I2S_SAMPLE_BITS;
}

int media_hal_get_src_bits(void)
{
    return MUSIC_BITS;
}

int media_hal_get_i2s_dac_mode(void)
{
    return MHC_I2S_DAC_EN;
}

int media_hal_get_i2s_adc_mode(void)
{
    return MHC_I2S_ADC_EN;
}

esp_err_t media_hal_pa_pwr(int en)
{
    esp_err_t ret;

    if (media_hal_env.state != MEDIA_HAL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    if (en) {
        ret = gpio_set_level(MHC_GPIO_PA_EN, 1);
    } else {
        ret = gpio_set_level(MHC_GPIO_PA_EN, 0);
    }

    return ret;
}

esp_err_t media_hal_get_state(media_hal_state_t *state)
{
    MEDIA_HAL_CHECK(state != NULL, "Get Media HAL, state is null", ESP_ERR_INVALID_ARG);

    *state = media_hal_env.state;
    return ESP_OK;
}
