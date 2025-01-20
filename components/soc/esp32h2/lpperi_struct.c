/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "assert.h"
#include "soc/lpperi_struct.h"

#define LPPERI_REV1_2_DATE   (0x2308030)
#define LPPERI_REG_ADDR(reg) ((LPPERI_REV1_2.date.lpperi_date >= LPPERI_REV1_2_DATE) ?  \
                              &LPPERI_REV1_2.reg :  \
                              (volatile typeof(LPPERI_REV1_2.reg) *)&LPPERI_REV0_0.reg)

/**
 * @brief Compatible LPPERI instance
 * @note  Set NULL as default so that to abort when use the LPPEIR before initialization
 *        If you want to use the LPPERI before it is initialized, please use `lpperi_reg.h` instead
 */
lpperi_dev_t LPPERI = {};

/**
 * @brief Initialize the LP_PERI register address according to the register version
 */
void lpperi_compatible_reg_addr_init(void)
{
    LPPERI.clk_en             = LPPERI_REG_ADDR(clk_en);
    LPPERI.reset_en           = LPPERI_REG_ADDR(reset_en);
    LPPERI.rng_cfg            = ((LPPERI_REV1_2.date.lpperi_date >= LPPERI_REV1_2_DATE) ? &LPPERI_REV1_2.rng_cfg : NULL);
    LPPERI.rng_data           = LPPERI_REG_ADDR(rng_data);
    LPPERI.cpu                = LPPERI_REG_ADDR(cpu);
    LPPERI.bus_timeout        = LPPERI_REG_ADDR(bus_timeout);
    LPPERI.bus_timeout_addr   = LPPERI_REG_ADDR(bus_timeout_addr);
    LPPERI.bus_timeout_uid    = LPPERI_REG_ADDR(bus_timeout_uid);
    LPPERI.mem_ctrl           = LPPERI_REG_ADDR(mem_ctrl);
    LPPERI.interrupt_source   = LPPERI_REG_ADDR(interrupt_source);
    LPPERI.debug_sel0         = LPPERI_REG_ADDR(debug_sel0);
    LPPERI.debug_sel1         = LPPERI_REG_ADDR(debug_sel1);
    LPPERI.date               = LPPERI_REG_ADDR(date);
}

__attribute__((constructor))
static void s_lpperi_compatible_init_check(void)
{
    // Make sure it is initialized
    assert(LPPERI.clk_en != NULL);
}
