// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_IO_MUX_STRUCT_H_
#define _SOC_IO_MUX_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t clk_out1:        4;
            uint32_t clk_out2:        4;
            uint32_t clk_out3:        4;
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } pin_ctrl;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio36;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio37;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio38;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio39;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio34;
    union {
        struct {
            uint32_t reserved0:      1;             /* N/A - Output enable in sleep mode */
            uint32_t slp_sel:        1;             /* Pin used for wakeup from sleep */
            uint32_t reserved2:      1;             /* N/A - Pulldown enable in sleep mode */
            uint32_t reserved3:      1;             /* N/A - Pullup enable in sleep mode */
            uint32_t slp_ie:         1;             /* Input enable in sleep mode */
            uint32_t reserved5:      2;             /* N/A - Drive strength in sleep mode */
            uint32_t reserved7:      1;             /* N/A - Pulldown enable */
            uint32_t reserved8:      1;             /* N/A - Pullup enable */
            uint32_t fun_ie:         1;             /* Input enable */
            uint32_t reserved10:     2;             /* N/A - Drive strength */
            uint32_t fun_sel:        3;             /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:     17;
        };
        uint32_t val;
    } gpio35;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio32;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio33;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio25;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio26;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio27;
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio14;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } mtms;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio12;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } mtdi;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio13;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } mtck;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio15;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } mtdo;
    };
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio2;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio0;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio4;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio16;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio17;
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio9;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_data_2;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio10;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_data_3;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio11;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_cmd;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio6;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_clk;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio7;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_data_0;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio8;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } sd_data_1;
    };
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio5;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio18;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio19;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } reserved_gpio20;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio21;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio22;
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio3;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } u0rxd;
    };
    union {
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } gpio1;
        union {
            struct {
                uint32_t slp_oe:      1;                /* Output enable in sleep mode */
                uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
                uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
                uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
                uint32_t slp_ie:      1;                /* Input enable in sleep mode */
                uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
                uint32_t fun_pd:      1;                /* Pulldown enable */
                uint32_t fun_pu:      1;                /* Pullup enable */
                uint32_t fun_ie:      1;                /* Input enable */
                uint32_t fun_drv:     2;                /* Drive strength */
                uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
                uint32_t reserved15:  17;                
            };
            uint32_t val;
        } u0txd;
    };
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio23;
    union {
        struct {
            uint32_t slp_oe:      1;                /* Output enable in sleep mode */
            uint32_t slp_sel:     1;                /* Pin used for wakeup from sleep */
            uint32_t slp_pd:      1;                /* Pulldown enable in sleep mode */
            uint32_t slp_pu:      1;                /* Pullup enable in sleep mode */
            uint32_t slp_ie:      1;                /* Input enable in sleep mode */
            uint32_t slp_drv:     2;                /* Drive strength in sleep mode */
            uint32_t fun_pd:      1;                /* Pulldown enable */
            uint32_t fun_pu:      1;                /* Pullup enable */
            uint32_t fun_ie:      1;                /* Input enable */
            uint32_t fun_drv:     2;                /* Drive strength */
            uint32_t fun_sel:     3;                /* Function select (possible values are defined for each pin as FUNC_pinname_function in io_mux_reg.h) */
            uint32_t reserved15:  17;                
        };
        uint32_t val;
    } gpio24;
} io_mux_dev_t;
extern io_mux_dev_t IOMUX;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_IO_MUX_STRUCT_H_ */
