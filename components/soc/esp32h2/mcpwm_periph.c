/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/mcpwm_periph.h"
#include "soc/gpio_sig_map.h"

const mcpwm_signal_conn_t mcpwm_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_MCPWM0_MODULE,
            .irq_id = ETS_MCPWM0_INTR_SOURCE,
            .operators = {
                [0] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_OUT0A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_OUT0B_IDX
                        }
                    }
                },
                [1] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_OUT1A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_OUT1B_IDX
                        }
                    }
                },
                [2] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM0_OUT2A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM0_OUT2B_IDX
                        }
                    }
                }
            },
            .gpio_faults = {
                [0] = {
                    .fault_sig = PWM0_F0_IN_IDX
                },
                [1] = {
                    .fault_sig = PWM0_F1_IN_IDX
                },
                [2] = {
                    .fault_sig = PWM0_F2_IN_IDX
                }
            },
            .captures = {
                [0] = {
                    .cap_sig = PWM0_CAP0_IN_IDX
                },
                [1] = {
                    .cap_sig = PWM0_CAP1_IN_IDX
                },
                [2] = {
                    .cap_sig = PWM0_CAP2_IN_IDX
                }
            },
            .gpio_synchros = {
                [0] = {
                    .sync_sig = PWM0_SYNC0_IN_IDX
                },
                [1] = {
                    .sync_sig = PWM0_SYNC1_IN_IDX
                },
                [2] = {
                    .sync_sig = PWM0_SYNC2_IN_IDX
                }
            }
        },
    }
};
