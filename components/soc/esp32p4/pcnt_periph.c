/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/pcnt_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/pcnt_reg.h"

const soc_pcnt_signal_desc_t soc_pcnt_signals[1] = {
    [0] = {
        .irq_id = ETS_PCNT_INTR_SOURCE,
        .module_name = "pcnt0",
        .units = {
            [0] = {
                .channels = {
                    [0] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH0_PAD_IN0_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH0_PAD_IN0_IDX
                    },
                    [1] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH1_PAD_IN0_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH1_PAD_IN0_IDX
                    }
                },
                .clear_sig_id_matrix = PCNT_RST_PAD_IN0_IDX
            },
            [1] = {
                .channels = {
                    [0] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH0_PAD_IN1_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH0_PAD_IN1_IDX,
                    },
                    [1] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH1_PAD_IN1_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH1_PAD_IN1_IDX
                    }
                },
                .clear_sig_id_matrix = PCNT_RST_PAD_IN1_IDX
            },
            [2] = {
                .channels = {
                    [0] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH0_PAD_IN2_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH0_PAD_IN2_IDX,
                    },
                    [1] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH1_PAD_IN2_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH1_PAD_IN2_IDX
                    }
                },
                .clear_sig_id_matrix = PCNT_RST_PAD_IN2_IDX
            },
            [3] = {
                .channels = {
                    [0] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH0_PAD_IN3_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH0_PAD_IN3_IDX,
                    },
                    [1] = {
                        .ctl_sig_id_matrix = PCNT_CTRL_CH1_PAD_IN3_IDX,
                        .pulse_sig_id_matrix = PCNT_SIG_CH1_PAD_IN3_IDX
                    }
                },
                .clear_sig_id_matrix = PCNT_RST_PAD_IN3_IDX
            }
        }
    }
};
