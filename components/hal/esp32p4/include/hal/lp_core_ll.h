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
#include "soc/lp_system_struct.h"
#include "soc/soc_etm_struct.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LP_CORE_LL_WAKEUP_SOURCE_LP_IO      BIT(9)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_UART    BIT(10)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER_0 BIT(13)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_BOD     BIT(14)
#define LP_CORE_LL_WAKEUP_SOURCE_ETM        BIT(17)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER_1 BIT(18)
#define LP_CORE_LL_WAKEUP_SOURCE_LP_VAD     BIT(19)
#define LP_CORE_LL_WAKEUP_SOURCE_HP_CPU     BIT(22)

/* Use lp timer 1 as the normal wakeup timer, timer 0 is used by deep sleep */
#define LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER_1

/**
 * @brief Enable the bus clock for LP-coree
 *
 * @param enable true to enable, false to disable
 */
static inline void lp_core_ll_enable_bus_clock(bool enable)
{
    LPPERI.clk_en.ck_en_lp_core = enable;
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
    LPPERI.reset_en.rst_en_lp_core = 1;
    LPPERI.reset_en.rst_en_lp_core = 0;
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
    LPPERI.cpu.lpcore_dbgm_unavailable = !enable;
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
    PMU.lp_ext.pwr2.wakeup_en = flags;
}

/**
 * @brief Get wake-up sources for the LP-core
 */
static inline uint32_t lp_core_ll_get_wakeup_source(void)
{
    return PMU.lp_ext.pwr2.wakeup_en;
}

/**
 * @brief Set boot address for lp core
 *
 * @param boot_address address which the lp core will start booting from
 */
static inline void lp_core_ll_set_boot_address(intptr_t boot_address)
{
    LP_SYS.lp_core_boot_addr.lp_cpu_boot_addr = boot_address;
}


/**
 * @brief Set address LP-ROM bootloader will jump to after initialization
 *
 * @param boot_address address which the LP-ROM bootloader will jump to
 */
static inline void lp_core_ll_set_app_boot_address(intptr_t boot_address)
{
    LP_SYS.boot_addr_hp_lp_reg.boot_addr_hp_lp = boot_address;
}

/**
 * @brief Request PMU to put LP core to sleep
 */
static inline void lp_core_ll_request_sleep(void)
{
    PMU.lp_ext.pwr1.sleep_req = 1;
}

/**
 * @brief Clear the ETM wakeup interrupt sources on the LP core
 *
 */
static inline void lp_core_ll_clear_etm_wakeup_status(void)
{
    LP_SYS.sys_ctrl.lp_core_etm_wakeup_flag_clr = 1;
}

/**
 * @brief Get the flag that marks whether LP CPU is awakened by ETM
 *
 * @return Return true if lpcore is woken up by soc etm flag
 */
static inline bool lp_core_ll_get_etm_wakeup_flag(void)
{
    return SOC_ETM.task_st5.ulp_task_wakeup_cpu_st;
}

/**
 * @brief Clear the flag that marks whether LP CPU is awakened by ETM
 */
static inline void lp_core_ll_clear_etm_wakeup_flag(void)
{
    SOC_ETM.task_st5_clr.ulp_task_wakeup_cpu_st_clr = 1;
}

#ifdef __cplusplus
}
#endif
