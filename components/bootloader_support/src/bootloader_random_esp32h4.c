/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD

 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/soc.h"
#include "soc/rng_reg.h"
#include "esp_rom_sys.h"



void bootloader_random_enable(void)
{
    REG_SET_BIT(RNG_DATE_REG, RNG_CLK_EN);
    REG_SET_BIT(RNG_CFG_REG, RNG_TIMER_EN);
    REG_SET_FIELD(RNG_CFG_REG, RNG_RTC_TIMER_EN, 0x3);
    REG_SET_BIT(RNG_CFG_REG, RNG_SAMPLE_ENABLE);


    for (int i = 0; i < 10; i++) {
        uint32_t random_sync = REG_READ(RNG_DATA_SYNC_REG);
        uint32_t random_unsync = REG_READ(RNG_DATA_REG);
        uint32_t rng_cfg = REG_READ(RNG_CFG_REG);
        uint32_t sample_cnt = (rng_cfg >> RNG_SAMPLE_CNT_S) & 0xFF;
        esp_rom_printf("  [%d] sync: 0x%08x, unsync: 0x%08x, sample_cnt: 0x%02x\r\n",
                       i + 1, random_sync, random_unsync, sample_cnt);
        esp_rom_delay_us(1000);
    }
}

void bootloader_random_disable(void)
{
    REG_CLR_BIT(RNG_CFG_REG, RNG_SAMPLE_ENABLE);
    REG_SET_FIELD(RNG_CFG_REG, RNG_RTC_TIMER_EN, 0x0);
    REG_CLR_BIT(RNG_CFG_REG, RNG_TIMER_EN);
    REG_CLR_BIT(RNG_DATE_REG, RNG_CLK_EN);
}
