// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "sdkconfig.h"
#include <esp_types.h>

#include "soc/soc_caps.h"
#include "driver/gpio.h"

#if SOC_GPIO_SUPPORT_SLP_SWITCH
#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
/**
  * @brief Emulate ESP32S2 behaviour to backup FUN_PU, FUN_PD information
  *
  * @note Need to be called before sleep.
  *
  * @return
  *      - ESP_OK Success
  */
esp_err_t gpio_sleep_pupd_config_apply(gpio_num_t gpio_num);

/**
  * @brief Emulate ESP32S2 behaviour to restore FUN_PU, FUN_PD information
  *
  * @note Need to be called after sleep.
  *
  * @return
  *      - ESP_OK Success
  */
esp_err_t gpio_sleep_pupd_config_unapply(gpio_num_t gpio_num);
#endif
#endif
