// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_INTR_H__
#define __ESP_INTR_H__

#include "rom/ets_sys.h"
#include "freertos/xtensa_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_CCOMPARE_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_CCOMPARE_INUM, (func), (void *)(arg))

#define ESP_EPWM_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_EPWM_INUM, (func), (void *)(arg))

#define ESP_MPWM_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_MPWM_INUM, (func), (void *)(arg))

#define ESP_SPI1_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_SPI1_INUM, (func), (void *)(arg))

#define ESP_SPI2_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_SPI2_INUM, (func), (void *)(arg))

#define ESP_SPI3_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_SPI3_INUM, (func), (void *)(arg))

#define ESP_I2S0_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_I2S0_INUM, (func), (void *)(arg))

#define ESP_PCNT_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_PCNT_INUM, (func), (void *)(arg))

#define ESP_LEDC_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_LEDC_INUM, (func), (void *)(arg))

#define ESP_WMAC_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_WMAC_INUM, (func), (void *)(arg))

#define ESP_FRC_TIMER1_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_FRC_TIMER1_INUM, (func), (void *)(arg))

#define ESP_FRC_TIMER2_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_FRC_TIMER2_INUM, (func), (void *)(arg))

#define ESP_GPIO_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_GPIO_INUM, (func), (void *)(arg))

#define ESP_UART0_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_UART0_INUM, (func), (void *)(arg))

#define ESP_WDT_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_WDT_INUM, (func), (void *)(arg))

#define ESP_RTC_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_RTC_INUM, (func), (void *)(arg))

#define ESP_SLC_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_SLC_INUM, (func), (void *)(arg))

#define ESP_RMT_CTRL_INTRL(func,arg)\
    xt_set_interrupt_handler(ETS_RMT_CTRL_INUM, (func), (void *)(arg))

#define ESP_INTR_ENABLE(inum) \
    xt_ints_on((1<<inum))

#define ESP_INTR_DISABLE(inum) \
    xt_ints_off((1<<inum))

#define ESP_CCOMPARE_INTR_ENBALE() \
    ESP_INTR_ENABLE(ETS_CCOMPARE_INUM)

#define ESP_CCOMPARE_INTR_DISBALE() \
    ESP_INTR_DISABLE(ETS_CCOMPARE_INUM)

#define ESP_SPI1_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_SPI1_INUM)

#define ESP_SPI1_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_SPI1_INUM)

#define ESP_SPI2_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_SPI2_INUM)

#define ESP_PWM_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_PWM_INUM)

#define ESP_PWM_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_PWM_INUM)

#define ESP_SPI2_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_SPI2_INUM)

#define ESP_SPI3_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_SPI3_INUM)

#define ESP_SPI3_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_SPI3_INUM)

#define ESP_I2S0_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_I2S0_INUM)

#define ESP_I2S0_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_I2S0_INUM)

#define ESP_I2S1_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_I2S1_INUM)

#define ESP_I2S1_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_I2S1_INUM)

#define ESP_MPWM_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_MPWM_INUM)

#define ESP_EPWM_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_EPWM_INUM)

#define ESP_MPWM_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_MPWM_INUM)

#define ESP_EPWM_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_EPWM_INUM)

#define ESP_BB_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_BB_INUM)

#define ESP_BB_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_BB_INUM)

#define ESP_UART0_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_UART0_INUM)

#define ESP_UART0_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_UART0_INUM)

#define ESP_LEDC_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_LEDC_INUM)

#define ESP_LEDC_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_LEDC_INUM)

#define ESP_GPIO_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_GPIO_INUM)

#define ESP_GPIO_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_GPIO_INUM)

#define ESP_WDT_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_WDT_INUM)

#define ESP_WDT_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_WDT_INUM)

#define ESP_FRC1_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_FRC_TIMER1_INUM)

#define ESP_FRC1_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_FRC_TIMER1_INUM)

#define ESP_FRC2_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_FRC_TIMER2_INUM)

#define ESP_FRC2_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_FRC_TIMER2_INUM)

#define ESP_RTC_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_RTC_INUM)

#define ESP_RTC_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_RTC_INUM)

#define ESP_SLC_INTR_ENABLE() \
    ESP_INTR_ENABLE(ETS_SLC_INUM)

#define ESP_SLC_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_SLC_INUM)

#define ESP_PCNT_INTR_ENABLE()  \
    ESP_INTR_ENABLE(ETS_PCNT_INUM)

#define ESP_PCNT_INTR_DISABLE() \
    ESP_INTR_DISABLE(ETS_PCNT_INUM)

#define ESP_RMT_CTRL_ENABLE() \
    ESP_INTR_ENABLE(ETS_RMT_CTRL_INUM)

#define ESP_RMT_CTRL_DIABLE() \
    ESP_INTR_DISABLE(ETS_RMT_CTRL_INUM)

#ifdef __cplusplus
}
#endif

#endif /* __ESP_INTR_H__ */
