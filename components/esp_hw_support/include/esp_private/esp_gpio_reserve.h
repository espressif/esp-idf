/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Background:
 *
 * - On some **modules**, specific GPIOs are connected to the PSRAM or Flash, and they shouldn't be used as general purpose IOs in the user's projects.
 * - Some GPIO may be not fan-out in the SiP variant.
 * - GPIO A is driven by peripheral M, we don't want peripheral N to use the same GPIO.
 * - User may deliver a board where a GPIO is used for a special purpose or even not fan-out on the PCB, they want to reserve it in the BSP package.
 * ...
 */

/**
 * Usage Attention:
 *
 * - If a GPIO is used by IO MUX, no matter it's used as Input or Output, we should reserve it, because IO MUX's different "FUNC" has its dedicated peripheral.
 * - If a GPIO is used by Matrix, and only use its output path, we should reserve it, because we can't bind multiple peripheral output signals to the same GPIO.
 * - When doing GPIO reserve, we must check its return value, to ensure the same GPIO is not reserved already.
 */

#pragma once

#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reserve the given GPIOs by mask, so they can't be used by others
 *
 * @param gpio_mask Mask of the GPIOs to be reserved
 * @return The mask of the GPIOs that were already reserved before this call
 */
uint64_t esp_gpio_reserve(uint64_t gpio_mask);

/**
 * @brief Revoke the given GPIOs by mask, so they can be reused again by others
 *
 * @param gpio_mask Mask of the GPIOs to be revoked
 * @return The mask of the GPIOs that were already reserved before this call
 */
uint64_t esp_gpio_revoke(uint64_t gpio_mask);

/**
 * @brief Check whether the given GPIOs are reserved
 *
 * @param gpio_mask Mask of the GPIOs to be checked
 * @return
 *      - true  Any of the given GPIO(s) is reserved
 *      - false Any of the given GPIO(s) is not reserved
 */
bool esp_gpio_is_reserved(uint64_t gpio_mask);

#ifdef __cplusplus
}
#endif
