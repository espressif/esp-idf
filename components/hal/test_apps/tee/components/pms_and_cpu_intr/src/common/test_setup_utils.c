/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "hal/gdma_ll.h"
#include "hal/gdma_types.h"
#if SOC_AHB_GDMA_VERSION == 2
#include "soc/ahb_dma_struct.h"
#elif SOC_AHB_GDMA_VERSION == 1
#include "soc/gdma_struct.h"
#endif

#include "soc/lp_aon_reg.h"
#include "soc/lpperi_reg.h"
#if CONFIG_ULP_COPROC_ENABLED
#include "ulp_lp_core.h"
#endif

#include "esp_attr.h"
#include "esp_rom_lldesc.h"
#include "esp_rom_sys.h"

#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "test_pms_priv.h"
#include "unity.h"
#include "sdkconfig.h"

/***************************** Utility - LP_CPU  *****************************/

#if CONFIG_ULP_COPROC_ENABLED
extern const uint8_t ulp_lp_core_main_bin_start[] asm("_binary_ulp_lp_core_pms_and_cpu_intr_bin_start");
extern const uint8_t ulp_lp_core_main_bin_end[]   asm("_binary_ulp_lp_core_pms_and_cpu_intr_bin_end");

void test_boot_lp_cpu(void)
{
    size_t ulp_lp_core_main_bin_size = (ulp_lp_core_main_bin_end - ulp_lp_core_main_bin_start);
    TEST_ESP_OK(ulp_lp_core_load_binary((void*)ulp_lp_core_main_bin_start, ulp_lp_core_main_bin_size));

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    SEND_MSG(MSG_SLAVE_CLEAR);
    TEST_ESP_OK(ulp_lp_core_run(&cfg));
}

void test_stop_lp_cpu(void)
{
    ulp_lp_core_stop();
}

void test_reset_lp_cpu(void)
{
    REG_SET_BIT(LPPERI_RESET_EN_REG, LPPERI_LP_CPU_RESET_EN);
}

void test_switch_lp_mem_speed(bool high_speed)
{
    if (high_speed) {
        REG_SET_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL);
    } else {
        REG_CLR_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL);
    }
    REG_SET_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL_UPDATE);
}
#endif

/***************************** Utility - GDMA  *****************************/

#if SOC_AHB_GDMA_VERSION == 2
#define TEST_DMA_DEV            (AHB_DMA)
#define DMA_LL_FUNC(func)       ahb_dma_ll_##func
#define DMA_MEM_TRANS_EN_FIELD  mem_trans_en_chn
#elif SOC_AHB_GDMA_VERSION == 1
#define TEST_DMA_DEV            (GDMA)
#define DMA_LL_FUNC(func)       gdma_ll_##func
#define DMA_MEM_TRANS_EN_FIELD  mem_trans_en
#endif

#define dma_ll_force_enable_reg_clock       DMA_LL_FUNC(force_enable_reg_clock)
#define dma_ll_tx_enable_data_burst         DMA_LL_FUNC(tx_enable_data_burst)
#define dma_ll_tx_enable_descriptor_burst   DMA_LL_FUNC(tx_enable_descriptor_burst)
#define dma_ll_rx_enable_data_burst         DMA_LL_FUNC(rx_enable_data_burst)
#define dma_ll_rx_enable_descriptor_burst   DMA_LL_FUNC(rx_enable_descriptor_burst)
#define dma_ll_tx_reset_channel             DMA_LL_FUNC(tx_reset_channel)
#define dma_ll_tx_connect_to_periph         DMA_LL_FUNC(tx_connect_to_periph)
#define dma_ll_rx_reset_channel             DMA_LL_FUNC(rx_reset_channel)
#define dma_ll_rx_connect_to_periph         DMA_LL_FUNC(rx_connect_to_periph)
#define dma_ll_tx_disconnect_all            DMA_LL_FUNC(tx_disconnect_all)
#define dma_ll_rx_disconnect_all            DMA_LL_FUNC(rx_disconnect_all)
#define dma_ll_tx_set_desc_addr             DMA_LL_FUNC(tx_set_desc_addr)
#define dma_ll_tx_start                     DMA_LL_FUNC(tx_start)
#define dma_ll_rx_set_desc_addr             DMA_LL_FUNC(rx_set_desc_addr)
#define dma_ll_rx_start                     DMA_LL_FUNC(rx_start)
#define dma_ll_tx_stop                      DMA_LL_FUNC(tx_stop)
#define dma_ll_rx_stop                      DMA_LL_FUNC(rx_stop)
#define dma_ll_tx_set_priority              DMA_LL_FUNC(tx_set_priority)
#define dma_ll_rx_set_priority              DMA_LL_FUNC(rx_set_priority)
#define dma_ll_tx_enable_interrupt          DMA_LL_FUNC(tx_enable_interrupt)
#define dma_ll_rx_enable_interrupt          DMA_LL_FUNC(rx_enable_interrupt)
#define dma_ll_tx_clear_interrupt_status    DMA_LL_FUNC(tx_clear_interrupt_status)
#define dma_ll_rx_clear_interrupt_status    DMA_LL_FUNC(rx_clear_interrupt_status)
#define dma_ll_tx_get_interrupt_status      DMA_LL_FUNC(tx_get_interrupt_status)
#define dma_ll_rx_get_interrupt_status      DMA_LL_FUNC(rx_get_interrupt_status)

#define TEST_DMA_CHN_NUM        0
#define TEST_DMA_BLK_LEN        1024
#define TEST_DMA_MAX_BLK_NUM    32

DRAM_ATTR static lldesc_t tx_link[TEST_DMA_MAX_BLK_NUM];
DRAM_ATTR static lldesc_t rx_link[TEST_DMA_MAX_BLK_NUM];

static void setup_gdma_link(lldesc_t *link, uint8_t *buf, size_t size, bool is_tx)
{
    int x = 0;
    for (x = 0; x < size / TEST_DMA_BLK_LEN; x++) {
        link[x].size = TEST_DMA_BLK_LEN;
        link[x].length = TEST_DMA_BLK_LEN;
        link[x].buf = buf;
        link[x].eof = 0;
        link[x].owner = 1;
        link[x].empty = (uint32_t)&link[x + 1];
        buf += TEST_DMA_BLK_LEN;
    }

    if (size % TEST_DMA_BLK_LEN) {
        link[x].size = size % TEST_DMA_BLK_LEN;
        link[x].length = size % TEST_DMA_BLK_LEN;
        link[x].buf = buf;
        link[x].eof = 1;
        link[x].owner = 1;
        link[x].empty = 0;
    } else {
        link[x - 1].eof = 1;
        link[x - 1].owner = 1;
        link[x - 1].empty = 0;
    }

    if (is_tx) {
        dma_ll_tx_set_desc_addr(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, (uint32_t)&link[0]);
    } else {
        dma_ll_rx_set_desc_addr(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, (uint32_t)&link[0]);
    }
}

void test_gdma_init(void)
{
    gdma_ll_enable_bus_clock(TEST_DMA_CHN_NUM, true);
    gdma_ll_reset_register(TEST_DMA_CHN_NUM);
    dma_ll_force_enable_reg_clock(&TEST_DMA_DEV, true);

#if SOC_AHB_GDMA_VERSION == 2
    ahb_dma_ll_set_default_memory_range(&TEST_DMA_DEV);
#endif

    dma_ll_tx_reset_channel(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
    dma_ll_rx_reset_channel(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);

    dma_ll_tx_enable_interrupt(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, UINT32_MAX, false);
    dma_ll_tx_clear_interrupt_status(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, UINT32_MAX);

    dma_ll_rx_enable_interrupt(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, UINT32_MAX, false);
    dma_ll_rx_clear_interrupt_status(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, UINT32_MAX);

    dma_ll_tx_enable_data_burst(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, true);
    dma_ll_tx_enable_descriptor_burst(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, true);

    dma_ll_rx_enable_data_burst(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, true);
    dma_ll_rx_enable_descriptor_burst(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, true);

    dma_ll_tx_set_priority(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, 1);
    dma_ll_rx_set_priority(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, 1);

    dma_ll_tx_connect_to_periph(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, SOC_GDMA_TRIG_PERIPH_SPI2);
    dma_ll_rx_connect_to_periph(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, SOC_GDMA_TRIG_PERIPH_SPI2);
    TEST_DMA_DEV.channel[TEST_DMA_CHN_NUM].in.in_conf0.DMA_MEM_TRANS_EN_FIELD = 1;

    dma_ll_tx_enable_interrupt(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, GDMA_LL_EVENT_TX_EOF, true);
    dma_ll_rx_enable_interrupt(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, GDMA_LL_EVENT_RX_SUC_EOF, true);
}

void test_gdma_deinit(void)
{
    dma_ll_tx_stop(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
    dma_ll_rx_stop(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);

    dma_ll_tx_disconnect_all(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
    dma_ll_rx_disconnect_all(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);

    dma_ll_tx_reset_channel(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
    dma_ll_rx_reset_channel(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);

    gdma_ll_reset_register(TEST_DMA_CHN_NUM);
    gdma_ll_enable_bus_clock(TEST_DMA_CHN_NUM, false);
}

void test_gdma_m2m_transfer(uint8_t *src, uint8_t *dest, size_t size)
{
    setup_gdma_link(tx_link, src, size, true);
    setup_gdma_link(rx_link, dest, size, false);

    dma_ll_rx_start(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
    dma_ll_tx_start(&TEST_DMA_DEV, TEST_DMA_CHN_NUM);
}

void test_gdma_wait_done(void)
{
    while (1) {
        uint32_t in_st = dma_ll_rx_get_interrupt_status(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, false);
        dma_ll_rx_clear_interrupt_status(&TEST_DMA_DEV, TEST_DMA_CHN_NUM, in_st);
        if (in_st & GDMA_LL_EVENT_RX_SUC_EOF) {
            break;
        }
    }
}

/***************************** Miscellaneous *****************************/

IRAM_ATTR void test_delay_ms(uint32_t ms)
{
    uint32_t us = ms * 1000U;
#if SOC_CPU_HAS_CSR_PC
    esp_rom_delay_us(us);
#else
    if (RV_READ_CSR(CSR_PRV_MODE) == PRV_M) {
        esp_rom_delay_us(us);
    } else {
        uint32_t start = RV_READ_CSR(cycle);
        uint32_t end = us * esp_rom_get_cpu_ticks_per_us();

        while ((RV_READ_CSR(cycle) - start) < end) {
            /* nothing to do */
        }
    }
#endif
}
