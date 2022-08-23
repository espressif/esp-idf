/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* INTERNAL API
 * implementation of generic interface to MMU memory protection features
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "soc/sensitive_reg.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "hal/memprot_ll.h"
#include "hal/memprot_peri_ll.h"
#include "esp32s2/memprot.h"
#include "esp_fault.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"

extern int _iram_text_end;
extern int _data_start;
extern int _rtc_text_end;
extern int _rtc_dummy_end;

static inline esp_err_t esp_memprot_ll_err_to_esp_err(memprot_hal_err_t err)
{
    switch (err) {
    case MEMP_HAL_OK: return ESP_OK;
    case MEMP_HAL_FAIL: return ESP_FAIL;
    case MEMP_HAL_ERR_SPLIT_ADDR_INVALID: return ESP_ERR_INVALID_STATE;
    case MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED: return ESP_ERR_INVALID_SIZE;
    case MEMP_HAL_ERR_UNI_BLOCK_INVALID: return ESP_ERR_NOT_FOUND;
    default:
        return ESP_FAIL;
    }
}

uint32_t *esp_memprot_iram0_sram_get_min_split_addr(void)
{
    return (uint32_t *)&_iram_text_end;
}

uint32_t *esp_memprot_iram0_rtcfast_get_min_split_addr(void)
{
    return (uint32_t *)&_rtc_text_end;
}

uint32_t *esp_memprot_dram0_sram_get_min_split_addr(void)
{
    return (uint32_t *)&_data_start;
}

uint32_t *esp_memprot_dram0_rtcfast_get_min_split_addr(void)
{
    return (uint32_t *)&_rtc_dummy_end;
}

uint32_t *esp_memprot_peri1_rtcslow_get_min_split_addr(void)
{
    return (uint32_t *)PERI1_RTCSLOW_ADDRESS_BASE;
}

uint32_t *esp_memprot_peri2_rtcslow_0_get_min_split_addr(void)
{
    return (uint32_t *)PERI2_RTCSLOW_0_ADDRESS_BASE;
}

uint32_t *esp_memprot_peri2_rtcslow_1_get_min_split_addr(void)
{
    return (uint32_t *)PERI2_RTCSLOW_1_ADDRESS_BASE;
}

uint32_t *esp_memprot_get_split_addr(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_min_split_addr();
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_dram0_sram_get_min_split_addr();
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_rtcfast_get_min_split_addr();
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_rtcfast_get_min_split_addr();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_rtcslow_get_min_split_addr();
    case MEMPROT_PERI2_RTCSLOW_0:
        return esp_memprot_peri2_rtcslow_0_get_min_split_addr();
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_rtcslow_1_get_min_split_addr();
    default:
        return (uint32_t *)MEMPROT_INVALID_ADDRESS;
    }
}

const char *esp_memprot_type_to_str(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return "IRAM0_SRAM";
    case MEMPROT_DRAM0_SRAM:
        return "DRAM0_SRAM";
    case MEMPROT_IRAM0_RTCFAST:
        return "IRAM0_RTCFAST";
    case MEMPROT_DRAM0_RTCFAST:
        return "DRAM0_RTCFAST";
    case MEMPROT_PERI1_RTCSLOW:
        return "PERI1_RTCSLOW";
    case MEMPROT_PERI2_RTCSLOW_0:
        return "PERI2_RTCSLOW_0";
    case MEMPROT_PERI2_RTCSLOW_1:
        return "PERI2_RTCSLOW_1";
    default:
        return "INVALID_MEM_TYPE";
    }
}

esp_err_t esp_memprot_intr_init(mem_type_prot_t mem_type)
{
    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_iram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_dram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_peri1_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_peri2_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    ESP_INTR_ENABLE(ETS_MEMACCESS_ERR_INUM);

    return ESP_OK;
}

esp_err_t esp_memprot_intr_ena(mem_type_prot_t mem_type, bool enable)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_intr_ena(enable);
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_intr_ena(enable);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_intr_ena(enable);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_intr_ena(enable);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

mem_type_prot_t esp_memprot_get_active_intr_memtype()
{
    if (memprot_ll_iram0_sram_is_intr_mine()) {
        return MEMPROT_IRAM0_SRAM;
    } else if (memprot_ll_iram0_rtcfast_is_intr_mine()) {
        return MEMPROT_IRAM0_RTCFAST;
    } else if (memprot_ll_dram0_sram_is_intr_mine()) {
        return MEMPROT_DRAM0_SRAM;
    } else if (memprot_ll_dram0_rtcfast_is_intr_mine()) {
        return MEMPROT_DRAM0_RTCFAST;
    } else if (memprot_ll_peri1_rtcslow_is_intr_mine()) {
        return MEMPROT_PERI1_RTCSLOW;
    } else if (memprot_ll_peri2_rtcslow_0_is_intr_mine()) {
        return MEMPROT_PERI2_RTCSLOW_0;
    } else if (memprot_ll_peri2_rtcslow_1_is_intr_mine()) {
        return MEMPROT_PERI2_RTCSLOW_1;
    }

    return MEMPROT_NONE;
}

esp_err_t esp_memprot_clear_intr(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_clear_intr();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_clear_intr();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_clear_intr();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_clear_intr();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_lock(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_set_lock();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_set_lock();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_set_lock();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_set_lock();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_lock(mem_type_prot_t mem_type, bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *locked = memprot_ll_iram0_get_lock_bit() > 0;
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *locked = memprot_ll_dram0_get_lock_bit() > 0;
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *locked = memprot_ll_peri1_get_lock_bit() > 0;
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *locked = memprot_ll_peri2_get_lock_bit() > 0;
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

bool esp_memprot_is_locked_any()
{
    return
        memprot_ll_iram0_get_lock_bit() > 0 ||
        memprot_ll_dram0_get_lock_bit() > 0 ||
        memprot_ll_peri1_get_lock_bit() > 0 ||
        memprot_ll_peri2_get_lock_bit() > 0;
}

esp_err_t esp_memprot_get_conf_reg(mem_type_prot_t mem_type, uint32_t *conf_reg_val)
{
    if (conf_reg_val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *conf_reg_val = memprot_ll_iram0_get_conf_reg();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *conf_reg_val = memprot_ll_dram0_get_conf_reg();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *conf_reg_val = memprot_ll_peri1_rtcslow_get_conf_reg();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        *conf_reg_val = memprot_ll_peri2_rtcslow_0_get_conf_reg();
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        *conf_reg_val = memprot_ll_peri2_rtcslow_1_get_conf_reg();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_fault_reg(mem_type_prot_t mem_type, uint32_t *fault_reg_val)
{
    if (fault_reg_val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *fault_reg_val = memprot_ll_iram0_get_fault_reg();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *fault_reg_val = memprot_ll_dram0_get_fault_reg();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *fault_reg_val = memprot_ll_peri1_get_fault_reg();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *fault_reg_val = memprot_ll_peri2_get_fault_reg();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_fault_status(mem_type_prot_t mem_type, uint32_t **faulting_address, uint32_t *op_type, uint32_t *op_subtype)
{
    if (*faulting_address == NULL || op_type == NULL || op_subtype == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        *faulting_address = (uint32_t *)memprot_ll_iram0_sram_get_fault_address();
        memprot_ll_iram0_get_fault_op_type(op_type, op_subtype);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        *faulting_address = (uint32_t *)memprot_ll_iram0_rtcfast_get_fault_address();
        memprot_ll_iram0_get_fault_op_type(op_type, op_subtype);
        break;
    case MEMPROT_DRAM0_SRAM:
        *faulting_address = (uint32_t *)memprot_ll_dram0_sram_get_fault_address();
        memprot_ll_dram0_get_fault_op_type(op_type, op_subtype);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        *faulting_address = (uint32_t *)memprot_ll_dram0_rtcfast_get_fault_address();
        memprot_ll_dram0_get_fault_op_type(op_type, op_subtype);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *faulting_address = (uint32_t *)memprot_ll_peri1_rtcslow_get_fault_address();
        memprot_ll_peri1_get_fault_op_type(op_type, op_subtype);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *faulting_address = (uint32_t *)memprot_ll_peri2_rtcslow_get_fault_address();
        memprot_ll_peri2_get_fault_op_type(op_type, op_subtype);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

bool esp_memprot_is_intr_ena_any()
{
    return
        memprot_ll_iram0_get_intr_ena_bit() > 0 ||
        memprot_ll_dram0_get_intr_ena_bit() > 0 ||
        memprot_ll_peri1_get_intr_ena_bit() > 0 ||
        memprot_ll_peri2_get_intr_ena_bit() > 0;
}

esp_err_t esp_memprot_get_intr_ena_bit(mem_type_prot_t mem_type, uint32_t *enable_bit)
{
    if (enable_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *enable_bit = memprot_ll_iram0_get_intr_ena_bit();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *enable_bit = memprot_ll_dram0_get_intr_ena_bit();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *enable_bit = memprot_ll_peri1_get_intr_ena_bit();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *enable_bit =  memprot_ll_peri2_get_intr_ena_bit();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_intr_on_bit(mem_type_prot_t mem_type, uint32_t *intr_on_bit)
{
    if (intr_on_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *intr_on_bit = memprot_ll_iram0_get_intr_on_bit();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *intr_on_bit = memprot_ll_dram0_get_intr_on_bit();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *intr_on_bit = memprot_ll_peri1_get_intr_on_bit();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *intr_on_bit = memprot_ll_peri2_get_intr_on_bit();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_intr_clr_bit(mem_type_prot_t mem_type, uint32_t *clear_bit)
{
    if (clear_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        *clear_bit = memprot_ll_iram0_get_intr_clr_bit();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        *clear_bit = memprot_ll_dram0_get_intr_clr_bit();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *clear_bit = memprot_ll_peri1_get_intr_clr_bit();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *clear_bit = memprot_ll_peri2_get_intr_clr_bit();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_uni_block_read_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *read_bit)
{
    if (read_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        if (!memprot_ll_iram0_sram_get_uni_block_read_bit(block, read_bit)) {
            ret = ESP_ERR_NOT_FOUND;
        }
        break;
    case MEMPROT_DRAM0_SRAM:
        if (!memprot_ll_dram0_sram_get_uni_block_read_bit(block, read_bit)) {
            ret = ESP_ERR_NOT_FOUND;
        }
        break;
    default:
        ret = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return ret;
}

esp_err_t esp_memprot_get_uni_block_write_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *write_bit)
{
    if (write_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        if (!memprot_ll_iram0_sram_get_uni_block_write_bit(block, write_bit)) {
            ret = ESP_ERR_NOT_FOUND;
        }
        break;
    case MEMPROT_DRAM0_SRAM:
        if (!memprot_ll_dram0_sram_get_uni_block_write_bit(block, write_bit)) {
            ret = ESP_ERR_NOT_FOUND;
        }
        break;
    default:
        ret = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return ret;
}

esp_err_t esp_memprot_get_uni_block_exec_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *exec_bit)
{
    if (exec_bit == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        if (!memprot_ll_iram0_sram_get_uni_block_exec_bit(block, exec_bit)) {
            return ESP_ERR_NOT_FOUND;
        }
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_uni_block_perm_dram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm)
{
    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_ll_err_to_esp_err(memprot_ll_dram0_sram_set_uni_block_perm(block, write_perm, read_perm));
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t esp_memprot_get_perm_uni_reg(mem_type_prot_t mem_type, uint32_t *perm_reg)
{
    if (perm_reg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        *perm_reg = memprot_ll_iram0_sram_get_perm_uni_reg();
        break;
    case MEMPROT_DRAM0_SRAM:
        *perm_reg = memprot_ll_dram0_sram_get_perm_reg();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_perm_split_reg(mem_type_prot_t mem_type, uint32_t *split_reg)
{
    if (split_reg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        *split_reg = memprot_ll_iram0_sram_get_perm_split_reg();
        break;
    case MEMPROT_IRAM0_RTCFAST:
        *split_reg = memprot_ll_iram0_rtcfast_get_perm_split_reg();
        break;
    case MEMPROT_DRAM0_SRAM:
        *split_reg = memprot_ll_dram0_sram_get_perm_reg();
        break;
    case MEMPROT_DRAM0_RTCFAST:
        *split_reg = memprot_ll_dram0_rtcfast_get_perm_split_reg();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *split_reg = memprot_ll_peri1_rtcslow_get_conf_reg();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        *split_reg = memprot_ll_peri2_rtcslow_0_get_conf_reg();
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        *split_reg = memprot_ll_peri2_rtcslow_1_get_conf_reg();
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_prot_dram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_ll_err_to_esp_err(memprot_ll_dram0_sram_set_prot(split_addr != NULL ? split_addr : esp_memprot_dram0_sram_get_min_split_addr(), lw, lr, hw, hr));
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_ll_err_to_esp_err(memprot_ll_dram0_rtcfast_set_prot(split_addr != NULL ? split_addr : esp_memprot_dram0_rtcfast_get_min_split_addr(), lw, lr, hw, hr));
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t esp_memprot_set_uni_block_perm_iram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm, bool exec_perm)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        if (!memprot_ll_iram0_sram_set_uni_block_perm(block, write_perm, read_perm, exec_perm)) {
            return ESP_ERR_INVALID_ARG;
        }
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_prot_iram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_ll_err_to_esp_err(memprot_ll_iram0_sram_set_prot(split_addr != NULL ? split_addr : esp_memprot_iram0_sram_get_min_split_addr(), lw, lr, lx, hw, hr, hx));
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_ll_err_to_esp_err(memprot_ll_iram0_rtcfast_set_prot(split_addr != NULL ? split_addr : esp_memprot_iram0_rtcfast_get_min_split_addr(), lw, lr, lx, hw, hr, hx));
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t esp_memprot_get_perm_split_bits_iram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    if (lw == NULL || lr == NULL || lx == NULL || hw == NULL || hr == NULL || hx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_sram_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_rtcfast_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_perm_split_bits_dram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr)
{
    if (lw == NULL || lr == NULL || hw == NULL || hr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_sram_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_rtcfast_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_perm_split_bits_peri1(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr)
{
    if (lw == NULL || lr == NULL || hw == NULL || hr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_rtcslow_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_prot_peri1(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    switch (mem_type) {
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_ll_err_to_esp_err(
                   memprot_ll_peri1_rtcslow_set_prot(
                       split_addr != NULL ? split_addr : esp_memprot_peri1_rtcslow_get_min_split_addr(),
                       lw,
                       lr,
                       hw,
                       hr)
               );
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t esp_memprot_get_perm_split_bits_peri2(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    if (lw == NULL || lr == NULL || lx == NULL || hw == NULL || hr == NULL || hx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_PERI2_RTCSLOW_0:
        memprot_ll_peri2_rtcslow_0_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_rtcslow_1_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_prot_peri2(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    switch (mem_type) {
    case MEMPROT_PERI2_RTCSLOW_0:
        return esp_memprot_ll_err_to_esp_err(
                   memprot_ll_peri2_rtcslow_0_set_prot(
                       split_addr != NULL ? split_addr : esp_memprot_peri2_rtcslow_0_get_min_split_addr(),
                       lw,
                       lr,
                       lx,
                       hw,
                       hr,
                       hx)
               );
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_ll_err_to_esp_err(
                   memprot_ll_peri2_rtcslow_1_set_prot(
                       split_addr != NULL ? split_addr : esp_memprot_peri2_rtcslow_1_get_min_split_addr(),
                       lw,
                       lr,
                       lx,
                       hw,
                       hr,
                       hx)
               );
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask)
{
    esp_err_t ret;

    //any IRAM0/DRAM0 enable/disable call applies to all memory modules connected
    uint32_t required_mem_prot = mem_type_mask == NULL ? (uint32_t)MEMPROT_ALL : *mem_type_mask;
    bool use_iram0 = required_mem_prot & MEMPROT_IRAM0_SRAM || required_mem_prot & MEMPROT_IRAM0_RTCFAST;
    bool use_dram0 = required_mem_prot & MEMPROT_DRAM0_SRAM || required_mem_prot & MEMPROT_DRAM0_RTCFAST;
    bool use_peri1 = required_mem_prot & MEMPROT_PERI1_RTCSLOW;
    bool use_peri2 = required_mem_prot & MEMPROT_PERI2_RTCSLOW_0 || required_mem_prot & MEMPROT_PERI2_RTCSLOW_1;

    //disable protection
    if (use_iram0 && (ret = esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, false)) != ESP_OK) {
        return ret;
    }
    if (use_dram0 && (ret = esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, false)) != ESP_OK) {
        return ret;
    }
    if (use_peri1 && (ret = esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, false)) != ESP_OK) {
        return ret;
    }
    if (use_peri2 && (ret = esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, false)) != ESP_OK) {
        return ret;
    }

    //if being debugged check we are not glitched and dont enable Memprot
    if (esp_cpu_dbgr_is_attached()) {
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());
    } else {
        //initialize for specific buses (any memory type does the job)
        if (invoke_panic_handler) {
            if (use_iram0 && (ret = esp_memprot_intr_init(MEMPROT_IRAM0_SRAM)) != ESP_OK) {
                return ret;
            }
            if (use_dram0 && (ret = esp_memprot_intr_init(MEMPROT_DRAM0_SRAM)) != ESP_OK) {
                return ret;
            }
            if (use_peri1 && (ret = esp_memprot_intr_init(MEMPROT_PERI1_RTCSLOW)) != ESP_OK) {
                return ret;
            }
            if (use_peri2 && (ret = esp_memprot_intr_init(MEMPROT_PERI2_RTCSLOW_0)) != ESP_OK) {
                return ret;
            }
        }

        //set permissions
        if (required_mem_prot & MEMPROT_IRAM0_SRAM) {
            ret = esp_memprot_set_prot_iram(MEMPROT_IRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_IRAM0_RTCFAST) {
            ret = esp_memprot_set_prot_iram(MEMPROT_IRAM0_RTCFAST, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_DRAM0_SRAM) {
            ret = esp_memprot_set_prot_dram(MEMPROT_DRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_DRAM0_RTCFAST) {
            ret = esp_memprot_set_prot_dram(MEMPROT_DRAM0_RTCFAST, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_PERI1_RTCSLOW) {
            ret = esp_memprot_set_prot_peri1(MEMPROT_PERI1_RTCSLOW, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_DIS, WR_HIGH_DIS, RD_HIGH_DIS);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_0) {
            ret = esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_0, DEF_SPLIT_LINE, WR_LOW_ENA, RD_LOW_ENA, EX_LOW_DIS, WR_HIGH_ENA, RD_HIGH_ENA, EX_HIGH_DIS);
            if (ret != ESP_OK) {
                return ret;
            }
        }
        if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_1) {
            ret = esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_1, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_DIS, EX_LOW_DIS, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
            if (ret != ESP_OK) {
                return ret;
            }
        }

        //reenable protection (bus based)
        if (use_iram0 && (ret = esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, true)) != ESP_OK) {
            return ret;
        }
        if (use_dram0 && (ret = esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, true)) != ESP_OK) {
            return ret;
        }
        if (use_peri1 && (ret = esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, true)) != ESP_OK) {
            return ret;
        }
        if (use_peri2 && (ret = esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, true)) != ESP_OK) {
            return ret;
        }

        //lock if required (bus based)
        if (lock_feature) {
            if (use_iram0 && (ret = esp_memprot_set_lock(MEMPROT_IRAM0_SRAM)) != ESP_OK) {
                return ret;
            }
            if (use_dram0 && (ret = esp_memprot_set_lock(MEMPROT_DRAM0_SRAM)) != ESP_OK) {
                return ret;
            }
            if (use_peri1 && (ret = esp_memprot_set_lock(MEMPROT_PERI1_RTCSLOW)) != ESP_OK) {
                return ret;
            }
            if (use_peri2 && (ret = esp_memprot_set_lock(MEMPROT_PERI2_RTCSLOW_0)) != ESP_OK) {
                return ret;
            }
        }
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_permissions(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    if (lw == NULL || lr == NULL || lx == NULL || hw == NULL || hr == NULL || hx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_sram_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_sram_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_rtcfast_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_rtcfast_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_rtcslow_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        memprot_ll_peri2_rtcslow_0_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_rtcslow_1_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_get_perm_read(mem_type_prot_t mem_type, bool *lr, bool *hr)
{
    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_err_t ret = esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    if (ret == ESP_OK) {
        *lr = _lr;
        *hr = _hr;
    }
    return ret;
}

esp_err_t esp_memprot_get_perm_write(mem_type_prot_t mem_type, bool *lw, bool *hw)
{
    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_err_t ret = esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    if (ret == ESP_OK) {
        *lw = _lw;
        *hw = _hw;
    }

    return ret;
}

esp_err_t esp_memprot_get_perm_exec(mem_type_prot_t mem_type, bool *lx, bool *hx)
{
    if (mem_type == MEMPROT_DRAM0_SRAM ||
            mem_type == MEMPROT_DRAM0_RTCFAST ||
            mem_type == MEMPROT_PERI1_RTCSLOW) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_err_t ret = esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    if (ret == ESP_OK) {
        *lx = _lx;
        *hx = _hx;
    }

    return ret;
}

uint32_t esp_memprot_get_low_limit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return IRAM0_SRAM_ADDRESS_LOW;
    case MEMPROT_DRAM0_SRAM:
        return DRAM0_SRAM_ADDRESS_LOW;
    case MEMPROT_IRAM0_RTCFAST:
        return IRAM0_RTCFAST_ADDRESS_LOW;
    case MEMPROT_DRAM0_RTCFAST:
        return DRAM0_RTCFAST_ADDRESS_LOW;
    case MEMPROT_PERI1_RTCSLOW:
        return PERI1_RTCSLOW_ADDRESS_LOW;
    case MEMPROT_PERI2_RTCSLOW_0:
        return PERI2_RTCSLOW_0_ADDRESS_LOW;
    case MEMPROT_PERI2_RTCSLOW_1:
        return PERI2_RTCSLOW_1_ADDRESS_LOW;
    default:
        return MEMPROT_INVALID_ADDRESS;
    }
}

uint32_t esp_memprot_get_high_limit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return IRAM0_SRAM_ADDRESS_HIGH;
    case MEMPROT_DRAM0_SRAM:
        return DRAM0_SRAM_ADDRESS_HIGH;
    case MEMPROT_IRAM0_RTCFAST:
        return IRAM0_RTCFAST_ADDRESS_HIGH;
    case MEMPROT_DRAM0_RTCFAST:
        return DRAM0_RTCFAST_ADDRESS_HIGH;
    case MEMPROT_PERI1_RTCSLOW:
        return PERI1_RTCSLOW_ADDRESS_HIGH;
    case MEMPROT_PERI2_RTCSLOW_0:
        return PERI2_RTCSLOW_0_ADDRESS_HIGH;
    case MEMPROT_PERI2_RTCSLOW_1:
        return PERI2_RTCSLOW_1_ADDRESS_HIGH;
    default:
        return MEMPROT_INVALID_ADDRESS;
    }
}

esp_err_t esp_memprot_set_read_perm(mem_type_prot_t mem_type, bool lr, bool hr)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_sram_set_read_perm(lr, hr);
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_sram_set_read_perm(lr, hr);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_rtcfast_set_read_perm(lr, hr);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_rtcfast_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_rtcslow_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        memprot_ll_peri2_rtcslow_0_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_rtcslow_1_set_read_perm(lr, hr);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_write_perm(mem_type_prot_t mem_type, bool lw, bool hw)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_sram_set_write_perm(lw, hw);
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_sram_set_write_perm(lw, hw);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_rtcfast_set_write_perm(lw, hw);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        memprot_ll_dram0_rtcfast_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        memprot_ll_peri1_rtcslow_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        memprot_ll_peri2_rtcslow_0_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_rtcslow_1_set_write_perm(lw, hw);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_memprot_set_exec_perm(mem_type_prot_t mem_type, bool lx, bool hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_sram_set_exec_perm(lx, hx);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        memprot_ll_iram0_rtcfast_set_exec_perm(lx, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        memprot_ll_peri2_rtcslow_0_set_exec_perm(lx, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        memprot_ll_peri2_rtcslow_1_set_exec_perm(lx, hx);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
