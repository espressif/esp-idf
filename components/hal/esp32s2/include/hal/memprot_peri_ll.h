/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/memprot_defs.h"
#include "hal/memprot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ========================================================================================
 * === PeriBus1 common
 * ========================================================================================
 */
static inline void memprot_ll_peri1_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
}

static inline uint32_t memprot_ll_peri1_get_intr_source_num(void)
{
    return ETS_PMS_PRO_DPORT_ILG_INTR_SOURCE;
}

static inline void memprot_ll_peri1_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
    }
}

static inline uint32_t memprot_ll_peri1_get_ctrl_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_6_REG);
}

static inline uint32_t memprot_ll_peri1_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_7_REG);
}

static inline void memprot_ll_peri1_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = memprot_ll_peri1_get_fault_reg();
    //*op_type = (uint32_t)status_bits & PERI1_INTR_ST_OP_RW_BIT;
    *op_type = 0;
    //! DPORT_PMS_PRO_DPORT_7_REG is missing op_type bit
    *op_subtype = (uint32_t)status_bits & PERI1_INTR_ST_OP_TYPE_BIT;
}

static inline bool memprot_ll_peri1_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_7_REG, DPORT_PMS_PRO_DPORT_ILG_INTR) > 0;
}

static inline uint32_t memprot_ll_peri1_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
}

static inline uint32_t memprot_ll_peri1_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_INTR);
}

static inline uint32_t memprot_ll_peri1_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
}

static inline uint32_t memprot_ll_peri1_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_0_REG);
}

//resets automatically on CPU restart
static inline void memprot_ll_peri1_set_lock(void)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DPORT_0_REG, DPORT_PMS_PRO_DPORT_LOCK);
}

static inline uint32_t memprot_ll_peri1_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_0_REG, DPORT_PMS_PRO_DPORT_LOCK);
}

/**
 * ========================================================================================
 * === PeriBus1 RTC SLOW
 * ========================================================================================
 */
static inline intptr_t memprot_ll_peri1_rtcslow_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_peri1_get_fault_reg();
    uint32_t fault_address = (status_bits & PERI1_INTR_ST_FAULTADDR_M) >> PERI1_INTR_ST_FAULTADDR_S;
    uint32_t high_bits = (status_bits & PERI1_INTR_ST_OP_HIGH_BITS) ? PERI1_RTCSLOW_INTR_ST_FAULTADDR_HI_0 : 0;
    return (intptr_t)(fault_address | high_bits);
}

static inline bool memprot_ll_peri1_rtcslow_is_intr_mine(void)
{
    if (memprot_ll_peri1_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_peri1_rtcslow_get_fault_address();
        return faulting_address >= PERI1_RTCSLOW_ADDRESS_LOW && faulting_address <= PERI1_RTCSLOW_ADDRESS_HIGH;
    }
    return false;
}

static inline memprot_hal_err_t memprot_ll_peri1_rtcslow_set_prot(uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    uint32_t addr = (uint32_t)split_addr;

    //check corresponding range fit & aligment to 32bit boundaries
    if (addr < PERI1_RTCSLOW_ADDRESS_LOW || addr > PERI1_RTCSLOW_ADDRESS_HIGH) {
        return MEMP_HAL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    uint32_t reg_split_addr = PERI1_RTCSLOW_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_DPORT_RTCSLOW_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_DPORT_RTCSLOW_L_R;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_DPORT_RTCSLOW_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_DPORT_RTCSLOW_H_R;
    }

    //write PERIBUS1 RTC SLOW cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DPORT_1_REG, reg_split_addr | permission_mask);

    return MEMP_HAL_OK;
}

static inline void memprot_ll_peri1_rtcslow_get_split_sgnf_bits(bool *lw, bool *lr, bool *hw, bool *hr)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_R);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_R);
}

static inline void memprot_ll_peri1_rtcslow_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_peri1_rtcslow_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_W, hw ? 1 : 0);
}

static inline uint32_t memprot_ll_peri1_rtcslow_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_1_REG);
}

/**
 * ========================================================================================
 * === PeriBus2 common
 * ========================================================================================
 */
static inline void memprot_ll_peri2_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
}

static inline uint32_t memprot_ll_peri2_get_intr_source_num(void)
{
    return ETS_PMS_PRO_AHB_ILG_INTR_SOURCE;
}

static inline void memprot_ll_peri2_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
    }
}

static inline uint32_t memprot_ll_peri2_get_ctrl_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_3_REG);
}

static inline uint32_t memprot_ll_peri2_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_4_REG);
}

static inline void memprot_ll_peri2_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = memprot_ll_peri2_get_fault_reg();
    *op_type = (uint32_t)status_bits & PERI2_INTR_ST_OP_RW_BIT;
    *op_subtype = (uint32_t)status_bits & PERI2_INTR_ST_OP_TYPE_BIT;
}

static inline bool memprot_ll_peri2_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_INTR) > 0;
}

static inline uint32_t memprot_ll_peri2_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
}

static inline uint32_t memprot_ll_peri2_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_INTR);
}

static inline uint32_t memprot_ll_peri2_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
}

static inline uint32_t memprot_ll_peri2_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_0_REG);
}

//resets automatically on CPU restart
static inline void memprot_ll_peri2_set_lock(void)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_AHB_0_REG, DPORT_PMS_PRO_AHB_LOCK);
}

static inline uint32_t memprot_ll_peri2_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_0_REG, DPORT_PMS_PRO_AHB_LOCK);
}

static inline intptr_t memprot_ll_peri2_rtcslow_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_peri2_get_fault_reg();
    return (intptr_t)(status_bits & PERI2_INTR_ST_FAULTADDR_M);
}

/**
 * ========================================================================================
 * === PeriBus2 RTC SLOW 0 (AHB0)
 * ========================================================================================
 */
static inline bool memprot_ll_peri2_rtcslow_0_is_intr_mine(void)
{
    if (memprot_ll_peri2_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_peri2_rtcslow_get_fault_address();
        return faulting_address >= PERI2_RTCSLOW_0_ADDRESS_LOW && faulting_address <= PERI2_RTCSLOW_0_ADDRESS_HIGH;
    }
    return false;
}

static inline memprot_hal_err_t memprot_ll_peri2_rtcslow_0_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;

    //check corresponding range fit & aligment to 32bit boundaries
    if (addr < PERI2_RTCSLOW_0_ADDRESS_LOW || addr > PERI2_RTCSLOW_0_ADDRESS_HIGH) {
        return MEMP_HAL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    uint32_t reg_split_addr = PERI2_RTCSLOW_0_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_L_R;
    }
    if (lx) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_L_F;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_H_R;
    }
    if (hx) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_0_H_F;
    }

    //write PERIBUS1 RTC SLOW cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_AHB_1_REG, reg_split_addr | permission_mask);

    return MEMP_HAL_OK;
}

static inline void memprot_ll_peri2_rtcslow_0_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_F);
}

static inline void memprot_ll_peri2_rtcslow_0_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_peri2_rtcslow_0_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_W, hw ? 1 : 0);
}

static inline void memprot_ll_peri2_rtcslow_0_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_F, hx ? 1 : 0);
}

static inline uint32_t memprot_ll_peri2_rtcslow_0_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_1_REG);
}

/**
 * ========================================================================================
 * === PeriBus2 RTC SLOW 1 (AHB1)
 * ========================================================================================
 */
static inline bool memprot_ll_peri2_rtcslow_1_is_intr_mine(void)
{
    if (memprot_ll_peri2_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_peri2_rtcslow_get_fault_address();
        return faulting_address >= PERI2_RTCSLOW_1_ADDRESS_LOW && faulting_address <= PERI2_RTCSLOW_1_ADDRESS_HIGH;
    }
    return false;
}

static inline memprot_hal_err_t memprot_ll_peri2_rtcslow_1_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;

    //check corresponding range fit & aligment to 32bit boundaries
    if (addr < PERI2_RTCSLOW_1_ADDRESS_LOW || addr > PERI2_RTCSLOW_1_ADDRESS_HIGH) {
        return MEMP_HAL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    uint32_t reg_split_addr = PERI2_RTCSLOW_1_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_L_R;
    }
    if (lx) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_L_F;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_H_R;
    }
    if (hx) {
        permission_mask |= DPORT_PMS_PRO_AHB_RTCSLOW_1_H_F;
    }

    //write PERIBUS1 RTC SLOW cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_AHB_2_REG, reg_split_addr | permission_mask);

    return MEMP_HAL_OK;
}

static inline void memprot_ll_peri2_rtcslow_1_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_F);
}

static inline void memprot_ll_peri2_rtcslow_1_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_peri2_rtcslow_1_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_W, hw ? 1 : 0);
}

static inline void memprot_ll_peri2_rtcslow_1_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_F, hx ? 1 : 0);
}

static inline uint32_t memprot_ll_peri2_rtcslow_1_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_2_REG);
}

#ifdef __cplusplus
}
#endif
