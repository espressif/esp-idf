// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#include <esp_err.h>
#include <esp_spi_flash.h> /* including in bootloader for error values */
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_CACHE_SUPPORT_WRAP
/**
 * @brief Set the burst mode setting command for specified wrap mode.
 *
 * @param mode The specified warp mode.
 * @return always ESP_OK
 */
esp_err_t bootloader_flash_wrap_set(spi_flash_wrap_mode_t mode);
#endif

