/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"

#if !CONFIG_IDF_TARGET_ESP32P4  //TODO: IDF-7532
#if !SOC_LP_TIMER_SUPPORTED
#include "hal/rtc_cntl_ll.h"
#endif
#endif  //#if !CONFIG_IDF_TARGET_ESP32P4
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "hal/rtc_io_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
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

#if SOC_PM_SUPPORT_EXT1_WAKEUP

#define rtc_hal_ext1_get_wakeup_status()                    rtc_cntl_ll_ext1_get_wakeup_status()

#define rtc_hal_ext1_clear_wakeup_status()                  rtc_cntl_ll_ext1_clear_wakeup_status()

#define rtc_hal_ext1_set_wakeup_pins(io_mask, mode_mask)    rtc_cntl_ll_ext1_set_wakeup_pins(io_mask, mode_mask)

#define rtc_hal_ext1_clear_wakeup_pins()                    rtc_cntl_ll_ext1_clear_wakeup_pins()

#define rtc_hal_ext1_get_wakeup_pins()                      rtc_cntl_ll_ext1_get_wakeup_pins()

#endif // SOC_PM_SUPPORT_EXT1_WAKEUP

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && (SOC_RTCIO_PIN_COUNT == 0)

#define rtc_hal_gpio_get_wakeup_status()                  rtc_cntl_ll_gpio_get_wakeup_status()

#define rtc_hal_gpio_clear_wakeup_status()                rtc_cntl_ll_gpio_clear_wakeup_status()

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

#ifdef __cplusplus
}
#endif
