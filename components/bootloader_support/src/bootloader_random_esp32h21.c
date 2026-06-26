/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/lpperi_reg.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"


void bootloader_random_enable(void)
{
    // Disable rtc_timer update for rng
    SET_PERI_REG_BITS(LPPERI_RNG_CFG_REG, LPPERI_RTC_TIMER_EN, 0, LPPERI_RTC_TIMER_EN_S);
    // Enable entropy source of ring
    SET_PERI_REG_MASK(LPPERI_RNG_CFG_REG, LPPERI_RNG_SAMPLE_ENABLE);

    //For dieharder test
    ets_printf("H21: Random bytes (%s) follow:\n", "test");
    while (1) {
        uint32_t w = READ_PERI_REG(LPPERI_RNG_DATA_SYNC_REG);
        ets_printf("rng_data:%d\n", w);
        uart_tx_wait_idle(0);

        // uart_tx_one_char(w >> 24);
        // uart_tx_one_char(w >> 16);
        // uart_tx_one_char(w >> 8);
        // uart_tx_one_char(w);
    }
}

void bootloader_random_disable(void)
{
    // Disable rtc_timer update for rng
    SET_PERI_REG_BITS(LPPERI_RNG_CFG_REG, LPPERI_RTC_TIMER_EN, 0, LPPERI_RTC_TIMER_EN_S);
    // Disable entropy source of ring
    CLEAR_PERI_REG_MASK(LPPERI_RNG_CFG_REG, LPPERI_RNG_SAMPLE_ENABLE);
}
