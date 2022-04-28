// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/soc.h"
#include "soc/mcpwm_periph.h"
#include "soc/gpio_sig_map.h"

const mcpwm_signal_conn_t mcpwm_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_PWM0_MODULE,
            .irq_id = ETS_PWM0_INTR_SOURCE,
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
        [1] = {
            .module = PERIPH_PWM1_MODULE,
            .irq_id = ETS_PWM1_INTR_SOURCE,
            .operators = {
                [0] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_OUT0A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_OUT0B_IDX
                        }
                    }
                },
                [1] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_OUT1A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_OUT1B_IDX
                        }
                    }
                },
                [2] = {
                    .generators = {
                        [0] = {
                            .pwm_sig = PWM1_OUT2A_IDX
                        },
                        [1] = {
                            .pwm_sig = PWM1_OUT2B_IDX
                        }
                    }
                }
            },
            .gpio_faults = {
                [0] = {
                    .fault_sig = PWM1_F0_IN_IDX
                },
                [1] = {
                    .fault_sig = PWM1_F1_IN_IDX
                },
                [2] = {
                    .fault_sig = PWM1_F2_IN_IDX
                }
            },
            .captures = {
                [0] = {
                    .cap_sig = PWM1_CAP0_IN_IDX
                },
                [1] = {
                    .cap_sig = PWM1_CAP1_IN_IDX
                },
                [2] = {
                    .cap_sig = PWM1_CAP2_IN_IDX
                }
            },
            .gpio_synchros = {
                [0] = {
                    .sync_sig = PWM1_SYNC0_IN_IDX
                },
                [1] = {
                    .sync_sig = PWM1_SYNC1_IN_IDX
                },
                [2] = {
                    .sync_sig = PWM1_SYNC2_IN_IDX
                }
            }
        }
    }
};
