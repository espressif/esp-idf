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
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "es8388_interface.h"
#include "media_hal.h"

#define ES8388_TAG "8388"

#define ES8388_CHECK(a, str, ret_val)                                       \
    if ((a) != ESP_OK) {                                                    \
        ESP_LOGE(ES8388_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str);    \
        return ret_val;                                                     \
    }

/**
 * @brief Write ES8388 register
 *
 * @param slave_addr : slave address
 * @param reg_addr    : register address
 * @param data      : data to write
 *
 * @return
 *     - (-1)  Error
 *     - (0)   Success
 */
static esp_err_t _es8388_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    esp_err_t res = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_write_byte(cmd, slave_addr, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_write_byte(cmd, data, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_stop(cmd);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
    if (res != ESP_OK) {
        return res;
    }

    i2c_cmd_link_delete(cmd);
    ES8388_CHECK(res, "_es8388_write_reg error", -1);

    return res;
}

/**
 * @brief Read ES8388 register
 *
 * @param reg_addr    : register address
 *
 * @return
 *     - (-1)     Error
 *     - (0)      Success
 */
static esp_err_t _es8388_read_reg(uint8_t reg_addr, uint8_t *p_data)
{
    uint8_t data;
    esp_err_t res = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_write_byte(cmd, ES8388_ADDR, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_stop(cmd);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
    if (res != ESP_OK) {
        return res;
    }

    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    res = i2c_master_start(cmd);
    if (res != ESP_OK) {
        return res;
    }


    res = i2c_master_write_byte(cmd, ES8388_ADDR | 0x01, 1 /*ACK_CHECK_EN*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_read_byte(cmd, &data, 0x01/*NACK_VAL*/);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_stop(cmd);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
    if (res != ESP_OK) {
        return res;
    }

    i2c_cmd_link_delete(cmd);

    ES8388_CHECK(res, "_es8388_read_reg error", -1);
    *p_data = data;

    return res;
}


/**
 * @brief Inite i2s master mode
 */
static esp_err_t i2c_init(const i2c_config_t *conf, int i2c_master_port)
{
    int res;

    res = i2c_param_config(i2c_master_port, conf);
    if (res != ESP_OK) {
        return res;
    }

    res = i2c_driver_install(i2c_master_port, conf->mode, 0, 0, 0);
    if (res != ESP_OK) {
        return res;
    }

    ES8388_CHECK(res, "i2c_init error", -1);

    return res;
}

/**
 * @brief Configure ES8388 ADC and DAC volume. Basicly you can consider this as ADC and DAC gain
 *
 * @param mode:             set ADC or DAC or all
 * @param volume:           -96 ~ 0              for example es8388_set_adc_dac_volume(CODEC_MODULE_ADC, 30, 6); means set ADC volume -30.5db
 * @param dot:              whether include 0.5. for example es8388_set_adc_dac_volume(CODEC_MODULE_ADC, 30, 4); means set ADC volume -30db
 *
 * @return
 *
 */
static esp_err_t es8388_set_adc_dac_volume(int module, int volume, int dot)
{
    esp_err_t res = ESP_OK;

    if ( volume < -96 || volume > 0 ) {
        ESP_LOGW(ES8388_TAG, "Warning: volume < -96! or > 0!\n");
        if (volume < -96) {
            volume = -96;
        } else {
            volume = 0;
        }
    }
    dot = (dot >= 5 ? 1 : 0);
    volume = (-volume << 1) + dot;
    if (module == CODEC_MODULE_ADC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL8, volume);
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL9, volume);  //ADC Right Volume=0db
        if (res != ESP_OK) {
            return res;
        }

    }
    if (module == CODEC_MODULE_DAC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL5, volume);
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL4, volume);
        if (res != ESP_OK) {
            return res;
        }
    }

    return res;
}

esp_err_t es8388_start(codec_module_t module)
{
    esp_err_t res = ESP_OK;
    uint8_t prevData = 0, data = 0;

    _es8388_read_reg(ES8388_DACCONTROL21, &prevData);

    if (module == CODEC_MODULE_LINE) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL16, 0x09); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2 by pass enable
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL17, 0x50); // left DAC to left mixer enable  and  LIN signal to left mixer enable 0db  : bupass enable
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL20, 0x50); // right DAC to right mixer enable  and  LIN signal to right mixer enable 0db : bupass enable
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL21, 0xC0); //enable adc
        if (res != ESP_OK) {
            return res;
        }

    } else {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL21, 0x80);   //enable dac
        if (res != ESP_OK) {
            return res;
        }
    }
    _es8388_read_reg(ES8388_DACCONTROL21, &data);
    if (prevData != data) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_CHIPPOWER, 0xF0);   //start state machine
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_CHIPPOWER, 0x00);   //start state machine
        if (res != ESP_OK) {
            return res;
        }

    }
    if (module == CODEC_MODULE_ADC || module == CODEC_MODULE_ADC_DAC || module == CODEC_MODULE_LINE)
        res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCPOWER, 0x00);   //power up adc and line in
        if (res != ESP_OK) {
            return res;
        }

    if (module == CODEC_MODULE_DAC || module == CODEC_MODULE_ADC_DAC || module == CODEC_MODULE_LINE) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACPOWER, 0x3c);   //power up dac and line out
        if (res != ESP_OK) {
            return res;
        }

        res = es8388_set_voice_mute(false);
        if (res != ESP_OK) {
            return res;
        }
    }

    return res;
}

esp_err_t es8388_stop(codec_module_t module)
{
    esp_err_t res = ESP_OK;

    if (module == CODEC_MODULE_LINE) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL21, 0x80); //enable dac
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL17, 0x90); // only left DAC to left mixer enable 0db
        if (res != ESP_OK) {
            return res;
        }

        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL20, 0x90); // only right DAC to right mixer enable 0db
        if (res != ESP_OK) {
            return res;
        }

        return res;
    }

    if (module == CODEC_MODULE_DAC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACPOWER, 0x00);
        if (res != ESP_OK) {
            return res;
        }

        res = es8388_set_voice_mute(true);
        if (res != ESP_OK) {
            return res;
        }
    }
    if (module == CODEC_MODULE_ADC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCPOWER, 0xFF);  //power down adc and line in
        if (res != ESP_OK) {
            return res;
        }
    }
    if (module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL21, 0x9C);  //disable mclk
        if (res != ESP_OK) {
            return res;
        }
    }

    return res;
}


esp_err_t es8388_i2s_config_clock(codec_i2s_clk_t cfg)
{
    esp_err_t res = ESP_OK;

    res = _es8388_write_reg(ES8388_ADDR, ES8388_MASTERMODE, cfg.sclk_div);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL5, cfg.lclk_div);  //ADCFsMode,singel SPEED,RATIO=256
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL2, cfg.lclk_div);  //ADCFsMode,singel SPEED,RATIO=256
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

void es8388_deinit(const media_hal_config_t *cfg)
{
    _es8388_write_reg(ES8388_ADDR, ES8388_CHIPPOWER, 0xFF);  //reset and stop es8388
    i2c_driver_delete(cfg->i2c_port);
}

esp_err_t es8388_init(const media_hal_config_t *cfg)
{
    esp_err_t res = ESP_OK;

    res = i2c_init(&cfg->i2c_cfg, cfg->i2c_port); // ESP32 in master mode
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL3, 0x04);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and  disabled digital volume control soft ramp
    if (res != ESP_OK) {
        return res;
    }

    /* Chip Control and Power Management */
    res = _es8388_write_reg(ES8388_ADDR, ES8388_CONTROL2, 0x50);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_CHIPPOWER, 0x00); //normal all and power up all
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_MASTERMODE, cfg->role); //CODEC IN I2S SLAVE ROLE
    if (res != ESP_OK) {
        return res;
    }

    /* dac */
    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACPOWER, 0xC0);  //disable DAC and disable Lout/Rout/1/2
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_CONTROL1, 0x12);  //Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL1, 0x18);//1a 0x18:16bit iis , 0x00:24
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL2, 0x02);  //DACFsMode,SINGLE SPEED; DACFsRatio,256
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL17, 0x90); // only left DAC to left mixer enable 0db
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL20, 0x90); // only right DAC to right mixer enable 0db
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL21, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL23, 0x00);   //vroi=0
    if (res != ESP_OK) {
        return res;
    }
    res = es8388_set_adc_dac_volume(CODEC_MODULE_DAC, 0, 0);          // 0db
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACPOWER, cfg->dac_output);  //0x3c Enable DAC and Enable Lout/Rout/1/2
    if (res != ESP_OK) {
        return res;
    }

    /* adc */
    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCPOWER, 0xFF);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL1, 0x88); //0x88 MIC PGA =24DB
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL2, cfg->adc_input);  //0x00 LINSEL & RINSEL, LIN1/RIN1 as ADC Input; DSSEL,use one DS Reg11; DSR, LINPUT1-RINPUT1
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL3, 0x02);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL4, 0x0c); //0d 0x0c I2S-16BIT, LEFT ADC DATA = LIN1 , RIGHT ADC DATA =RIN1
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL5, 0x02);  //ADCFsMode,singel SPEED,RATIO=256
    if (res != ESP_OK) {
        return res;
    }

    //ALC for Microphone
    res = es8388_set_adc_dac_volume(CODEC_MODULE_ADC, 0, 0);      // 0db
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCPOWER, 0x09); //Power up ADC, Enable LIN&RIN, Power down MICBIAS, set int1lp to low power mode
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

esp_err_t es8388_config_fmt(codec_module_t module, codec_i2s_fmt_t fmt)
{
    esp_err_t res = ESP_OK;
    uint8_t reg = 0;

    if (module == CODEC_MODULE_ADC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_read_reg(ES8388_ADCCONTROL4, &reg);
        if (res != ESP_OK) {
            return res;
        }

        reg = reg & 0xfc;
        res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL4, reg | fmt);
        if (res != ESP_OK) {
            return res;
        }
    }
    if (module == CODEC_MODULE_DAC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_read_reg(ES8388_DACCONTROL1, &reg);
        if (res != ESP_OK) {
            return res;
        }

        reg = reg & 0xf9;
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL1, reg | (fmt << 1));
        if (res != ESP_OK) {
            return res;
        }
    }

    return res;
}

esp_err_t es8388_set_voice_volume(int volume)
{
    int res;

    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    volume /= 3;
    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL24, volume);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL25, volume);  //ADC Right Volume=0db
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL26, 0);
    if (res != ESP_OK) {
        return res;
    }

    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL27, 0);
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

esp_err_t es8388_get_voice_volume(int *volume)
{
    int res;
    uint8_t reg = 0;

    res = _es8388_read_reg(ES8388_DACCONTROL24, &reg);
    if (res == ESP_FAIL) {
        *volume = 0;
    } else {
        *volume = reg;
        *volume *= 3;
        if (*volume == 99)
            *volume = 100;
    }

    return res;
}

esp_err_t es8388_set_bits_per_sample(codec_module_t module, bits_length_t bit_per_sample)
{
    esp_err_t res = ESP_OK;
    uint8_t reg = 0;
    int bits = (int)bit_per_sample;

    if (module == CODEC_MODULE_ADC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_read_reg(ES8388_ADCCONTROL4, &reg);
        if (res != ESP_OK) {
            return res;
        }

        reg = reg & 0xe3;
        res =  _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL4, reg | (bits << 2));
        if (res != ESP_OK) {
            return res;
        }
    }
    if (module == CODEC_MODULE_DAC || module == CODEC_MODULE_ADC_DAC) {
        res = _es8388_read_reg(ES8388_DACCONTROL1, &reg);
        if (res != ESP_OK) {
            return res;
        }

        reg = reg & 0xc7;
        res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL1, reg | (bits << 3));
        if (res != ESP_OK) {
            return res;
        }
    }

    return res;
}

esp_err_t es8388_set_voice_mute(int enable)
{
    int res;
    uint8_t reg = 0;

    res = _es8388_read_reg(ES8388_DACCONTROL3, &reg);
    if (res != ESP_OK) {
        return res;
    }

    reg = reg & 0xFB;
    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACCONTROL3, reg | (((int)enable) << 2));
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

esp_err_t es8388_get_voice_mute(int *mute)
{
    int res = -1;
    uint8_t reg = 0;

    res = _es8388_read_reg(ES8388_DACCONTROL3, &reg);
    if (res == ESP_OK) {
        reg = (reg & 0x04) >> 2;
    }
    *mute = reg;

    return res;
}

esp_err_t es8388_config_dac_output(int output)
{
    int res;

    uint8_t reg = 0;

    res = _es8388_read_reg(ES8388_DACPOWER, &reg);
    if (res != ESP_OK) {
        return res;
    }

    reg = reg & 0xc3;
    res = _es8388_write_reg(ES8388_ADDR, ES8388_DACPOWER, reg | output);
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

esp_err_t es8388_config_adc_input(adc_input_t input)
{
    int res;
    uint8_t reg = 0;

    res = _es8388_read_reg(ES8388_ADCCONTROL2, &reg);
    if (res != ESP_OK) {
        return res;
    }

    reg = reg & 0x0f;
    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL2, reg | input);
    if (res != ESP_OK) {
        return res;
    }

    return res;
}

esp_err_t es8388_set_mic_gain(mic_gain_t gain)
{
    int res, gain_n;

    gain_n = (int)gain / 3;
    res = _es8388_write_reg(ES8388_ADDR, ES8388_ADCCONTROL1, gain_n); //MIC PGA

    return res;
}

void es8388_read_all()
{
    for (int i = 0; i < 50; i++) {
        uint8_t reg = 0;
        _es8388_read_reg(i, &reg);
        ESP_LOGI(ES8388_TAG, "%x: %x\n", i, reg);
    }
}

esp_err_t es8388_write_reg(uint8_t reg_addr, uint8_t data)
{
    return _es8388_write_reg(ES8388_ADDR, reg_addr, data);
}
