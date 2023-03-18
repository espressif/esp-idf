/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * File Introduction:
 * This file is used to reserve the GPIOs runtime, which has been occupied by FLASH/PSRAM or
 * the GPIOs that not fan out.
 *
 * The FLASH pins can be tuned according to eFuse, pins will be reserved in the `esp_mspi_pin_init`
 * while starting the CPU.
 *
 * As for the PSRAM pins, they are initialized after CPU started. They will be reserved in
 * the `psram_gpio_config` when enabling the PSRAM.
 */

#pragma once

#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the reserved pin
 * @note  A same gpio can be reserve repetitively, but can't be clear once it is reserved
 *
 * @param[in]  mask Mask of GPIO reserved pins
 */
void esp_gpio_reserve_pins(uint64_t mask);

/**
 * @brief Check whether the pin has been reserved
 *
 * @param[in]  gpio_num  GPIO pin number, please input a gpio number within `SOC_GPIO_PIN_COUNT`
 * @return
 *      - true  This gpio is reserved for FLASH or PSRAM
 *      - false This gpio is available for other purposes
 */
bool esp_gpio_is_pin_reserved(uint32_t gpio_num);

#ifdef __cplusplus
}
#endif
