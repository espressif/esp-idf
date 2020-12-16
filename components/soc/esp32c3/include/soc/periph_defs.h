// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

typedef enum {
    PERIPH_LEDC_MODULE = 0,
    PERIPH_UART0_MODULE,
    PERIPH_UART1_MODULE,
    PERIPH_USB_DEVICE_MODULE,
    PERIPH_I2C0_MODULE,
    PERIPH_I2S1_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_RMT_MODULE,
    PERIPH_SPI_MODULE,  //SPI1
    PERIPH_SPI2_MODULE, //SPI2
    PERIPH_TWAI_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
    PERIPH_BT_BASEBAND_MODULE,
    PERIPH_BT_LC_MODULE,
    PERIPH_RSA_MODULE,
    PERIPH_AES_MODULE,
    PERIPH_SHA_MODULE,
    PERIPH_HMAC_MODULE,
    PERIPH_DS_MODULE,
    PERIPH_GDMA_MODULE,
    PERIPH_SYSTIMER_MODULE,
    PERIPH_SARADC_MODULE,
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
    ETS_I2C_MASTER_SOURCE,                      /**< interrupt of I2C Master, level*/
    ETS_SLC0_INTR_SOURCE,                       /**< interrupt of SLC0, level*/
    ETS_SLC1_INTR_SOURCE,                       /**< interrupt of SLC1, level*/
    ETS_APB_CTRL_INTR_SOURCE,                   /**< interrupt of APB ctrl, ?*/
    ETS_UHCI0_INTR_SOURCE,                      /**< interrupt of UHCI0, level*/
    ETS_GPIO_INTR_SOURCE,                       /**< interrupt of GPIO, level*/
    ETS_GPIO_NMI_SOURCE,                        /**< interrupt of GPIO, NMI*/
    ETS_SPI1_INTR_SOURCE,                       /**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
    ETS_SPI2_INTR_SOURCE,                       /**< interrupt of SPI2, level*/
    ETS_I2S1_INTR_SOURCE,                       /**< interrupt of I2S1, level*/
    ETS_UART0_INTR_SOURCE,                      /**< interrupt of UART0, level*/
    ETS_UART1_INTR_SOURCE,                      /**< interrupt of UART1, level*/
    ETS_LEDC_INTR_SOURCE,                       /**< interrupt of LED PWM, level*/
    ETS_EFUSE_INTR_SOURCE,                      /**< interrupt of efuse, level, not likely to use*/
    ETS_TWAI_INTR_SOURCE,                        /**< interrupt of can, level*/
    ETS_USB_INTR_SOURCE,                        /**< interrupt of USB, level*/
    ETS_RTC_CORE_INTR_SOURCE,                   /**< interrupt of rtc core, level, include rtc watchdog*/
    ETS_RMT_INTR_SOURCE,                        /**< interrupt of remote controller, level*/
    ETS_I2C_EXT0_INTR_SOURCE,                   /**< interrupt of I2C controller1, level*/
    ETS_TIMER1_INTR_SOURCE,
    ETS_TIMER2_INTR_SOURCE,
    ETS_TG0_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP0, TIMER0, level*/
    ETS_TG0_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP0, WATCH DOG, level*/
    ETS_TG1_T0_LEVEL_INTR_SOURCE,               /**< interrupt of TIMER_GROUP1, TIMER0, level*/
    ETS_TG1_WDT_LEVEL_INTR_SOURCE,              /**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
    ETS_CACHE_IA_INTR_SOURCE,                   /**< interrupt of Cache Invalied Access, LEVEL*/
    ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE,      /**< interrupt of system timer 0, EDGE*/
    ETS_SYSTIMER_TARGET1_EDGE_INTR_SOURCE,      /**< interrupt of system timer 1, EDGE*/
    ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE,      /**< interrupt of system timer 2, EDGE*/
    ETS_SPI_MEM_REJECT_CACHE_INTR_SOURCE,       /**< interrupt of SPI0 Cache access and SPI1 access rejected, LEVEL*/
    ETS_ICACHE_PRELOAD0_INTR_SOURCE,            /**< interrupt of ICache perload operation, LEVEL*/
    ETS_ICACHE_SYNC0_INTR_SOURCE,               /**< interrupt of instruction cache sync done, LEVEL*/
    ETS_APB_ADC_INTR_SOURCE,                    /**< interrupt of APB ADC, LEVEL*/
    ETS_DMA_CH0_INTR_SOURCE,                    /**< interrupt of general DMA channel 0, LEVEL*/
    ETS_DMA_CH1_INTR_SOURCE,                    /**< interrupt of general DMA channel 1, LEVEL*/
    ETS_DMA_CH2_INTR_SOURCE,                    /**< interrupt of general DMA channel 2, LEVEL*/
    ETS_RSA_INTR_SOURCE,                        /**< interrupt of RSA accelerator, level*/
    ETS_AES_INTR_SOURCE,                        /**< interrupt of AES accelerator, level*/
    ETS_SHA_INTR_SOURCE,                        /**< interrupt of SHA accelerator, level*/
    ETS_FROM_CPU_INTR0_SOURCE,                  /**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR1_SOURCE,                  /**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
    ETS_FROM_CPU_INTR2_SOURCE,                  /**< interrupt2 generated from a CPU, level*/ /* Used for DPORT Access */
    ETS_FROM_CPU_INTR3_SOURCE,                  /**< interrupt3 generated from a CPU, level*/ /* Used for DPORT Access */
    ETS_ASSIST_DEBUG_INTR_SOURCE,               /**< interrupt of Assist debug module, LEVEL*/
    ETS_DMA_APBPERI_PMS_INTR_SOURCE,
    ETS_CORE0_IRAM0_PMS_INTR_SOURCE,
    ETS_CORE0_DRAM0_PMS_INTR_SOURCE,
    ETS_CORE0_PIF_PMS_INTR_SOURCE,
    ETS_CORE0_PIF_PMS_SIZE_INTR_SOURCE,
    ETS_BAK_PMS_VIOLATE_INTR_SOURCE,
    ETS_CACHE_CORE0_ACS_INTR_SOURCE,
    ETS_MAX_INTR_SOURCE,
} periph_interrput_t;

#ifdef __cplusplus
}
#endif
