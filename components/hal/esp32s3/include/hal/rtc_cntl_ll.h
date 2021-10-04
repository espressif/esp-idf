// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/apb_ctrl_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_CNTL_LL_RETENTION_TARGET_CPU         (BIT(0))
#define RTC_CNTL_LL_RETENTION_TARGET_TAGMEM      (BIT(1))

static inline void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_MAIN_TIMER_INT_CLR_M);
    SET_PERI_REG_MASK(RTC_CNTL_SLP_TIMER1_REG, RTC_CNTL_MAIN_TIMER_ALARM_EN_M);
}

static inline uint32_t rtc_cntl_ll_ext1_get_wakeup_pins(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
}

static inline void rtc_cntl_ll_ext1_set_wakeup_pins(uint32_t mask, int mode)
{
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, mask);
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            mode, RTC_CNTL_EXT_WAKEUP1_LV_S);
}

static inline void rtc_cntl_ll_ext1_clear_wakeup_pins(void)
{
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
}

static inline void rtc_cntl_ll_ulp_wakeup_enable(void)
{
    SET_PERI_REG_BITS(RTC_CNTL_STATE0_REG, RTC_CNTL_WAKEUP_ENA_V, 0x800, RTC_CNTL_WAKEUP_ENA_S);
}

static inline void rtc_cntl_ll_set_tagmem_retention_link_addr(uint32_t link_addr)
{
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL1_REG, APB_CTRL_RETENTION_TAG_LINK_ADDR, link_addr);
}

static inline void rtc_cntl_ll_enable_tagmem_retention(void)
{
    /* Enable i/d-cache tagmem retenttion. cpu: 1, tagmem: 2, cpu + tagmem: 3 */
    uint32_t target = REG_GET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET);
    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET, (target | RTC_CNTL_LL_RETENTION_TARGET_TAGMEM));
}

static inline void rtc_cntl_ll_enable_icache_tagmem_retention(uint32_t start_point, uint32_t vld_size, uint32_t size)
{
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL2_REG, APB_CTRL_RET_ICACHE_START_POINT, start_point);
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL2_REG, APB_CTRL_RET_ICACHE_VLD_SIZE, vld_size);
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL2_REG, APB_CTRL_RET_ICACHE_SIZE, size);
    REG_SET_BIT(APB_CTRL_RETENTION_CTRL2_REG, APB_CTRL_RET_ICACHE_ENABLE);
}

static inline void rtc_cntl_ll_enable_dcache_tagmem_retention(uint32_t start_point, uint32_t vld_size, uint32_t size)
{
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL3_REG, APB_CTRL_RET_DCACHE_START_POINT, start_point);
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL3_REG, APB_CTRL_RET_DCACHE_VLD_SIZE, vld_size);
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL3_REG, APB_CTRL_RET_DCACHE_SIZE, size);
    REG_SET_BIT(APB_CTRL_RETENTION_CTRL3_REG, APB_CTRL_RET_DCACHE_ENABLE);
}

static inline void rtc_cntl_ll_disable_tagmem_retention(void)
{
    /* Enable i/d-cache tagmem retenttion. cpu: 1, tagmem: 2, cpu + tagmem: 3 */
    uint32_t target = REG_GET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET);
    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET, (target & ~RTC_CNTL_LL_RETENTION_TARGET_TAGMEM));
}

static inline void rtc_cntl_ll_disable_icache_tagmem_retention(void)
{
    REG_CLR_BIT(APB_CTRL_RETENTION_CTRL2_REG, APB_CTRL_RET_ICACHE_ENABLE);
}

static inline void rtc_cntl_ll_disable_dcache_tagmem_retention(void)
{
    REG_CLR_BIT(APB_CTRL_RETENTION_CTRL3_REG, APB_CTRL_RET_DCACHE_ENABLE);
}

static inline void rtc_cntl_ll_set_cpu_retention_link_addr(uint32_t link_addr)
{
    REG_SET_FIELD(APB_CTRL_RETENTION_CTRL_REG, APB_CTRL_RETENTION_CPU_LINK_ADDR, link_addr);
}

static inline void rtc_cntl_ll_enable_cpu_retention_clock(void)
{
    REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN); /* Enable internal 20 MHz clock */
}

static inline void rtc_cntl_ll_enable_cpu_retention(void)
{
    uint32_t target = REG_GET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET);

    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_TARGET, (target | RTC_CNTL_LL_RETENTION_TARGET_CPU));
    /* Enable retention when cpu sleep enable */
    REG_SET_BIT(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_EN);
}

static inline void rtc_cntl_ll_config_cpu_retention_timing(int wait, int clkoff_wait, int done_wait)
{
    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_WAIT, wait);
    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_CLKOFF_WAIT, clkoff_wait);
    REG_SET_FIELD(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_DONE_WAIT, done_wait);
}

static inline void rtc_cntl_ll_disable_cpu_retention(void)
{
    REG_CLR_BIT(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_EN);
}

#ifdef __cplusplus
}
#endif
