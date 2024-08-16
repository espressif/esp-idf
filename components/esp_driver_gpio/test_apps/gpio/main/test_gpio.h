/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

// GPIO self-test pins (GPIO_MODE_INPUT_OUTPUT)
#define TEST_GPIO_INPUT_OUTPUT_IO1         (4)
#define TEST_GPIO_INPUT_OUTPUT_IO2         (5)

#if CONFIG_IDF_TARGET_ESP32
#define TEST_GPIO_EXT_OUT_IO               (18)
#define TEST_GPIO_EXT_IN_IO                (19)
#define TEST_GPIO_INPUT_ONLY_PIN           (34)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (4)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC224_IDX)
#define TEST_GPIO_DEEP_SLEEP_HOLD_PIN      (5)
#elif CONFIG_IDF_TARGET_ESP32S2
#define TEST_GPIO_EXT_OUT_IO               (17)
#define TEST_GPIO_EXT_IN_IO                (21)
#define TEST_GPIO_INPUT_ONLY_PIN           (46)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (1)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC223_IDX)
#define TEST_GPIO_DEEP_SLEEP_HOLD_PIN      (45)
#elif CONFIG_IDF_TARGET_ESP32S3
#define TEST_GPIO_EXT_OUT_IO               (17)
#define TEST_GPIO_EXT_IN_IO                (21)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (1)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC208_IDX)
#define TEST_GPIO_DEEP_SLEEP_HOLD_PIN      (45)
#elif CONFIG_IDF_TARGET_ESP32P4
#define TEST_GPIO_EXT_OUT_IO               (2)
#define TEST_GPIO_EXT_IN_IO                (3)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (1)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC250_IDX)
#elif CONFIG_IDF_TARGET_ESP32H2
#define TEST_GPIO_EXT_OUT_IO               (2)
#define TEST_GPIO_EXT_IN_IO                (3)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (1)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC97_IDX)
#define TEST_GPIO_DEEP_SLEEP_HOLD_PIN      (25)
#else
#define TEST_GPIO_EXT_OUT_IO               (2)
#define TEST_GPIO_EXT_IN_IO                (3)
#define TEST_GPIO_INPUT_LEVEL_LOW_PIN      (1)
#define TEST_GPIO_SIGNAL_IDX               (SIG_IN_FUNC97_IDX)
#define TEST_GPIO_DEEP_SLEEP_HOLD_PIN      (9)
#endif

#ifdef __cplusplus
}
#endif
