/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
                            .control_sig = 0,
                            .pulse_sig = 0
                        },
                        [1] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        }
                    }
                },
                [1] = {
                    .channels = {
                        [0] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        },
                        [1] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        }
                    }
                },
                [2] = {
                    .channels = {
                        [0] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        },
                        [1] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        }
                    }
                },
                [3] = {
                    .channels = {
                        [0] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        },
                        [1] = {
                            .control_sig = 0,
                            .pulse_sig = 0
                        }
                    }
                }
            }
        }
    }
};
