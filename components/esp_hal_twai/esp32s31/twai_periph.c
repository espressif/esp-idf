/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/twai_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"
#include "soc/twaifd_reg.h"

const twai_signal_conn_t twai_periph_signals[2] = {
    [0] = {
        .module_name = "TWAI0",
        .irq_id = ETS_TWAI0_INTR_SOURCE,
        .timer_irq_id = ETS_TWAI0_TIMER_INTR_SOURCE,
        .tx_sig = TWAI0_TX_IDX,
        .rx_sig = TWAI0_RX_IDX,
        .bus_off_sig = -1,
        .clk_out_sig = -1,
        .stand_by_sig = -1,
    },
    [1] = {
        .module_name = "TWAI1",
        .irq_id = ETS_TWAI1_INTR_SOURCE,
        .timer_irq_id = ETS_TWAI1_TIMER_INTR_SOURCE,
        .tx_sig = TWAI1_TX_IDX,
        .rx_sig = TWAI1_RX_IDX,
        .bus_off_sig = -1,
        .clk_out_sig = -1,
        .stand_by_sig = -1,
    },
};

/**
 * TWAI Registers to be saved during sleep retention
 * - TWAIFD_MODE_SETTINGS_REG
 * - TWAIFD_STATUS_REG
 * - TWAIFD_COMMAND_REG
 * - TWAIFD_INT_ENA_SET_REG
 * - TWAIFD_INT_ENA_CLR_REG
 * - TWAIFD_BTR_REG
 * - TWAIFD_BTR_FD_REG
 * - TWAIFD_EWL_ERP_FAULT_STATE_REG
 * - TWAIFD_REC_TEC_REG
 * - TWAIFD_ERR_NORM_ERR_FD_REG
 * - TWAIFD_FILTER_A_MASK_REG
 * - TWAIFD_FILTER_A_VAL_REG
 * - TWAIFD_FILTER_B_MASK_REG
 * - TWAIFD_FILTER_B_VAL_REG
 * - TWAIFD_FILTER_C_MASK_REG
 * - TWAIFD_FILTER_C_VAL_REG
 * - TWAIFD_FILTER_RAN_LOW_REG
 * - TWAIFD_FILTER_RAN_HIGH_REG
 * - TWAIFD_FILTER_CONTROL_FILTER_STATUS_REG
 * - TWAIFD_RX_STATUS_RX_SETTINGS_REG
 * - TWAIFD_TX_COMMAND_TXTB_INFO_REG
 * - TWAIFD_TX_PRIORITY_REG
 * - TWAIFD_TRV_DELAY_SSP_CFG_REG
 * - TWAIFD_RX_FR_CTR_REG
 * - TWAIFD_TX_FR_CTR_REG
 */
#define TWAI_RETENTION_REGS_CNT 25
#define TWAI_RETENTION_REGS_BASE(i) REG_TWAIFD_BASE(i)
static const uint32_t twai_regs_map[4] = {0xb27fdf37, 0x3, 0x0, 0x0};

// twai timer registers is far away from twai registers, so we need a separate link
// - TWAIFD_TIMER_INT_ENA_REG
// - TWAIFD_TIMER_CFG_REG
#define TWAI_TIMER_RETENTION_REGS_CNT 2
#define TWAI_TIMER_RETENTION_REGS_BASE(i) (REG_TWAIFD_BASE(i) + 0xfe0)
static const uint32_t twai_timer_regs_map[4] = {0x5, 0x0, 0x0, 0x0};

#define TWAI_SLEEP_RETENTION_ENTRIES(id) { \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT( \
                        REGDMA_TWAI_LINK(0x00), \
                        TWAI_RETENTION_REGS_BASE(id), TWAI_RETENTION_REGS_BASE(id), \
                        TWAI_RETENTION_REGS_CNT, 0, 0, \
                        twai_regs_map[0], twai_regs_map[1], \
                        twai_regs_map[2], twai_regs_map[3]), \
            .owner = ENTRY(0) | ENTRY(2) }, \
    [1] = { .config = REGDMA_LINK_ADDR_MAP_INIT( \
                        REGDMA_TWAI_LINK(0x01), \
                        TWAI_TIMER_RETENTION_REGS_BASE(id), TWAI_TIMER_RETENTION_REGS_BASE(id), \
                        TWAI_TIMER_RETENTION_REGS_CNT, 0, 0, \
                        twai_timer_regs_map[0], twai_timer_regs_map[1], \
                        twai_timer_regs_map[2], twai_timer_regs_map[3]), \
    .owner = ENTRY(0) | ENTRY(2) }, \
}

static const regdma_entries_config_t twai0_regs_retention[] = TWAI_SLEEP_RETENTION_ENTRIES(0);
static const regdma_entries_config_t twai1_regs_retention[] = TWAI_SLEEP_RETENTION_ENTRIES(1);

const twai_reg_retention_info_t twai_reg_retention_info[2] = {
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
