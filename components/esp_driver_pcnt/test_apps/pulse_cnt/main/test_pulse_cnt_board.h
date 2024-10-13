/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#define TEST_PCNT_GPIO_A 20
#define TEST_PCNT_GPIO_B 21
#define TEST_PCNT_GPIO_Z 22
#else
#define TEST_PCNT_GPIO_A 0
#define TEST_PCNT_GPIO_B 2
#define TEST_PCNT_GPIO_Z 4
#endif

#if CONFIG_PCNT_ISR_IRAM_SAFE
#define TEST_PCNT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_PCNT_CALLBACK_ATTR
#endif // CONFIG_PCNT_ISR_IRAM_SAFE

void test_gpio_init_for_simulation(int gpio_sig);
void test_gpio_simulate_rising_edge(int gpio_sig, size_t times);
void test_gpio_simulate_quadrature_signals(int gpio_sig_a, int gpio_sig_b, size_t times);

#ifdef __cplusplus
}
#endif
