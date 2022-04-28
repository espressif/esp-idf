/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_EFUSE_STRUCT_H_
#define _SOC_EFUSE_STRUCT_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct efuse_dev_s {
    uint32_t blk0_rdata0;
    uint32_t blk0_rdata1;
    uint32_t blk0_rdata2;
    uint32_t blk0_rdata3;
    uint32_t blk0_rdata4;
    uint32_t blk0_rdata5;
    uint32_t blk0_rdata6;

    uint32_t blk0_wdata0;
    uint32_t blk0_wdata1;
    uint32_t blk0_wdata2;
    uint32_t blk0_wdata3;
    uint32_t blk0_wdata4;
    uint32_t blk0_wdata5;
    uint32_t blk0_wdata6;

    uint32_t blk1_rdata0;
    uint32_t blk1_rdata1;
    uint32_t blk1_rdata2;
    uint32_t blk1_rdata3;
    uint32_t blk1_rdata4;
    uint32_t blk1_rdata5;
    uint32_t blk1_rdata6;
    uint32_t blk1_rdata7;

    uint32_t blk2_rdata0;
    uint32_t blk2_rdata1;
    uint32_t blk2_rdata2;
    uint32_t blk2_rdata3;
    uint32_t blk2_rdata4;
    uint32_t blk2_rdata5;
    uint32_t blk2_rdata6;
    uint32_t blk2_rdata7;

    uint32_t blk3_rdata0;
    uint32_t blk3_rdata1;
    uint32_t blk3_rdata2;
    uint32_t blk3_rdata3;
    uint32_t blk3_rdata4;
    uint32_t blk3_rdata5;
    uint32_t blk3_rdata6;
    uint32_t blk3_rdata7;

    uint32_t blk1_wdata0;
    uint32_t blk1_wdata1;
    uint32_t blk1_wdata2;
    uint32_t blk1_wdata3;
    uint32_t blk1_wdata4;
    uint32_t blk1_wdata5;
    uint32_t blk1_wdata6;
    uint32_t blk1_wdata7;

    uint32_t blk2_wdata0;
    uint32_t blk2_wdata1;
    uint32_t blk2_wdata2;
    uint32_t blk2_wdata3;
    uint32_t blk2_wdata4;
    uint32_t blk2_wdata5;
    uint32_t blk2_wdata6;
    uint32_t blk2_wdata7;

    uint32_t blk3_wdata0;
    uint32_t blk3_wdata1;
    uint32_t blk3_wdata2;
    uint32_t blk3_wdata3;
    uint32_t blk3_wdata4;
    uint32_t blk3_wdata5;
    uint32_t blk3_wdata6;
    uint32_t blk3_wdata7;

    uint32_t clk;
    uint32_t conf;
    uint32_t status;
    uint32_t cmd;
    uint32_t int_raw;
    uint32_t int_st;
    uint32_t int_ena;
    uint32_t int_clr;
    uint32_t dac_conf;
    uint32_t dec_status;
    uint32_t reserve[55];
    uint32_t date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_EFUSE_STRUCT_H_ */
