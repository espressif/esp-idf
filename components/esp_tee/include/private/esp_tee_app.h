/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interface function that allows untrusted applications to invoke secure services through TEE
 *
 * @param argc Number of arguments being passed to the secure service
 *
 * @return Value returned by the secure service function
 */
uint32_t esp_tee_service_call(int argc, ...);

/**
 * @brief Interface function that allows untrusted applications to invoke secure services through TEE,
 *        with the scheduler and the non-IRAM interrupts disabled
 *
 * @param argc Number of arguments being passed to the secure service
 *
 * @return Value returned by the secure service function
 */
uint32_t esp_tee_service_call_with_noniram_intr_disabled(int argc, ...);

#ifdef __cplusplus
}
#endif
