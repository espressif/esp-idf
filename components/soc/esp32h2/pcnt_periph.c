/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/pcnt_periph.h"
#include "soc/gpio_sig_map.h"

const pcnt_signal_conn_t pcnt_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_PCNT_MODULE,
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
                    }
                },
                [1] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN1_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN1_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN1_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN1_IDX
                        }
                    }
                },
                [2] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN2_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN2_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN2_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN2_IDX
                        }
                    }
                },
                [3] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN3_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN3_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN3_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN3_IDX
                        }
                    }
                }
            }
        }
    }
};
