/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/interrupts.h"

const char *const esp_isr_names[] = {
    [0] = "WIFI_MAC",
    [1] = "WIFI_MAC_NMI",
    [2] = "WIFI_PWR",
    [3] = "WIFI_BB",
    [4] = "BT_MAC",
    [5] = "BT_BB",
    [6] = "BT_BB_NMI",
    [7] = "LP_TIMER",
    [8] = "COEX",
    [9] = "BLE_TIMER",
    [10] = "BLE_SEC",
    [11] = "I2C_MST",
    [12] = "APB_CTRL",
    [13] = "GPIO_INTR_PRO",
    [14] = "GPIO_INTR_PRO_NMI",
    [15] = "SPI_INTR_1",
    [16] = "SPI_INTR_2",
    [17] = "UART",
    [18] = "UART1",
    [19] = "LEDC",
    [20] = "EFUSE",
    [21] = "RTC_CORE",
    [22] = "I2C_EXT0",
    [23] = "TG0_T0_LEVEL",
    [24] = "TG0_WDT_LEVEL",
    [25] = "CACHE_IA",
    [26] = "SYSTIMER_TARGET0_EDGE",
    [27] = "SYSTIMER_TARGET1_EDGE",
    [28] = "SYSTIMER_TARGET2_EDGE",
    [29] = "SPI_MEM_REJECT_CACHE",
    [30] = "ICACHE_PRELOAD0",
    [31] = "ICACHE_SYNC0",
    [32] = "APB_ADC",
    [33] = "DMA_CH0",
    [34] = "SHA",
    [35] = "ECC",
    [36] = "ETS_FROM_CPU_INTR0",
    [37] = "ETS_FROM_CPU_INTR1",
    [38] = "ETS_FROM_CPU_INTR2",
    [39] = "ETS_FROM_CPU_INTR3",
    [40] = "ETS_ASSIST_DEBUG",
    [41] = "ETS_CORE0_PIF_PMS_SIZE",
    [42] = "ETS_CACHE_CORE0_ACS",
};
