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


#ifndef __MEDIA_HAL_CONFIG_H__
#define __MEDIA_HAL_CONFIG_H__

#include "sdkconfig.h"

#ifdef CONFIG_RUN_ON_ESP32_LYRAT
#define CONFIG_CODECCHIP_IS_ES8388
#endif

//I2S config
#define MHC_I2S_PORT                CONFIG_I2S_PORT
#define MHC_I2S_SCLK                CONFIG_I2S_SCLK_PIN
#define MHC_I2S_WS                  CONFIG_I2S_WS_PIN
#define MHC_I2S_DATA_IN             CONFIG_I2S_DATA_IN_PIN
#define MHC_I2S_DATA_OUT            CONFIG_I2S_DATA_OUT_PIN
#define MHC_I2S_SAMPLE_BITS         CONFIG_I2S_BITS_PER_SAMPLE

#ifdef CONFIG_I2S_MCLK_OUT_GPIO0_ENABLE
#define MHC_I2S_MCLK_OUT_GPIO0_EN   1
#else
#define MHC_I2S_MCLK_OUT_GPIO0_EN   0
#endif

#define MHC_I2S_OUT_VOL_DEFAULT     CONFIG_VOLUME_DEFAULT

//Internal DAC and ADC
#ifdef CONFIG_MHC_I2S_DAC_EN
#define MHC_I2S_DAC_EN              1
#else
#define MHC_I2S_DAC_EN              0
#endif

#ifdef CONFIG_MHC_I2S_ADC_EN
#define MHC_I2S_ADC_EN              1
#else
#define MHC_I2S_ADC_EN              0
#endif

/* PA pad config */
#ifdef CONFIG_PA_EN_GPIO_ENABLE
#define MHC_PA_EN_GPIO_EN           1
#define MHC_GPIO_PA_EN              CONFIG_PA_EN_GPIO
#define MHC_GPIO_SEL_PA_EN          (1<<CONFIG_PA_EN_GPIO)
#else
#define MHC_PA_EN_GPIO_EN           0
#define MHC_GPIO_PA_EN              -1
#define MHC_GPIO_SEL_PA_EN          0
#endif

/* I2C pad config */
#define MHC_I2C_PORT                CONFIG_I2C_PORT
#define MHC_I2C_CLK_PORT            CONFIG_I2C_CLK_PIN
#define MHC_I2C_CLK_DATA            CONFIG_I2C_DATA_PIN

#endif /* __MEDIA_HAL_CONFIG_H__ */
