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

#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "hal/rtc_cntl_ll.h"
#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2
#include "hal/rtc_io_ll.h"
#endif

typedef struct rtc_cntl_sleep_retent {
#if SOC_PM_SUPPORT_CPU_PD
    void     *cpu_pd_mem;   /* Internal ram address for cpu retention */
#endif // SOC_PM_SUPPORT_CPU_PD
#if SOC_PM_SUPPORT_TAGMEM_PD
    struct {
        void     *link_addr;  /* Internal ram address for tagmem retention */
        struct {
            uint32_t start_point: 8,    /* the row of start for i-cache tag memory */
                     vld_size: 8,       /* valid size of i-cache tag memory, unit: 4 i-cache tagmem blocks */
                     size: 8,           /* i-cache tag memory size, unit: 4 i-cache tagmem blocks */
                     enable: 1;         /* enable or disable i-cache tagmem retention */
        } icache;
        struct {
            uint32_t start_point: 9,    /* the row of start for d-cache tag memory */
                     vld_size: 9,       /* valid size of d-cache tag memory, unit: 4 d-cache tagmem blocks */
                     size: 9,           /* d-cache tag memory size, unit: 4 d-cache tagmem blocks */
                     enable: 1;         /* enable or disable d-cache tagmem retention */
        } dcache;
    } tagmem;
#endif // SOC_PM_SUPPORT_TAGMEM_PD
} rtc_cntl_sleep_retent_t;

#define RTC_HAL_DMA_LINK_NODE_SIZE      (16)

#if SOC_PM_SUPPORT_EXT_WAKEUP

#define rtc_hal_ext1_get_wakeup_pins()                    rtc_cntl_ll_ext1_get_wakeup_pins()

#define rtc_hal_ext1_set_wakeup_pins(mask, mode)          rtc_cntl_ll_ext1_set_wakeup_pins(mask, mode)

#define rtc_hal_ext1_clear_wakeup_pins()                  rtc_cntl_ll_ext1_clear_wakeup_pins()

#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

#define rtc_hal_gpio_get_wakeup_pins()                    rtc_cntl_ll_gpio_get_wakeup_pins()

#define rtc_hal_gpio_clear_wakeup_pins()                  rtc_cntl_ll_gpio_clear_wakeup_pins()

#define rtc_hal_gpio_set_wakeup_pins()                    rtc_cntl_ll_gpio_set_wakeup_pins()

#endif

#define rtc_hal_set_wakeup_timer(ticks)                   rtc_cntl_ll_set_wakeup_timer(ticks)

void * rtc_cntl_hal_dma_link_init(void *elem, void *buff, int size, void *next);

#if SOC_PM_SUPPORT_CPU_PD

void rtc_cntl_hal_enable_cpu_retention(void *addr);

void rtc_cntl_hal_disable_cpu_retention(void *addr);

#endif

#if SOC_PM_SUPPORT_TAGMEM_PD

void rtc_cntl_hal_enable_tagmem_retention(void *addr);

void rtc_cntl_hal_disable_tagmem_retention(void *addr);

#endif

/*
 * Enable wakeup from ULP coprocessor.
 */
#define rtc_hal_ulp_wakeup_enable()                       rtc_cntl_ll_ulp_wakeup_enable()

#define rtc_hal_ulp_int_clear()                           rtc_cntl_ll_ulp_int_clear()
