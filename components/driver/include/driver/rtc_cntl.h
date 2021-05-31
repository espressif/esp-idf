/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register a handler for specific RTC_CNTL interrupts
 *
 * Multiple handlers can be registered using this function. Whenever an
 * RTC interrupt happens, all handlers with matching rtc_intr_mask values
 * will be called.
 *
 * @param handler  handler function to call
 * @param handler_arg  argument to be passed to the handler
 * @param rtc_intr_mask  combination of RTC_CNTL_*_INT_ENA bits indicating the
 *                       sources to call the handler for
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory to allocate handler structure
 *      - other errors returned by esp_intr_alloc
 */
esp_err_t rtc_isr_register(intr_handler_t handler, void* handler_arg,
                            uint32_t rtc_intr_mask);
/**
 * @brief Deregister the handler previously registered using rtc_isr_register
 * @param handler  handler function to call (as passed to rtc_isr_register)
 * @param handler_arg  argument of the handler (as passed to rtc_isr_register)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if a handler matching both handler and
 *        handler_arg isn't registered
 */
esp_err_t rtc_isr_deregister(intr_handler_t handler, void* handler_arg);

#ifdef __cplusplus
}
#endif
