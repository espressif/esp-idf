/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/ext_mem_defs.h"
#include "soc/memprot_defs.h"
#include "hal/memprot_types.h"
#include "soc/sensitive_reg.h"

/* Uncomment to enable MPS debug assertions on false register writes.
 * It irregularly happens the PMS registers cannot be written which causes unpredictable malfunction of the Memprot feature
 * Once the issue is found & resolved, the assertions can be removed
 */
//#define PMS_DEBUG_ASSERTIONS

#ifdef PMS_DEBUG_ASSERTIONS
#include "hal/assert.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//highest address of each Level slot in the SRAM's 3rd memory region (I/D access, 416kB)
//quick resolver of split-address category bits
static const intptr_t sram_rg3_level_hlimits[] = {
        0x4037FFFF, //level 2 (32KB)
        0x4038FFFF, //level 3 (64KB)
        0x4039FFFF, //level 4 (64KB)
        0x403AFFFF, //level 5 (64KB)
        0x403BFFFF, //level 6 (64KB)
        0x403CFFFF, //level 7 (64KB)
        0x403DFFFF  //level 8 (64KB)
};

/* ******************************************************************************************************
 * *** COMMON ***
 * ******************************************************************************************************/
static inline void memprot_ll_set_iram0_dram0_split_line_lock(void)
{
    REG_WRITE(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_0_REG) == 1) && "Value not stored to required register");
#endif
}

static inline bool memprot_ll_get_iram0_dram0_split_line_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_0_REG) == 1;
}

static inline void *memprot_ll_get_split_addr_from_reg(const uint32_t regval, const uint32_t base)
{
    constrain_reg_fields_t reg_val;
    reg_val.val = regval;

    uint32_t off = reg_val.splitaddr << I_D_SPLIT_LINE_SHIFT;
    uint32_t level_off = 0;

    do {
        if (reg_val.cat0 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE/2;
        if (reg_val.cat1 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE;
        if (reg_val.cat2 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE;
        if (reg_val.cat3 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE;
        if (reg_val.cat4 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE;
        if (reg_val.cat5 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        level_off += I_D_SRAM_SEGMENT_SIZE;
        if (reg_val.cat6 == MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA) break;
        return NULL; //wrong configuration
    } while(0);

    return (void *)(base + level_off + off);
}

/* ******************************************************************************************************
 * *** ICACHE ***
 * ******************************************************************************************************/
static inline uint32_t memprot_ll_icache_set_permissions(const bool r, const bool w, const bool x)
{
    uint32_t permissions = 0;
    if (r) {
        permissions |= SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    }
    if (w) {
        permissions |= SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_W;
    }
    if (x) {
        permissions |= SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_F;
    }

    return permissions;
}

static inline void memprot_ll_icache_set_pms_area_0(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_0, memprot_ll_icache_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_0);
    HAL_ASSERT((expected == memprot_ll_icache_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_icache_set_pms_area_1(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_1, memprot_ll_icache_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_1);
    HAL_ASSERT((expected == memprot_ll_icache_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_icache_get_permissions(const uint32_t perms, bool *r, bool *w, bool *x)
{
    *r = perms & SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    *w = perms & SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_W;
    *x = perms & SENSITIVE_CORE_X_ICACHE_PMS_CONSTRAIN_SRAM_WORLD_X_F;
}

static inline void memprot_ll_icache_get_pms_area_0(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_0);
    memprot_ll_icache_get_permissions(permissions, r, w, x);
}

static inline void memprot_ll_icache_get_pms_area_1(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_CACHEDATAARRAY_PMS_1);
    memprot_ll_icache_get_permissions(permissions, r, w, x);
}

/* ******************************************************************************************************
 * *** IRAM0 ***
 * ******************************************************************************************************/
static inline memprot_hal_err_t memprot_ll_iram0_get_intr_source_num(const int core, uint32_t* src_num)
{
    switch (core) {
        case PRO_CPU_NUM:
            *src_num = ETS_CORE0_IRAM0_PMS_INTR_SOURCE;
            break;
        case APP_CPU_NUM:
            *src_num = ETS_CORE1_IRAM0_PMS_INTR_SOURCE;
            break;
        default:
            return MEMP_HAL_FAIL;
    }

    return MEMP_HAL_OK;
}

///////////////////////////////////
// IRAM0 - SPLIT LINES
static inline uint32_t memprot_ll_get_iram0_split_line_main_I_D_regval(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_1_REG);
}

static inline uint32_t memprot_ll_get_iram0_split_line_main_I_0_regval(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG);
}

static inline uint32_t memprot_ll_get_iram0_split_line_main_I_1_regval(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG);
}

static inline void memprot_ll_prepare_iram0_split_line_regval(const uint32_t addr, uint32_t* regval)
{
    //set category bits for given split line
    uint32_t cat[7] = {[0 ... 6]=MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_ABOVE_SA};
    for (size_t x=0; x<7; x++) {
        if (addr <= sram_rg3_level_hlimits[x]) {
            cat[x] = MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA;
            break;
        } else {
            cat[x] = MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_BELOW_SA;
        }
    }

    //resolve split address' significant bits
    uint32_t conf_add = ((addr >> I_D_SPLIT_LINE_SHIFT) & SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SRAM_SPLITADDR_V);

    //write values to required configuration-register
    constrain_reg_fields_t cfg_reg_val = {
            .cat0 = cat[0],
            .cat1 = cat[1],
            .cat2 = cat[2],
            .cat3 = cat[3],
            .cat4 = cat[4],
            .cat5 = cat[5],
            .cat6 = cat[6],
            .splitaddr = conf_add,
            .reserved = 0
    };

    *regval = cfg_reg_val.val;
}

// all the split lines registers have the same layout
static inline memprot_hal_err_t memprot_ll_set_iram0_split_line(const void *line_addr, const uint32_t sensitive_reg)
{
    uint32_t addr = (uint32_t)line_addr;

    //sanity check
    MEMP_HAL_CHECK_IRAM_ADDR_IN_RANGE(addr)
    MEMP_HAL_CHECK_SPLIT_ADDR_ALIGNED(addr)

    uint32_t regval;
    memprot_ll_prepare_iram0_split_line_regval(addr, &regval);

    REG_WRITE(sensitive_reg, regval);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((REG_READ(sensitive_reg) == regval) && "Value not stored to required register");
#endif

    return MEMP_HAL_OK;
}

// set the main I/D splitting address - can be defined within either IRAM0 or DRAM0 range (IRAM0 preferred as the Memprot API stereotype)
static inline memprot_hal_err_t memprot_ll_set_iram0_split_line_main_I_D(const void *line_addr)
{
    return memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_1_REG);
}

// set auxiliary split lines (IRAM0 range address required)
static inline memprot_hal_err_t memprot_ll_set_iram0_split_line_I_0(const void *line_addr)
{
    return memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG);
}

static inline memprot_hal_err_t memprot_ll_set_iram0_split_line_I_1(const void *line_addr)
{
    return memprot_ll_set_iram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG);
}

// get configured category bits
static inline uint32_t memprot_ll_get_iram0_split_line_main_I_D_cat(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_1_REG) & 0x3FFF;
}

static inline uint32_t memprot_ll_get_iram0_split_line_I_0_cat(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG) & 0x3FFF;
}

static inline uint32_t memprot_ll_get_iram0_split_line_I_1_cat(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG) & 0x3FFF;
}

// get configured splitting address (IRAM0-based)
static inline void *memprot_ll_get_iram0_split_line_main_I_D(void)
{
    return memprot_ll_get_split_addr_from_reg(REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_1_REG), SOC_DIRAM_IRAM_LOW);
}

static inline void *memprot_ll_get_iram0_split_line_I_0(void)
{
    return memprot_ll_get_split_addr_from_reg(REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_2_REG), SOC_DIRAM_IRAM_LOW);
}

static inline void *memprot_ll_get_iram0_split_line_I_1(void)
{
    return memprot_ll_get_split_addr_from_reg(REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_3_REG), SOC_DIRAM_IRAM_LOW);
}

///////////////////////////////////
// IRAM0 - PMS CONFIGURATION

// lock
static inline void memprot_ll_iram0_set_pms_lock(void)
{
    REG_WRITE(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((REG_READ(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_0_REG) == 1) && "Value not stored to required register");
#endif
}

static inline bool memprot_ll_iram0_get_pms_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_0_REG) == 1;
}

// permission settings
static inline uint32_t memprot_ll_iram0_set_permissions(const bool r, const bool w, const bool x)
{
    uint32_t permissions = 0;
    if (r) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    }
    if (w) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W;
    }
    if (x) {
        permissions |= SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_F;
    }

    return permissions;
}

static inline void memprot_ll_iram0_set_pms_area_0(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0, memprot_ll_iram0_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0);
    HAL_ASSERT((expected == memprot_ll_iram0_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_iram0_set_pms_area_1(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1, memprot_ll_iram0_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1);
    HAL_ASSERT((expected == memprot_ll_iram0_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_iram0_set_pms_area_2(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2, memprot_ll_iram0_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2);
    HAL_ASSERT((expected == memprot_ll_iram0_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_iram0_set_pms_area_3(const bool r, const bool w, const bool x)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3, memprot_ll_iram0_set_permissions(r, w, x));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3);
    HAL_ASSERT((expected == memprot_ll_iram0_set_permissions(r, w, x)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_iram0_get_permissions(const uint32_t perms, bool *r, bool *w, bool *x)
{
    *r = perms & SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    *w = perms & SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W;
    *x = perms & SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_F;
}

static inline void memprot_ll_iram0_get_pms_area_0(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0);
    memprot_ll_iram0_get_permissions(permissions, r, w, x);
}

static inline void memprot_ll_iram0_get_pms_area_1(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1);
    memprot_ll_iram0_get_permissions(permissions, r, w, x);
}

static inline void memprot_ll_iram0_get_pms_area_2(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2);
    memprot_ll_iram0_get_permissions(permissions, r, w, x);
}

static inline void memprot_ll_iram0_get_pms_area_3(bool *r, bool *w, bool *x)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_2_REG, SENSITIVE_CORE_X_IRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3);
    memprot_ll_iram0_get_permissions(permissions, r, w, x);
}


///////////////////////////////////
// IRAM0 - MONITOR

// lock
static inline memprot_hal_err_t memprot_ll_iram0_set_monitor_lock(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_lock(const int core, bool* locked)
{
    switch (core) {
        case PRO_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_0_REG) == 1;
            break;
        case APP_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_0_REG) == 1;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// interrupt enable/clear
static inline memprot_hal_err_t memprot_ll_iram0_set_monitor_en(const int core, const bool enable)
{
    switch (core) {
        case PRO_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        case APP_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_en(const int core, bool* enabled)
{
    switch (core) {
        case PRO_CPU_NUM:
            *enabled = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_EN) == 1;
            break;
        case APP_CPU_NUM:
            *enabled = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_EN) == 1;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_set_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_reset_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_intrclr(const int core, bool* cleared)
{
    switch (core) {
        case PRO_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        case APP_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_enable_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_1_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_1_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// permission violation status
static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_intr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_INTR);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_INTR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_fault_wr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WR);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_fault_loadstore(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_LOADSTORE);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_STATUS_LOADSTORE);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_fault_world(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_fault_addr(const int core, void** addr)
{
    uint32_t reg_off;

    switch (core) {
        case PRO_CPU_NUM:
            reg_off = REG_GET_FIELD(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR);
            break;
        case APP_CPU_NUM:
            reg_off = REG_GET_FIELD(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    *addr = (void*)(reg_off > 0 ? (reg_off << I_FAULT_ADDR_SHIFT) + IRAM0_VIOLATE_STATUS_ADDR_OFFSET : 0);

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_iram0_get_monitor_status_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}


/* ******************************************************************************************************
 * *** RTC_FAST ***
 */

static inline memprot_hal_err_t memprot_ll_rtcfast_get_intr_source_num(const int core, uint32_t* src_num)
{
    switch (core) {
        case PRO_CPU_NUM:
            *src_num = ETS_CORE0_PIF_PMS_INTR_SOURCE;
            break;
        case APP_CPU_NUM:
            *src_num = ETS_CORE1_PIF_PMS_INTR_SOURCE;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

//shared PIF PMS lock
//!!!: use after ALL the constraints have been set
static inline memprot_hal_err_t memprot_ll_set_pif_constraint_lock(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_get_pif_constraint_lock(const int core, bool* locked)
{
    switch (core) {
        case PRO_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_0_REG) == 1;
            break;
        case APP_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_0_REG) == 1;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_splitaddr_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_9_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_9_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

/* ********************************
 * IRAM0 RTCFAST - SPLIT LINES
 */

static inline memprot_hal_err_t memprot_ll_set_rtcfast_split_line(const int core, const void *line_addr, const memprot_hal_world_t world)
{
    uint32_t addr = (uint32_t)line_addr;

    if (addr < SOC_RTC_IRAM_LOW || addr >= SOC_RTC_IRAM_HIGH) {
        return MEMP_HAL_ERR_SPLIT_ADDR_OUT_OF_RANGE;
    }

    if (addr % 0x4 != 0) {
        return MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    if (core != PRO_CPU_NUM && core != APP_CPU_NUM) {
        return MEMP_HAL_ERR_CORE_INVALID;
    }

    uint32_t mask;
    uint32_t val;

    switch (world) {
    case MEMP_HAL_WORLD_0:
        mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_M;
        val = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_V : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_V;
        break;
    case MEMP_HAL_WORLD_1:
        mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_M;
        val = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_V : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_V;
        break;
    default:
        return MEMP_HAL_ERR_WORLD_INVALID;
    }

    uint32_t reg = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_9_REG : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_9_REG;

    CLEAR_PERI_REG_MASK(reg, mask);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((GET_PERI_REG_MASK(reg, mask) == 0) && "Value not stored to required register");
#endif
    REG_SET_BITS(reg, mask, (addr >> 2) & val);
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_READ(reg) & mask;
    HAL_ASSERT((expected == ((addr >> 2) & val)) && "Value not stored to required register");
#endif
    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_get_rtcfast_split_line(const int core, const memprot_hal_world_t world, void **line_addr)
{
    if (core != PRO_CPU_NUM && core != APP_CPU_NUM) {
        return MEMP_HAL_ERR_CORE_INVALID;
    }

    uint32_t mask;
    uint32_t shift;

    switch (world) {
    case MEMP_HAL_WORLD_0:
        mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_M;
        shift = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_0_S;
        break;
    case MEMP_HAL_WORLD_1:
        mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_M;
        shift = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_SPLTADDR_WORLD_1_S;
        break;
    default:
        return MEMP_HAL_ERR_WORLD_INVALID;
    }

    uint32_t reg_addr = REG_READ(core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_9_REG : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_9_REG);

    *line_addr = (void *)((((reg_addr & mask) >> shift) << 2) + SOC_RTC_IRAM_LOW);

    return MEMP_HAL_OK;
}

///////////////////////////////////
// RTC_FAST - PMS CONFIGURATION

// permission settings
static inline uint32_t memprot_ll_rtcfast_set_permissions(const bool r, const bool w, const bool x)
{
    uint32_t permissions = 0;
    if (r) {
        permissions |= SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_R;
    }
    if (w) {
        permissions |= SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_W;
    }
    if (x) {
        permissions |= SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_F;
    }

    return permissions;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_set_pms_area(const int core, const bool r, const bool w, const bool x, const memprot_hal_world_t world, const memprot_hal_area_t area)
{
    if (core != PRO_CPU_NUM && core != APP_CPU_NUM) {
        return MEMP_HAL_ERR_CORE_INVALID;
    }

    uint32_t bits;
    uint32_t mask;

    switch (world) {
    case MEMP_HAL_WORLD_0: {
        switch (area) {
        case MEMP_HAL_AREA_LOW:
            bits = memprot_ll_rtcfast_set_permissions(r, w, x) << (core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_L_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_L_S);
            mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_L_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_L_M;
            break;
        case MEMP_HAL_AREA_HIGH:
            bits = memprot_ll_rtcfast_set_permissions(r, w, x) << (core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H_S);
            mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H_M;
            break;
        default:
            return MEMP_HAL_ERR_AREA_INVALID;
        }
    } break;
    case MEMP_HAL_WORLD_1: {
        switch (area) {
        case MEMP_HAL_AREA_LOW:
            bits = memprot_ll_rtcfast_set_permissions(r, w, x) << (core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L_S);
            mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L_M;
            break;
        case MEMP_HAL_AREA_HIGH:
            bits = memprot_ll_rtcfast_set_permissions(r, w, x) << (core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H_S : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H_S);
            mask = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H_M : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H_M;
            break;
        default:
            return MEMP_HAL_ERR_AREA_INVALID;
        }
    } break;
    default:
        return MEMP_HAL_ERR_WORLD_INVALID;
    }

    uint32_t reg = core == PRO_CPU_NUM ? SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG : SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG;

    CLEAR_PERI_REG_MASK(reg, mask);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((GET_PERI_REG_MASK(reg, mask) == 0) && "Value not stored to required register");
#endif
    REG_SET_BITS(reg, bits, mask);
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_READ(reg) & mask;
    HAL_ASSERT((expected == bits) && "Value not stored to required register");
#endif

    return MEMP_HAL_OK;
}

static inline void memprot_ll_rtcfast_get_permissions(const uint32_t perms, bool *r, bool *w, bool *x)
{
    *r = perms & SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_R;
    *w = perms & SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_W;
    *x = perms & SENSITIVE_CORE_X_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_X_F;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_pms_area(const int core, bool *r, bool *w, bool *x, const memprot_hal_world_t world, const memprot_hal_area_t area)
{
    if (core != PRO_CPU_NUM && core != APP_CPU_NUM) {
        return MEMP_HAL_ERR_CORE_INVALID;
    }

    uint32_t permissions = 0;

    switch (world) {
    case MEMP_HAL_WORLD_0: {
        switch (area) {
        case MEMP_HAL_AREA_LOW:
            if (core == PRO_CPU_NUM) {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_L);
            } else {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L);
            }
            break;
        case MEMP_HAL_AREA_HIGH:
            if (core == PRO_CPU_NUM) {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H);
            } else {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_0_H);
            }
            break;
        default:
            return MEMP_HAL_ERR_AREA_INVALID;
        }
    } break;
    case MEMP_HAL_WORLD_1: {
        switch (area) {
        case MEMP_HAL_AREA_LOW:
            if (core == PRO_CPU_NUM) {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L);
            } else {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_L);
            }
            break;
        case MEMP_HAL_AREA_HIGH:
            if (core == PRO_CPU_NUM) {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H);
            } else {
                permissions = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG, SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_RTCFAST_WORLD_1_H);
            }
            break;
        default:
            return MEMP_HAL_ERR_AREA_INVALID;
        }
    } break;
    default:
        return MEMP_HAL_ERR_WORLD_INVALID;
    }

    memprot_ll_rtcfast_get_permissions(permissions, r, w, x);

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_permission_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_PIF_PMS_CONSTRAIN_10_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_PIF_PMS_CONSTRAIN_10_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

///////////////////////////////////
// RTC_FAST - MONITOR

// lock
static inline memprot_hal_err_t memprot_ll_rtcfast_set_monitor_lock(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_0_PIF_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_0_PIF_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_1_PIF_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_1_PIF_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_lock(const int core, bool* locked)
{
    switch (core) {
        case PRO_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_0_PIF_PMS_MONITOR_0_REG) == 1;
            break;
        case APP_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_1_PIF_PMS_MONITOR_0_REG) == 1;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// interrupt enable/clear
static inline memprot_hal_err_t memprot_ll_rtcfast_set_monitor_en(const int core, const bool enable)
{
    switch (core) {
        case PRO_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        case APP_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_en(const int core, bool* enabled)
{
    switch (core) {
        case PRO_CPU_NUM:
            *enabled = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_EN) > 0;
            break;
        case APP_CPU_NUM:
            *enabled = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_EN) > 0;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_intrclr(const int core, bool* cleared)
{
    switch (core) {
        case PRO_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        case APP_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_set_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_reset_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_PIF_PMS_MONITOR_1_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_PIF_PMS_MONITOR_1_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// permission violation status
static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_status_intr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_INTR);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_INTR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_status_fault_addr(const int core, void** addr)
{
    switch (core) {
        case PRO_CPU_NUM:
            *addr = (void*)REG_READ(SENSITIVE_CORE_0_PIF_PMS_MONITOR_3_REG);
            break;
        case APP_CPU_NUM:
            *addr = (void*)REG_READ(SENSITIVE_CORE_1_PIF_PMS_MONITOR_3_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_status_fault_world(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_STATUS_HWORLD);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_STATUS_HWORLD);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_status_fault_loadstore(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_STATUS_HPORT_0);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_STATUS_HPORT_0);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_rtcfast_get_monitor_status_fault_wr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_PIF_PMS_MONITOR_VIOLATE_STATUS_HWRITE);
            break;
        case APP_CPU_NUM:
            *regval =  REG_GET_FIELD(SENSITIVE_CORE_1_PIF_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_PIF_PMS_MONITOR_VIOLATE_STATUS_HWRITE);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}


/* ******************************************************************************************************
 * *** DRAM0 ***
 * ******************************************************************************************************/

static inline memprot_hal_err_t memprot_ll_dram0_get_intr_source_num(const int core, uint32_t* src_num)
{
    switch (core) {
        case PRO_CPU_NUM:
            *src_num = ETS_CORE0_DRAM0_PMS_INTR_SOURCE;
            break;
        case APP_CPU_NUM:
            *src_num = ETS_CORE1_DRAM0_PMS_INTR_SOURCE;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

///////////////////////////////////
// DRAM0 - SPLIT LINES
static inline uint32_t memprot_ll_get_dram0_split_line_main_D_0_regval(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_4_REG);
}

static inline uint32_t memprot_ll_get_dram0_split_line_main_D_1_regval(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_5_REG);
}

static inline void memprot_ll_prepare_dram0_split_line_regval(const uint32_t addr, uint32_t* regval)
{
    //set category bits for given split line
    uint32_t cat[7] = {[0 ... 6]=MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_ABOVE_SA};
    for (size_t x=0; x<7; x++) {
        if (addr <= MAP_IRAM_TO_DRAM(sram_rg3_level_hlimits[x])) {
            cat[x] = MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA;
            break;
        } else {
            cat[x] = MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_BELOW_SA;
        }
    }

    //resolve split address' significant bits
    uint32_t conf_add = ((addr >> I_D_SPLIT_LINE_SHIFT) & SENSITIVE_CORE_X_DRAM0_DMA_SRAM_LINE_0_SPLITADDR_V);

    //write values to required configuration-register
    constrain_reg_fields_t cfg_reg_val = {
            .cat0 = cat[0],
            .cat1 = cat[1],
            .cat2 = cat[2],
            .cat3 = cat[3],
            .cat4 = cat[4],
            .cat5 = cat[5],
            .cat6 = cat[6],
            .splitaddr = conf_add,
            .reserved = 0
    };


    *regval = cfg_reg_val.val;
}

static inline memprot_hal_err_t memprot_ll_set_dram0_split_line(const void *line_addr, const uint32_t sensitive_reg)
{
    uint32_t addr = (uint32_t)line_addr;

    //sanity check
    MEMP_HAL_CHECK_DRAM_ADDR_IN_RANGE(addr)
    MEMP_HAL_CHECK_SPLIT_ADDR_ALIGNED(addr)

    uint32_t regval;
    memprot_ll_prepare_dram0_split_line_regval(addr, &regval);

    REG_WRITE(sensitive_reg, regval);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((REG_READ(sensitive_reg) == regval) && "Value not stored to required register");
#endif

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_set_dram0_split_line_D_0(const void *line_addr)
{
    return memprot_ll_set_dram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_4_REG);
}

static inline memprot_hal_err_t memprot_ll_set_dram0_split_line_D_1(const void *line_addr)
{
    return memprot_ll_set_dram0_split_line(line_addr, SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_5_REG);
}

static inline void *memprot_ll_get_dram0_split_line_D_0(void)
{
    return memprot_ll_get_split_addr_from_reg(REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_4_REG), SOC_DIRAM_DRAM_LOW);
}

static inline void *memprot_ll_get_dram0_split_line_D_1(void)
{
    return memprot_ll_get_split_addr_from_reg(REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_5_REG), SOC_DIRAM_DRAM_LOW);
}

static inline uint32_t memprot_ll_get_dram0_split_line_D_0_cat(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_4_REG) & 0x3FFF;
}

static inline uint32_t memprot_ll_get_dram0_split_line_D_1_cat(void)
{
    return REG_READ(SENSITIVE_CORE_X_IRAM0_DRAM0_DMA_SPLIT_LINE_CONSTRAIN_5_REG) & 0x3FFF;
}

///////////////////////////////////
// DRAM0 - PMS CONFIGURATION

// lock
static inline void memprot_ll_dram0_set_pms_lock(void)
{
    REG_WRITE(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
    HAL_ASSERT((REG_READ(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_0_REG) == 1) && "Value not stored to required register");
#endif
}

static inline bool memprot_ll_dram0_get_pms_lock(void)
{
    return REG_READ(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_0_REG) == 1;
}

// permission settings
static inline uint32_t memprot_ll_dram0_set_permissions(const bool r, const bool w)
{
    uint32_t permissions = 0;
    if (r) {
        permissions |= SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    }
    if (w) {
        permissions |= SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W;
    }

    return permissions;
}

static inline void memprot_ll_dram0_set_pms_area_0(const bool r, const bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0, memprot_ll_dram0_set_permissions(r, w));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0);
    HAL_ASSERT((expected == memprot_ll_dram0_set_permissions(r, w)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_dram0_set_pms_area_1(const bool r, const bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1, memprot_ll_dram0_set_permissions(r, w));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1);
    HAL_ASSERT((expected == memprot_ll_dram0_set_permissions(r, w)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_dram0_set_pms_area_2(const bool r, const bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2, memprot_ll_dram0_set_permissions(r, w));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2);
    HAL_ASSERT((expected == memprot_ll_dram0_set_permissions(r, w)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_dram0_set_pms_area_3(const bool r, const bool w)
{
    REG_SET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3, memprot_ll_dram0_set_permissions(r, w));
#ifdef PMS_DEBUG_ASSERTIONS
    uint32_t expected = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3);
    HAL_ASSERT((expected == memprot_ll_dram0_set_permissions(r, w)) && "Value not stored to required register");
#endif
}

static inline void memprot_ll_dram0_get_permissions(const uint32_t perms, bool *r, bool *w )
{
    *r = perms & SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_R;
    *w = perms & SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_X_W;
}

static inline void memprot_ll_dram0_get_pms_area_0(bool *r, bool *w)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_0);
    memprot_ll_dram0_get_permissions(permissions, r, w);
}

static inline void memprot_ll_dram0_get_pms_area_1(bool *r, bool *w)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_1);
    memprot_ll_dram0_get_permissions(permissions, r, w);
}

static inline void memprot_ll_dram0_get_pms_area_2(bool *r, bool *w)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_2);
    memprot_ll_dram0_get_permissions(permissions, r, w);
}

static inline void memprot_ll_dram0_get_pms_area_3(bool *r, bool *w)
{
    uint32_t permissions = REG_GET_FIELD(SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_1_REG, SENSITIVE_CORE_X_DRAM0_PMS_CONSTRAIN_SRAM_WORLD_0_PMS_3);
    memprot_ll_dram0_get_permissions(permissions, r, w);
}


///////////////////////////////////
// DRAM0 - MONITOR

// lock
static inline memprot_hal_err_t memprot_ll_dram0_set_monitor_lock(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_WRITE(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_0_REG, 1);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_READ(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_0_REG) == 1) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_lock(const int core, bool* locked)
{
    switch (core) {
        case PRO_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_0_REG) == 1;
            break;
        case APP_CPU_NUM:
            *locked = REG_READ(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_0_REG) == 1;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// interrupt enable/clear
static inline memprot_hal_err_t memprot_ll_dram0_set_monitor_en(const int core, const bool enable)
{
    switch (core) {
        case PRO_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        case APP_CPU_NUM:
            if (enable) {
                REG_SET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_EN) > 0) && "Value not stored to required register");
#endif
            } else {
                REG_CLR_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_EN);
#ifdef PMS_DEBUG_ASSERTIONS
                HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_EN) == 0) && "Value not stored to required register");
#endif
            }
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_en(const int core, bool* enabled)
{
    switch (core) {
        case PRO_CPU_NUM:
            *enabled = REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_EN) > 0;
            break;
        case APP_CPU_NUM:
            *enabled = REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_EN) > 0;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_set_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_SET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_CLR) > 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_reset_monitor_intrclr(const int core)
{
    switch (core) {
        case PRO_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        case APP_CPU_NUM:
            REG_CLR_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_CLR);
#ifdef PMS_DEBUG_ASSERTIONS
            HAL_ASSERT((REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_CLR) == 0) && "Value not stored to required register");
#endif
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_intrclr(const int core, bool* cleared)
{
    switch (core) {
        case PRO_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        case APP_CPU_NUM:
            *cleared = REG_GET_BIT(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_CLR) > 0;
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_enable_register(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_1_REG);
            break;
        case APP_CPU_NUM:
            *regval = REG_READ(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_1_REG);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

// permission violation status
static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_status_intr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_INTR);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_INTR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_status_fault_world(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WORLD);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_status_fault_addr(const int core, void** addr)
{
    uint32_t reg_off;

    switch (core) {
        case PRO_CPU_NUM:
            reg_off = REG_GET_FIELD(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR);
            break;
        case APP_CPU_NUM:
            reg_off = REG_GET_FIELD(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_STATUS_ADDR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    *addr = (void*)(reg_off > 0 ? (reg_off << D_FAULT_ADDR_SHIFT) + DRAM0_VIOLATE_STATUS_ADDR_OFFSET : 0);

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_status_fault_wr(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_3_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WR);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_3_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_STATUS_WR);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

static inline memprot_hal_err_t memprot_ll_dram0_get_monitor_status_fault_byte_en(const int core, uint32_t* regval)
{
    switch (core) {
        case PRO_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_VIOLATE_STATUS_BYTEEN);
            break;
        case APP_CPU_NUM:
            *regval = REG_GET_FIELD(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_2_REG, SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_VIOLATE_STATUS_BYTEEN);
            break;
        default:
            return MEMP_HAL_ERR_CORE_INVALID;
    }

    return MEMP_HAL_OK;
}

#ifdef __cplusplus
}
#endif
