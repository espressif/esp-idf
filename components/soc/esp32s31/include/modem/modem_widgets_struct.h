/**
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t reg_clk_en                    :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t reg_diag_bus_fix_sel_high     :    5;
            uint32_t reg_diag_bus_fix_sel_low      :    5;
            uint32_t reg_diag_bus_fix_low_en       :    1;
            uint32_t reserved11                    :    21;
        };
        uint32_t val;
    } modem_diag_fix_sel;
    union {
        struct {
            uint32_t reg_diag_bus_exchange0_sel    :    4;
            uint32_t reg_diag_bus_exchange1_sel    :    4;
            uint32_t reg_diag_bus_exchange2_sel    :    4;
            uint32_t reg_diag_bus_exchange3_sel    :    4;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } modem_diag_exchange;
    union {
        struct {
            uint32_t reg_date                      :    28;
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } date;
} modem_widgets_dev_t;
extern modem_widgets_dev_t MODEM_WIDGETS;

#ifndef __cplusplus
_Static_assert(sizeof(modem_widgets_dev_t) == 0x10, "Invalid size of modem_widgets_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
