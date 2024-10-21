/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/twai_periph.h"
#include "soc/twai_reg.h"
#include "soc/gpio_sig_map.h"

const twai_controller_signal_conn_t twai_controller_periph_signals = {
    .controllers = {
        [0] = {
            .module = PERIPH_TWAI0_MODULE,
            .irq_id = ETS_TWAI0_INTR_SOURCE,
            .tx_sig = TWAI0_TX_IDX,
            .rx_sig = TWAI0_RX_IDX,
            .bus_off_sig = TWAI0_BUS_OFF_ON_IDX,
            .clk_out_sig = TWAI0_CLKOUT_IDX,
            .stand_by_sig = TWAI0_STANDBY_IDX,
        },
        [1] = {
            .module = PERIPH_TWAI1_MODULE,
            .irq_id = ETS_TWAI1_INTR_SOURCE,
            .tx_sig = TWAI1_TX_IDX,
            .rx_sig = TWAI1_RX_IDX,
            .bus_off_sig = TWAI1_BUS_OFF_ON_IDX,
            .clk_out_sig = TWAI1_CLKOUT_IDX,
            .stand_by_sig = TWAI1_STANDBY_IDX,
        }
    }
};

/**
 * TWAI Registers to be saved during sleep retention
 * - TWAI_MODE_REG
 * - TWAI_INTERRUPT_ENABLE_REG
 * - TWAI_BUS_TIMING_0_REG / TWAI_BUS_TIMING_1_REG
 * - TWAI_ERR_WARNING_LIMIT_REG
 * - TWAI_RX_ERR_CNT_REG
 * - TWAI_TX_ERR_CNT_REG
 * - TWAI_DATA_x_REG
 * - TWAI_CLOCK_DIVIDER_REG
 * - TWAI_SW_STANDBY_CFG_REG
 * - TWAI_HW_CFG_REG
 * - TWAI_HW_STANDBY_CNT_REG
 * - TWAI_IDLE_INTR_CNT_REG
 * - TWAI_ECO_CFG_REG
 * - TWAI_TIMESTAMP_PRESCALER_REG
 * - TWAI_TIMESTAMP_CFG_REG
*/
#define TWAI_RETENTION_REGS_CNT 28
#define TWAI_RETENTION_REGS_BASE(i) REG_TWAI_BASE(i)
static const uint32_t twai_regs_map[4] = {0x9fffe0d1, 0xdf, 0x0, 0x0};
#define TWAI_SLEEP_RETENTION_ENTRIES(id) { \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TWAI_LINK(0x00), \
                                               TWAI_RETENTION_REGS_BASE(id), TWAI_RETENTION_REGS_BASE(id), \
                                               TWAI_RETENTION_REGS_CNT, 0, 0, \
                                               twai_regs_map[0], twai_regs_map[1], \
                                               twai_regs_map[2], twai_regs_map[3]), \
                                               .owner = ENTRY(0) | ENTRY(2) }, \
}

static const regdma_entries_config_t twai0_regs_retention[] = TWAI_SLEEP_RETENTION_ENTRIES(0);
static const regdma_entries_config_t twai1_regs_retention[] = TWAI_SLEEP_RETENTION_ENTRIES(1);

const twai_reg_retention_info_t twai_reg_retention_info[SOC_TWAI_CONTROLLER_NUM] = {
    [0] = {
        .module_id = SLEEP_RETENTION_MODULE_TWAI0,
        .entry_array = twai0_regs_retention,
        .array_size = ARRAY_SIZE(twai0_regs_retention)
    },
    [1] = {
        .module_id = SLEEP_RETENTION_MODULE_TWAI1,
        .entry_array = twai1_regs_retention,
        .array_size = ARRAY_SIZE(twai1_regs_retention)
    },
};
