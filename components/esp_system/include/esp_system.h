/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_SYSTEM_H__
#define __ESP_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "esp_idf_version.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset reasons
 */
typedef enum {
    ESP_RST_UNKNOWN,    //!< Reset reason can not be determined
    ESP_RST_POWERON,    //!< Reset due to power-on event
    ESP_RST_EXT,        //!< Reset by external pin (not applicable for ESP32)
    ESP_RST_SW,         //!< Software reset via esp_restart
    ESP_RST_PANIC,      //!< Software reset due to exception/panic
    ESP_RST_INT_WDT,    //!< Reset (software or hardware) due to interrupt watchdog
    ESP_RST_TASK_WDT,   //!< Reset due to task watchdog
    ESP_RST_WDT,        //!< Reset due to other watchdogs
    ESP_RST_DEEPSLEEP,  //!< Reset after exiting deep sleep mode
    ESP_RST_BROWNOUT,   //!< Brownout reset (software or hardware)
    ESP_RST_SDIO,       //!< Reset over SDIO
    ESP_RST_USB,        //!< Reset by USB peripheral
    ESP_RST_JTAG,       //!< Reset by JTAG
    ESP_RST_EFUSE,      //!< Reset due to efuse error
    ESP_RST_PWR_GLITCH, //!< Reset due to power glitch detected
    ESP_RST_CPU_LOCKUP, //!< Reset due to CPU lock up (double exception)
} esp_reset_reason_t;

/**
 * Shutdown handler type
 */
typedef void (*shutdown_handler_t)(void);

/**
 * @brief Register a shutdown handler
 *
 * Registers a handler invoked before the application is restarted via
 * esp_restart(). The dispatch order between handlers registered through
 * this function is unspecified; if a deterministic order is required,
 * use ESP_SHUTDOWN_HANDLER_REGISTER instead, which dispatches in priority
 * order and runs before any handlers registered here.
 *
 * The number of registrations is bounded only by available heap.
 *
 * @param handle Function to execute at shutdown. Must not be NULL.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if `handle` is NULL
 *      - ESP_ERR_INVALID_STATE if the handler is already registered
 *      - ESP_ERR_NO_MEM if memory allocation failed
 */
esp_err_t esp_register_shutdown_handler(shutdown_handler_t handle);

/**
 * @brief Unregister a shutdown handler
 *
 * Removes a handler previously registered via esp_register_shutdown_handler().
 *
 * @param handle The handler to unregister. Must not be NULL.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if `handle` is NULL
 *      - ESP_ERR_INVALID_STATE if the handler is not currently registered
 */
esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handle);

/**
 * @brief Register a static (link-time) shutdown handler
 *
 * Handlers registered with this macro are called at shutdown time in
 * priority order (lower priority values run first), before any handlers
 * registered dynamically via esp_register_shutdown_handler().
 *
 * @param fn Handler function name (signature: esp_err_t fn(void *user_arg, void *ctx))
 * @param prio Priority (lower runs first)
 */
#define ESP_SHUTDOWN_HANDLER_REGISTER(fn, prio) \
    static esp_err_t fn(void *user_arg, void *ctx); \
    static _SECTION_ATTR_IMPL_GENERIC("esysev_shdn", prio) \
        struct { esp_err_t (*handler)(void *user_arg, void *ctx); } _esysev_SHUTDOWN_##fn = { .handler = fn }; \
    static esp_err_t fn(void *user_arg, void *ctx)

/**
  * @brief  Restart PRO and APP CPUs.
  *
  * This function can be called both from PRO and APP CPUs.
  * After successful restart, CPU reset reason will be SW_CPU_RESET.
  * Peripherals (except for Wi-Fi, BT, UART0, SPI1, and legacy timers) are not reset.
  * This function does not return.
  */
void esp_restart(void) __attribute__((__noreturn__));

/**
 * @brief  Get reason of last reset
 * @return See description of esp_reset_reason_t for explanation of each value.
 */
esp_reset_reason_t esp_reset_reason(void);

/**
  * @brief  Get the size of available heap.
  *
  * @note Note that the returned value may be larger than the maximum contiguous block
  * which can be allocated.
  *
  * @return Available heap size, in bytes.
  */
uint32_t esp_get_free_heap_size(void);

/**
  * @brief  Get the size of available internal heap.
  *
  * @note Note that the returned value may be larger than the maximum contiguous block
  * which can be allocated.
  *
  * @return Available internal heap size, in bytes.
  */
uint32_t esp_get_free_internal_heap_size(void);

/**
  * @brief Get the minimum heap that has ever been available
  *
  * @return Minimum free heap ever available
  */
uint32_t esp_get_minimum_free_heap_size(void);

/**
 * @brief Trigger a software abort
 *
 * @param details Details that will be displayed during panic handling.
 */
void  __attribute__((__noreturn__)) esp_system_abort(const char* details);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SYSTEM_H__ */
