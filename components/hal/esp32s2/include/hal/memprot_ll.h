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

#include <stdbool.h>
#include "soc/memprot_defs.h"
#include "hal/memprot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ========================================================================================
 * === IRAM0 common
 * ========================================================================================
 */
static inline void memprot_ll_iram0_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_CLR);
}

static inline uint32_t memprot_ll_iram0_get_intr_source_num(void)
{
    return ETS_PMS_PRO_IRAM0_ILG_INTR_SOURCE;
}

static inline void memprot_ll_iram0_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_EN);
    }
}

static inline uint32_t memprot_ll_iram0_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_4_REG);
}

static inline uint32_t memprot_ll_iram0_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_5_REG);
}

static inline void memprot_ll_iram0_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = memprot_ll_iram0_get_fault_reg();
    *op_type = (uint32_t)status_bits & IRAM0_INTR_ST_OP_RW_BIT;
    *op_subtype = (uint32_t)status_bits & IRAM0_INTR_ST_OP_TYPE_BIT;
}

static inline bool memprot_ll_iram0_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_INTR) > 0;
}

static inline uint32_t memprot_ll_iram0_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_EN);
}

static inline uint32_t memprot_ll_iram0_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_INTR);
}

static inline uint32_t memprot_ll_iram0_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_4_REG, DPORT_PMS_PRO_IRAM0_ILG_CLR);
}

//resets automatically on CPU restart
static inline void memprot_ll_iram0_set_lock(void)
{
    DPORT_WRITE_PERI_REG( DPORT_PMS_PRO_IRAM0_0_REG, DPORT_PMS_PRO_IRAM0_LOCK);
}

static inline uint32_t memprot_ll_iram0_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_0_REG);
}

static inline uint32_t memprot_ll_iram0_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_0_REG, DPORT_PMS_PRO_IRAM0_LOCK);
}

/**
 * ========================================================================================
 * === IRAM0 SRAM
 * ========================================================================================
 */
static inline intptr_t memprot_ll_iram0_sram_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_iram0_get_fault_reg();
    return (intptr_t)((status_bits & IRAM0_INTR_ST_FAULTADDR_M) | IRAM0_SRAM_INTR_ST_FAULTADDR_HI);
}

static inline bool memprot_ll_iram0_sram_is_intr_mine(void)
{
    if (memprot_ll_iram0_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_iram0_sram_get_fault_address();
        return faulting_address >= IRAM0_SRAM_ADDRESS_LOW && faulting_address <= IRAM0_SRAM_ADDRESS_HIGH;
    }
    return false;
}

//block 0-3
static inline bool memprot_ll_iram0_sram_set_uni_block_perm(uint32_t block, bool write_perm, bool read_perm, bool exec_perm)
{
    uint32_t write_bit, read_bit, exec_bit;

    switch (block) {
    case IRAM0_SRAM_UNI_BLOCK_0:
        write_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_W;
        read_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_R;
        exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_1:
        write_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_W;
        read_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_R;
        exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_2:
        write_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_W;
        read_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_R;
        exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_3:
        write_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_W;
        read_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_R;
        exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_F;
        break;
    default:
        return false;
    }

    if (write_perm) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, write_bit);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, write_bit);
    }

    if (read_perm) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, read_bit);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, read_bit);
    }

    if (exec_perm) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, exec_bit);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_IRAM0_1_REG, exec_bit);
    }

    return true;
}

static inline bool memprot_ll_iram0_sram_get_uni_block_read_bit(uint32_t block, uint32_t *read_bit)
{
    switch (block) {
    case IRAM0_SRAM_UNI_BLOCK_0:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_0_R);
        break;
    case IRAM0_SRAM_UNI_BLOCK_1:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_1_R);
        break;
    case IRAM0_SRAM_UNI_BLOCK_2:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_2_R);
        break;
    case IRAM0_SRAM_UNI_BLOCK_3:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_3_R);
        break;
    default:
        return false;
    }

    return true;
}

static inline bool memprot_ll_iram0_sram_get_uni_block_write_bit(uint32_t block, uint32_t *write_bit)
{
    switch (block) {
    case IRAM0_SRAM_UNI_BLOCK_0:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_0_W);
        break;
    case IRAM0_SRAM_UNI_BLOCK_1:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_1_W);
        break;
    case IRAM0_SRAM_UNI_BLOCK_2:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_2_W);
        break;
    case IRAM0_SRAM_UNI_BLOCK_3:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_3_W);
        break;
    default:
        return false;
    }

    return true;
}

static inline bool memprot_ll_iram0_sram_get_uni_block_exec_bit(uint32_t block, uint32_t *exec_bit)
{
    switch (block) {
    case IRAM0_SRAM_UNI_BLOCK_0:
        *exec_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_0_F);
        break;
    case IRAM0_SRAM_UNI_BLOCK_1:
        *exec_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_1_F);
        break;
    case IRAM0_SRAM_UNI_BLOCK_2:
        *exec_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_2_F);
        break;
    case IRAM0_SRAM_UNI_BLOCK_3:
        *exec_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_1_REG, DPORT_PMS_PRO_IRAM0_SRAM_3_F);
        break;
    default:
        return false;
    }

    return true;
}

static inline bool memprot_ll_iram0_sram_get_uni_block_sgnf_bits(uint32_t block, uint32_t *write_bit, uint32_t *read_bit, uint32_t *exec_bit)
{
    switch (block) {
    case IRAM0_SRAM_UNI_BLOCK_0:
        *write_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_W;
        *read_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_R;
        *exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_0_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_1:
        *write_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_W;
        *read_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_R;
        *exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_1_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_2:
        *write_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_W;
        *read_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_R;
        *exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_2_F;
        break;
    case IRAM0_SRAM_UNI_BLOCK_3:
        *write_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_W;
        *read_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_R;
        *exec_bit = DPORT_PMS_PRO_IRAM0_SRAM_3_F;
        break;
    default:
        return false;
    }

    return true;
}

static inline uint32_t memprot_ll_iram0_sram_get_perm_uni_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_1_REG);
}

static inline uint32_t memprot_ll_iram0_sram_get_perm_split_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_2_REG);
}

static inline memprot_ll_err_t memprot_ll_iram0_sram_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;

    //sanity check: split address required above unified mgmt region & 32bit aligned
    if (addr > IRAM0_SRAM_SPL_BLOCK_HIGH) {
        return MEMP_LL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    //find possible split.address in low region blocks
    int uni_blocks_low = -1;
    if (addr >= IRAM0_SRAM_UNI_BLOCK_0_LOW) {
        uni_blocks_low++;
    }
    if (addr >= IRAM0_SRAM_UNI_BLOCK_1_LOW) {
        uni_blocks_low++;
    }
    if (addr >= IRAM0_SRAM_UNI_BLOCK_2_LOW) {
        uni_blocks_low++;
    }
    if (addr >= IRAM0_SRAM_UNI_BLOCK_3_LOW) {
        uni_blocks_low++;
    }

    //unified mgmt settings per block (bits W/R/X: [11:9] bl3, [8:6] bl2, [5:3] bl1, [2:0] bl0)
    uint32_t write_bit, read_bit, exec_bit;
    uint32_t uni_block_perm = 0;

    for (int x = 0; x < IRAM0_SRAM_TOTAL_UNI_BLOCKS; x++) {
        if (!memprot_ll_iram0_sram_get_uni_block_sgnf_bits(x, &write_bit, &read_bit, &exec_bit)) {
            return MEMP_LL_ERR_UNI_BLOCK_INVALID;
        }
        if (x <= uni_blocks_low) {
            if (lw) {
                uni_block_perm |= write_bit;
            }
            if (lr) {
                uni_block_perm |= read_bit;
            }
            if (lx) {
                uni_block_perm |= exec_bit;
            }
        } else {
            if (hw) {
                uni_block_perm |= write_bit;
            }
            if (hr) {
                uni_block_perm |= read_bit;
            }
            if (hx) {
                uni_block_perm |= exec_bit;
            }
        }
    }

    //if splt.ddr not set yet, do required normalization to make the addr writeble into splt.mgmt cfg register
    uint32_t reg_split_addr = 0;

    if (addr >= IRAM0_SRAM_SPL_BLOCK_LOW) {
        reg_split_addr = IRAM0_SRAM_ADDR_TO_CONF_REG(addr); //cfg reg - [16:0]
    }

    //prepare high & low permission mask (bits: [22:20] high range, [19:17] low range)
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_L_R;
    }
    if (lx) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_L_F;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_H_R;
    }
    if (hx) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_SRAM_4_H_F;
    }

    //write IRAM SRAM uni & splt cfg. registers
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_IRAM0_1_REG, uni_block_perm);
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_IRAM0_2_REG, (uint32_t)(reg_split_addr | permission_mask));

    return MEMP_LL_OK;
}

static inline void memprot_ll_iram0_sram_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_F);
}

static inline void memprot_ll_iram0_sram_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_iram0_sram_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_W, hw ? 1 : 0);
}

static inline void memprot_ll_iram0_sram_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_2_REG, DPORT_PMS_PRO_IRAM0_SRAM_4_H_F, hx ? 1 : 0);
}


/**
 * ========================================================================================
 * === IRAM0 RTC FAST
 * ========================================================================================
 */
static inline intptr_t memprot_ll_iram0_rtcfast_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_iram0_get_fault_reg();
    return (intptr_t)((status_bits & IRAM0_INTR_ST_FAULTADDR_M) | IRAM0_RTCFAST_INTR_ST_FAULTADDR_HI);
}

static inline bool memprot_ll_iram0_rtcfast_is_intr_mine(void)
{
    if (memprot_ll_iram0_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_iram0_rtcfast_get_fault_address();
        return faulting_address >= IRAM0_RTCFAST_ADDRESS_LOW && faulting_address <= IRAM0_RTCFAST_ADDRESS_HIGH;
    }
    return false;
}

static inline uint32_t memprot_ll_iram0_rtcfast_get_perm_split_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_IRAM0_3_REG);
}

static inline memprot_ll_err_t memprot_ll_iram0_rtcfast_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;

    //32bit aligned
    if (addr < IRAM0_RTCFAST_ADDRESS_LOW || addr > IRAM0_RTCFAST_ADDRESS_HIGH) {
        return MEMP_LL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    //conf reg [10:0]
    uint32_t reg_split_addr = IRAM0_RTCFAST_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask (bits: [16:14] high range, [13:11] low range)
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_L_R;
    }
    if (lx) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_L_F;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_H_R;
    }
    if (hx) {
        permission_mask |= DPORT_PMS_PRO_IRAM0_RTCFAST_H_F;
    }

    //write IRAM0 RTCFAST cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_IRAM0_3_REG, reg_split_addr | permission_mask);

    return MEMP_LL_OK;
}

static inline void memprot_ll_iram0_rtcfast_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_F);
}

static inline void memprot_ll_iram0_rtcfast_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_iram0_rtcfast_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_W, hw ? 1 : 0);
}

static inline void memprot_ll_iram0_rtcfast_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_IRAM0_3_REG, DPORT_PMS_PRO_IRAM0_RTCFAST_H_F, hx ? 1 : 0);
}


/**
 * ========================================================================================
 * === DRAM0 common
 * ========================================================================================
 */
static inline uint32_t memprot_ll_dram0_get_intr_source_num(void)
{
    return ETS_PMS_PRO_DRAM0_ILG_INTR_SOURCE;
}

static inline void memprot_ll_dram0_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_EN);
    }
}

static inline bool memprot_ll_dram0_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_INTR) > 0;
}

static inline void memprot_ll_dram0_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_CLR);
}

static inline uint32_t memprot_ll_dram0_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_EN);
}

static inline uint32_t memprot_ll_dram0_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_INTR);
}

static inline uint32_t memprot_ll_dram0_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_3_REG, DPORT_PMS_PRO_DRAM0_ILG_CLR);
}

//lock resets automatically on CPU restart
static inline void memprot_ll_dram0_set_lock(void)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DRAM0_0_REG, DPORT_PMS_PRO_DRAM0_LOCK);
}

static inline uint32_t memprot_ll_dram0_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DRAM0_0_REG);
}

static inline uint32_t memprot_ll_dram0_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_0_REG, DPORT_PMS_PRO_DRAM0_LOCK);
}

static inline uint32_t memprot_ll_dram0_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DRAM0_3_REG);
}

static inline uint32_t memprot_ll_dram0_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DRAM0_4_REG);
}

static inline void memprot_ll_dram0_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = memprot_ll_dram0_get_fault_reg();
    *op_type = status_bits & DRAM0_INTR_ST_OP_RW_BIT;
    *op_subtype = status_bits & DRAM0_INTR_ST_OP_ATOMIC_BIT;
}

/**
 * ========================================================================================
 * === DRAM0 SRAM
 * ========================================================================================
 */
static inline intptr_t memprot_ll_dram0_sram_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_dram0_get_fault_reg();
    return (intptr_t)(((status_bits & DRAM0_INTR_ST_FAULTADDR_M) >> DRAM0_INTR_ST_FAULTADDR_S) | DRAM0_SRAM_INTR_ST_FAULTADDR_HI);
}

static inline bool memprot_ll_dram0_sram_is_intr_mine(void)
{
    if (memprot_ll_dram0_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_dram0_sram_get_fault_address();
        return faulting_address >= DRAM0_SRAM_ADDRESS_LOW && faulting_address <= DRAM0_SRAM_ADDRESS_HIGH;
    }
    return false;
}

static inline bool memprot_ll_dram0_sram_get_uni_block_sgnf_bits(uint32_t block, uint32_t *write_bit, uint32_t *read_bit)
{
    switch (block) {
    case DRAM0_SRAM_UNI_BLOCK_0:
        *write_bit = DPORT_PMS_PRO_DRAM0_SRAM_0_W;
        *read_bit = DPORT_PMS_PRO_DRAM0_SRAM_0_R;
        break;
    case DRAM0_SRAM_UNI_BLOCK_1:
        *write_bit = DPORT_PMS_PRO_DRAM0_SRAM_1_W;
        *read_bit = DPORT_PMS_PRO_DRAM0_SRAM_1_R;
        break;
    case DRAM0_SRAM_UNI_BLOCK_2:
        *write_bit = DPORT_PMS_PRO_DRAM0_SRAM_2_W;
        *read_bit = DPORT_PMS_PRO_DRAM0_SRAM_2_R;
        break;
    case DRAM0_SRAM_UNI_BLOCK_3:
        *write_bit = DPORT_PMS_PRO_DRAM0_SRAM_3_W;
        *read_bit = DPORT_PMS_PRO_DRAM0_SRAM_3_R;
        break;
    default:
        return false;
    }

    return true;
}

static inline memprot_ll_err_t memprot_ll_dram0_sram_set_uni_block_perm(uint32_t block, bool write_perm, bool read_perm)
{
    //get block-specific WR flags offset within the conf.register
    uint32_t write_bit_offset, read_bit_offset;
    if (!memprot_ll_dram0_sram_get_uni_block_sgnf_bits(block, &write_bit_offset, &read_bit_offset)) {
        return MEMP_LL_ERR_UNI_BLOCK_INVALID;
    }

    //set/reset required flags
    if (write_perm) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_1_REG, write_bit_offset);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_1_REG, write_bit_offset);
    }

    if (read_perm) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_1_REG, read_bit_offset);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DRAM0_1_REG, read_bit_offset);
    }

    return MEMP_LL_OK;
}

static inline bool memprot_ll_dram0_sram_get_uni_block_read_bit(uint32_t block, uint32_t *read_bit)
{
    switch (block) {
    case DRAM0_SRAM_UNI_BLOCK_0:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_0_R);
        break;
    case DRAM0_SRAM_UNI_BLOCK_1:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_1_R);
        break;
    case DRAM0_SRAM_UNI_BLOCK_2:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_2_R);
        break;
    case DRAM0_SRAM_UNI_BLOCK_3:
        *read_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_3_R);
        break;
    default:
        return false;
    }

    return true;
}

static inline bool memprot_ll_dram0_sram_get_uni_block_write_bit(uint32_t block, uint32_t *write_bit)
{
    switch (block) {
    case DRAM0_SRAM_UNI_BLOCK_0:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_0_W);
        break;
    case DRAM0_SRAM_UNI_BLOCK_1:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_1_W);
        break;
    case DRAM0_SRAM_UNI_BLOCK_2:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_2_W);
        break;
    case DRAM0_SRAM_UNI_BLOCK_3:
        *write_bit = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_3_W);
        break;
    default:
        return false;
    }

    return true;
}

//DRAM0 has both unified blocks and split address configured in 1 register
static inline uint32_t memprot_ll_dram0_sram_get_perm_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DRAM0_1_REG);
}

static inline memprot_ll_err_t memprot_ll_dram0_sram_set_prot(uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    uint32_t addr = (uint32_t)split_addr;

    //low boundary check provided by LD script. see comment in memprot_ll_iram0_sram_set_prot()
    if (addr > DRAM0_SRAM_SPL_BLOCK_HIGH) {
        return MEMP_LL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    //set low region
    int uni_blocks_low = -1;
    if (addr >= DRAM0_SRAM_UNI_BLOCK_0_LOW) {
        uni_blocks_low++;
    }
    if (addr >= DRAM0_SRAM_UNI_BLOCK_1_LOW) {
        uni_blocks_low++;
    }
    if (addr >= DRAM0_SRAM_UNI_BLOCK_2_LOW) {
        uni_blocks_low++;
    }
    if (addr >= DRAM0_SRAM_UNI_BLOCK_3_LOW) {
        uni_blocks_low++;
    }

    //set unified mgmt region
    uint32_t write_bit, read_bit, uni_block_perm = 0;
    for (int x = 0; x < DRAM0_SRAM_TOTAL_UNI_BLOCKS; x++) {
        if (!memprot_ll_dram0_sram_get_uni_block_sgnf_bits(x, &write_bit, &read_bit)) {
            return MEMP_LL_ERR_UNI_BLOCK_INVALID;
        }
        if (x <= uni_blocks_low) {
            if (lw) {
                uni_block_perm |= write_bit;
            }
            if (lr) {
                uni_block_perm |= read_bit;
            }
        } else {
            if (hw) {
                uni_block_perm |= write_bit;
            }
            if (hr) {
                uni_block_perm |= read_bit;
            }
        }
    }

    //conf reg [24:8]
    uint32_t reg_split_addr = DRAM0_SRAM_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_SRAM_4_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_SRAM_4_L_R;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_SRAM_4_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_SRAM_4_H_R;
    }

    //write DRAM0 SRAM cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DRAM0_1_REG, reg_split_addr | permission_mask | uni_block_perm);

    return MEMP_LL_OK;
}

static inline void memprot_ll_dram0_sram_get_split_sgnf_bits(bool *lw, bool *lr, bool *hw, bool *hr)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_L_R);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_H_R);
}

static inline void memprot_ll_dram0_sram_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_dram0_sram_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_1_REG, DPORT_PMS_PRO_DRAM0_SRAM_4_H_W, hw ? 1 : 0);
}


/**
 * ========================================================================================
 * === DRAM0 RTC FAST
 * ========================================================================================
 */
static inline intptr_t memprot_ll_dram0_rtcfast_get_fault_address(void)
{
    uint32_t status_bits = memprot_ll_dram0_get_fault_reg();
    return (intptr_t)(((status_bits & DRAM0_INTR_ST_FAULTADDR_M) >> DRAM0_INTR_ST_FAULTADDR_S) | DRAM0_RTCFAST_INTR_ST_FAULTADDR_HI);
}

static inline bool memprot_ll_dram0_rtcfast_is_intr_mine(void)
{
    if (memprot_ll_dram0_is_assoc_intr()) {
        uint32_t faulting_address = (uint32_t)memprot_ll_dram0_rtcfast_get_fault_address();
        return faulting_address >= DRAM0_RTCFAST_ADDRESS_LOW && faulting_address <= DRAM0_RTCFAST_ADDRESS_HIGH;
    }
    return false;
}

static inline memprot_ll_err_t memprot_ll_dram0_rtcfast_set_prot(uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    uint32_t addr = (uint32_t)split_addr;

    //addr: 32bit aligned, inside corresponding range
    if (addr < DRAM0_RTCFAST_ADDRESS_LOW || addr > DRAM0_RTCFAST_ADDRESS_HIGH) {
        return MEMP_LL_ERR_SPLIT_ADDR_INVALID;
    }
    if (addr % 0x4 != 0) {
        return MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED;
    }

    //conf reg [10:0]
    uint32_t reg_split_addr = DRAM0_RTCFAST_ADDR_TO_CONF_REG(addr);

    //prepare high & low permission mask
    uint32_t permission_mask = 0;
    if (lw) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_RTCFAST_L_W;
    }
    if (lr) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_RTCFAST_L_R;
    }
    if (hw) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_RTCFAST_H_W;
    }
    if (hr) {
        permission_mask |= DPORT_PMS_PRO_DRAM0_RTCFAST_H_R;
    }

    //write DRAM0 RTC FAST cfg register
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DRAM0_2_REG, reg_split_addr | permission_mask);

    return MEMP_LL_OK;
}

static inline void memprot_ll_dram0_rtcfast_get_split_sgnf_bits(bool *lw, bool *lr, bool *hw, bool *hr)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_L_R);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_H_R);
}

static inline uint32_t memprot_ll_dram0_rtcfast_get_perm_split_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DRAM0_2_REG);
}

static inline void memprot_ll_dram0_rtcfast_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_H_R, hr ? 1 : 0);
}

static inline void memprot_ll_dram0_rtcfast_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DRAM0_2_REG, DPORT_PMS_PRO_DRAM0_RTCFAST_H_W, hw ? 1 : 0);
}

#ifdef __cplusplus
}
#endif
