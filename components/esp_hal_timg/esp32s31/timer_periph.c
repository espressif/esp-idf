/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/timer_periph.h"

const soc_timg_gptimer_signal_desc_t soc_timg_gptimer_signals[2][2] = {
    [0] = {
        [0] = {
            .module_name = "TIMG0T0",
            .parent_module = PERIPH_TIMG0_MODULE,
            .irq_id = ETS_TIMERGRP0_T0_INTR_SOURCE,
        },
        [1] = {
            .module_name = "TIMG0T1",
            .parent_module = PERIPH_TIMG0_MODULE,
            .irq_id = ETS_TIMERGRP0_T1_INTR_SOURCE,
        },
    },
    [1] = {
        [0] = {
            .module_name = "TIMG1T0",
            .parent_module = PERIPH_TIMG1_MODULE,
            .irq_id = ETS_TIMERGRP1_T0_INTR_SOURCE,
        },
        [1] = {
            .module_name = "TIMG1T1",
            .parent_module = PERIPH_TIMG1_MODULE,
            .irq_id = ETS_TIMERGRP1_T1_INTR_SOURCE,
        },
    }
};

#if SOC_PAU_SUPPORTED
/*  Registers in retention context:
 *      TIMG_T0CONFIG_REG
 *      TIMG_T0ALARMLO_REG
 *      TIMG_T0ALARMHI_REG
 *      TIMG_T0LOADLO_REG
 *      TIMG_T0LOADHI_REG
 *      TIMG_INT_ENA_TIMERS_REG
 *      TIMG_REGCLK_REG
 */
#define TG0_TIMER0_RETENTION_REGS_BASE   (REG_TIMG_BASE(0))
#define TG1_TIMER0_RETENTION_REGS_BASE   (REG_TIMG_BASE(1))
#define TG_TIMER0_RETENTION_REGS_CNT 7
static const uint32_t tg_timer0_regs_map[4] = {0x100000f1, 0x80000000, 0x0, 0x0};

/*  Registers in retention context:
 *      TIMG_T1CONFIG_REG
 *      TIMG_T1ALARMLO_REG
 *      TIMG_T1ALARMHI_REG
 *      TIMG_T1LOADLO_REG
 *      TIMG_T1LOADHI_REG
 *      TIMG_INT_ENA_TIMERS_REG
 *      TIMG_REGCLK_REG
 */
#define TG0_TIMER1_RETENTION_REGS_BASE   (REG_TIMG_BASE(0) + 0x24)
#define TG1_TIMER1_RETENTION_REGS_BASE   (REG_TIMG_BASE(1) + 0x24)
#define TG_TIMER1_RETENTION_REGS_CNT    7
static const uint32_t tg_timer1_regs_map[4] = {0x800f1, 0x400000, 0x0, 0x0};

const regdma_entries_config_t tg0_timer0_regdma_entries[] = {
    // backup stage: trigger a soft capture
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_TIMER_LINK(0x00),
                                         TIMG_T0UPDATE_REG(0), TIMG_T0_UPDATE, TIMG_T0_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the capture done
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_TG0_TIMER_LINK(0x01),
                                        TIMG_T0UPDATE_REG(0), 0x0, TIMG_T0_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: save the captured counter value
    // restore stage: store the captured counter value to the loader register
    [2] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG0_TIMER_LINK(0x02),
                                              TIMG_T0LO_REG(0), TIMG_T0LOADLO_REG(0), 2, 0, 0),
        .owner = ENTRY(0)
    },
    // restore stage: trigger a soft reload, so the timer can continue from where it was backed up
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_TIMER_LINK(0x03),
                                         TIMG_T0LOAD_REG(0), 0x1, TIMG_T0_LOAD_M, 1, 0),
        .owner = ENTRY(0)
    },
    // backup stage: save other configuration and status registers
    // restore stage: restore the configuration and status registers
    [4] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TG0_TIMER_LINK(0x04),
                                            TG0_TIMER0_RETENTION_REGS_BASE, TG0_TIMER0_RETENTION_REGS_BASE,
                                            TG_TIMER0_RETENTION_REGS_CNT, 0, 0,
                                            tg_timer0_regs_map[0], tg_timer0_regs_map[1],
                                            tg_timer0_regs_map[2], tg_timer0_regs_map[3]),
        .owner = ENTRY(0)
    },
};

const regdma_entries_config_t tg0_timer1_regdma_entries[] = {
    // backup stage: trigger a soft capture
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_TIMER_LINK(0x05),
                                         TIMG_T1UPDATE_REG(0), TIMG_T1_UPDATE, TIMG_T1_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the capture done
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_TG0_TIMER_LINK(0x06),
                                        TIMG_T1UPDATE_REG(0), 0x0, TIMG_T1_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: save the captured counter value
    // restore stage: store the captured counter value to the loader register
    [2] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG0_TIMER_LINK(0x07),
                                              TIMG_T1LO_REG(0), TIMG_T1LOADLO_REG(0), 2, 0, 0),
        .owner = ENTRY(0)
    },
    // restore stage: trigger a soft reload, so the timer can continue from where it was backed up
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_TIMER_LINK(0x08),
                                         TIMG_T1LOAD_REG(0), 0x1, TIMG_T1_LOAD_M, 1, 0),
        .owner = ENTRY(0)
    },
    // backup stage: save other configuration and status registers
    // restore stage: restore the configuration and status registers
    [4] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TG0_TIMER_LINK(0x09),
                                            TG0_TIMER1_RETENTION_REGS_BASE, TG0_TIMER1_RETENTION_REGS_BASE,
                                            TG_TIMER1_RETENTION_REGS_CNT, 0, 0,
                                            tg_timer1_regs_map[0], tg_timer1_regs_map[1],
                                            tg_timer1_regs_map[2], tg_timer1_regs_map[3]),
        .owner = ENTRY(0)
    },
};

const regdma_entries_config_t tg1_timer0_regdma_entries[] = {
    // backup stage: trigger a soft capture
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_TIMER_LINK(0x00),
                                         TIMG_T0UPDATE_REG(1), TIMG_T0_UPDATE, TIMG_T0_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the capture done
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_TG1_TIMER_LINK(0x01),
                                        TIMG_T0UPDATE_REG(1), 0x0, TIMG_T0_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: save the captured counter value
    // restore stage: store the captured counter value to the loader register
    [2] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG1_TIMER_LINK(0x02),
                                              TIMG_T0LO_REG(1), TIMG_T0LOADLO_REG(1), 2, 0, 0),
        .owner = ENTRY(0)
    },
    // restore stage: trigger a soft reload, so the timer can continue from where it was backed up
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_TIMER_LINK(0x03),
                                         TIMG_T0LOAD_REG(1), 0x1, TIMG_T0_LOAD_M, 1, 0),
        .owner = ENTRY(0)
    },
    // backup stage: save other configuration and status registers
    // restore stage: restore the configuration and status registers
    [4] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TG1_TIMER_LINK(0x04),
                                            TG1_TIMER0_RETENTION_REGS_BASE, TG1_TIMER0_RETENTION_REGS_BASE,
                                            TG_TIMER0_RETENTION_REGS_CNT, 0, 0,
                                            tg_timer0_regs_map[0], tg_timer0_regs_map[1],
                                            tg_timer0_regs_map[2], tg_timer0_regs_map[3]),
        .owner = ENTRY(0)
    },
};

const regdma_entries_config_t tg1_timer1_regdma_entries[] = {
    // backup stage: trigger a soft capture
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_TIMER_LINK(0x05),
                                         TIMG_T1UPDATE_REG(1), TIMG_T1_UPDATE, TIMG_T1_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the capture done
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_TG1_TIMER_LINK(0x06),
                                        TIMG_T1UPDATE_REG(1), 0x0, TIMG_T1_UPDATE_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: save the captured counter value
    // restore stage: store the captured counter value to the loader register
    [2] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG1_TIMER_LINK(0x07),
                                              TIMG_T1LO_REG(1), TIMG_T1LOADLO_REG(1), 2, 0, 0),
        .owner = ENTRY(0)
    },
    // restore stage: trigger a soft reload, so the timer can continue from where it was backed up
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_TIMER_LINK(0x08),
                                         TIMG_T1LOAD_REG(1), 0x1, TIMG_T1_LOAD_M, 1, 0),
        .owner = ENTRY(0)
    },
    // backup stage: save other configuration and status registers
    // restore stage: restore the configuration and status registers
    [4] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TG1_TIMER_LINK(0x09),
                                            TG1_TIMER1_RETENTION_REGS_BASE, TG1_TIMER1_RETENTION_REGS_BASE,
                                            TG_TIMER1_RETENTION_REGS_CNT, 0, 0,
                                            tg_timer1_regs_map[0], tg_timer1_regs_map[1],
                                            tg_timer1_regs_map[2], tg_timer1_regs_map[3]),
        .owner = ENTRY(0)
    },
};

const soc_timg_gptimer_retention_desc_t soc_timg_gptimer_retention_infos[2][2] = {
    [0] = {
        [0] = {
            .module = SLEEP_RETENTION_MODULE_TG0_TIMER0,
            .regdma_entry_array = tg0_timer0_regdma_entries,
            .array_size = ARRAY_SIZE(tg0_timer0_regdma_entries)
        },
        [1] = {
            .module = SLEEP_RETENTION_MODULE_TG0_TIMER1,
            .regdma_entry_array = tg0_timer1_regdma_entries,
            .array_size = ARRAY_SIZE(tg0_timer1_regdma_entries)
        },
    },
    [1] = {
        [0] = {
            .module = SLEEP_RETENTION_MODULE_TG1_TIMER0,
            .regdma_entry_array = tg1_timer0_regdma_entries,
            .array_size = ARRAY_SIZE(tg1_timer0_regdma_entries)
        },
        [1] = {
            .module = SLEEP_RETENTION_MODULE_TG1_TIMER1,
            .regdma_entry_array = tg1_timer1_regdma_entries,
            .array_size = ARRAY_SIZE(tg1_timer1_regdma_entries)
        },
    },
};
#endif // SOC_PAU_SUPPORTED
