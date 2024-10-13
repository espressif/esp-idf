/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "hal/gpio_types.h"
#include "ulp_riscv_register_ops.h"
#include "ulp_riscv_interrupt.h"

typedef enum {
    ULP_RISCV_GPIO_INTR_DISABLE = 0,    /*!< Disable RTC GPIO interrupt                             */
    ULP_RISCV_GPIO_INTR_POSEDGE = 1,    /*!< RTC GPIO interrupt type : rising edge                  */
    ULP_RISCV_GPIO_INTR_NEGEDGE = 2,    /*!< RTC GPIO interrupt type : falling edge                 */
    ULP_RISCV_GPIO_INTR_ANYEDGE = 3,    /*!< RTC GPIO interrupt type : both rising and falling edge */
    ULP_RISCV_GPIO_INTR_LOW_LEVEL = 4,  /*!< RTC GPIO interrupt type : input low level trigger      */
    ULP_RISCV_GPIO_INTR_HIGH_LEVEL = 5, /*!< RTC GPIO interrupt type : input high level trigger     */
    ULP_RISCV_GPIO_INTR_MAX
} ulp_riscv_gpio_int_type_t;

typedef enum {
    RTCIO_MODE_OUTPUT = 0,
    RTCIO_MODE_OUTPUT_OD = 1,
} rtc_io_out_mode_t;

static inline void ulp_riscv_gpio_init(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32S2
    SET_PERI_REG_MASK(SENS_SAR_IO_MUX_CONF_REG, SENS_IOMUX_CLK_GATE_EN_M);
#elif CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_IOMUX_CLK_EN_M);
#endif
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_MUX_SEL);
    REG_SET_FIELD(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_FUN_SEL, 0);
}

static inline void ulp_riscv_gpio_deinit(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_MUX_SEL);
}

static inline void ulp_riscv_gpio_output_enable(gpio_num_t gpio_num)
{
    REG_SET_FIELD(RTC_GPIO_ENABLE_W1TS_REG, RTC_GPIO_ENABLE_W1TS, BIT(gpio_num));
}

static inline void ulp_riscv_gpio_output_disable(gpio_num_t gpio_num)
{
    REG_SET_FIELD(RTC_GPIO_ENABLE_W1TC_REG, RTC_GPIO_ENABLE_W1TC, BIT(gpio_num));
}

static inline void ulp_riscv_gpio_input_enable(gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_FUN_IE);
}

static inline void ulp_riscv_gpio_input_disable(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_FUN_IE);
}

static inline void ulp_riscv_gpio_output_level(gpio_num_t gpio_num, uint8_t level)
{
    if (level) {
        REG_SET_FIELD(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS, BIT(gpio_num));
    } else {
        REG_SET_FIELD(RTC_GPIO_OUT_W1TC_REG, RTC_GPIO_OUT_DATA_W1TS, BIT(gpio_num));
    }
}

static inline uint8_t ulp_riscv_gpio_get_level(gpio_num_t gpio_num)
{
    return (uint8_t)((REG_GET_FIELD(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT) & BIT(gpio_num)) ? 1 : 0);
}

static inline void ulp_riscv_gpio_set_output_mode(gpio_num_t gpio_num, rtc_io_out_mode_t mode)
{
    REG_SET_FIELD(RTC_GPIO_PIN0_REG + gpio_num * 4, RTC_GPIO_PIN0_PAD_DRIVER, mode);
}

static inline void ulp_riscv_gpio_pullup(gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_RUE);
}

static inline void ulp_riscv_gpio_pullup_disable(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_RUE);
}

static inline void ulp_riscv_gpio_pulldown(gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_RDE);
}

static inline void ulp_riscv_gpio_pulldown_disable(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_RDE);
}

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE

/**
 * @brief Set RTC IO interrupt type and handler
 *
 * @param gpio_num      GPIO number
 * @param intr_type     Interrupt type (See rtc_io_types.h)
 * @param handler       Interrupt handler
 * @param arg           Interrupt handler argument
 *
 * @return              ESP_OK on success
 */
esp_err_t ulp_riscv_gpio_isr_register(gpio_num_t gpio_num, ulp_riscv_gpio_int_type_t intr_type, intr_handler_t handler, void *arg);

/**
 * @brief Remove RTC IO interrupt handler
 *
 * @param gpio_num      GPIO number
 *
 * @return              ESP_OK on success
 */
esp_err_t ulp_riscv_gpio_isr_deregister(gpio_num_t gpio_num);

#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

#ifdef __cplusplus
}
#endif
