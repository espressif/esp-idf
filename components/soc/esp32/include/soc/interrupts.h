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
    ETS_WIFI_MAC_INTR_SOURCE = 0,            /**< interrupt of WiFi MAC, level*/
    ETS_WIFI_MAC_NMI_SOURCE,                 /**< interrupt of WiFi MAC, NMI, use if MAC have bug to fix in NMI*/
    ETS_WIFI_BB_INTR_SOURCE,                 /**< interrupt of WiFi BB, level, we can do some calibartion*/
    ETS_BT_MAC_INTR_SOURCE,                  /**< will be cancelled*/
    ETS_BT_BB_INTR_SOURCE,                   /**< interrupt of BT BB, level*/
    ETS_BT_BB_NMI_SOURCE,                    /**< interrupt of BT BB, NMI, use if BB have bug to fix in NMI*/
    ETS_RWBT_INTR_SOURCE,                    /**< interrupt of RWBT, level*/
    ETS_RWBLE_INTR_SOURCE,                   /**< interrupt of RWBLE, level*/
    ETS_RWBT_NMI_SOURCE,                     /**< interrupt of RWBT, NMI, use if RWBT have bug to fix in NMI*/
    ETS_RWBLE_NMI_SOURCE,                    /**< interrupt of RWBLE, NMI, use if RWBT have bug to fix in NMI*/
    ETS_SLC0_INTR_SOURCE,                    /**< interrupt of SLC0, level*/
    ETS_SLC1_INTR_SOURCE,                    /**< interrupt of SLC1, level*/
    ETS_UHCI0_INTR_SOURCE,                   /**< interrupt of UHCI0, level*/
    ETS_UHCI1_INTR_SOURCE,                   /**< interrupt of UHCI1, level*/
    ETS_TG0_T0_LEVEL_INTR_SOURCE,            /**< interrupt of TIMER_GROUP0, TIMER0, level, we would like use EDGE for timer if permission*/
    ETS_TG0_T1_LEVEL_INTR_SOURCE,            /**< interrupt of TIMER_GROUP0, TIMER1, level, we would like use EDGE for timer if permission*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,           /**< interrupt of TIMER_GROUP0, WATCHDOG, level*/
    ETS_TG0_LACT_LEVEL_INTR_SOURCE,          /**< interrupt of TIMER_GROUP0, LACT, level*/
    ETS_TG1_T0_LEVEL_INTR_SOURCE,            /**< interrupt of TIMER_GROUP1, TIMER0, level, we would like use EDGE for timer if permission*/
    ETS_TG1_T1_LEVEL_INTR_SOURCE,            /**< interrupt of TIMER_GROUP1, TIMER1, level, we would like use EDGE for timer if permission*/
    ETS_TG1_WDT_LEVEL_INTR_SOURCE,           /**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
    ETS_TG1_LACT_LEVEL_INTR_SOURCE,          /**< interrupt of TIMER_GROUP1, LACT, level*/
    ETS_GPIO_INTR_SOURCE,                    /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                     /**< interrupt of GPIO, NMI*/
    ETS_FROM_CPU_INTR0_SOURCE,          /**< interrupt0 generated from a CPU, level*/      /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR1_SOURCE,          /**< interrupt1 generated from a CPU, level*/      /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR2_SOURCE,          /**< interrupt2 generated from a CPU, level*/      /* Used for IPC_ISR */
    ETS_FROM_CPU_INTR3_SOURCE,          /**< interrupt3 generated from a CPU, level*/      /* Used for IPC_ISR */
    ETS_SPI0_INTR_SOURCE,                    /**< interrupt of SPI0, level, SPI0 is for Cache Access, do not use this*/
    ETS_SPI1_INTR_SOURCE,                    /**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_INTR_SOURCE,                    /**< interrupt of SPI2, level*/
    ETS_SPI3_INTR_SOURCE,                    /**< interrupt of SPI3, level*/
    ETS_I2S0_INTR_SOURCE,                    /**< interrupt of I2S0, level*/
    ETS_I2S1_INTR_SOURCE,                    /**< interrupt of I2S1, level*/
    ETS_UART0_INTR_SOURCE,                   /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                   /**< interrupt of UART1, level*/
    ETS_UART2_INTR_SOURCE,                   /**< interrupt of UART2, level*/
    ETS_SDIO_HOST_INTR_SOURCE,               /**< interrupt of SD/SDIO/MMC HOST, level*/
    ETS_ETH_MAC_INTR_SOURCE,                 /**< interrupt of ethernet mac, level*/
    ETS_PWM0_INTR_SOURCE,                    /**< interrupt of PWM0, level, Reserved*/
    ETS_PWM1_INTR_SOURCE,                    /**< interrupt of PWM1, level, Reserved*/

    ETS_LEDC_INTR_SOURCE = 43,                  /**< interrupt of LED PWM, level*/
    ETS_EFUSE_INTR_SOURCE,                   /**< interrupt of efuse, level, not likely to use*/
    ETS_TWAI_INTR_SOURCE,                    /**< interrupt of twai, level*/
    ETS_RTC_CORE_INTR_SOURCE,                /**< interrupt of rtc core, level, include rtc watchdog*/
    ETS_RMT_INTR_SOURCE,                     /**< interrupt of remote controller, level*/
    ETS_PCNT_INTR_SOURCE,                    /**< interrupt of pluse count, level*/
    ETS_I2C_EXT0_INTR_SOURCE,                /**< interrupt of I2C controller1, level*/
    ETS_I2C_EXT1_INTR_SOURCE,                /**< interrupt of I2C controller0, level*/
    ETS_RSA_INTR_SOURCE,                     /**< interrupt of RSA accelerator, level*/
    ETS_SPI1_DMA_INTR_SOURCE,                /**< interrupt of SPI1 DMA, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_DMA_INTR_SOURCE,                /**< interrupt of SPI2 DMA, level*/
    ETS_SPI3_DMA_INTR_SOURCE,                /**< interrupt of SPI3 DMA, level*/
    ETS_WDT_INTR_SOURCE,                     /**< will be cancelled*/
    ETS_TIMER1_INTR_SOURCE,                  /**< will be cancelled*/
    ETS_TIMER2_INTR_SOURCE,                  /**< will be cancelled*/
    ETS_TG0_T0_EDGE_INTR_SOURCE,             /**< interrupt of TIMER_GROUP0, TIMER0, EDGE*/
    ETS_TG0_T1_EDGE_INTR_SOURCE,             /**< interrupt of TIMER_GROUP0, TIMER1, EDGE*/
    ETS_TG0_WDT_EDGE_INTR_SOURCE,            /**< interrupt of TIMER_GROUP0, WATCH DOG, EDGE*/
    ETS_TG0_LACT_EDGE_INTR_SOURCE,           /**< interrupt of TIMER_GROUP0, LACT, EDGE*/
    ETS_TG1_T0_EDGE_INTR_SOURCE,             /**< interrupt of TIMER_GROUP1, TIMER0, EDGE*/
    ETS_TG1_T1_EDGE_INTR_SOURCE,             /**< interrupt of TIMER_GROUP1, TIMER1, EDGE*/
    ETS_TG1_WDT_EDGE_INTR_SOURCE,            /**< interrupt of TIMER_GROUP1, WATCHDOG, EDGE*/
    ETS_TG1_LACT_EDGE_INTR_SOURCE,           /**< interrupt of TIMER_GROUP0, LACT, EDGE*/
    ETS_MMU_IA_INTR_SOURCE,                  /**< interrupt of MMU Invalid Access, LEVEL*/
    ETS_MPU_IA_INTR_SOURCE,                  /**< interrupt of MPU Invalid Access, LEVEL*/
    ETS_CACHE_IA_INTR_SOURCE,                /**< interrupt of Cache Invalied Access, LEVEL*/
    ETS_MAX_INTR_SOURCE,                     /**< total number of interrupt sources*/
} periph_interrput_t;

#define ETS_CAN_INTR_SOURCE ETS_TWAI_INTR_SOURCE

extern const char *const esp_isr_names[ETS_MAX_INTR_SOURCE];

#ifdef __cplusplus
}
#endif
