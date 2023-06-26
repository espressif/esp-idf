/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PMS register configuration structure for I/D splitting address.
 * Category bits define the splitting address being below, inside or above specific memory level range:
 *  - for details of ESP32S3 memory layout, see 725_mem_map.* documents
 *  - for category bits settings, see MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS*
 *    (components/hal/include/hal/memprot_types.h)
 *  - for details on assembling full splitting address
 *    see function memprot_ll_get_split_addr_from_reg() (components/hal/esp32s3/include/hal/memprot_ll.h)
 */
typedef union {
    struct {
        uint32_t cat0       : 2; /**< category bits - level 2 */
        uint32_t cat1       : 2; /**< category bits - level 3 */
        uint32_t cat2       : 2; /**< category bits - level 4 */
        uint32_t cat3       : 2; /**< category bits - level 5 */
        uint32_t cat4       : 2; /**< category bits - level 6 */
        uint32_t cat5       : 2; /**< category bits - level 7 */
        uint32_t cat6       : 2; /**< category bits - level 8 */
        uint32_t splitaddr  : 8; /**< splitting address significant bits */
        uint32_t reserved   : 10;
    };
    uint32_t val;
} constrain_reg_fields_t;

#define I_D_SRAM_SEGMENT_SIZE       0x10000
#define I_D_SPLIT_LINE_ALIGN        0x100
#define I_D_SPLIT_LINE_SHIFT        0x8
#define I_FAULT_ADDR_SHIFT          0x2
#define D_FAULT_ADDR_SHIFT          0x4

#define IRAM0_VIOLATE_STATUS_ADDR_OFFSET  0x40000000
#define DRAM0_VIOLATE_STATUS_ADDR_OFFSET  0x3C000000

//Icache
#define SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_R  0x1
#define SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_W  0x2
#define SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_F  0x4

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
