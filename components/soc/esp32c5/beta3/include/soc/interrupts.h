/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//Interrupt hardware source table
//This table is decided by hardware, don't touch this.
typedef enum {
    ETS_WIFI_MAC_INTR_SOURCE = 0,               /**< interrupt of WiFi MAC, level*/
    ETS_WIFI_MAC_NMI_SOURCE,                    /**< interrupt of WiFi MAC, NMI, use if MAC have bug to fix in NMI*/
    ETS_WIFI_PWR_INTR_SOURCE,                   /**< */
    ETS_WIFI_BB_INTR_SOURCE,                    /**< interrupt of WiFi BB, level, we can do some calibration*/
    ETS_BT_MAC_INTR_SOURCE,                     /**< will be cancelled*/
    ETS_BT_BB_INTR_SOURCE,                      /**< interrupt of BT BB, level*/
    ETS_BT_BB_NMI_SOURCE,                       /**< interrupt of BT BB, NMI, use if BB have bug to fix in NMI*/
    ETS_LP_TIMER_INTR_SOURCE,
    ETS_COEX_INTR_SOURCE,
    ETS_BLE_TIMER_INTR_SOURCE,
    ETS_BLE_SEC_INTR_SOURCE,
    ETS_I2C_MASTER_SOURCE,                      /**< interrupt of I2C Master, level*/
    ETS_ZB_MAC_INTR_SOURCE,
    ETS_PMU_INTR_SOURCE,
    ETS_EFUSE_INTR_SOURCE,                      /**< interrupt of efuse, level, not likely to use*/
    ETS_LP_RTC_TIMER_INTR_SOURCE,
    ETS_LP_UART_INTR_SOURCE,
    ETS_LP_I2C_INTR_SOURCE,
    ETS_LP_WDT_INTR_SOURCE,
    ETS_LP_PERI_TIMEOUT_INTR_SOURCE,
    ETS_LP_APM_M0_INTR_SOURCE,
    ETS_LP_APM_M1_INTR_SOURCE,
    ETS_HUK_INTR_SOURCE,
    ETS_FROM_CPU_INTR0_SOURCE,                  /**< interrupt0 generated from a CPU, level*/
    ETS_FROM_CPU_INTR1_SOURCE,                  /**< interrupt1 generated from a CPU, level*/
    ETS_FROM_CPU_INTR2_SOURCE,                  /**< interrupt2 generated from a CPU, level*/
    ETS_FROM_CPU_INTR3_SOURCE,                  /**< interrupt3 generated from a CPU, level*/
    ETS_ASSIST_DEBUG_INTR_SOURCE,               /**< interrupt of Assist debug module, LEVEL*/
    ETS_TRACE_INTR_SOURCE,
    ETS_CACHE_INTR_SOURCE,
    ETS_CPU_PERI_TIMEOUT_INTR_SOURCE,
    ETS_GPIO_INTR_SOURCE,                       /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                        /**< interrupt of GPIO, NMI*/
    ETS_GPIO_SD_INTR_SOURCE,
    ETS_PAU_INTR_SOURCE,
    ETS_HP_PERI_TIMEOUT_INTR_SOURCE,
    ETS_MODEM_PERI_TIMEOUT_INTR_SOURCE,
    ETS_HP_APM_M0_INTR_SOURCE,
    ETS_HP_APM_M1_INTR_SOURCE,
    ETS_HP_APM_M2_INTR_SOURCE,
    ETS_HP_APM_M3_INTR_SOURCE,
    ETS_LP_APM0_INTR_SOURCE,
    ETS_MSPI_INTR_SOURCE,
    ETS_I2S1_INTR_SOURCE,                       /**< interrupt of I2S1, level*/
    ETS_UHCI0_INTR_SOURCE,                      /**< interrupt of UHCI0, level*/
    ETS_UART0_INTR_SOURCE,                      /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                      /**< interrupt of UART1, level*/
    ETS_LEDC_INTR_SOURCE,                       /**< interrupt of LED PWM, level*/
    ETS_TWAI0_INTR_SOURCE,                      /**< interrupt of twai0, level*/
    ETS_TWAI1_INTR_SOURCE,                      /**< interrupt of twai1, level*/
    ETS_USB_SERIAL_JTAG_INTR_SOURCE,            /**< interrupt of USB, level*/
    ETS_RMT_INTR_SOURCE,                        /**< interrupt of remote controller, level*/
    ETS_I2C_EXT0_INTR_SOURCE,                   /**< interrupt of I2C controller1, level*/
    ETS_TG0_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER0, level*/
    ETS_TG0_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER1, level*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, WATCH DOG, level*/
    ETS_TG1_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER0, level*/
    ETS_TG1_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER1, level*/
    ETS_TG1_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
    ETS_SYSTIMER_TARGET0_INTR_SOURCE,           /**< interrupt of system timer 0 */
    ETS_SYSTIMER_TARGET1_INTR_SOURCE,           /**< interrupt of system timer 1 */
    ETS_SYSTIMER_TARGET2_INTR_SOURCE,           /**< interrupt of system timer 2 */
    ETS_APB_ADC_INTR_SOURCE,                    /**< interrupt of APB ADC, LEVEL*/
    ETS_MCPWM0_INTR_SOURCE,                     /**< interrupt of MCPWM0, LEVEL*/
    ETS_PCNT_INTR_SOURCE,
    ETS_PARL_IO_TX_INTR_SOURCE,
    ETS_PARL_IO_RX_INTR_SOURCE,
    ETS_SLC0_INTR_SOURCE,
    ETS_SLC1_INTR_SOURCE,
    ETS_USB_OTG20_INTR_SOURCE,
    ETS_USB_OTG20_MULTI_PROC_INTR_SOURCE,
    ETS_USB_OTG20_MISC_INTR_SOURCE,
    ETS_DMA_IN_CH0_INTR_SOURCE,                    /**< interrupt of general DMA IN channel 0, LEVEL*/
    ETS_DMA_IN_CH1_INTR_SOURCE,                    /**< interrupt of general DMA IN channel 1, LEVEL*/
    ETS_DMA_IN_CH2_INTR_SOURCE,                    /**< interrupt of general DMA IN channel 2, LEVEL*/
    ETS_DMA_OUT_CH0_INTR_SOURCE,                   /**< interrupt of general DMA OUT channel 0, LEVEL*/
    ETS_DMA_OUT_CH1_INTR_SOURCE,                   /**< interrupt of general DMA OUT channel 1, LEVEL*/
    ETS_DMA_OUT_CH2_INTR_SOURCE,                   /**< interrupt of general DMA OUT channel 2, LEVEL*/
    ETS_GPSPI2_INTR_SOURCE,
    ETS_AES_INTR_SOURCE,                        /**< interrupt of AES accelerator, level*/
    ETS_SHA_INTR_SOURCE,                        /**< interrupt of SHA accelerator, level*/
    ETS_RSA_INTR_SOURCE,                        /**< interrupt of RSA accelerator, level*/
    ETS_ECC_INTR_SOURCE,                        /**< interrupt of ECC accelerator, level*/
    ETS_ECDSA_INTR_SOURCE,
    ETS_KM_INTR_SOURCE,
    ETS_MAX_INTR_SOURCE,
} periph_interrupt_t;

extern const char * const esp_isr_names[ETS_MAX_INTR_SOURCE];

#ifdef __cplusplus
}
#endif
