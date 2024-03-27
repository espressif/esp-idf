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
#include "ulp_riscv_register_ops.h"
#include "ulp_riscv_interrupt.h"

typedef enum {
    GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_NUM_2 = 2,     /*!< GPIO2, input and output */
    GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */
    GPIO_NUM_20 = 20,   /*!< GPIO20, input and output */
    GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_NUM_MAX,
} gpio_num_t;

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
    REG_SET_FIELD(RTC_IO_TOUCH_PAD0_REG + gpio_num * 4, RTC_IO_TOUCH_PAD0_DRV, mode);
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
