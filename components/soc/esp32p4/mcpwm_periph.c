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
            .module = PERIPH_MCPWM1_MODULE,
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
