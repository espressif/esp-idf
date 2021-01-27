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

#pragma once

#include "soc/sensitive_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * === globals ====
 */
#ifndef SRAM_IRAM_START
#define SRAM_IRAM_START             0x4037C000
#endif

#ifndef SRAM_DRAM_START
#define SRAM_DRAM_START             0x3FC7C000
#endif

/* ICache size is fixed to 16KB on ESP32-C3 */
#ifndef ICACHE_SIZE
#define ICACHE_SIZE                 0x4000
#endif

#ifndef I_D_SRAM_SEGMENT_SIZE
#define I_D_SRAM_SEGMENT_SIZE       0x20000
#endif

#ifndef I_D_SRAM_OFFSET
#define I_D_SRAM_OFFSET             (SRAM_IRAM_START - SRAM_DRAM_START)
#endif

/* 2nd stage bootloader iram_loader_seg start address */
#ifndef SRAM_DRAM_END
#define SRAM_DRAM_END               (0x403D0000 - I_D_SRAM_OFFSET)
#endif

#ifndef SRAM_IRAM_ORG
#define SRAM_IRAM_ORG               (SRAM_IRAM_START + ICACHE_SIZE)
#endif

#ifndef SRAM_DRAM_ORG
#define SRAM_DRAM_ORG               (SRAM_DRAM_START + ICACHE_SIZE)
#endif

#ifndef I_D_SRAM_SIZE
#define I_D_SRAM_SIZE               SRAM_DRAM_END - SRAM_DRAM_ORG
#endif

#define I_D_SPLIT_LINE_SHIFT        0x9

#define MAP_DRAM_TO_IRAM(addr)       (addr - SRAM_DRAM_START + SRAM_IRAM_START)
#define MAP_IRAM_TO_DRAM(addr)       (addr - SRAM_IRAM_START + SRAM_DRAM_START)


static inline void memprot_ll_set_iram0_dram0_split_line_lock(bool lock)
{
    REG_WRITE(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_0_REG, lock ? 1 : 0);
}

static inline bool memprot_ll_get_iram0_dram0_split_line_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_0_REG) == 1;
}


/**
 * === IRAM0 ====
 */
//16kB (CACHE)
#define IRAM0_SRAM_LEVEL_0_LOW      SRAM_IRAM_START //0x40370000
#define IRAM0_SRAM_LEVEL_0_HIGH     (IRAM0_SRAM_LEVEL_0_LOW + ICACHE_SIZE - 0x1) //0x4037FFFF

//128kB (LEVEL 1)
#define IRAM0_SRAM_LEVEL_1_LOW      (IRAM0_SRAM_LEVEL_0_HIGH + 0x1) //0x40380000
#define IRAM0_SRAM_LEVEL_1_HIGH     (IRAM0_SRAM_LEVEL_1_LOW + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x4039FFFF

//128kB (LEVEL 2)
#define IRAM0_SRAM_LEVEL_2_LOW      (IRAM0_SRAM_LEVEL_1_HIGH + 0x1) //0x403A0000
#define IRAM0_SRAM_LEVEL_2_HIGH     (IRAM0_SRAM_LEVEL_2_LOW + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x403BFFFF

//128kB (LEVEL 3)
#define IRAM0_SRAM_LEVEL_3_LOW      (IRAM0_SRAM_LEVEL_2_HIGH + 0x1) //0x403C0000
#define IRAM0_SRAM_LEVEL_3_HIGH     (IRAM0_SRAM_LEVEL_3_LOW + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x403DFFFF

//permission bits
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_R  0x1
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_W  0x2
#define SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_F  0x4

static inline uint32_t memprot_ll_iram0_get_intr_source_num(void)
{
    return ETS_CORE0_IRAM0_PMS_INTR_SOURCE;
}

/* SPLIT LINE */

static inline void memprot_ll_set_iram0_split_line(const void *line_addr, uint32_t sensitive_reg)
{
    uint32_t addr = (uint32_t)line_addr;
    assert( addr >= IRAM0_SRAM_LEVEL_1_LOW && addr <= IRAM0_SRAM_LEVEL_3_HIGH );

    uint32_t category[3] = {0};
    if (addr <= IRAM0_SRAM_LEVEL_1_HIGH) {
        category[0] = 0x2;
        category[1] = category[2] = 0x3;
    } else if (addr >= IRAM0_SRAM_LEVEL_2_LOW && addr <= IRAM0_SRAM_LEVEL_2_HIGH) {
        category[1] = 0x2;
        category[2] = 0x3;
    } else {
        category[2] = 0x2;
    }

    //category bits are the same for all areas
    uint32_t category_bits =
        (category[0] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_0_S) |
        (category[1] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_1_S) |
        (category[2] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_2_S);

    uint32_t conf_addr = ((addr >> I_D_SPLIT_LINE_SHIFT) & SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_SPLITADDR_V) << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_SPLITADDR_S;

    uint32_t reg_cfg = conf_addr | category_bits;

    REG_WRITE(sensitive_reg, reg_cfg);
}

/* can be both IRAM0/DRAM0 address */
static inline void memprot_ll_set_iram0_split_line_main_I_D(const void *line_addr)
{
    memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_1_REG);
}

static inline void memprot_ll_set_iram0_split_line_I_0(const void *line_addr)
{
    memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG);
}

static inline void memprot_ll_set_iram0_split_line_I_1(const void *line_addr)
{
    memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG);
}


/* PMS */

static inline void memprot_ll_iram0_set_pms_lock(bool lock)
{
    REG_WRITE(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_0_REG, lock ? 1 : 0);
}

static inline bool memprot_ll_iram0_get_pms_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_0_REG) == 1;
}

//world_0 permissions
static inline uint32_t memprot_ll_iram0_set_permissions(bool r, bool w, bool x)
{
    uint32_t permissions = 0;
    if ( r ) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_R;
    }
    if ( w ) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_W;
    }
    if ( x ) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_F;
    }

    return permissions;
}

static inline void memprot_ll_iram0_set_pms_area_0(bool r, bool w, bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0, memprot_ll_iram0_set_permissions(r, w, x));
}

static inline void memprot_ll_iram0_set_pms_area_1(bool r, bool w, bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1, memprot_ll_iram0_set_permissions(r, w, x));
}

static inline void memprot_ll_iram0_set_pms_area_2(bool r, bool w, bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2, memprot_ll_iram0_set_permissions(r, w, x));
}

static inline void memprot_ll_iram0_set_pms_area_3(bool r, bool w, bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3, memprot_ll_iram0_set_permissions(r, w, x));
}

/* MONITOR */

static inline void memprot_ll_iram0_set_monitor_lock(bool lock)
{
    REG_WRITE(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_0_REG, lock ? 1 : 0);
}

static inline bool memprot_ll_iram0_get_monitor_lock(void)
{
    return REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_0_REG) == 1;
}

static inline void memprot_ll_iram0_set_monitor_en(bool enable)
{
    if ( enable ) {
        REG_SET_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN );
    } else {
        REG_CLR_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN );
    }
}

static inline bool memprot_ll_iram0_get_monitor_en(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN ) == 1;
}

static inline void memprot_ll_iram0_clear_monitor_intr(void)
{
    REG_CLR_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR );
}

static inline uint32_t memprot_ll_iram0_get_monitor_status_intr(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_INTR );
}

static inline uint32_t memprot_ll_iram0_get_monitor_status_fault_wr(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WR );
}

static inline uint32_t memprot_ll_iram0_get_monitor_status_fault_loadstore(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_LOADSTORE );
}

static inline uint32_t memprot_ll_iram0_get_monitor_status_fault_world(void)
{
    return REG_GET_FIELD( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD );
}

static inline uint32_t memprot_ll_iram0_get_monitor_status_fault_addr(void)
{
    return REG_GET_FIELD( SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR );
}


/**
 * === DRAM0 ====
 */

//cache not available from DRAM
#define DRAM0_SRAM_LEVEL_0_LOW      SRAM_DRAM_START //0x3FC7C000
#define DRAM0_SRAM_LEVEL_0_HIGH     (DRAM0_SRAM_LEVEL_0_LOW + ICACHE_SIZE - 0x1) //0x3FC7FFFF

//128kB
#define DRAM0_SRAM_LEVEL_1_LOW      (DRAM0_SRAM_LEVEL_0_HIGH + 0x1) //0x3FC80000
#define DRAM0_SRAM_LEVEL_1_HIGH     (DRAM0_SRAM_LEVEL_1_LOW + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x3FC9FFFF

//128kB
#define DRAM0_SRAM_LEVEL_2_LOW      (DRAM0_SRAM_LEVEL_1_HIGH + 0x1) //0x3FCA0000
#define DRAM0_SRAM_LEVEL_2_HIGH     (DRAM0_SRAM_LEVEL_2_LOW + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x3FCBFFFF

//128kB
#define DRAM0_SRAM_LEVEL_3_LOW      (DRAM0_SRAM_LEVEL_2_HIGH + 0x1) //0x3FCC0000
#define DRAM0_SRAM_LEVEL_3_HIGH     (DRAM0_SRAM_LEVEL_3_LOW  + I_D_SRAM_SEGMENT_SIZE - 0x1) //0x3FCDFFFF

#define SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_W  0x2
#define SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_R  0x1


static inline uint32_t memprot_ll_dram0_get_intr_source_num(void)
{
    return ETS_CORE0_DRAM0_PMS_INTR_SOURCE;
}


/* SPLIT LINE */

static inline void memprot_ll_set_dram0_split_line(const void *line_addr, uint32_t sensitive_reg)
{
    uint32_t addr = (uint32_t)line_addr;
    assert( addr >= DRAM0_SRAM_LEVEL_1_LOW && addr <= DRAM0_SRAM_LEVEL_3_HIGH );

    uint32_t category[3] = {0};
    if (addr <= DRAM0_SRAM_LEVEL_1_HIGH) {
        category[0] = 0x2;
        category[1] = category[2] = 0x3;
    } else if (addr >= DRAM0_SRAM_LEVEL_2_LOW && addr <= DRAM0_SRAM_LEVEL_2_HIGH) {
        category[1] = 0x2;
        category[2] = 0x3;
    } else {
        category[2] = 0x2;
    }

    //category bits are the same for all areas
    uint32_t category_bits =
        (category[0] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_0_S) |
        (category[1] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_1_S) |
        (category[2] << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_2_S);

    uint32_t conf_addr = ((addr >> I_D_SPLIT_LINE_SHIFT) & SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_SPLITADDR_V) << SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_SPLITADDR_S;

    uint32_t reg_cfg = conf_addr | category_bits;

    REG_WRITE(sensitive_reg, reg_cfg);
}

static inline void memprot_ll_set_dram0_split_line_D_0(const void *line_addr)
{
    memprot_ll_set_dram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG);
}

static inline void memprot_ll_set_dram0_split_line_D_1(const void *line_addr)
{
    memprot_ll_set_dram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG);
}


/* PMS */

static inline void memprot_ll_dram0_set_pms_lock(bool lock)
{
    REG_WRITE(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_0_REG, lock ? 1 : 0);
}

static inline bool memprot_ll_dram0_get_pms_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_0_REG) == 1;
}

static inline uint32_t memprot_ll_dram0_set_permissions(bool r, bool w)
{
    uint32_t permissions = 0;
    if ( r ) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_R;
    }
    if ( w ) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_W;
    }

    return permissions;
}

static inline void memprot_ll_dram0_set_pms_area_0(bool r, bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0, memprot_ll_dram0_set_permissions(r, w));
}

static inline void memprot_ll_dram0_set_pms_area_1(bool r, bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1, memprot_ll_dram0_set_permissions(r, w));
}

static inline void memprot_ll_dram0_set_pms_area_2(bool r, bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2, memprot_ll_dram0_set_permissions(r, w));
}

static inline void memprot_ll_dram0_set_pms_area_3(bool r, bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3, memprot_ll_dram0_set_permissions(r, w));
}


/* MONITOR */

static inline void memprot_ll_dram0_set_monitor_lock(bool lock)
{
    REG_WRITE(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_0_REG, lock ? 1 : 0);
}

static inline bool memprot_ll_dram0_get_monitor_lock(void)
{
    return REG_READ(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_0_REG) == 1;
}

static inline void memprot_ll_dram0_set_monitor_en(bool enable)
{
    if ( enable ) {
        REG_SET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN );
    } else {
        REG_CLR_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN );
    }
}

static inline bool memprot_ll_dram0_get_monitor_en(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN ) == 1;
}

static inline void memprot_ll_dram0_clear_monitor_intr(void)
{
    REG_CLR_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_intr(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_INTR );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_fault_lock(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_LOCK );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_fault_world(void)
{
    return REG_GET_FIELD( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_fault_addr(void)
{
    return REG_GET_FIELD( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_fault_wr(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_3_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WR );
}

static inline uint32_t memprot_ll_dram0_get_monitor_status_fault_byte_en(void)
{
    return REG_GET_BIT( SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_BYTEEN );
}


#ifdef __cplusplus
}
#endif
