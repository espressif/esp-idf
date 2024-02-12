/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "ulp_riscv_gpio.h"
#include "include/ulp_riscv_gpio.h"

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE
esp_err_t ulp_riscv_gpio_isr_register(gpio_num_t gpio_num, ulp_riscv_gpio_int_type_t intr_type, intr_handler_t handler, void *arg)
{
    if (gpio_num < 0 || gpio_num >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (intr_type < 0 || intr_type >= ULP_RISCV_GPIO_INTR_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!handler) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Set the interrupt type */
    REG_SET_FIELD(RTC_GPIO_PIN0_REG + 4 * gpio_num, RTC_GPIO_PIN0_INT_TYPE, intr_type);

    /* Set the interrupt handler */
    return ulp_riscv_intr_alloc(ULP_RISCV_RTCIO0_INTR_SOURCE + gpio_num, handler, arg);
}

esp_err_t ulp_riscv_gpio_isr_deregister(gpio_num_t gpio_num)
{
    return ulp_riscv_intr_free(ULP_RISCV_RTCIO0_INTR_SOURCE + gpio_num);
}
#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */
