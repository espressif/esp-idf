/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The LL layer for RTC(LP) watchdog register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/lpwdt_ll.h"

typedef lp_wdt_dev_t rwdt_dev_t;

#define RWDT_DEV_GET() &LP_WDT

#define rwdt_ll_enable(hw) \
        lpwdt_ll_enable(hw)

#define rwdt_ll_disable(hw) \
        lpwdt_ll_disable(hw)

#define rwdt_ll_check_if_enabled(hw) \
        lpwdt_ll_check_if_enabled(hw)

#define rwdt_ll_config_stage(hw, stage, timeout_ticks, behavior) \
        lpwdt_ll_config_stage(hw, stage, timeout_ticks, behavior)

#define rwdt_ll_disable_stage(hw, stage) \
        lpwdt_ll_disable_stage(hw, stage)

#define rwdt_ll_set_cpu_reset_length(hw, length) \
        lpwdt_ll_set_cpu_reset_length(hw, length)

#define rwdt_ll_set_sys_reset_length(hw, length) \
        lpwdt_ll_set_sys_reset_length(hw, length)

#define rwdt_ll_set_flashboot_en(hw, enable) \
        lpwdt_ll_set_flashboot_en(hw, enable)

#define rwdt_ll_set_procpu_reset_en(hw, enable) \
        lpwdt_ll_set_procpu_reset_en(hw, enable)

#define rwdt_ll_set_appcpu_reset_en(hw, enable) \
        lpwdt_ll_set_appcpu_reset_en(hw, enable)

#define rwdt_ll_set_pause_in_sleep_en(hw, enable) \
        lpwdt_ll_set_pause_in_sleep_en(hw, enable)

#define rwdt_ll_set_chip_reset_en(hw, enable) \
        lpwdt_ll_set_chip_reset_en(hw, enable)

#define rwdt_ll_set_chip_reset_width(hw, width) \
        lpwdt_ll_set_chip_reset_width(hw, width)

#define rwdt_ll_feed(hw) \
        lpwdt_ll_feed(hw)

#define rwdt_ll_write_protect_enable(hw) \
        lpwdt_ll_write_protect_enable(hw)

#define rwdt_ll_write_protect_disable(hw) \
        lpwdt_ll_write_protect_disable(hw)

#define rwdt_ll_set_intr_enable(hw, enable) \
        lpwdt_ll_set_intr_enable(hw, enable)

#define rwdt_ll_check_intr_status(hw) \
        lpwdt_ll_check_intr_status(hw)

#define rwdt_ll_clear_intr_status(hw) \
        lpwdt_ll_clear_intr_status(hw)

#ifdef __cplusplus
}
#endif
