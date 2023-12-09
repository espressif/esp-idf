/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
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
    ETS_WIFI_BB_INTR_SOURCE,                    /**< interrupt of WiFi BB, level, we can do some calibartion*/
    ETS_BT_MAC_INTR_SOURCE,                     /**< will be cancelled*/
    ETS_BT_BB_INTR_SOURCE,                      /**< interrupt of BT BB, level*/
    ETS_BT_BB_NMI_SOURCE,                       /**< interrupt of BT BB, NMI, use if BB have bug to fix in NMI*/
    ETS_LP_TIMER_SOURCE,                       /**< interrupt of RWBT, level*/
    ETS_COEX_SOURCE,                      /**< interrupt of RWBLE, level*/
    ETS_BLE_TIMER_SOURCE,                        /**< interrupt of RWBT, NMI, use if RWBT have bug to fix in NMI*/
    ETS_BLE_SEC_SOURCE,                       /**< interrupt of RWBLE, NMI, use if RWBT have bug to fix in NMI*/
    ETS_I2C_MASTER_SOURCE,                      /**< interrupt of I2C Master, level*/
    ETS_APB_CTRL_INTR_SOURCE,                   /**< interrupt of APB ctrl, ?*/
    ETS_GPIO_INTR_SOURCE,                       /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                        /**< interrupt of GPIO, NMI*/
    ETS_SPI1_INTR_SOURCE,                       /**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_INTR_SOURCE,                       /**< interrupt of SPI2, level*/
    ETS_UART0_INTR_SOURCE,                      /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                      /**< interrupt of UART1, level*/
    ETS_LEDC_INTR_SOURCE,                       /**< interrupt of LED PWM, level*/
    ETS_EFUSE_INTR_SOURCE,                      /**< interrupt of efuse, level, not likely to use*/
    ETS_RTC_CORE_INTR_SOURCE,                   /**< interrupt of rtc core, level, include rtc watchdog*/
    ETS_I2C_EXT0_INTR_SOURCE,                   /**< interrupt of I2C controller1, level*/
    ETS_TG0_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER0, level*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, WATCH DOG, level*/
    ETS_CACHE_IA_INTR_SOURCE,                   /**< interrupt of Cache Invalied Access, LEVEL*/
    ETS_SYSTIMER_TARGET0_INTR_SOURCE,           /**< interrupt of system timer 0 */
    ETS_SYSTIMER_TARGET1_INTR_SOURCE,           /**< interrupt of system timer 1 */
    ETS_SYSTIMER_TARGET2_INTR_SOURCE,           /**< interrupt of system timer 2 */
    ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET0_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET0_INTR_SOURCE */
    ETS_SYSTIMER_TARGET1_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET1_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET1_INTR_SOURCE */
    ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET2_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET2_INTR_SOURCE */
    ETS_SPI_MEM_REJECT_CACHE_INTR_SOURCE = 29,  /**< interrupt of SPI0 Cache access and SPI1 access rejected, LEVEL*/
    ETS_ICACHE_PRELOAD0_INTR_SOURCE,            /**< interrupt of ICache perload operation, LEVEL*/
    ETS_ICACHE_SYNC0_INTR_SOURCE,               /**< interrupt of instruction cache sync done, LEVEL*/
    ETS_APB_ADC_INTR_SOURCE,                    /**< interrupt of APB ADC, LEVEL*/
    ETS_DMA_CH0_INTR_SOURCE,                    /**< interrupt of general DMA channel 0, LEVEL*/
    ETS_SHA_INTR_SOURCE,                        /**< interrupt of SHA accelerator, level*/
    ETS_ECC_INTR_SOURCE,                        /**< interrupt of ECC accelerator, level*/
    ETS_FROM_CPU_INTR0_SOURCE,                  /**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR1_SOURCE,                  /**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR2_SOURCE,                  /**< interrupt2 generated from a CPU, level*/ /* Used for DPORT Access */
    ETS_FROM_CPU_INTR3_SOURCE,                  /**< interrupt3 generated from a CPU, level*/ /* Used for DPORT Access */
    ETS_ASSIST_DEBUG_INTR_SOURCE,               /**< interrupt of Assist debug module, LEVEL*/
    ETS_CORE0_PIF_PMS_SIZE_INTR_SOURCE,
    ETS_CACHE_CORE0_ACS_INTR_SOURCE,
    ETS_MAX_INTR_SOURCE,
} periph_interrput_t;

extern const char * const esp_isr_names[ETS_MAX_INTR_SOURCE];

#ifdef __cplusplus
}
#endif
