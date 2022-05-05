/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIPH_LEDC_MODULE = 0,
    PERIPH_UART0_MODULE,
    PERIPH_UART1_MODULE,
    PERIPH_USB_MODULE,
    PERIPH_I2C0_MODULE,
    PERIPH_I2C1_MODULE,
    PERIPH_I2S0_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_UHCI1_MODULE,
    PERIPH_RMT_MODULE,
    PERIPH_PCNT_MODULE,
    PERIPH_SPI_MODULE,  //SPI1
    PERIPH_FSPI_MODULE, //SPI2
    PERIPH_HSPI_MODULE, //SPI3
    PERIPH_SPI2_DMA_MODULE,
    PERIPH_SPI3_DMA_MODULE,
    PERIPH_TWAI_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
    PERIPH_SYSTIMER_MODULE,
    PERIPH_AES_MODULE,
    PERIPH_SHA_MODULE,
    PERIPH_RSA_MODULE,
    PERIPH_CRYPTO_DMA_MODULE,        //this DMA is shared between AES and SHA
    PERIPH_AES_DMA_MODULE,
    PERIPH_SHA_DMA_MODULE,
    PERIPH_DEDIC_GPIO_MODULE,
    PERIPH_SARADC_MODULE,
    PERIPH_TEMPSENSOR_MODULE,
    PERIPH_MODULE_MAX
} periph_module_t;

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
    ETS_SLC0_INTR_SOURCE,                       /**< interrupt of SLC0, level*/
    ETS_SLC1_INTR_SOURCE,                       /**< interrupt of SLC1, level*/
    ETS_UHCI0_INTR_SOURCE,                      /**< interrupt of UHCI0, level*/
    ETS_UHCI1_INTR_SOURCE,                      /**< interrupt of UHCI1, level*/
    ETS_TG0_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER0, level, we would like use EDGE for timer if permission*/

    ETS_TG0_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER1, level, we would like use EDGE for timer if permission*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, WATCHDOG, level*/
    ETS_TG0_LACT_LEVEL_INTR_SOURCE,             /**< interrupt of TIMER_GROUP0, LACT, level*/
    ETS_TG1_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER0, level, we would like use EDGE for timer if permission*/
    ETS_TG1_T1_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER1, level, we would like use EDGE for timer if permission*/
    ETS_TG1_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
    ETS_TG1_LACT_LEVEL_INTR_SOURCE,             /**< interrupt of TIMER_GROUP1, LACT, level*/
    ETS_GPIO_INTR_SOURCE,                       /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                        /**< interrupt of GPIO, NMI*/
    ETS_GPIO_INTR_SOURCE2,                      /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE2,                       /**< interrupt of GPIO, NMI*/
    ETS_DEDICATED_GPIO_INTR_SOURCE,             /**< interrupt of dedicated GPIO, level*/
    ETS_FROM_CPU_INTR0_SOURCE,                  /**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR1_SOURCE,                  /**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR2_SOURCE,                  /**< interrupt2 generated from a CPU, level*/
    ETS_FROM_CPU_INTR3_SOURCE,                  /**< interrupt3 generated from a CPU, level*/

    ETS_SPI1_INTR_SOURCE = 32,                  /**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_INTR_SOURCE,                       /**< interrupt of SPI2, level*/
    ETS_SPI3_INTR_SOURCE,                       /**< interrupt of SPI3, level*/
    ETS_I2S0_INTR_SOURCE,                       /**< interrupt of I2S0, level*/
    ETS_UART0_INTR_SOURCE = 37,                 /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                      /**< interrupt of UART1, level*/
    ETS_UART2_INTR_SOURCE,                      /**< interrupt of UART2, level*/
    ETS_SDIO_HOST_INTR_SOURCE,                  /**< interrupt of SD/SDIO/MMC HOST, level*/
    ETS_LEDC_INTR_SOURCE = 45,                  /**< interrupt of LED PWM, level*/
    ETS_EFUSE_INTR_SOURCE,                      /**< interrupt of efuse, level, not likely to use*/
    ETS_TWAI_INTR_SOURCE ,                      /**< interrupt of twai, level*/

    ETS_USB_INTR_SOURCE = 48,                   /**< interrupt of USB, level*/
    ETS_RTC_CORE_INTR_SOURCE,                   /**< interrupt of rtc core, level, include rtc watchdog*/
    ETS_RMT_INTR_SOURCE,                        /**< interrupt of remote controller, level*/
    ETS_PCNT_INTR_SOURCE,                       /**< interrupt of pluse count, level*/
    ETS_I2C_EXT0_INTR_SOURCE,                   /**< interrupt of I2C controller1, level*/
    ETS_I2C_EXT1_INTR_SOURCE,                   /**< interrupt of I2C controller0, level*/
    ETS_RSA_INTR_SOURCE,                        /**< interrupt of RSA accelerator, level*/
    ETS_SHA_INTR_SOURCE,                        /**< interrupt of SHA accelerator, level*/
    ETS_AES_INTR_SOURCE,                        /**< interrupt of AES accelerator, level*/
    ETS_SPI2_DMA_INTR_SOURCE,                   /**< interrupt of SPI2 DMA, level*/
    ETS_SPI3_DMA_INTR_SOURCE,                   /**< interrupt of SPI3 DMA, level*/
    ETS_WDT_INTR_SOURCE,                        /**< will be cancelled*/
    ETS_TIMER1_INTR_SOURCE,                     /**< will be cancelled*/
    ETS_TIMER2_INTR_SOURCE,                     /**< will be cancelled*/
    ETS_TG0_T0_EDGE_INTR_SOURCE,                /**< interrupt of TIMER_GROUP0, TIMER0, EDGE*/
    ETS_TG0_T1_EDGE_INTR_SOURCE,                /**< interrupt of TIMER_GROUP0, TIMER1, EDGE*/

    ETS_TG0_WDT_EDGE_INTR_SOURCE = 64,          /**< interrupt of TIMER_GROUP0, WATCH DOG, EDGE*/
    ETS_TG0_LACT_EDGE_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, LACT, EDGE*/
    ETS_TG1_T0_EDGE_INTR_SOURCE,                /**< interrupt of TIMER_GROUP1, TIMER0, EDGE*/
    ETS_TG1_T1_EDGE_INTR_SOURCE,                /**< interrupt of TIMER_GROUP1, TIMER1, EDGE*/
    ETS_TG1_WDT_EDGE_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, WATCHDOG, EDGE*/
    ETS_TG1_LACT_EDGE_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, LACT, EDGE*/
    ETS_CACHE_IA_INTR_SOURCE,                   /**< interrupt of Cache Invalied Access, LEVEL*/
    ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE,      /**< interrupt of system timer 0, EDGE*/
    ETS_SYSTIMER_TARGET1_EDGE_INTR_SOURCE,      /**< interrupt of system timer 1, EDGE*/
    ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE,      /**< interrupt of system timer 2, EDGE*/
    ETS_ASSIST_DEBUG_INTR_SOURCE,               /**< interrupt of Assist debug module, LEVEL*/
    ETS_PMS_PRO_IRAM0_ILG_INTR_SOURCE,          /**< interrupt of illegal IRAM1 access, LEVEL*/
    ETS_PMS_PRO_DRAM0_ILG_INTR_SOURCE,          /**< interrupt of illegal DRAM0 access, LEVEL*/
    ETS_PMS_PRO_DPORT_ILG_INTR_SOURCE,          /**< interrupt of illegal DPORT access, LEVEL*/
    ETS_PMS_PRO_AHB_ILG_INTR_SOURCE,            /**< interrupt of illegal AHB access, LEVEL*/
    ETS_PMS_PRO_CACHE_ILG_INTR_SOURCE,          /**< interrupt of illegal CACHE access, LEVEL*/

    ETS_PMS_DMA_APB_I_ILG_INTR_SOURCE = 80,     /**< interrupt of illegal APB access, LEVEL*/
    ETS_PMS_DMA_RX_I_ILG_INTR_SOURCE,           /**< interrupt of illegal DMA RX access, LEVEL*/
    ETS_PMS_DMA_TX_I_ILG_INTR_SOURCE,           /**< interrupt of illegal DMA TX access, LEVEL*/
    ETS_SPI_MEM_REJECT_CACHE_INTR_SOURCE,       /**< interrupt of SPI0 Cache access and SPI1 access rejected, LEVEL*/
    ETS_DMA_COPY_INTR_SOURCE,                   /**< interrupt of DMA copy, LEVEL*/
    ETS_SPI4_DMA_INTR_SOURCE,                   /**< interrupt of SPI4 DMA, LEVEL*/
    ETS_SPI4_INTR_SOURCE,                       /**< interrupt of SPI4, LEVEL*/
    ETS_ICACHE_PRELOAD_INTR_SOURCE,             /**< interrupt of ICache perload operation, LEVEL*/
    ETS_DCACHE_PRELOAD_INTR_SOURCE,             /**< interrupt of DCache preload operation, LEVEL*/
    ETS_APB_ADC_INTR_SOURCE,                    /**< interrupt of APB ADC, LEVEL*/
    ETS_CRYPTO_DMA_INTR_SOURCE,                 /**< interrupt of encrypted DMA, LEVEL*/
    ETS_CPU_PERI_ERROR_INTR_SOURCE,             /**< interrupt of CPU peripherals error, LEVEL*/
    ETS_APB_PERI_ERROR_INTR_SOURCE,             /**< interrupt of APB peripherals error, LEVEL*/
    ETS_DCACHE_SYNC_INTR_SOURCE,                /**< interrupt of data cache sync done, LEVEL*/
    ETS_ICACHE_SYNC_INTR_SOURCE,                /**< interrupt of instruction cache sync done, LEVEL*/
    ETS_MAX_INTR_SOURCE,                        /**< number of interrupt sources */
} periph_interrput_t;

#ifdef __cplusplus
}
#endif
