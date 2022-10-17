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
 * implementation of PMS memory protection features
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "soc/sensitive_reg.h"
#include "soc/dport_access.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "hal/memprot_ll.h"
#include "esp32c3/memprot.h"
#include "riscv/interrupt.h"
#include "esp32c3/rom/ets_sys.h"
#include "esp_fault.h"
#include "soc/cpu.h"


extern int _iram_text_end;

const char *esp_memprot_mem_type_to_str(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_NONE:
        return "NONE";
    case MEMPROT_IRAM0_SRAM:
        return "IRAM0_SRAM";
    case MEMPROT_DRAM0_SRAM:
        return "DRAM0_SRAM";
    case MEMPROT_ALL:
        return "ALL";
    default:
        return "UNKNOWN";
    }
}

const char *esp_memprot_split_line_to_str(split_line_t line_type)
{
    switch (line_type) {
    case MEMPROT_IRAM0_DRAM0_SPLITLINE:
        return "MEMPROT_IRAM0_DRAM0_SPLITLINE";
    case MEMPROT_IRAM0_LINE_0_SPLITLINE:
        return "MEMPROT_IRAM0_LINE_0_SPLITLINE";
    case MEMPROT_IRAM0_LINE_1_SPLITLINE:
        return "MEMPROT_IRAM0_LINE_1_SPLITLINE";
    case MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE:
        return "MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE";
    case MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE:
        return "MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE";
    default:
        return "UNKNOWN";
    }
}

const char *esp_memprot_pms_to_str(pms_area_t area_type)
{
    switch (area_type) {
    case MEMPROT_IRAM0_PMS_AREA_0:
        return "MEMPROT_IRAM0_PMS_AREA_0";
    case MEMPROT_IRAM0_PMS_AREA_1:
        return "MEMPROT_IRAM0_PMS_AREA_1";
    case MEMPROT_IRAM0_PMS_AREA_2:
        return "MEMPROT_IRAM0_PMS_AREA_2";
    case MEMPROT_IRAM0_PMS_AREA_3:
        return "MEMPROT_IRAM0_PMS_AREA_3";
    case MEMPROT_DRAM0_PMS_AREA_0:
        return "MEMPROT_DRAM0_PMS_AREA_0";
    case MEMPROT_DRAM0_PMS_AREA_1:
        return "MEMPROT_DRAM0_PMS_AREA_1";
    case MEMPROT_DRAM0_PMS_AREA_2:
        return "MEMPROT_DRAM0_PMS_AREA_2";
    case MEMPROT_DRAM0_PMS_AREA_3:
        return "MEMPROT_DRAM0_PMS_AREA_3";
    default:
        return "UNKNOWN";
    }
}


/* split lines */

void *esp_memprot_get_default_main_split_addr()
{
    return &_iram_text_end;
}

uint32_t *esp_memprot_get_split_addr(split_line_t line_type)
{
    switch ( line_type ) {
    case MEMPROT_IRAM0_DRAM0_SPLITLINE:
        return memprot_ll_get_iram0_split_line_main_I_D();
    case MEMPROT_IRAM0_LINE_0_SPLITLINE:
        return memprot_ll_get_iram0_split_line_I_0();
    case MEMPROT_IRAM0_LINE_1_SPLITLINE:
        return memprot_ll_get_iram0_split_line_I_1();
    case MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE:
        return memprot_ll_get_dram0_split_line_D_0();
    case MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE:
        return memprot_ll_get_dram0_split_line_D_1();
    default:
        abort();
    }
}

void esp_memprot_set_split_line_lock()
{
    memprot_ll_set_iram0_dram0_split_line_lock();
}

bool esp_memprot_get_split_line_lock()
{
    return memprot_ll_get_iram0_dram0_split_line_lock();
}

void esp_memprot_set_split_line(split_line_t line_type, const void *line_addr)
{
    //split-line must be divisible by 512 (PMS module restriction)
    assert( ((uint32_t)line_addr) % 0x200 == 0 );

    switch ( line_type ) {
    case MEMPROT_IRAM0_DRAM0_SPLITLINE:
        memprot_ll_set_iram0_split_line_main_I_D(line_addr);
        break;
    case MEMPROT_IRAM0_LINE_0_SPLITLINE:
        memprot_ll_set_iram0_split_line_I_0(line_addr);
        break;
    case MEMPROT_IRAM0_LINE_1_SPLITLINE:
        memprot_ll_set_iram0_split_line_I_1(line_addr);
        break;
    case MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE:
        memprot_ll_set_dram0_split_line_D_0(line_addr);
        break;
    case MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE:
        memprot_ll_set_dram0_split_line_D_1(line_addr);
        break;
    default:
        abort();
    }
}


/* PMS */

void esp_memprot_set_pms_lock(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_set_pms_lock();
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_set_pms_lock();
        break;
    default:
        abort();
    }
}

bool esp_memprot_get_pms_lock(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_pms_lock();
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_pms_lock();
    default:
        abort();
    }
}

void esp_memprot_iram_set_pms_area(pms_area_t area_type, bool r, bool w, bool x)
{
    switch ( area_type ) {
    case MEMPROT_IRAM0_PMS_AREA_0:
        memprot_ll_iram0_set_pms_area_0(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_1:
        memprot_ll_iram0_set_pms_area_1(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_2:
        memprot_ll_iram0_set_pms_area_2(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_3:
        memprot_ll_iram0_set_pms_area_3(r, w, x);
        break;
    default:
        abort();
    }
}

void esp_memprot_iram_get_pms_area(pms_area_t area_type, bool *r, bool *w, bool *x)
{
    switch ( area_type ) {
    case MEMPROT_IRAM0_PMS_AREA_0:
        memprot_ll_iram0_get_pms_area_0(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_1:
        memprot_ll_iram0_get_pms_area_1(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_2:
        memprot_ll_iram0_get_pms_area_2(r, w, x);
        break;
    case MEMPROT_IRAM0_PMS_AREA_3:
        memprot_ll_iram0_get_pms_area_3(r, w, x);
        break;
    default:
        abort();
    }
}

void esp_memprot_dram_set_pms_area(pms_area_t area_type, bool r, bool w)
{
    switch ( area_type ) {
    case MEMPROT_DRAM0_PMS_AREA_0:
        memprot_ll_dram0_set_pms_area_0(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_1:
        memprot_ll_dram0_set_pms_area_1(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_2:
        memprot_ll_dram0_set_pms_area_2(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_3:
        memprot_ll_dram0_set_pms_area_3(r, w);
        break;
    default:
        abort();
    }
}

void esp_memprot_dram_get_pms_area(pms_area_t area_type, bool *r, bool *w)
{
    switch ( area_type ) {
    case MEMPROT_DRAM0_PMS_AREA_0:
        memprot_ll_dram0_get_pms_area_0(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_1:
        memprot_ll_dram0_get_pms_area_1(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_2:
        memprot_ll_dram0_get_pms_area_2(r, w);
        break;
    case MEMPROT_DRAM0_PMS_AREA_3:
        memprot_ll_dram0_get_pms_area_3(r, w);
        break;
    default:
        abort();
    }
}


/* monitor */

void esp_memprot_set_monitor_lock(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_set_monitor_lock();
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_set_monitor_lock();
        break;
    default:
        abort();
    }
}

bool esp_memprot_get_monitor_lock(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_lock();
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_lock();
    default:
        abort();
    }
}

void esp_memprot_set_monitor_en(mem_type_prot_t mem_type, bool enable)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_set_monitor_en(enable);
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_set_monitor_en(enable);
        break;
    default:
        abort();
    }
}

bool esp_memprot_get_monitor_en(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_en();
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_en();
    default:
        abort();
    }
}

bool esp_memprot_is_intr_ena_any()
{
    return esp_memprot_get_monitor_en(MEMPROT_IRAM0_SRAM) || esp_memprot_get_monitor_en(MEMPROT_DRAM0_SRAM);
}

void esp_memprot_monitor_clear_intr(mem_type_prot_t mem_type)
{

    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        memprot_ll_iram0_clear_monitor_intr();
        memprot_ll_iram0_reset_clear_monitor_intr();
        break;
    case MEMPROT_DRAM0_SRAM:
        memprot_ll_dram0_clear_monitor_intr();
        memprot_ll_dram0_reset_clear_monitor_intr();
        break;
    default:
        abort();
    }
}

mem_type_prot_t esp_memprot_get_active_intr_memtype()
{
    if ( memprot_ll_iram0_get_monitor_status_intr() > 0 ) {
        return MEMPROT_IRAM0_SRAM;
    } else if ( memprot_ll_dram0_get_monitor_status_intr() ) {
        return MEMPROT_DRAM0_SRAM;
    }

    return MEMPROT_NONE;
}

bool esp_memprot_is_locked_any()
{
    return
        esp_memprot_get_split_line_lock() ||
        esp_memprot_get_pms_lock(MEMPROT_IRAM0_SRAM) ||
        esp_memprot_get_pms_lock(MEMPROT_DRAM0_SRAM) ||
        esp_memprot_get_monitor_lock(MEMPROT_IRAM0_SRAM) ||
        esp_memprot_get_monitor_lock(MEMPROT_DRAM0_SRAM);
}

bool esp_memprot_get_violate_intr_on(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_status_intr() == 1;
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_status_intr() == 1;
    default:
        abort();
    }
}

uint32_t esp_memprot_get_violate_addr(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_status_fault_addr();
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_status_fault_addr();
    default:
        abort();
    }
}

pms_world_t esp_memprot_get_violate_world(mem_type_prot_t mem_type)
{
    uint32_t world = 0;

    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        world = memprot_ll_iram0_get_monitor_status_fault_world();
        break;
    case MEMPROT_DRAM0_SRAM:
        world = memprot_ll_dram0_get_monitor_status_fault_world();
        break;
    default:
        abort();
    }

    switch ( world ) {
    case 0x01: return MEMPROT_PMS_WORLD_0;
    case 0x10: return MEMPROT_PMS_WORLD_1;
    default: return MEMPROT_PMS_WORLD_INVALID;
    }
}

pms_operation_type_t esp_memprot_get_violate_wr(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_status_fault_wr() == 1 ? MEMPROT_PMS_OP_WRITE : MEMPROT_PMS_OP_READ;
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_status_fault_wr() == 1 ? MEMPROT_PMS_OP_WRITE : MEMPROT_PMS_OP_READ;
    default:
        abort();
    }
}

bool esp_memprot_get_violate_loadstore(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_status_fault_loadstore() == 1;
    default:
        abort();
    }
}

uint32_t esp_memprot_get_violate_byte_en(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_status_fault_byte_en();
    default:
        abort();
    }
}

int esp_memprot_intr_get_cpuid()
{
    return PRO_CPU_NUM;
}

void esp_memprot_set_intr_matrix(mem_type_prot_t mem_type)
{
    ESP_INTR_DISABLE(ETS_MEMPROT_ERR_INUM);

    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        intr_matrix_set(esp_memprot_intr_get_cpuid(), memprot_ll_iram0_get_intr_source_num(), ETS_MEMPROT_ERR_INUM);
        break;
    case MEMPROT_DRAM0_SRAM:
        intr_matrix_set(esp_memprot_intr_get_cpuid(), memprot_ll_dram0_get_intr_source_num(), ETS_MEMPROT_ERR_INUM);
        break;
    default:
        abort();
    }

    /* Set the type and priority to cache error interrupts. */
    esprv_intc_int_set_type(ETS_MEMPROT_ERR_INUM, INTR_TYPE_LEVEL);
    esprv_intc_int_set_priority(ETS_MEMPROT_ERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    ESP_INTR_ENABLE(ETS_MEMPROT_ERR_INUM);
}

void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask)
{
    esp_memprot_set_prot_int(invoke_panic_handler, lock_feature, NULL, mem_type_mask);
}

void esp_memprot_set_prot_int(bool invoke_panic_handler, bool lock_feature, void *split_addr, uint32_t *mem_type_mask)
{
    //if being debugged check we are not glitched and dont enable Memprot
    if (esp_cpu_in_ocd_debug_mode()) {
        ESP_FAULT_ASSERT(esp_cpu_in_ocd_debug_mode());
    } else {
        uint32_t required_mem_prot = mem_type_mask == NULL ? (uint32_t) MEMPROT_ALL : *mem_type_mask;
        bool use_iram0 = required_mem_prot & MEMPROT_IRAM0_SRAM;
        bool use_dram0 = required_mem_prot & MEMPROT_DRAM0_SRAM;

        if (required_mem_prot == MEMPROT_NONE) {
            return;
        }

        //disable protection
        if (use_iram0) {
            esp_memprot_set_monitor_en(MEMPROT_IRAM0_SRAM, false);
        }
        if (use_dram0) {
            esp_memprot_set_monitor_en(MEMPROT_DRAM0_SRAM, false);
        }

        //panic handling
        if (invoke_panic_handler) {
            if (use_iram0) {
                esp_memprot_set_intr_matrix(MEMPROT_IRAM0_SRAM);
            }
            if (use_dram0) {
                esp_memprot_set_intr_matrix(MEMPROT_DRAM0_SRAM);
            }
        }

        //set split lines (must-have for all mem_types)
        const void *line_addr = split_addr == NULL ? esp_memprot_get_default_main_split_addr() : split_addr;
        esp_memprot_set_split_line(MEMPROT_IRAM0_LINE_1_SPLITLINE, line_addr);
        esp_memprot_set_split_line(MEMPROT_IRAM0_LINE_0_SPLITLINE, line_addr);
        esp_memprot_set_split_line(MEMPROT_IRAM0_DRAM0_SPLITLINE, line_addr);
        esp_memprot_set_split_line(MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE,
                                   (void *) (MAP_IRAM_TO_DRAM((uint32_t) line_addr)));
        esp_memprot_set_split_line(MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE,
                                   (void *) (MAP_IRAM_TO_DRAM((uint32_t) line_addr)));

        //set permissions
        if (required_mem_prot & MEMPROT_IRAM0_SRAM) {
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_0, true, false, true);
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_1, true, false, true);
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_2, true, false, true);
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_3, true, true, false);
        }
        if (required_mem_prot & MEMPROT_DRAM0_SRAM) {
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_0, true, false);
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_1, true, true);
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_2, true, true);
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_3, true, true);
        }

        //reenable protection
        if (use_iram0) {
            esp_memprot_monitor_clear_intr(MEMPROT_IRAM0_SRAM);
            esp_memprot_set_monitor_en(MEMPROT_IRAM0_SRAM, true);
        }
        if (use_dram0) {
            esp_memprot_monitor_clear_intr(MEMPROT_DRAM0_SRAM);
            esp_memprot_set_monitor_en(MEMPROT_DRAM0_SRAM, true);
        }

        //lock if required
        if (lock_feature) {
            esp_memprot_set_split_line_lock();
            if (use_iram0) {
                esp_memprot_set_pms_lock(MEMPROT_IRAM0_SRAM);
                esp_memprot_set_monitor_lock(MEMPROT_IRAM0_SRAM);
            }
            if (use_dram0) {
                esp_memprot_set_pms_lock(MEMPROT_DRAM0_SRAM);
                esp_memprot_set_monitor_lock(MEMPROT_DRAM0_SRAM);
            }
        }
    }
}

uint32_t esp_memprot_get_dram_status_reg_1()
{
    return memprot_ll_dram0_get_monitor_status_register_1();
}

uint32_t esp_memprot_get_dram_status_reg_2()
{
    return memprot_ll_dram0_get_monitor_status_register_2();
}

uint32_t esp_memprot_get_iram_status_reg()
{
    return memprot_ll_iram0_get_monitor_status_register();
}

uint32_t esp_memprot_get_monitor_enable_reg(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return memprot_ll_iram0_get_monitor_enable_register();
    case MEMPROT_DRAM0_SRAM:
        return memprot_ll_dram0_get_monitor_enable_register();
    default:
        abort();
    }
}
