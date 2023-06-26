/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


typedef volatile struct iomux_dev_s {
    uint32_t reserved_0;
    union {
        struct {
            uint32_t mcu_oe                        :    1;  /*output enable on sleep mode*/
            uint32_t slp_sel                       :    1;  /*io sleep mode enable. set 1 to enable sleep mode.*/
            uint32_t mcu_wpd                       :    1;  /*pull-down enable on sleep mode*/
            uint32_t mcu_wpu                       :    1;  /*pull-up enable on sleep mode*/
            uint32_t mcu_ie                        :    1;  /*input enable on sleep mode*/
            uint32_t mcu_drv                       :    2;  /*select drive strenth on sleep mode*/
            uint32_t fun_wpd                       :    1;  /*pull-down enable*/
            uint32_t fun_wpu                       :    1;  /*pull-up enable*/
            uint32_t fun_ie                        :    1;  /*input enable*/
            uint32_t fun_drv                       :    2;  /*select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA*/
            uint32_t mcu_sel                       :    3;  /*0:select function0, 1:select function1 ...*/
            uint32_t filter_en                     :    1;  /*input filter enable*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } gpio[57];
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    uint32_t reserved_100;
    union {
        struct {
            uint32_t reg_date                      :    28;  /*csv date*/
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } io_mux_date;
} iomux_dev_t;

extern iomux_dev_t IOMUX;

#ifdef __cplusplus
}
#endif
