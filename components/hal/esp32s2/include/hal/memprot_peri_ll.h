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

#ifdef __cplusplus
extern "C" {
#endif

#define RTCSLOW_MEMORY_SIZE                 0x00002000

/**
 * ========================================================================================
 * === PeriBus1 common
 * ========================================================================================
 */
//PeriBus1 interrupt status bitmasks
#define PERI1_INTR_ST_OP_TYPE_BIT           BIT(4)      //0: non-atomic, 1: atomic
#define PERI1_INTR_ST_OP_HIGH_BITS          BIT(5)      //0: high bits = unchanged, 1: high bits = 0x03F40000
#define PERI1_INTR_ST_FAULTADDR_M           0x03FFFFC0  //(bits 25:6 in the reg)
#define PERI1_INTR_ST_FAULTADDR_S           0x4         //(bits 21:2 of real address)


static inline void esp_memprot_peri1_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
}

static inline uint32_t esp_memprot_peri1_get_intr_source_num(void)
{
    return ETS_PMS_PRO_DPORT_ILG_INTR_SOURCE;
}

static inline void esp_memprot_peri1_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
    }
}

static inline uint32_t esp_memprot_peri1_get_ctrl_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_6_REG);
}

static inline uint32_t esp_memprot_peri1_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_7_REG);
}

static inline void esp_memprot_peri1_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = esp_memprot_peri1_get_fault_reg();
    //*op_type = (uint32_t)status_bits & PERI1_INTR_ST_OP_RW_BIT;
    *op_type = 0;
    //! DPORT_PMS_PRO_DPORT_7_REG is missing op_type bit
    *op_subtype = (uint32_t)status_bits & PERI1_INTR_ST_OP_TYPE_BIT;
}

static inline bool esp_memprot_peri1_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_DPORT_7_REG, DPORT_PMS_PRO_DPORT_ILG_INTR) > 0;
}

static inline uint32_t esp_memprot_peri1_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_EN);
}

static inline uint32_t esp_memprot_peri1_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_INTR);
}

static inline uint32_t esp_memprot_peri1_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_CLR);
}

static inline uint32_t esp_memprot_peri1_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_0_REG);
}

//resets automatically on CPU restart
static inline void esp_memprot_peri1_set_lock(void)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_DPORT_0_REG, DPORT_PMS_PRO_DPORT_LOCK);
}

static inline uint32_t esp_memprot_peri1_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_0_REG, DPORT_PMS_PRO_DPORT_LOCK);
}


/**
 * ========================================================================================
 * === PeriBus1 RTC SLOW
 * ========================================================================================
 */
#define PERI1_RTCSLOW_ADDRESS_BASE              0x3F421000
#define PERI1_RTCSLOW_ADDRESS_LOW               PERI1_RTCSLOW_ADDRESS_BASE
#define PERI1_RTCSLOW_ADDRESS_HIGH              PERI1_RTCSLOW_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE
#define PERI1_RTCSLOW_INTR_ST_FAULTADDR_HI_0    0x3F400000

#define PERI1_RTCSLOW_ADDR_TO_CONF_REG(addr)    (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_DPORT_RTCSLOW_SPLTADDR) << DPORT_PMS_PRO_DPORT_RTCSLOW_SPLTADDR_S)

static inline uint32_t *esp_memprot_peri1_rtcslow_get_fault_address(void)
{
    uint32_t status_bits = esp_memprot_peri1_get_fault_reg();
    uint32_t fault_address = (status_bits & PERI1_INTR_ST_FAULTADDR_M) >> PERI1_INTR_ST_FAULTADDR_S;
    uint32_t high_bits = (status_bits & PERI1_INTR_ST_OP_HIGH_BITS) ? PERI1_RTCSLOW_INTR_ST_FAULTADDR_HI_0 : 0;
    return (uint32_t *)(fault_address | high_bits);
}

static inline bool esp_memprot_peri1_rtcslow_is_intr_mine(void)
{
    if (esp_memprot_dram0_is_assoc_intr()) {
        uint32_t *faulting_address = esp_memprot_peri1_rtcslow_get_fault_address();
        return (uint32_t)faulting_address >= PERI1_RTCSLOW_ADDRESS_LOW && (uint32_t)faulting_address <= PERI1_RTCSLOW_ADDRESS_HIGH;
    }
    return false;
}

static inline void esp_memprot_peri1_rtcslow_set_prot(uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    uint32_t addr = (uint32_t)split_addr;
    assert( addr % 0x4 == 0 );

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
}

static inline void esp_memprot_peri1_rtcslow_get_split_sgnf_bits(bool *lw, bool *lr, bool *hw, bool *hr)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_R);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_R);
}

static inline void esp_memprot_peri1_rtcslow_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_R, hr ? 1 : 0);
}

static inline void esp_memprot_peri1_rtcslow_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_DPORT_1_REG, DPORT_PMS_PRO_DPORT_RTCSLOW_H_W, hw ? 1 : 0);
}

static inline uint32_t esp_memprot_peri1_rtcslow_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_1_REG);
}


/**
 * ========================================================================================
 * === PeriBus2 common
 * ========================================================================================
 */
//PeriBus2 interrupt status bitmasks
#define PERI2_INTR_ST_OP_TYPE_BIT           BIT(1)      //instruction: 0, data: 1
#define PERI2_INTR_ST_OP_RW_BIT             BIT(0)      //read: 0, write: 1
#define PERI2_INTR_ST_FAULTADDR_M           0xFFFFFFFC  //(bits 31:2 in the reg)

static inline void esp_memprot_peri2_clear_intr(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
}

static inline uint32_t esp_memprot_peri2_get_intr_source_num(void)
{
    return ETS_PMS_PRO_AHB_ILG_INTR_SOURCE;
}

static inline void esp_memprot_peri2_intr_ena(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
    }
}

static inline uint32_t esp_memprot_peri2_get_ctrl_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_3_REG);
}

static inline uint32_t esp_memprot_peri2_get_fault_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_4_REG);
}

static inline void esp_memprot_peri2_get_fault_op_type(uint32_t *op_type, uint32_t *op_subtype)
{
    uint32_t status_bits = esp_memprot_peri2_get_fault_reg();
    *op_type = (uint32_t)status_bits & PERI2_INTR_ST_OP_RW_BIT;
    *op_subtype = (uint32_t)status_bits & PERI2_INTR_ST_OP_TYPE_BIT;
}

static inline bool esp_memprot_peri2_is_assoc_intr(void)
{
    return DPORT_GET_PERI_REG_MASK(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_INTR) > 0;
}

static inline uint32_t esp_memprot_peri2_get_intr_ena_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_EN);
}

static inline uint32_t esp_memprot_peri2_get_intr_on_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_INTR);
}

static inline uint32_t esp_memprot_peri2_get_intr_clr_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG, DPORT_PMS_PRO_AHB_ILG_CLR);
}

static inline uint32_t esp_memprot_peri2_get_lock_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_AHB_0_REG);
}

//resets automatically on CPU restart
static inline void esp_memprot_peri2_set_lock(void)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_PRO_AHB_0_REG, DPORT_PMS_PRO_AHB_LOCK);
}

static inline uint32_t esp_memprot_peri2_get_lock_bit(void)
{
    return DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_0_REG, DPORT_PMS_PRO_AHB_LOCK);
}

static inline uint32_t *esp_memprot_peri2_rtcslow_get_fault_address(void)
{
    uint32_t status_bits = esp_memprot_peri2_get_fault_reg();
    return (uint32_t *)(status_bits & PERI2_INTR_ST_FAULTADDR_M);
}


/**
 * ========================================================================================
 * === PeriBus2 RTC SLOW 0 (AHB0)
 * ========================================================================================
 */
#define PERI2_RTCSLOW_0_ADDRESS_BASE             0x50000000
#define PERI2_RTCSLOW_0_ADDRESS_LOW              PERI2_RTCSLOW_0_ADDRESS_BASE
#define PERI2_RTCSLOW_0_ADDRESS_HIGH             PERI2_RTCSLOW_0_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE

#define PERI2_RTCSLOW_0_ADDR_TO_CONF_REG(addr)    (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_AHB_RTCSLOW_0_SPLTADDR) << DPORT_PMS_PRO_AHB_RTCSLOW_0_SPLTADDR_S)

static inline bool esp_memprot_peri2_rtcslow_0_is_intr_mine(void)
{
    if (esp_memprot_peri2_is_assoc_intr()) {
        uint32_t *faulting_address = esp_memprot_peri2_rtcslow_get_fault_address();
        return (uint32_t)faulting_address >= PERI2_RTCSLOW_0_ADDRESS_LOW && (uint32_t)faulting_address <= PERI2_RTCSLOW_0_ADDRESS_HIGH;
    }
    return false;
}

static inline void esp_memprot_peri2_rtcslow_0_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;
    assert( addr % 0x4 == 0 );

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
}

static inline void esp_memprot_peri2_rtcslow_0_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_F);
}

static inline void esp_memprot_peri2_rtcslow_0_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_R, hr ? 1 : 0);
}

static inline void esp_memprot_peri2_rtcslow_0_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_W, hw ? 1 : 0);
}

static inline void esp_memprot_peri2_rtcslow_0_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_1_REG, DPORT_PMS_PRO_AHB_RTCSLOW_0_H_F, hx ? 1 : 0);
}

static inline uint32_t esp_memprot_peri2_rtcslow_0_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_1_REG);
}

/**
 * ========================================================================================
 * === PeriBus2 RTC SLOW 1 (AHB1)
 * ========================================================================================
 */
#define PERI2_RTCSLOW_1_ADDRESS_BASE             0x60021000
#define PERI2_RTCSLOW_1_ADDRESS_LOW              PERI2_RTCSLOW_1_ADDRESS_BASE
#define PERI2_RTCSLOW_1_ADDRESS_HIGH             PERI2_RTCSLOW_1_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE

#define PERI2_RTCSLOW_1_ADDR_TO_CONF_REG(addr)   (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_AHB_RTCSLOW_1_SPLTADDR) << DPORT_PMS_PRO_AHB_RTCSLOW_1_SPLTADDR_S)

static inline bool esp_memprot_peri2_rtcslow_1_is_intr_mine(void)
{
    if (esp_memprot_peri2_is_assoc_intr()) {
        uint32_t *faulting_address = esp_memprot_peri2_rtcslow_get_fault_address();
        return (uint32_t)faulting_address >= PERI2_RTCSLOW_1_ADDRESS_LOW && (uint32_t)faulting_address <= PERI2_RTCSLOW_1_ADDRESS_HIGH;
    }
    return false;
}

static inline void esp_memprot_peri2_rtcslow_1_set_prot(uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    uint32_t addr = (uint32_t)split_addr;
    assert( addr % 0x4 == 0 );

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
}

static inline void esp_memprot_peri2_rtcslow_1_get_split_sgnf_bits(bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    *lw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_W);
    *lr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_R);
    *lx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_F);
    *hw = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_W);
    *hr = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_R);
    *hx = DPORT_REG_GET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_F);
}

static inline void esp_memprot_peri2_rtcslow_1_set_read_perm(bool lr, bool hr)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_R, lr ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_R, hr ? 1 : 0);
}

static inline void esp_memprot_peri2_rtcslow_1_set_write_perm(bool lw, bool hw)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_W, lw ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_W, hw ? 1 : 0);
}

static inline void esp_memprot_peri2_rtcslow_1_set_exec_perm(bool lx, bool hx)
{
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_L_F, lx ? 1 : 0);
    DPORT_REG_SET_FIELD(DPORT_PMS_PRO_AHB_2_REG, DPORT_PMS_PRO_AHB_RTCSLOW_1_H_F, hx ? 1 : 0);
}

static inline uint32_t esp_memprot_peri2_rtcslow_1_get_conf_reg(void)
{
    return DPORT_READ_PERI_REG(DPORT_PMS_PRO_DPORT_2_REG);
}

#ifdef __cplusplus
}
#endif
