/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/ledc_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every LEDC peripheral: GPIO signals
*/
const ledc_signal_conn_t ledc_periph_signal[1] = {
    {
        .sig_out0_idx = LEDC_LS_SIG_OUT0_IDX,
    }
};

/**
 * LEDC registers to be saved for sleep retention
 *
 * channel:
 * LEDC_CHx_CONF0_REG, LEDC_CHx_HPOINT_REG, LEDC_CHx_DUTY_R_REG -> LEDC_CHx_DUTY_REG,
 * LEDC_CHx_GAMMA_CONF_REG, LEDC_CHx_GAMMA_RANGEi_REG
 *
 * timer:
 * LEDC_TIMERn_CONF_REG, LEDC_TIMERn_CMP_REG,
 *
 * common:
 * LEDC_INT_ENA_REG,
 * LEDC_EVT_TASK_EN0_REG, LEDC_EVT_TASK_EN1_REG, LEDC_EVT_TASK_EN2_REG,
 * LEDC_CONF_REG,
 *
 * Note 1: Gamma parameter registers are backuped and restored. But we won't start a fade automatically after wake-up.
 *         Instead, we will only start a PWM with a fixed duty cycle, the same value as before entering the sleep.
 *
 * Note 2: For timer/channel registers to get synced, update bits need to be set
 *
 * Note 3: Gamma RAM registers R/W relies both APB and function clock, therefore, retention requires the existence of function clock
 */
#define LEDC_COMMON_RETENTION_REGS_CNT  5
#define LEDC_COMMON_RETENTION_REGS_BASE (DR_REG_LEDC_BASE + 0xc8)
static const uint32_t ledc_common_regs_map[4] = {0x1c00001, 0x400, 0x0, 0x0};
static const regdma_entries_config_t ledc_common_regdma_entries[] = {
    // If a fade is in process, the DUTY_CHNG_END_CHx intr bit is enabled, however, we don't want it to be restored after wake-up (no fade after wake-up).
    // Therefore, we can set it to 0 before backup the LEDC_INT_ENA_REG.
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_LEDC_LINK(0x00),
			 							    LEDC_INT_ENA_REG, 0,
			 							    (LEDC_DUTY_CHNG_END_CH0_INT_ENA_M | LEDC_DUTY_CHNG_END_CH1_INT_ENA_M | LEDC_DUTY_CHNG_END_CH2_INT_ENA_M | LEDC_DUTY_CHNG_END_CH3_INT_ENA_M | LEDC_DUTY_CHNG_END_CH4_INT_ENA_M | LEDC_DUTY_CHNG_END_CH5_INT_ENA_M), 0, 1),
		    .owner = LEDC_RETENTION_ENTRY },
    [1] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_LEDC_LINK(0x01),
                                               LEDC_COMMON_RETENTION_REGS_BASE, LEDC_COMMON_RETENTION_REGS_BASE,
                                               LEDC_COMMON_RETENTION_REGS_CNT, 0, 0,
                                               ledc_common_regs_map[0], ledc_common_regs_map[1],
                                               ledc_common_regs_map[2], ledc_common_regs_map[3]),
            .owner = LEDC_RETENTION_ENTRY },
};

#define LEDC_TIMER_RETENTION_ENTRIES(timer) { \
	[0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_LEDC_LINK(0x00), \
			 								     LEDC_TIMER##timer##_CONF_REG, LEDC_TIMER##timer##_CONF_REG, \
			 								     1, 0, 0), \
		    .owner = LEDC_RETENTION_ENTRY }, \
	[1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_LEDC_LINK(0x01), \
			 								     LEDC_TIMER##timer##_CMP_REG, LEDC_TIMER##timer##_CMP_REG, \
			 								     1, 0, 0), \
		    .owner = LEDC_RETENTION_ENTRY }, \
	[2] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_LEDC_LINK(0x02), \
			 							    LEDC_TIMER##timer##_CONF_REG, LEDC_TIMER##timer##_PARA_UP, \
			 							    LEDC_TIMER##timer##_PARA_UP_M, 1, 0), \
		    .owner = LEDC_RETENTION_ENTRY }, \
}

#define LEDC_CHANNEL_RETENTION_REGS_CNT 2
static const uint32_t ledc_channel_regs_map[4] = {0x3, 0x0, 0x0, 0x0};
static const uint32_t ledc_channel_gamma_regs_map[4] = {0xffff, 0x0, 0x0, 0x0};
#define LEDC_CHANNEL_RETENTION_ENTRIES(chan) { \
	[0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_LEDC_LINK(0x00), \
                                               LEDC_CH##chan##_CONF0_REG, LEDC_CH##chan##_CONF0_REG, \
                                               LEDC_CHANNEL_RETENTION_REGS_CNT, 0, 0, \
                                               ledc_channel_regs_map[0], ledc_channel_regs_map[1], \
                                               ledc_channel_regs_map[2], ledc_channel_regs_map[3]), \
            .owner = LEDC_RETENTION_ENTRY }, \
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_LEDC_LINK(0x01), \
                                                 LEDC_CH##chan##_DUTY_R_REG, LEDC_CH##chan##_DUTY_REG, \
                                                 1, 0, 0), \
            .owner = LEDC_RETENTION_ENTRY }, \
	[2] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_LEDC_LINK(0x02), \
			 							    LEDC_CH##chan##_CONF1_REG, LEDC_DUTY_START_CH##chan, \
			 							    LEDC_DUTY_START_CH##chan##_M, 1, 0), \
		    .owner = LEDC_RETENTION_ENTRY }, \
	[3] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_LEDC_LINK(0x03), \
			 							    LEDC_CH##chan##_CONF0_REG, LEDC_PARA_UP_CH##chan, \
			 							    LEDC_PARA_UP_CH##chan##_M, 1, 0), \
		    .owner = LEDC_RETENTION_ENTRY }, \
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_LEDC_LINK(0x04), \
                                                 LEDC_CH##chan##_GAMMA_CONF_REG, LEDC_CH##chan##_GAMMA_CONF_REG, \
                                                 1, 0, 0), \
            .owner = LEDC_RETENTION_ENTRY }, \
    [5] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_LEDC_LINK(0x05), \
                                               LEDC_CH##chan##_GAMMA_RANGE0_REG, LEDC_CH##chan##_GAMMA_RANGE0_REG, \
                                               SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, 0, 0, \
                                               ledc_channel_gamma_regs_map[0], ledc_channel_gamma_regs_map[1], \
                                               ledc_channel_gamma_regs_map[2], ledc_channel_gamma_regs_map[3]), \
            .owner = LEDC_RETENTION_ENTRY }, \
}

static const regdma_entries_config_t ledc_timer0_regdma_entries[] = LEDC_TIMER_RETENTION_ENTRIES(0);
static const regdma_entries_config_t ledc_timer1_regdma_entries[] = LEDC_TIMER_RETENTION_ENTRIES(1);
static const regdma_entries_config_t ledc_timer2_regdma_entries[] = LEDC_TIMER_RETENTION_ENTRIES(2);
static const regdma_entries_config_t ledc_timer3_regdma_entries[] = LEDC_TIMER_RETENTION_ENTRIES(3);

static const regdma_entries_config_t ledc_channel0_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(0);
static const regdma_entries_config_t ledc_channel1_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(1);
static const regdma_entries_config_t ledc_channel2_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(2);
static const regdma_entries_config_t ledc_channel3_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(3);
static const regdma_entries_config_t ledc_channel4_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(4);
static const regdma_entries_config_t ledc_channel5_regdma_entries[] = LEDC_CHANNEL_RETENTION_ENTRIES(5);

const ledc_reg_retention_info_t ledc_reg_retention_info = {
    .common = {
		.regdma_entry_array = ledc_common_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_common_regdma_entries),
	},
	.timer[0] = {
		.regdma_entry_array = ledc_timer0_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_timer0_regdma_entries),
	},
	.timer[1] = {
		.regdma_entry_array = ledc_timer1_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_timer1_regdma_entries),
	},
	.timer[2] = {
		.regdma_entry_array = ledc_timer2_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_timer2_regdma_entries),
	},
	.timer[3] = {
		.regdma_entry_array = ledc_timer3_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_timer3_regdma_entries),
	},
	.channel[0] = {
		.regdma_entry_array = ledc_channel0_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel0_regdma_entries),
	},
	.channel[1] = {
		.regdma_entry_array = ledc_channel1_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel1_regdma_entries),
	},
	.channel[2] = {
		.regdma_entry_array = ledc_channel2_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel2_regdma_entries),
	},
	.channel[3] = {
		.regdma_entry_array = ledc_channel3_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel3_regdma_entries),
	},
	.channel[4] = {
		.regdma_entry_array = ledc_channel4_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel4_regdma_entries),
	},
	.channel[5] = {
		.regdma_entry_array = ledc_channel5_regdma_entries,
		.array_size = ARRAY_SIZE(ledc_channel5_regdma_entries),
	},
	.module_id = SLEEP_RETENTION_MODULE_LEDC,
};
