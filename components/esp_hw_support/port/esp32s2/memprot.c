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

/* INTERNAL API
 * implementation of generic interface to MMU memory protection features
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "soc/sensitive_reg.h"
#include "soc/dport_access.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"

#include "esp_log.h"
static const char *TAG = "memprot";

#include "esp32s2/memprot.h"
#include "hal/memprot_ll.h"
#include "hal/memprot_peri_ll.h"
#include "esp_fault.h"

#include "soc/cpu.h"

extern int _iram_text_end;
extern int _data_start;
extern int _rtc_text_end;
extern int _rtc_dummy_end;


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
    return (uint32_t *)(PERI1_RTCSLOW_ADDRESS_BASE);
}

uint32_t *esp_memprot_peri2_rtcslow_0_get_min_split_addr(void)
{
    return (uint32_t *)(PERI2_RTCSLOW_0_ADDRESS_BASE);
}

uint32_t *esp_memprot_peri2_rtcslow_1_get_min_split_addr(void)
{
    return (uint32_t *)(PERI2_RTCSLOW_1_ADDRESS_BASE);
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
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
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
        return "UNKOWN";
    }
}

void esp_memprot_intr_init(mem_type_prot_t mem_type)
{
    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_iram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_dram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_peri1_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_peri2_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }

    ESP_INTR_ENABLE(ETS_MEMACCESS_ERR_INUM);
}

void esp_memprot_intr_ena(mem_type_prot_t mem_type, bool enable)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_intr_ena(enable);
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_intr_ena(enable);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_intr_ena(enable);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_intr_ena(enable);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

mem_type_prot_t esp_memprot_get_active_intr_memtype()
{
    if (esp_memprot_iram0_sram_is_intr_mine()) {
        return MEMPROT_IRAM0_SRAM;
    } else if (esp_memprot_iram0_rtcfast_is_intr_mine()) {
        return MEMPROT_IRAM0_RTCFAST;
    } else if (esp_memprot_dram0_sram_is_intr_mine()) {
        return MEMPROT_DRAM0_SRAM;
    } else if (esp_memprot_dram0_rtcfast_is_intr_mine()) {
        return MEMPROT_DRAM0_RTCFAST;
    } else if (esp_memprot_peri1_rtcslow_is_intr_mine()) {
        return MEMPROT_PERI1_RTCSLOW;
    } else if (esp_memprot_peri2_rtcslow_0_is_intr_mine()) {
        return MEMPROT_PERI2_RTCSLOW_0;
    } else if (esp_memprot_peri2_rtcslow_1_is_intr_mine()) {
        return MEMPROT_PERI2_RTCSLOW_1;
    }

    return MEMPROT_NONE;
}

void esp_memprot_clear_intr(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_clear_intr();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_clear_intr();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_clear_intr();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_clear_intr();
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_lock(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_set_lock();
        break;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_set_lock();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_set_lock();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_set_lock();
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_get_lock(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_lock_bit() > 0;
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_lock_bit() > 0;
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_lock_bit() > 0;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_lock_bit() > 0;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_is_locked_any()
{
    return
        esp_memprot_iram0_get_lock_bit() > 0 ||
        esp_memprot_dram0_get_lock_bit() > 0 ||
        esp_memprot_peri1_get_lock_bit() > 0 ||
        esp_memprot_peri2_get_lock_bit() > 0;
}

uint32_t esp_memprot_get_lock_bit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_lock_bit();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_lock_bit();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_lock_bit();
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_lock_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_conf_reg(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_conf_reg();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_conf_reg();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_rtcslow_get_conf_reg();
    case MEMPROT_PERI2_RTCSLOW_0:
        return esp_memprot_peri2_rtcslow_0_get_conf_reg();
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_rtcslow_1_get_conf_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_fault_reg(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_fault_reg();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_fault_reg();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_fault_reg();
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_fault_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_fault_status(mem_type_prot_t mem_type, uint32_t **faulting_address, uint32_t *op_type, uint32_t *op_subtype)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        *faulting_address = esp_memprot_iram0_sram_get_fault_address();
        break;
    case MEMPROT_IRAM0_RTCFAST:
        *faulting_address = esp_memprot_iram0_rtcfast_get_fault_address();
        break;
    case MEMPROT_DRAM0_SRAM:
        *faulting_address = esp_memprot_dram0_sram_get_fault_address();
        break;
    case MEMPROT_DRAM0_RTCFAST:
        *faulting_address = esp_memprot_dram0_rtcfast_get_fault_address();
        break;
    case MEMPROT_PERI1_RTCSLOW:
        *faulting_address = esp_memprot_peri1_rtcslow_get_fault_address();
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        *faulting_address = esp_memprot_peri2_rtcslow_get_fault_address();
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }

    if (mem_type == MEMPROT_IRAM0_SRAM || mem_type == MEMPROT_IRAM0_RTCFAST) {
        esp_memprot_iram0_get_fault_op_type(op_type, op_subtype);
    } else if (mem_type == MEMPROT_DRAM0_SRAM || mem_type == MEMPROT_DRAM0_RTCFAST) {
        esp_memprot_dram0_get_fault_op_type(op_type, op_subtype);
    } else if (mem_type == MEMPROT_PERI1_RTCSLOW) {
        esp_memprot_peri1_get_fault_op_type(op_type, op_subtype);
    } else if (mem_type == MEMPROT_PERI2_RTCSLOW_0 || mem_type == MEMPROT_PERI2_RTCSLOW_1) {
        esp_memprot_peri2_get_fault_op_type(op_type, op_subtype);
    }
}

bool esp_memprot_is_intr_ena_any()
{
    return
        esp_memprot_iram0_get_intr_ena_bit() > 0 ||
        esp_memprot_dram0_get_intr_ena_bit() > 0 ||
        esp_memprot_peri1_get_intr_ena_bit() > 0 ||
        esp_memprot_peri2_get_intr_ena_bit() > 0;
}

uint32_t esp_memprot_get_intr_ena_bit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_intr_ena_bit();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_intr_ena_bit();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_intr_ena_bit();
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_intr_ena_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_intr_on_bit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_intr_on_bit();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_intr_on_bit();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_intr_on_bit();
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_intr_on_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_intr_clr_bit(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_get_intr_clr_bit();
    case MEMPROT_DRAM0_SRAM:
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_get_intr_clr_bit();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_get_intr_clr_bit();
    case MEMPROT_PERI2_RTCSLOW_0:
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_get_intr_clr_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_read_bit(mem_type_prot_t mem_type, uint32_t block)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_uni_block_read_bit(block);
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_dram0_sram_get_uni_block_read_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_write_bit(mem_type_prot_t mem_type, uint32_t block)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_uni_block_write_bit(block);
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_dram0_sram_get_uni_block_write_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_exec_bit(mem_type_prot_t mem_type, uint32_t block)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_uni_block_exec_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

void esp_memprot_set_uni_block_perm_dram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm)
{
    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_set_uni_block_perm(block, write_perm, read_perm);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_perm_uni_reg(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_perm_uni_reg();
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_dram0_sram_get_perm_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_perm_split_reg(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_iram0_sram_get_perm_split_reg();
    case MEMPROT_IRAM0_RTCFAST:
        return esp_memprot_iram0_rtcfast_get_perm_split_reg();
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_dram0_sram_get_perm_reg();
    case MEMPROT_DRAM0_RTCFAST:
        return esp_memprot_dram0_rtcfast_get_perm_split_reg();
    case MEMPROT_PERI1_RTCSLOW:
        return esp_memprot_peri1_rtcslow_get_conf_reg();
    case MEMPROT_PERI2_RTCSLOW_0:
        return esp_memprot_peri2_rtcslow_0_get_conf_reg();
    case MEMPROT_PERI2_RTCSLOW_1:
        return esp_memprot_peri2_rtcslow_1_get_conf_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_dram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_set_prot(split_addr != NULL ? split_addr : esp_memprot_dram0_sram_get_min_split_addr(), lw, lr, hw, hr);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_rtcfast_set_prot(split_addr != NULL ? split_addr : esp_memprot_dram0_rtcfast_get_min_split_addr(), lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_uni_block_perm_iram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm, bool exec_perm)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_set_uni_block_perm(block, write_perm, read_perm, exec_perm);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d (unified block management not supported)", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_iram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_set_prot(split_addr != NULL ? split_addr : esp_memprot_iram0_sram_get_min_split_addr(), lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_set_prot(split_addr != NULL ? split_addr : esp_memprot_iram0_rtcfast_get_min_split_addr(), lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_split_bits_iram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_split_bits_dram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr)
{
    switch (mem_type) {
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_rtcfast_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_split_bits_peri1(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr)
{
    switch (mem_type) {
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_rtcslow_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_peri1(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    switch (mem_type) {
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_rtcslow_set_prot(split_addr != NULL ? split_addr : esp_memprot_peri1_rtcslow_get_min_split_addr(), lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_split_bits_peri2(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    switch (mem_type) {
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_peri2(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    switch (mem_type) {
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_set_prot(split_addr != NULL ? split_addr : esp_memprot_peri2_rtcslow_0_get_min_split_addr(), lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_set_prot(split_addr != NULL ? split_addr : esp_memprot_peri2_rtcslow_1_get_min_split_addr(), lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask)
{
    //any IRAM0/DRAM0 enable/disable call applies to all memory modules connected
    uint32_t required_mem_prot = mem_type_mask == NULL ? (uint32_t)MEMPROT_ALL : *mem_type_mask;
    bool use_iram0 = required_mem_prot & MEMPROT_IRAM0_SRAM || required_mem_prot & MEMPROT_IRAM0_RTCFAST;
    bool use_dram0 = required_mem_prot & MEMPROT_DRAM0_SRAM || required_mem_prot & MEMPROT_DRAM0_RTCFAST;
    bool use_peri1 = required_mem_prot & MEMPROT_PERI1_RTCSLOW;
    bool use_peri2 = required_mem_prot & MEMPROT_PERI2_RTCSLOW_0 || required_mem_prot & MEMPROT_PERI2_RTCSLOW_1;

    //disable protection
    if (use_iram0) {
        esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, false);
    }
    if (use_dram0) {
        esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, false);
    }
    if (use_peri1) {
        esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, false);
    }
    if (use_peri2) {
        esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, false);
    }

    //connect to intr. matrix if not being debugged
    if (!esp_cpu_in_ocd_debug_mode()) {

        ESP_FAULT_ASSERT(!esp_cpu_in_ocd_debug_mode());

        //initialize for specific buses (any memory type does the job)
        if (invoke_panic_handler) {
            if (use_iram0) {
                esp_memprot_intr_init(MEMPROT_IRAM0_SRAM);
            }
            if (use_dram0) {
                esp_memprot_intr_init(MEMPROT_DRAM0_SRAM);
            }
            if (use_peri1) {
                esp_memprot_intr_init(MEMPROT_PERI1_RTCSLOW);
            }
            if (use_peri2) {
                esp_memprot_intr_init(MEMPROT_PERI2_RTCSLOW_0);
            }
        }

        //set permissions
        if (required_mem_prot & MEMPROT_IRAM0_SRAM) {
            esp_memprot_set_prot_iram(MEMPROT_IRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
        }
        if (required_mem_prot & MEMPROT_IRAM0_RTCFAST) {
            esp_memprot_set_prot_iram(MEMPROT_IRAM0_RTCFAST, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
        }
        if (required_mem_prot & MEMPROT_DRAM0_SRAM) {
            esp_memprot_set_prot_dram(MEMPROT_DRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
        }
        if (required_mem_prot & MEMPROT_DRAM0_RTCFAST) {
            esp_memprot_set_prot_dram(MEMPROT_DRAM0_RTCFAST, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
        }
        if (required_mem_prot & MEMPROT_PERI1_RTCSLOW) {
            esp_memprot_set_prot_peri1(MEMPROT_PERI1_RTCSLOW, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_DIS, WR_HIGH_DIS, RD_HIGH_DIS);
        }
        if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_0) {
            esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_0, DEF_SPLIT_LINE, WR_LOW_ENA, RD_LOW_ENA, EX_LOW_DIS, WR_HIGH_ENA, RD_HIGH_ENA, EX_HIGH_DIS);
        }
        if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_1) {
            esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_1, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_DIS, EX_LOW_DIS, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
        }

        //reenable protection (bus based)
        if (use_iram0) {
            esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, true);
        }
        if (use_dram0) {
            esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, true);
        }
        if (use_peri1) {
            esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, true);
        }
        if (use_peri2) {
            esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, true);
        }

        //lock if required (bus based)
        if (lock_feature) {
            if (use_iram0) {
                esp_memprot_set_lock(MEMPROT_IRAM0_SRAM);
            }
            if (use_dram0) {
                esp_memprot_set_lock(MEMPROT_DRAM0_SRAM);
            }
            if (use_peri1) {
                esp_memprot_set_lock(MEMPROT_PERI1_RTCSLOW);
            }
            if (use_peri2) {
                esp_memprot_set_lock(MEMPROT_PERI2_RTCSLOW_0);
            }
        }
    }
}

void esp_memprot_get_permissions(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_rtcfast_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_rtcslow_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_read(mem_type_prot_t mem_type, bool *lr, bool *hr)
{
    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    *lr = _lr;
    *hr = _hr;
}

void esp_memprot_get_perm_write(mem_type_prot_t mem_type, bool *lw, bool *hw)
{
    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    *lw = _lw;
    *hw = _hw;
}

void esp_memprot_get_perm_exec(mem_type_prot_t mem_type, bool *lx, bool *hx)
{
    if ( mem_type == MEMPROT_DRAM0_SRAM ||
            mem_type == MEMPROT_DRAM0_RTCFAST ||
            mem_type == MEMPROT_PERI1_RTCSLOW ) {
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }

    bool _lw, _lr, _lx, _hw, _hr, _hx;
    esp_memprot_get_permissions(mem_type, &_lw, &_lr, &_lx, &_hw, &_hr, &_hx);
    *lx = _lx;
    *hx = _hx;
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
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
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
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_read_perm(mem_type_prot_t mem_type, bool lr, bool hr)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_set_read_perm(lr, hr);
        break;
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_set_read_perm(lr, hr);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_set_read_perm(lr, hr);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_rtcfast_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_rtcslow_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_set_read_perm(lr, hr);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_set_read_perm(lr, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_write_perm(mem_type_prot_t mem_type, bool lw, bool hw)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_set_write_perm(lw, hw);
        break;
    case MEMPROT_DRAM0_SRAM:
        esp_memprot_dram0_sram_set_write_perm(lw, hw);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_set_write_perm(lw, hw);
        break;
    case MEMPROT_DRAM0_RTCFAST:
        esp_memprot_dram0_rtcfast_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI1_RTCSLOW:
        esp_memprot_peri1_rtcslow_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_set_write_perm(lw, hw);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_set_write_perm(lw, hw);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_exec_perm(mem_type_prot_t mem_type, bool lx, bool hx)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        esp_memprot_iram0_sram_set_exec_perm(lx, hx);
        break;
    case MEMPROT_IRAM0_RTCFAST:
        esp_memprot_iram0_rtcfast_set_exec_perm(lx, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_0:
        esp_memprot_peri2_rtcslow_0_set_exec_perm(lx, hx);
        break;
    case MEMPROT_PERI2_RTCSLOW_1:
        esp_memprot_peri2_rtcslow_1_set_exec_perm(lx, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}
