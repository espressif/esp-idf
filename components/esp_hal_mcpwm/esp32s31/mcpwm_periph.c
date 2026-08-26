/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/mcpwm_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"

const soc_mcpwm_signal_desc_t soc_mcpwm_signals[4] = {
    {
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
    {
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
    },
    {
        .module_name = "MCPWM2",
        .irq_id = ETS_PWM2_INTR_SOURCE,
        .operators = {
            [0] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM2_CH0_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM2_CH0_B_PAD_OUT_IDX
                    }
                }
            },
            [1] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM2_CH1_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM2_CH1_B_PAD_OUT_IDX
                    }
                }
            },
            [2] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM2_CH2_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM2_CH2_B_PAD_OUT_IDX
                    }
                }
            }
        },
        .gpio_faults = {
            [0] = {
                .fault_sig = PWM2_F0_PAD_IN_IDX
            },
            [1] = {
                .fault_sig = PWM2_F1_PAD_IN_IDX
            },
            [2] = {
                .fault_sig = PWM2_F2_PAD_IN_IDX
            }
        },
        .captures = {
            [0] = {
                .cap_sig = PWM2_CAP0_PAD_IN_IDX
            },
            [1] = {
                .cap_sig = PWM2_CAP1_PAD_IN_IDX
            },
            [2] = {
                .cap_sig = PWM2_CAP2_PAD_IN_IDX
            }
        },
        .gpio_synchros = {
            [0] = {
                .sync_sig = PWM2_SYNC0_PAD_IN_IDX
            },
            [1] = {
                .sync_sig = PWM2_SYNC1_PAD_IN_IDX
            },
            [2] = {
                .sync_sig = PWM2_SYNC2_PAD_IN_IDX
            }
        }
    },
    {
        .module_name = "MCPWM3",
        .irq_id = ETS_PWM3_INTR_SOURCE,
        .operators = {
            [0] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM3_CH0_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM3_CH0_B_PAD_OUT_IDX
                    }
                }
            },
            [1] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM3_CH1_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM3_CH1_B_PAD_OUT_IDX
                    }
                }
            },
            [2] = {
                .generators = {
                    [0] = {
                        .pwm_sig = PWM3_CH2_A_PAD_OUT_IDX
                    },
                    [1] = {
                        .pwm_sig = PWM3_CH2_B_PAD_OUT_IDX
                    }
                }
            }
        },
        .gpio_faults = {
            [0] = {
                .fault_sig = PWM3_F0_PAD_IN_IDX
            },
            [1] = {
                .fault_sig = PWM3_F1_PAD_IN_IDX
            },
            [2] = {
                .fault_sig = PWM3_F2_PAD_IN_IDX
            }
        },
        .captures = {
            [0] = {
                .cap_sig = PWM3_CAP0_PAD_IN_IDX
            },
            [1] = {
                .cap_sig = PWM3_CAP1_PAD_IN_IDX
            },
            [2] = {
                .cap_sig = PWM3_CAP2_PAD_IN_IDX
            }
        },
        .gpio_synchros = {
            [0] = {
                .sync_sig = PWM3_SYNC0_PAD_IN_IDX
            },
            [1] = {
                .sync_sig = PWM3_SYNC1_PAD_IN_IDX
            },
            [2] = {
                .sync_sig = PWM3_SYNC2_PAD_IN_IDX
            }
        }
    }
};
