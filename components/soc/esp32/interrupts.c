// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/interrupts.h"

const char * const esp_isr_names[ETS_MAX_INTR_SOURCE] = {
    [0] = "WIFI_MAC",
    [1] = "WIFI_NMI",
    [2] = "WIFI_BB",
    [3] = "BT_MAC",
    [4] = "BT_BB",
    [5] = "BT_BB_NMI",
    [6] = "RWBT",
    [7] = "RWBLE",
    [8] = "RWBT_NMI",
    [9] = "RWBLE_NMI",
    [10] = "SLC0",
    [11] = "SLC1",
    [12] = "UHCI0",
    [13] = "UHCI1",
    [14] = "TG0_T0_LEVEL",
    [15] = "TG0_T1_LEVEL",
    [16] = "TG0_WDT_LEVEL",
    [17] = "TG0_LACT_LEVEL",
    [18] = "TG1_T0_LEVEL",
    [19] = "TG1_T1_LEVEL",
    [20] = "TG1_WDT_LEVEL",
    [21] = "TG1_LACT_LEVEL",
    [22] = "GPIO",
    [23] = "GPIO_NMI",
    [24] = "FROM_CPU0",
    [25] = "FROM_CPU1",
    [26] = "FROM_CPU2",
    [27] = "FROM_CPU3",
    [28] = "SPI0",
    [29] = "SPI1",
    [30] = "SPI2",
    [31] = "SPI3",
    [32] = "I2S0",
    [33] = "I2S1",
    [34] = "UART0",
    [35] = "UART1",
    [36] = "UART2",
    [37] = "SDIO_HOST",
    [38] = "ETH_MAC",
    [39] = "PWM0",
    [40] = "PWM1",
    [41] = "PWM2",
    [42] = "PWM3",
    [43] = "LEDC",
    [44] = "EFUSE",
    [45] = "CAN",
    [46] = "RTC_CORE",
    [47] = "RMT",
    [48] = "PCNT",
    [49] = "I2C_EXT0",
    [50] = "I2C_EXT1",
    [51] = "RSA",
    [52] = "SPI1_DMA",
    [53] = "SPI2_DMA",
    [54] = "SPI3_DMA",
    [55] = "WDT",
    [56] = "TIMER1",
    [57] = "TIMER2",
    [58] = "TG0_T0_EDGE",
    [59] = "TG0_T1_EDGE",
    [60] = "TG0_WDT_EDGE",
    [61] = "TG0_LACT_EDGE",
    [62] = "TG1_T0_EDGE",
    [63] = "TG1_T1_EDGE",
    [64] = "TG1_WDT_EDGE",
    [65] = "TG1_LACT_EDGE",
    [66] = "MMU_IA",
    [67] = "MPU_IA",
    [68] = "CACHE_IA",
};
