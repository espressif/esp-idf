/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/wdt_types.h"
#include "hal/mwdt_ll.h"
#include "hal/rwdt_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    wdt_inst_t inst;                /**< Which WDT instance this HAL context is using (i.e. MWDT0, MWDT1, RWDT)*/
    union {
        timg_dev_t *mwdt_dev;       /**< Starting address of the MWDT */
        rwdt_dev_t *rwdt_dev;       /**< Starting address of the RWDT*/
    };
} wdt_hal_context_t;

#define RWDT_HAL_CONTEXT_DEFAULT() { \
    .inst = WDT_RWDT,                \
    .rwdt_dev = RWDT_DEV_GET()       \
}

/* ---------------------------- Init and Config ----------------------------- */

/**
 * @brief Initialize one of the WDTs associated HAL context
 *
 * This function initializes one of the WDTs (MWDT0, MWDT1, or RWDT) hardware by
 * doing the following:
 * - Disables the WDT and all of its stages
 * - Sets some registers with default values
 * - Sets the WDTs source clock prescaler (not applicable to RWDT)
 * - Optionally enables the level interrupt
 *
 * The HAL context is initialized by storing the type (i.e. MWDT or RWDT) of
 * this WDT instance, and a pointer to the associated registers.
 *
 * @param hal Context of HAL layer
 * @param wdt_inst Which WDT instance to initialize (MWDT0, MWDT1, or RWDT)
 * @param prescaler MWDT source clock prescaler. Unused for RWDT
 * @param enable_intr True to enable level interrupt. False to disable
 *
 * @note Although the WDTs on the ESP32 have an edge interrupt, this HAL does
 *       not utilize it and will always disables it.
 * @note RWDT does not have a prescaler. Its tick rate is equal to the
 *       frequency of its source clock (RTC slow clock).
 */
void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr);

/**
 * @brief Deinitialize a WDT and its HAL context
 *
 * This function deinitializes a WDT by feeding then disabling it. The WDT's
 * interrupt is also cleared and disabled. The HAL context is cleared.
 *
 * @param hal Context of HAL layer
 */
void wdt_hal_deinit(wdt_hal_context_t *hal);

/**
 * @brief Configure a particular stage of a WDT
 *
 * @param hal Context of HAL layer
 * @param stage Stage to configure (0 to 3)
 * @param timeout Number of WDT ticks for the stage to time out
 * @param behavior What action to take when the stage times out. Note that only
 *                 the RWDT supports the RTC reset action.
 *
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

/* -------------------------------- Runtime --------------------------------- */

/**
 * @brief Disable write protection of the WDT registers
 *
 * @param hal Context of HAL layer
 */
void wdt_hal_write_protect_disable(wdt_hal_context_t *hal);

/**
 * @brief Enable write protection of the WDT registers
 *
 * @param hal Context of HAL layer
 */
void wdt_hal_write_protect_enable(wdt_hal_context_t *hal);

/**
 * @brief Enable the WDT
 *
 * The WDT will start counting when enabled. This function also feeds the WDT
 * before enabling it.
 *
 * @param hal Context of HAL layer
 *
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_enable(wdt_hal_context_t *hal);

/**
 * @brief Disable the WDT
 *
 * @param hal Context of HAL layer
 *
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_disable(wdt_hal_context_t *hal);

/**
 * @brief Handle WDT interrupt
 *
 * Clears the interrupt status bit and feeds the WDT
 *
 * @param hal Context of HAL layer
 *
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_handle_intr(wdt_hal_context_t *hal);

/**
 * @brief Feed the WDT
 *
 * Feeding the WDT will reset the internal count and current stage.
 *
 * @param hal Context of HAL layer
 *
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_feed(wdt_hal_context_t *hal);

/**
 * @brief Enable/Disable the WDT flash boot mode
 *
 * @param hal Context of HAL layer
 * @param enable True to enable flash boot mode, false to disable.
 *
 * @note Flash boot mode can trigger a time out even if the WDT is disabled.
 * @note This function can only be called when the WDT is unlocked. Call
 *       wdt_hal_write_protect_disable() first.
 */
void wdt_hal_set_flashboot_en(wdt_hal_context_t *hal, bool enable);

/**
 * @brief Check if the WDT is enabled
 *
 * @param hal Context of HAL layer
 * @return True if enabled, false otherwise
 */
bool wdt_hal_is_enabled(wdt_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
