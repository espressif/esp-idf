/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/pcnt_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/pcnt_reg.h"

const pcnt_signal_conn_t pcnt_periph_signals = {
    .groups = {
        [0] = {
            .irq = ETS_PCNT_INTR_SOURCE,
            .units = {
                [0] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN0_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN0_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN0_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN0_IDX
                        }
                    },
                    .clear_sig = PCNT_RST_IN0_IDX
                },
                [1] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN1_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN1_IDX,
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN1_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN1_IDX
                        }
                    },
                    .clear_sig = PCNT_RST_IN1_IDX
                },
                [2] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN2_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN2_IDX,
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN2_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN2_IDX
                        }
                    },
                    .clear_sig = PCNT_RST_IN2_IDX
                },
                [3] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN3_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN3_IDX,
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN3_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN3_IDX
                        }
                    },
                    .clear_sig = PCNT_RST_IN3_IDX
                }
            }
        }
    }
};

/**
 * PCNT Registers to be saved during sleep retention
 * - Configuration registers, e.g.: PCNT_CTRL_REG, PCNT_U0_CONF0_REG, PCNT_U0_CONF1_REG, PCNT_U0_CONF2_REG, PCNT_U1_CONF0_REG...
 * - Step Configuration registers, e.g.: PCNT_U0_CHANGE_CONF_REG, PCNT_U1_CHANGE_CONF_REG, PCNT_U2_CHANGE_CONF_REG, PCNT_U3_CHANGE_CONF_REG
 * - Interrupt enable registers, e.g.: PCNT_INT_ENA_REG
*/
#define PCNT_RETENTION_REGS_CNT 18
#define PCNT_RETENTION_REGS_BASE (DR_REG_PCNT_BASE + 0x0)
static const uint32_t pcnt_regs_map[4] = {0x1f040fff, 0x0, 0x0, 0x0};
static const regdma_entries_config_t pcnt_regs_retention[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_PCNT_LINK(0x00), \
                                               PCNT_RETENTION_REGS_BASE, PCNT_RETENTION_REGS_BASE, \
                                               PCNT_RETENTION_REGS_CNT, 0, 0, \
                                               pcnt_regs_map[0], pcnt_regs_map[1], \
                                               pcnt_regs_map[2], pcnt_regs_map[3]), \
                                               .owner = ENTRY(0) | ENTRY(2) }, \
};

const pcnt_reg_retention_info_t pcnt_reg_retention_info[SOC_PCNT_GROUPS] = {
    [0] = {
        .regdma_entry_array = pcnt_regs_retention,
        .array_size = ARRAY_SIZE(pcnt_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_PCNT0
    },
};
