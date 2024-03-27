/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include "soc/lpperi_struct.h"
#include "soc/pmu_struct.h"
#include "soc/lp_aon_struct.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LP_CORE_LL_WAKEUP_SOURCE_HP_CPU    BIT(0) // Started by HP core (1 single wakeup)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_UART   BIT(1) // Enable wake-up by a certain number of LP UART RX pulses
#define LP_CORE_LL_WAKEUP_SOURCE_LP_IO     BIT(2) // Enable wake-up by LP IO interrupt
#define LP_CORE_LL_WAKEUP_SOURCE_ETM       BIT(3) // Enable wake-up by ETM event
#define LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER  BIT(4) // Enable wake-up by LP timer

/**
 * @brief Enable the bus clock for LP-core
 *
 * @param enable Enable if true, disable if false
 */
static inline void lp_core_ll_enable_bus_clock(bool enable)
{
    /* ESP32C6 does not have clk/rst periph control for LP-core */
    (void)enable;
}

/**
 * @brief Reset the lp_core module
 *
 */
static inline void lp_core_ll_reset_register(void)
{
    /* ESP32C6 does not have clk/rst periph control for LP-core */
}

/**
 * @brief Enable fast access of LP memory
 *
 * @note When fast access is activated, LP-core cannot access LP mem during deep sleep
 *
 * @param enable Enable if true, disable if false
 */
static inline void lp_core_ll_fast_lp_mem_enable(bool enable)
{
    LP_AON.lpbus.fast_mem_mux_sel = enable;
    LP_AON.lpbus.fast_mem_mux_sel_update = 1;
}

/**
 * @brief Trigger a LP_CORE_LL_WAKEUP_SOURCE_HP_CPU wake-up on the LP-core
 *
 */
static inline void lp_core_ll_hp_wake_lp(void)
{
    PMU.hp_lp_cpu_comm.hp_trigger_lp = 1;
}

/**
 * @brief Enable the debug module of LP-core, allowing JTAG to connect
 *
 * @param enable Enable if true, disable if false
 */
static inline void lp_core_ll_debug_module_enable(bool enable)
{
    LPPERI.cpu.lpcore_dbgm_unavaliable = !enable;
}

/**
 * @brief Enable CPU reset at sleep
 *
 * @param enable Enable if true, disable if false
 */
static inline void lp_core_ll_rst_at_sleep_enable(bool enable)
{
    PMU.lp_ext.pwr0.slp_reset_en = enable;
}

/**
 * @brief Stall LP-core at sleep requests
 *
 * @param enable Enable if true, disable if false
 */
static inline void lp_core_ll_stall_at_sleep_request(bool enable)
{
    PMU.lp_ext.pwr0.slp_stall_en = enable;
}

/**
 * @brief Set wake-up sources for the LP-core
 *
 * @param flags Wake-up sources
 */
static inline void lp_core_ll_set_wakeup_source(uint32_t flags)
{
    PMU.lp_ext.pwr1.wakeup_en = flags;
}

/**
 * @brief Get wake-up sources for the LP-core
 */
static inline uint32_t lp_core_ll_get_wakeup_source(void)
{
    return PMU.lp_ext.pwr1.wakeup_en;
}

/**
 * @brief Request PMU to put LP core to sleep
 */
static inline void lp_core_ll_request_sleep(void)
{
    PMU.lp_ext.pwr1.sleep_req = 1;
}

#ifdef __cplusplus
}
#endif
