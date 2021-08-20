// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD clock source
 * @note User should select the clock source based on the real requirement:
 * ╔═════════════════════╦══════════════════════════╦════════════════════════════╗
 * ║ LCD clock source    ║ Features                 ║ Power Management           ║
 * ╠═════════════════════╬══════════════════════════╬════════════════════════════╣
 * ║ LCD_CLK_SRC_PLL160M ║ High resolution, fixed   ║ ESP_PM_APB_FREQ_MAX lock   ║
 * ╠═════════════════════╬══════════════════════════╬════════════════════════════╣
 * ║ LCD_CLK_SRC_APLL    ║ Configurable resolution  ║ ESP_PM_NO_LIGHT_SLEEP lock ║
 * ╠═════════════════════╬══════════════════════════╬════════════════════════════╣
 * ║ LCD_CLK_SRC_XTAL    ║ Medium resolution, fixed ║ No PM lock                 ║
 * ╚═════════════════════╩══════════════════════════╩════════════════════════════╝
 */
typedef enum {
    LCD_CLK_SRC_PLL160M, /*!< Select PLL160M as the source clock */
    LCD_CLK_SRC_APLL,    /*!< Select APLL as the source clock */
    LCD_CLK_SRC_XTAL,    /*!< Select XTAL as the source clock */
} lcd_clock_source_t;

#ifdef __cplusplus
}
#endif
