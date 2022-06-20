/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/sensitive_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint32_t cat0       : 2;
        uint32_t cat1       : 2;
        uint32_t cat2       : 2;
        uint32_t cat3       : 2;
        uint32_t cat4       : 2;
        uint32_t cat5       : 2;
        uint32_t cat6       : 2;
        uint32_t splitaddr  : 8;
        uint32_t reserved   : 10;
    };
    uint32_t val;
} constrain_reg_fields_t;

#define I_D_SRAM_SEGMENT_SIZE       0x10000
#define I_D_SPLIT_LINE_ALIGN        0x100
#define I_D_SPLIT_LINE_SHIFT        0x8
#define I_D_FAULT_ADDR_SHIFT        0x2

//IRAM0
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R  0x1
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W  0x2
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_F  0x4

//DRAM0
#define SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R  0x1
#define SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W  0x2

//RTC FAST
#define SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_W  0x1
#define SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_R  0x2
#define SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_F  0x4

#ifdef __cplusplus
}
#endif
