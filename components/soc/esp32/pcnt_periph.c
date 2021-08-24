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
                },
                [4] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN4_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN4_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN4_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN4_IDX
                        }
                    }
                },
                [5] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN5_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN5_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN5_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN5_IDX
                        }
                    }
                },
                [6] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN6_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN6_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN6_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN6_IDX
                        }
                    }
                },
                [7] = {
                    .channels = {
                        [0] = {
                            .control_sig = PCNT_CTRL_CH0_IN7_IDX,
                            .pulse_sig = PCNT_SIG_CH0_IN7_IDX
                        },
                        [1] = {
                            .control_sig = PCNT_CTRL_CH1_IN7_IDX,
                            .pulse_sig = PCNT_SIG_CH1_IN7_IDX
                        }
                    }
                }
            }
        }
    }
};
