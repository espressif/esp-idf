/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/etm_periph.h"
#include "soc/soc_etm_reg.h"

/**
 * ETM Registers to be saved during sleep retention
 * - Channel configuration registers, e.g.: SOC_ETM_CH0_EVT_ID_REG, SOC_ETM_CH0_TASK_ID_REG
*/
#define ETM_RETENTION_REGS_CNT ((SOC_ETM_CH49_TASK_ID_REG - SOC_ETM_CH0_EVT_ID_REG) / 4 + 1)

static const regdma_entries_config_t etm_regdma_entries[] = {
    // backup stage: save the status of enabled channels
    // restore stage: store the enabled channels
    [0] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_ETM_LINK(0x00),
        SOC_ETM_CH_ENA_AD0_REG, SOC_ETM_CH_ENA_AD0_SET_REG, 1, 0, 0),
        .owner = ENTRY(0) | ENTRY(2),
    },
    [1] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_ETM_LINK(0x01),
        SOC_ETM_CH_ENA_AD1_REG, SOC_ETM_CH_ENA_AD1_SET_REG, 1, 0, 0),
        .owner = ENTRY(0) | ENTRY(2),
    },
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [2] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_ETM_LINK(0x02),
        SOC_ETM_CH0_EVT_ID_REG, SOC_ETM_CH0_EVT_ID_REG, ETM_RETENTION_REGS_CNT, 0, 0),
        .owner = ENTRY(0) | ENTRY(2),
    },
};

const etm_reg_retention_info_t etm_reg_retention_info[SOC_ETM_GROUPS] = {
    [0] = {
        .module = SLEEP_RETENTION_MODULE_ETM0,
        .regdma_entry_array = etm_regdma_entries,
        .array_size = ARRAY_SIZE(etm_regdma_entries)
    },
};
