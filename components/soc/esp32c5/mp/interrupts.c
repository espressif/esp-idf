/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    [11] = "I2C_MASTER",
    [12] = "ZB_MAC",
    [13] = "PMU",
    [14] = "EFUSE",
    [15] = "LP_RTC_TIMER",
    [16] = "LP_UART",
    [17] = "LP_I2C",
    [18] = "LP_WDT",
    [19] = "LP_PERI_TIMEOUT",
    [20] = "LP_APM_M0",
    [21] = "LP_APM_M1",
    [22] = "HUK",
    [23] = "CPU_FROM_CPU_0",
    [24] = "CPU_FROM_CPU_1",
    [25] = "CPU_FROM_CPU_2",
    [26] = "CPU_FROM_CPU_3",
    [27] = "ASSIST_DEBUG",
    [28] = "TRACE",
    [29] = "CACHE",
    [30] = "CPU_PERI_TIMEOUT",
    [31] = "GPIO_INTERRUPT_PRO",
    [32] = "GPIO_INTERRUPT_EXT",
    [33] = "PAU",
    [34] = "HP_PERI_TIMEOUT",
    [35] = "MODEM_PERI_TIMEOUT",
    [36] = "HP_APM_M0",
    [37] = "HP_APM_M1",
    [38] = "HP_APM_M2",
    [39] = "HP_APM_M3",
    [40] = "HP_APM_M4",
    [41] = "LP_APM0",
    [42] = "MSPI",
    [43] = "I2S",
    [44] = "UHCI0",
    [45] = "UART0",
    [46] = "UART1",
    [47] = "LEDC",
    [48] = "TWAI0",
    [49] = "TWAI0_TIMER",
    [50] = "TWAI1",
    [51] = "TWAI1_TIMER",
    [52] = "USB_SERIAL_JTAG",
    [53] = "RMT",
    [54] = "I2C_EXT0",
    [55] = "TG0_T0",
    [56] = "TG0_WDT",
    [57] = "TG1_T0",
    [58] = "TG1_WDT",
    [59] = "SYSTIMER_TARGET0",
    [60] = "SYSTIMER_TARGET1",
    [61] = "SYSTIMER_TARGET2",
    [62] = "APB_ADC",
    [63] = "PWM",
    [64] = "PCNT",
    [65] = "PARL_IO_TX",
    [66] = "PARL_IO_RX",
    [67] = "DMA_IN_CH0",
    [68] = "DMA_IN_CH1",
    [69] = "DMA_IN_CH2",
    [70] = "DMA_OUT_CH0",
    [71] = "DMA_OUT_CH1",
    [72] = "DMA_OUT_CH2",
    [73] = "GPSPI2",
    [74] = "AES",
    [75] = "SHA",
    [76] = "RSA",
    [77] = "ECC",
    [78] = "ECDSA",
    [79] = "KM",
};
