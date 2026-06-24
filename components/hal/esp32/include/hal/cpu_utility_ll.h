/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void cpu_utility_ll_reset_cpu(uint32_t cpu_no)
{
    /*
    Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
    "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
    file's "rodata" section (see IDF-5214).
    */
    uint32_t rtc_cntl_rst = (cpu_no == 0) ? RTC_CNTL_SW_PROCPU_RST_M : RTC_CNTL_SW_APPCPU_RST_M;
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_rst);
}

#if SOC_CPU_CORES_NUM > 1   // We only allow stalling/unstalling of other cores
FORCE_INLINE_ATTR void cpu_utility_ll_stall_cpu(uint32_t cpu_no)
{
    /*
        We need to write the value "0x86" to stall a particular core. The write location is split into two separate
        bit fields named "c0" and "c1", and the two fields are located in different registers. Each core has its own pair of
        "c0" and "c1" bit fields.

        Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
        "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
        file's "rodata" section (see IDF-5214).
    */
    uint32_t rtc_cntl_c0_m = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_M : RTC_CNTL_SW_STALL_APPCPU_C0_M;
    uint32_t rtc_cntl_c0_s = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_S : RTC_CNTL_SW_STALL_APPCPU_C0_S;
    uint32_t rtc_cntl_c1_m = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_M : RTC_CNTL_SW_STALL_APPCPU_C1_M;
    uint32_t rtc_cntl_c1_s = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_S : RTC_CNTL_SW_STALL_APPCPU_C1_S;
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_c0_m);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, 2 << rtc_cntl_c0_s);
    CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, rtc_cntl_c1_m);
    SET_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, 0x21 << rtc_cntl_c1_s);
}

FORCE_INLINE_ATTR void cpu_utility_ll_unstall_cpu(uint32_t cpu_no)
{
    /*
        We need to write clear the value "0x86" to unstall a particular core. The location of this value is split into
        two separate bit fields named "c0" and "c1", and the two fields are located in different registers. Each core has
        its own pair of "c0" and "c1" bit fields.

        Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
        "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
        file's "rodata" section (see IDF-5214).
    */
    uint32_t rtc_cntl_c0 = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_M : RTC_CNTL_SW_STALL_APPCPU_C0_M;
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_c0);
    int rtc_cntl_c1 = (cpu_no == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_M : RTC_CNTL_SW_STALL_APPCPU_C1_M;
    CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, rtc_cntl_c1);
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_debug(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        DPORT_REG_SET_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_PDEBUG_ENABLE);
    } else {
        DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_PDEBUG_ENABLE);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_record(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        DPORT_REG_SET_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_RECORD_ENABLE);
        DPORT_REG_CLR_BIT(DPORT_PRO_CPU_RECORD_CTRL_REG, DPORT_PRO_CPU_RECORD_ENABLE);
    } else {
        DPORT_REG_SET_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
        DPORT_REG_CLR_BIT(DPORT_APP_CPU_RECORD_CTRL_REG, DPORT_APP_CPU_RECORD_ENABLE);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu(void)
{
    if (!DPORT_GET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN)) {
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_C_REG, DPORT_APPCPU_RUNSTALL);
        DPORT_SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu_int_matrix(void)
{
}

#endif // SOC_CPU_CORES_NUM > 1

#ifdef __cplusplus
}
#endif
