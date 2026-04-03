/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/trng_reg.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"


void bootloader_random_enable(void)
{
    // Enable entropy source of ring
    SET_PERI_REG_MASK(TRNG_CONF_REG, TRNG_SAMPLE_ENABLE);

    // Enable entropy source of crc
    SET_PERI_REG_MASK(TRNG_CONF_REG, TRNG_NOISE_CRC_EN);

    // //For dieharder test
    // ets_printf("S31: Random bytes (%s) follow:\n","test");
    // while (1)
    // {
    //     uint32_t w = READ_PERI_REG(TRNG_CRC_SYNC_DATA_REG);
    //     // ets_printf("rng_data:%d\n", w);
    //     // uart_tx_wait_idle(0);

    //     uart_tx_one_char(w >> 24);
    //     uart_tx_one_char(w >> 16);
    //     uart_tx_one_char(w >> 8);
    //     uart_tx_one_char(w);
    // }
}

void bootloader_random_disable(void)
{
    CLEAR_PERI_REG_MASK(TRNG_CONF_REG, TRNG_SAMPLE_ENABLE);
    CLEAR_PERI_REG_MASK(TRNG_CONF_REG, TRNG_NOISE_CRC_EN);
}
