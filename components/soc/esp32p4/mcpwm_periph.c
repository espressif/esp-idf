/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/mcpwm_periph.h"
#include "soc/mcpwm_reg.h"
#include "soc/gpio_sig_map.h"

const mcpwm_signal_conn_t mcpwm_periph_signals = {
    .groups = {
        [0] = {
            .module_name = "MCPWM0",
            .irq_id = ETS_PWM0_INTR_SOURCE,
            .operators = {
                [0] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_CH0_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_CH0_B_PAD_OUT_IDX
                        }
                    }
                },
                [1] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_CH1_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_CH1_B_PAD_OUT_IDX
                        }
                    }
                },
                [2] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_CH2_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_CH2_B_PAD_OUT_IDX
                        }
                    }
                }
            },
            .gpio_faults = {
                [0] = {
                    .fault_sig = PWM0_F0_PAD_IN_IDX
                },
                [1] = {
                    .fault_sig = PWM0_F1_PAD_IN_IDX
                },
                [2] = {
                    .fault_sig = PWM0_F2_PAD_IN_IDX
                }
            },
            .captures = {
                [0] = {
                    .cap_sig = PWM0_CAP0_PAD_IN_IDX
                },
                [1] = {
                    .cap_sig = PWM0_CAP1_PAD_IN_IDX
                },
                [2] = {
                    .cap_sig = PWM0_CAP2_PAD_IN_IDX
                }
            },
            .gpio_synchros = {
                [0] = {
                    .sync_sig = PWM0_SYNC0_PAD_IN_IDX
                },
                [1] = {
                    .sync_sig = PWM0_SYNC1_PAD_IN_IDX
                },
                [2] = {
                    .sync_sig = PWM0_SYNC2_PAD_IN_IDX
                }
            }
        },
        [1] = {
            .module_name = "MCPWM1",
            .irq_id = ETS_PWM1_INTR_SOURCE,
            .operators = {
                [0] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_CH0_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_CH0_B_PAD_OUT_IDX
                        }
                    }
                },
                [1] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_CH1_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_CH1_B_PAD_OUT_IDX
                        }
                    }
                },
                [2] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_CH2_A_PAD_OUT_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_CH2_B_PAD_OUT_IDX
                        }
                    }
                }
            },
            .gpio_faults = {
                [0] = {
                    .fault_sig = PWM1_F0_PAD_IN_IDX
                },
                [1] = {
                    .fault_sig = PWM1_F1_PAD_IN_IDX
                },
                [2] = {
                    .fault_sig = PWM1_F2_PAD_IN_IDX
                }
            },
            .captures = {
                [0] = {
                    .cap_sig = PWM1_CAP0_PAD_IN_IDX
                },
                [1] = {
                    .cap_sig = PWM1_CAP1_PAD_IN_IDX
                },
                [2] = {
                    .cap_sig = PWM1_CAP2_PAD_IN_IDX
                }
            },
            .gpio_synchros = {
                [0] = {
                    .sync_sig = PWM1_SYNC0_PAD_IN_IDX
                },
                [1] = {
                    .sync_sig = PWM1_SYNC1_PAD_IN_IDX
                },
                [2] = {
                    .sync_sig = PWM1_SYNC2_PAD_IN_IDX
                }
            }
        }
    }
};

#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
/**
 * MCPWM Registers to be saved during sleep retention
 * - Clk Configuration registers, e.g.: MCPWM_CLK_CFG_REG
 * - Timer Configuration registers, e.g.: MCPWM_TIMER_SYNCI_CFG_REG, MCPWM_TIMER0_CFG0_REG, MCPWM_TIMER0_CFG1_REG, MCPWM_TIMER0_CFG1_REG
 * - Operator Configuration registers, e.g.: MCPWM_OPERATOR_TIMERSEL_REG
 *   |- Generator Configuration registers, e.g.: MCPWM_GEN0_STMP_CFG_REG, MCPWM_GEN0_TSTMP_A_REG, MCPWM_GEN0_TSTMP_B_REG, MCPWM_GEN0_CFG0_REG, MCPWM_GEN0_FORCE_REG, MCPWM_GEN0_A_REG, MCPWM_GEN0_B_REG
 *   |- Dead Time Configuration registers, e.g.: MCPWM_DT0_CFG_REG, MCPWM_DT0_FED_CFG_REG, MCPWM_DT0_RED_CFG_REG
 *   |- Carrier Configuration registers, e.g.: MCPWM_CARRIER0_CFG_REG
 *   └- Fault Handle Configuration registers, e.g.: MCPWM_FAULT_DETECT_REG, MCPWM_FH0_CFG0_REG, MCPWM_FH0_CFG1_REG
 * - Capture Timer Configuration registers, e.g.: MCPWM_CAP_TIMER_CFG_REG, MCPWM_CAP_TIMER_PHASE_REG, MCPWM_CAP_CH0_CFG_REG, MCPWM_CAP_CH1_CFG_REG, MCPWM_CAP_CH2_CFG_REG
 * - Interrupt enable registers, e.g.: MCPWM_INT_ENA_REG
 * - ETM Configurations, e.g.: MCPWM_EVT_EN_REG, MCPWM_TASK_EN_REG
 * - Misc Configurations, e.g.: MCPWM_UPDATE_CFG_REG
*/
#define MCPWM_RETENTION_REGS_CNT 68
#define MCPWM_RETENTION_REGS_BASE(i) REG_MCPWM_BASE(i)
static const uint32_t mcpwm_regs_map[4] = {0xefffeeef, 0x7efffbff, 0x1ff18, 0x0};
#define MCPWM_SLEEP_RETENTION_ENTRIES(mcpwm_port) {  \
    /* backup stage: save configuration registers \
       restore stage: restore the configuration registers */ \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(  \
                REGDMA_MCPWM_LINK(0x00), \
                MCPWM_RETENTION_REGS_BASE(mcpwm_port),  \
                MCPWM_RETENTION_REGS_BASE(mcpwm_port), \
                MCPWM_RETENTION_REGS_CNT, 0, 0, \
                mcpwm_regs_map[0], mcpwm_regs_map[1], \
                mcpwm_regs_map[2], mcpwm_regs_map[3]), \
                .owner = ENTRY(0)}, \
    /* restore stage: trigger a forced update of all active registers*/ \
    [1] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_MCPWM_LINK(0x01),  \
                                                MCPWM_UPDATE_CFG_REG(mcpwm_port), MCPWM_GLOBAL_FORCE_UP, MCPWM_GLOBAL_FORCE_UP_M, 1, 0),  \
                                                .owner = ENTRY(0) }, \
    [2] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_MCPWM_LINK(0x02),  \
                                                MCPWM_UPDATE_CFG_REG(mcpwm_port), 0, MCPWM_GLOBAL_FORCE_UP_M, 1, 0),  \
                                                .owner = ENTRY(0) },  \
};

static const regdma_entries_config_t mcpwm0_regs_retention[] = MCPWM_SLEEP_RETENTION_ENTRIES(0);
static const regdma_entries_config_t mcpwm1_regs_retention[] = MCPWM_SLEEP_RETENTION_ENTRIES(1);

const mcpwm_reg_retention_info_t mcpwm_reg_retention_info[SOC_MCPWM_GROUPS] = {
    [0] = {
        .regdma_entry_array = mcpwm0_regs_retention,
        .array_size = ARRAY_SIZE(mcpwm0_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_MCPWM0
    },
    [1] = {
        .regdma_entry_array = mcpwm1_regs_retention,
        .array_size = ARRAY_SIZE(mcpwm1_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_MCPWM1
    },
};
#endif //SOC_MCPWM_SUPPORT_SLEEP_RETENTION
