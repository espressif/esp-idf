/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define MHZ (1000000)

#define DPORT_CPUPERIOD_SEL_80      0
#define DPORT_CPUPERIOD_SEL_160     1

#define DPORT_SOC_CLK_SEL_XTAL    0
#define DPORT_SOC_CLK_SEL_PLL    1
#define DPORT_SOC_CLK_SEL_8M     2

#define RTC_FAST_CLK_FREQ_8M        8500000

#ifdef __cplusplus
extern "C" {
#endif

void rtc_clk_cpu_freq_to_xtal(int freq, int div);

/* Values of RTC_XTAL_FREQ_REG and RTC_APB_FREQ_REG are stored as two copies in
 * lower and upper 16-bit halves. These are the routines to work with such a
 * representation.
 */
static inline bool clk_val_is_valid(uint32_t val)
{
    return (val & 0xffff) == ((val >> 16) & 0xffff) &&
           val != 0 &&
           val != UINT32_MAX;
}

static inline uint32_t reg_val_to_clk_val(uint32_t val)
{
    return val & UINT16_MAX;
}

static inline uint32_t clk_val_to_reg_val(uint32_t val)
{
    return (val & UINT16_MAX) | ((val & UINT16_MAX) << 16);
}

#ifdef __cplusplus
}
#endif
