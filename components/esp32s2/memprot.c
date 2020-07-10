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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/sensitive_reg.h"
#include "soc/dport_access.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "esp32s2/memprot.h"
#include "hal/memprot_ll.h"
#include "esp_fault.h"
#include "esp_log.h"
#include "soc/cpu.h"

extern int _iram_text_end;
extern int _data_start;
static const char *TAG = "memprot";


uint32_t *esp_memprot_iram0_get_min_split_addr(void)
{
    return (uint32_t *)&_iram_text_end;
}

uint32_t *esp_memprot_dram0_get_min_split_addr(void)
{
    return (uint32_t *)&_data_start;
}

uint32_t *esp_memprot_get_split_addr(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_min_split_addr();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_min_split_addr();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

const char *esp_memprot_type_to_str(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0:
        return "IRAM0";
    case MEMPROT_DRAM0:
        return "DRAM0";
    default:
        return "UNKOWN";
    }
}

void esp_memprot_intr_init(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_iram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_DRAM0:
        intr_matrix_set(PRO_CPU_NUM, esp_memprot_dram0_get_intr_source_num(), ETS_MEMACCESS_ERR_INUM);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }

    ESP_INTR_ENABLE(ETS_MEMACCESS_ERR_INUM);
}

void esp_memprot_intr_ena(mem_type_prot_t mem_type, bool enable)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_intr_ena(enable);
        break;
    case MEMPROT_DRAM0:
        esp_memprot_dram0_intr_ena(enable);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_is_assoc_intr_any()
{
    return esp_memprot_iram0_is_assoc_intr() || esp_memprot_dram0_is_assoc_intr();
}

mem_type_prot_t esp_memprot_get_intr_memtype()
{
    if ( esp_memprot_is_assoc_intr(MEMPROT_IRAM0) ) {
        return MEMPROT_IRAM0;
    } else if ( esp_memprot_is_assoc_intr(MEMPROT_DRAM0) ) {
        return MEMPROT_DRAM0;
    }

    return MEMPROT_UNKNOWN;
}

bool esp_memprot_is_assoc_intr(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_is_assoc_intr();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_is_assoc_intr();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_clear_intr(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_clear_intr();
        break;
    case MEMPROT_DRAM0:
        esp_memprot_dram0_clear_intr();
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_lock(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_set_lock();
        break;
    case MEMPROT_DRAM0:
        esp_memprot_dram0_set_lock();
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_get_lock(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_lock_reg() > 0;
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_lock_reg() > 0;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_is_locked_any()
{
    return esp_memprot_iram0_get_lock_reg() > 0 || esp_memprot_iram0_get_lock_reg() > 0;
}

uint32_t esp_memprot_get_lock_bit(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_lock_bit();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_lock_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_ena_reg(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_ena_reg();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_ena_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_fault_reg(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_fault_reg();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_fault_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_fault_status(mem_type_prot_t mem_type, uint32_t **faulting_address, uint32_t *op_type, uint32_t *op_subtype)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_get_fault_status(faulting_address, op_type, op_subtype);
        break;
    case MEMPROT_DRAM0:
        esp_memprot_dram0_get_fault_status(faulting_address, op_type, op_subtype);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

bool esp_memprot_is_intr_ena_any()
{
    return esp_memprot_iram0_get_intr_ena_bit() > 0 || esp_memprot_dram0_get_intr_ena_bit() > 0;
}

uint32_t esp_memprot_get_intr_ena_bit(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_intr_ena_bit();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_intr_ena_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_intr_on_bit(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_intr_on_bit();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_intr_on_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_intr_clr_bit(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_intr_clr_bit();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_intr_clr_bit();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_read_bit(mem_type_prot_t mem_type, uint32_t block)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_uni_block_read_bit(block);
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_uni_block_read_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_write_bit(mem_type_prot_t mem_type, uint32_t block)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_uni_block_write_bit(block);
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_uni_block_write_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_uni_block_exec_bit(mem_type_prot_t mem_type, uint32_t block)
{
    assert(mem_type == MEMPROT_IRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_uni_block_exec_bit(block);
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_uni_block_perm_dram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm)
{
    assert(mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_DRAM0:
        esp_memprot_dram0_set_uni_block_perm(block, write_perm, read_perm);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_perm_uni_reg(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_perm_uni_reg();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_perm_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

uint32_t esp_memprot_get_perm_split_reg(mem_type_prot_t mem_type)
{
    assert(mem_type == MEMPROT_IRAM0 || mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        return esp_memprot_iram0_get_perm_split_reg();
    case MEMPROT_DRAM0:
        return esp_memprot_dram0_get_perm_reg();
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_dram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr)
{
    assert(mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_DRAM0:
        esp_memprot_dram0_set_prot(split_addr != NULL ? split_addr : esp_memprot_dram0_get_min_split_addr(), lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_uni_block_perm_iram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm, bool exec_perm)
{
    assert(mem_type == MEMPROT_IRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_set_uni_block_perm(block, write_perm, read_perm, exec_perm);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot_iram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx)
{
    assert(mem_type == MEMPROT_IRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_set_prot(split_addr != NULL ? split_addr : esp_memprot_iram0_get_min_split_addr(), lw, lr, lx, hw, hr, hx);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_get_perm_split_bits_iram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx)
{
    assert(mem_type == MEMPROT_IRAM0);

    switch (mem_type) {
    case MEMPROT_IRAM0:
        esp_memprot_iram0_get_split_sgnf_bits(lw, lr, lx, hw, hr, hx);
        break;
    default:
        assert(0);
    }
}

void esp_memprot_get_perm_split_bits_dram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr)
{
    assert(mem_type == MEMPROT_DRAM0);

    switch (mem_type) {
    case MEMPROT_DRAM0:
        esp_memprot_dram0_get_split_sgnf_bits(lw, lr, hw, hr);
        break;
    default:
        ESP_LOGE(TAG, "Invalid mem_type %d", mem_type);
        abort();
    }
}

void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature)
{
    esp_memprot_intr_ena(MEMPROT_DRAM0, false);
    esp_memprot_intr_ena(MEMPROT_IRAM0, false);

    if (!esp_cpu_in_ocd_debug_mode()) {

        ESP_FAULT_ASSERT(!esp_cpu_in_ocd_debug_mode());

        if ( invoke_panic_handler ) {
            esp_memprot_intr_init(MEMPROT_DRAM0);
            esp_memprot_intr_init(MEMPROT_IRAM0);
        }

        esp_memprot_set_prot_dram(MEMPROT_DRAM0, NULL, false, true, true, true);
        esp_memprot_set_prot_iram(MEMPROT_IRAM0, NULL, false, true, true, true, true, false);

        esp_memprot_intr_ena(MEMPROT_DRAM0, true);
        esp_memprot_intr_ena(MEMPROT_IRAM0, true);

        if ( lock_feature ) {
            esp_memprot_set_lock(MEMPROT_DRAM0);
            esp_memprot_set_lock(MEMPROT_IRAM0);
        }
    }
}

