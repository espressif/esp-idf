/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief Enable the bus clock for LP-coree
 *
 * @param enable true to enable, false to disable
 */
static inline void lp_core_ll_enable_bus_clock(bool enable)
{
    LPPERI.clk_en.lp_cpu_ck_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_core_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_core_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the lp_core module
 *
 */
static inline void lp_core_ll_reset_register(void)
{
    LPPERI.reset_en.lp_cpu_reset_en = 1;
    LPPERI.reset_en.lp_cpu_reset_en = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_core_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_core_ll_reset_register(__VA_ARGS__)


/**
 * @brief Trigger a LP_CORE_LL_WAKEUP_SOURCE_HP_CPU wake-up on the lp core
 *
 */
static inline void lp_core_ll_hp_wake_lp(void)
{
    PMU.hp_lp_cpu_comm.hp_trigger_lp = 1;
}

/**
 * @brief Enables the LP core debug module, allowing JTAG to connect
 *
 * @param enable enable if true, disable if false
 */
static inline void lp_core_ll_debug_module_enable(bool enable)
{
    LPPERI.cpu.lpcore_dbgm_unavaliable = !enable;
}

/**
 * @brief Enables CPU reset at sleep
 *
 * @param enable enable if true, disable if false
 */
static inline void lp_core_ll_rst_at_sleep_enable(bool enable)
{
    PMU.lp_ext.pwr0.slp_reset_en = enable;
}

/**
 * @brief Stall lp core cpu at sleep request
 *
 * @param enable enable if true, disable if false
 */
static inline void lp_core_ll_stall_at_sleep_request(bool enable)
{
    PMU.lp_ext.pwr0.slp_stall_en = enable;
}

/**
 * @brief Set the wake-up source for the lp-core
 *
 * @param flags wake-up sources
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

/**
 * @brief Get which interrupts have triggered on the LP core
 *
 * @return uint8_t bit mask of triggered LP interrupt sources
 */
static inline uint8_t lp_core_ll_get_triggered_interrupt_srcs(void)
{
    return LPPERI.interrupt_source.lp_interrupt_source;
}

/**
 * @brief Get the flag that marks whether LP CPU is awakened by ETM
 *
 * @return Return true if lpcore is woken up by soc_etm
 */
static inline bool lp_core_ll_get_etm_wakeup_flag(void)
{
    return LP_AON.lpcore.lpcore_etm_wakeup_flag;
}

/**
 * @brief Clear the flag that marks whether LP CPU is awakened by soc_etm
 */
static inline void lp_core_ll_clear_etm_wakeup_flag(void)
{
    LP_AON.lpcore.lpcore_etm_wakeup_flag_clr = 0x01;
}

#ifdef __cplusplus
}
#endif
