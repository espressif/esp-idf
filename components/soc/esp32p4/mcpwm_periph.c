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
            .irq_id = ETS_PWM0_INTR_SOURCE,
            .operators = {
                [0] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = 0
                        },
                        [1] = {
                            .pwm_sig = 0
                        }
                    }
                },
                [1] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = 0
                        },
                        [1] = {
                            .pwm_sig = 0
                        }
                    }
                },
                [2] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = 0
                        },
                        [1] = {
                            .pwm_sig = 0
                        }
                    }
                }
            },
            .gpio_faults = {
                [0] = {
                    .fault_sig = 0
                },
                [1] = {
                    .fault_sig = 0
                },
                [2] = {
                    .fault_sig = 0
                }
            },
            .captures = {
                [0] = {
                    .cap_sig = 0
                },
                [1] = {
                    .cap_sig = 0
                },
                [2] = {
                    .cap_sig = 0
                }
            },
            .gpio_synchros = {
                [0] = {
                    .sync_sig = 0
                },
                [1] = {
                    .sync_sig = 0
                },
                [2] = {
                    .sync_sig = 0
                }
            }
        },
    }
};
