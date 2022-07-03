/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable an entropy source for RNG if RF subsystem is disabled
 *
 * @warning This function is not safe to use if any other subsystem is accessing the RF subsystem or
 *          the ADC at the same time!
 *
 * The exact internal entropy source mechanism depends on the chip in use but
 * all SoCs use the SAR ADC to continuously mix random bits (an internal
 * noise reading) into the HWRNG. Consult the SoC Technical Reference
 * Manual for more information.
 *
 * Can also be called from app code, if true random numbers are required without initialized RF subsystem.
 * This might be the case in early startup code of the application when the RF subsystem has not
 * started yet or if the RF subsystem should not be enabled for power saving.
 *
 * Consult ESP-IDF Programming Guide "Random Number Generation" section for
 * details.
 */
void bootloader_random_enable(void);

/**
 * @brief Disable entropy source for RNG
 *
 * Disables internal entropy source. Must be called after
 * bootloader_random_enable() and before RF subsystem features, ADC, or
 * I2S (ESP32 only) are initialized.
 *
 * Consult the ESP-IDF Programming Guide "Random Number Generation"
 * section for details.
 */
void bootloader_random_disable(void);

/**
 * @brief Fill buffer with 'length' random bytes
 *
 * @note If this function is being called from app code only, and never
 * from the bootloader, then it's better to call esp_fill_random().
 *
 * @param buffer Pointer to buffer
 * @param length This many bytes of random data will be copied to buffer
 */
void bootloader_fill_random(void *buffer, size_t length);

#ifdef __cplusplus
}
#endif
