/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_RANDOM_H__
#define __ESP_RANDOM_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get one random 32-bit word from hardware RNG
 *
 * If Wi-Fi or Bluetooth are enabled, this function returns true random numbers. In other
 * situations, if true random numbers are required then consult the ESP-IDF Programming
 * Guide "Random Number Generation" section for necessary prerequisites.
 *
 * This function automatically busy-waits to ensure enough external entropy has been
 * introduced into the hardware RNG state, before returning a new random number. This delay
 * is very short (always less than 100 CPU cycles).
 *
 * @return Random value between 0 and UINT32_MAX
 */
uint32_t esp_random(void);

/**
 * @brief Fill a buffer with random bytes from hardware RNG
 *
 * @note This function is implemented via calls to esp_random(), so the same
 * constraints apply.
 *
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 */
void esp_fill_random(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_RANDOM_H__ */
