/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_PERI_BACKUP_STRUCT_H_
#define _SOC_PERI_BACKUP_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct peri_backup_dev_s {
    union {
        struct {
            uint32_t reg_peri_backup_flow_err      :    3;
            uint32_t reg_peri_backup_addr_map_mode :    1;
            uint32_t reg_peri_backup_burst_limit   :    5;
            uint32_t reg_peri_backup_tout_thres    :    10;
            uint32_t reg_peri_backup_size          :    10;
            uint32_t reg_peri_backup_start         :    1;
            uint32_t reg_peri_backup_to_mem        :    1;
            uint32_t reg_peri_backup_ena           :    1;
        };
        uint32_t val;
    } config;
    uint32_t apb_addr;
    uint32_t mem_addr;
    uint32_t reg_map0;
    uint32_t reg_map1;
    uint32_t reg_map2;
    uint32_t reg_map3;
    union {
        struct {
            uint32_t reg_peri_backup_done_int_raw  :    1;
            uint32_t reg_peri_backup_err_int_raw   :    1;
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t reg_peri_backup_done_int_st   :    1;
            uint32_t reg_peri_backup_err_int_st    :    1;
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t reg_peri_backup_done_int_ena  :    1;
            uint32_t reg_peri_backup_err_int_ena   :    1;
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t reg_peri_backup_done_int_clr  :    1;
            uint32_t reg_peri_backup_err_int_clr   :    1;
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_clr;
    uint32_t reserved_2c;
    uint32_t reserved_30;
    uint32_t reserved_34;
    uint32_t reserved_38;
    uint32_t reserved_3c;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    union {
        struct {
            uint32_t reg_date                      :    28;
            uint32_t reserved28                    :    3;
            uint32_t reg_clk_en                    :    1;  /*register file clk gating*/
        };
        uint32_t val;
    } date;
} peri_backup_dev_t;
extern peri_backup_dev_t PERI_BACKUP;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_PERI_BACKUP_STRUCT_H_ */
