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
    ETS_RWBT_INTR_SOURCE,                       /**< interrupt of RWBT, level*/
    ETS_RWBLE_INTR_SOURCE,                      /**< interrupt of RWBLE, level*/
    ETS_RWBT_NMI_SOURCE,                        /**< interrupt of RWBT, NMI, use if RWBT have bug to fix in NMI*/
    ETS_RWBLE_NMI_SOURCE,                       /**< interrupt of RWBLE, NMI, use if RWBT have bug to fix in NMI*/
    ETS_I2C_MASTER_SOURCE,                      /**< interrupt of I2C Master, level*/
    ETS_SLC0_INTR_SOURCE,                       /**< interrupt of SLC0, level*/
    ETS_SLC1_INTR_SOURCE,                       /**< interrupt of SLC1, level*/
    ETS_UHCI0_INTR_SOURCE,                      /**< interrupt of UHCI0, level*/
    ETS_UHCI1_INTR_SOURCE,                      /**< interrupt of UHCI1, level*/

    ETS_GPIO_INTR_SOURCE = 16,                  /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                        /**< interrupt of GPIO, NMI*/
    ETS_GPIO_INTR_SOURCE2,                      /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE2,                       /**< interrupt of GPIO, NMI*/
    ETS_SPI1_INTR_SOURCE,                       /**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_INTR_SOURCE,                       /**< interrupt of SPI2, level*/
    ETS_SPI3_INTR_SOURCE,                       /**< interrupt of SPI3, level*/
    ETS_LCD_CAM_INTR_SOURCE = 24,               /**< interrupt of LCD camera, level*/
    ETS_I2S0_INTR_SOURCE,                       /**< interrupt of I2S0, level*/
    ETS_I2S1_INTR_SOURCE,                       /**< interrupt of I2S1, level*/
    ETS_UART0_INTR_SOURCE,                      /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                      /**< interrupt of UART1, level*/
    ETS_UART2_INTR_SOURCE,                      /**< interrupt of UART2, level*/
    ETS_SDIO_HOST_INTR_SOURCE,                  /**< interrupt of SD/SDIO/MMC HOST, level*/
    ETS_PWM0_INTR_SOURCE,                       /**< interrupt of PWM0, level, Reserved*/
    ETS_PWM1_INTR_SOURCE,                       /**< interrupt of PWM1, level, Reserved*/
    ETS_LEDC_INTR_SOURCE = 35,                  /**< interrupt of LED PWM, level*/
    ETS_EFUSE_INTR_SOURCE,                      /**< interrupt of efuse, level, not likely to use*/
    ETS_TWAI_INTR_SOURCE,                       /**< interrupt of can, level*/
    ETS_USB_INTR_SOURCE,                        /**< interrupt of USB, level*/
    ETS_RTC_CORE_INTR_SOURCE,                   /**< interrupt of rtc core, level, include rtc watchdog*/
    ETS_RMT_INTR_SOURCE,                        /**< interrupt of remote controller, level*/
    ETS_PCNT_INTR_SOURCE,                       /**< interrupt of pluse count, level*/
    ETS_I2C_EXT0_INTR_SOURCE,                   /**< interrupt of I2C controller1, level*/
    ETS_I2C_EXT1_INTR_SOURCE,                   /**< interrupt of I2C controller0, level*/
    ETS_SPI2_DMA_INTR_SOURCE,                   /**< interrupt of SPI2 DMA, level*/
    ETS_SPI3_DMA_INTR_SOURCE,                   /**< interrupt of SPI3 DMA, level*/
    ETS_WDT_INTR_SOURCE = 47,                   /**< will be cancelled*/

    ETS_TIMER1_INTR_SOURCE = 48,
    ETS_TIMER2_INTR_SOURCE,
    ETS_TG0_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER0, EDGE*/
    ETS_TG0_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER1, EDGE*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, WATCH DOG, EDGE*/
    ETS_TG1_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER0, EDGE*/
    ETS_TG1_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER1, EDGE*/
    ETS_TG1_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP1, WATCHDOG, EDGE*/
    ETS_CACHE_IA_INTR_SOURCE,                   /**< interrupt of Cache Invalied Access, LEVEL*/
    ETS_SYSTIMER_TARGET0_INTR_SOURCE,           /**< interrupt of system timer 0 */
    ETS_SYSTIMER_TARGET1_INTR_SOURCE,           /**< interrupt of system timer 1 */
    ETS_SYSTIMER_TARGET2_INTR_SOURCE,           /**< interrupt of system timer 2 */
    ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET0_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET0_INTR_SOURCE */
    ETS_SYSTIMER_TARGET1_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET1_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET1_INTR_SOURCE */
    ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE = ETS_SYSTIMER_TARGET2_INTR_SOURCE, /**< use ETS_SYSTIMER_TARGET2_INTR_SOURCE */
    ETS_SPI_MEM_REJECT_CACHE_INTR_SOURCE = 60,  /**< interrupt of SPI0 Cache access and SPI1 access rejected, LEVEL*/
    ETS_DCACHE_PRELOAD0_INTR_SOURCE,            /**< interrupt of DCache preload operation, LEVEL*/
    ETS_ICACHE_PRELOAD0_INTR_SOURCE,            /**< interrupt of ICache perload operation, LEVEL*/
    ETS_DCACHE_SYNC0_INTR_SOURCE,               /**< interrupt of data cache sync done, LEVEL*/
    ETS_ICACHE_SYNC0_INTR_SOURCE,               /**< interrupt of instruction cache sync done, LEVEL*/
    ETS_APB_ADC_INTR_SOURCE,                    /**< interrupt of APB ADC, LEVEL*/
    ETS_DMA_IN_CH0_INTR_SOURCE,                 /**< interrupt of general DMA RX channel 0, LEVEL*/
    ETS_DMA_IN_CH1_INTR_SOURCE,                 /**< interrupt of general DMA RX channel 1, LEVEL*/
    ETS_DMA_IN_CH2_INTR_SOURCE,                 /**< interrupt of general DMA RX channel 2, LEVEL*/
    ETS_DMA_IN_CH3_INTR_SOURCE,                 /**< interrupt of general DMA RX channel 3, LEVEL*/
    ETS_DMA_IN_CH4_INTR_SOURCE,                 /**< interrupt of general DMA RX channel 4, LEVEL*/
    ETS_DMA_OUT_CH0_INTR_SOURCE,                /**< interrupt of general DMA TX channel 0, LEVEL*/
    ETS_DMA_OUT_CH1_INTR_SOURCE,                /**< interrupt of general DMA TX channel 1, LEVEL*/
    ETS_DMA_OUT_CH2_INTR_SOURCE,                /**< interrupt of general DMA TX channel 2, LEVEL*/
    ETS_DMA_OUT_CH3_INTR_SOURCE,                /**< interrupt of general DMA TX channel 3, LEVEL*/
    ETS_DMA_OUT_CH4_INTR_SOURCE,                /**< interrupt of general DMA TX channel 4, LEVEL*/
    ETS_RSA_INTR_SOURCE,                        /**< interrupt of RSA accelerator, level*/
    ETS_AES_INTR_SOURCE,                        /**< interrupt of AES accelerator, level*/
    ETS_SHA_INTR_SOURCE,                        /**< interrupt of SHA accelerator, level*/
    ETS_FROM_CPU_INTR0_SOURCE,                  /**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR1_SOURCE,                  /**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR2_SOURCE,                  /**< interrupt2 generated from a CPU, level*/ /* Used for IPC_ISR */
    ETS_FROM_CPU_INTR3_SOURCE,                  /**< interrupt3 generated from a CPU, level*/ /* Used for IPC_ISR */
    ETS_ASSIST_DEBUG_INTR_SOURCE,               /**< interrupt of Assist debug module, LEVEL*/
    ETS_DMA_APBPERI_PMS_INTR_SOURCE,
    ETS_CORE0_IRAM0_PMS_INTR_SOURCE,
    ETS_CORE0_DRAM0_PMS_INTR_SOURCE,
    ETS_CORE0_PIF_PMS_INTR_SOURCE,
    ETS_CORE0_PIF_PMS_SIZE_INTR_SOURCE,
    ETS_CORE1_IRAM0_PMS_INTR_SOURCE,
    ETS_CORE1_DRAM0_PMS_INTR_SOURCE,
    ETS_CORE1_PIF_PMS_INTR_SOURCE,
    ETS_CORE1_PIF_PMS_SIZE_INTR_SOURCE,
    ETS_BACKUP_PMS_VIOLATE_INTR_SOURCE,
    ETS_CACHE_CORE0_ACS_INTR_SOURCE,
    ETS_CACHE_CORE1_ACS_INTR_SOURCE,
    ETS_USB_SERIAL_JTAG_INTR_SOURCE,
    ETS_PERI_BACKUP_INTR_SOURCE,
    ETS_DMA_EXTMEM_REJECT_SOURCE,
    ETS_MAX_INTR_SOURCE,                        /**< number of interrupt sources */
} periph_interrput_t;

extern const char * const esp_isr_names[ETS_MAX_INTR_SOURCE];

#ifdef __cplusplus
}
#endif
