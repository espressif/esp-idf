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
 * generic interface to PMS memory protection features
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEMPROT_NONE =              0x00000000,
    MEMPROT_IRAM0_SRAM =        0x00000001,
    MEMPROT_DRAM0_SRAM =        0x00000002,
    MEMPROT_ALL =               0xFFFFFFFF
} mem_type_prot_t;

typedef enum {
    MEMPROT_IRAM0_DRAM0_SPLITLINE,
    MEMPROT_IRAM0_LINE_0_SPLITLINE,
    MEMPROT_IRAM0_LINE_1_SPLITLINE,
    MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE,
    MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE
} split_line_t;

typedef enum {
    MEMPROT_IRAM0_PMS_AREA_0,
    MEMPROT_IRAM0_PMS_AREA_1,
    MEMPROT_IRAM0_PMS_AREA_2,
    MEMPROT_IRAM0_PMS_AREA_3,
    MEMPROT_DRAM0_PMS_AREA_0,
    MEMPROT_DRAM0_PMS_AREA_1,
    MEMPROT_DRAM0_PMS_AREA_2,
    MEMPROT_DRAM0_PMS_AREA_3
} pms_area_t;


const char *mem_type_to_str(mem_type_prot_t mem_type);
const char *split_line_to_str(split_line_t line_type);
const char *pms_to_str(pms_area_t area_type);

void *esp_memprot_get_main_split_addr(void);
void esp_memprot_set_split_line_lock(bool lock);
bool esp_memprot_get_split_line_lock(void);
void esp_memprot_set_split_line(split_line_t line_type, const void *line_addr);

void esp_memprot_set_pms_lock(mem_type_prot_t mem_type, bool lock);
bool esp_memprot_get_pms_lock(mem_type_prot_t mem_type);
void esp_memprot_iram_set_pms_area(pms_area_t area_type, bool r, bool w, bool x);
void esp_memprot_dram_set_pms_area(pms_area_t area_type, bool r, bool w);

void esp_memprot_set_monitor_lock(mem_type_prot_t mem_type, bool lock);
bool esp_memprot_get_monitor_lock(mem_type_prot_t mem_type);
void esp_memprot_set_monitor_en(mem_type_prot_t mem_type, bool enable);
bool esp_memprot_get_monitor_en(mem_type_prot_t mem_type);

int IRAM_ATTR esp_memprot_intr_get_cpuid(void);
void IRAM_ATTR esp_memprot_monitor_clear_intr(mem_type_prot_t mem_type);
mem_type_prot_t IRAM_ATTR esp_memprot_get_active_intr_memtype(void);
bool IRAM_ATTR esp_memprot_is_locked_any(void);
bool IRAM_ATTR esp_memprot_is_intr_ena_any(void);

uint32_t IRAM_ATTR esp_memprot_get_violate_intr_on(mem_type_prot_t mem_type);
uint32_t IRAM_ATTR esp_memprot_get_violate_addr(mem_type_prot_t mem_type);
uint32_t IRAM_ATTR esp_memprot_get_violate_world(mem_type_prot_t mem_type);
uint32_t IRAM_ATTR esp_memprot_get_violate_wr(mem_type_prot_t mem_type);
uint32_t IRAM_ATTR esp_memprot_get_violate_loadstore(mem_type_prot_t mem_type);
uint32_t IRAM_ATTR esp_memprot_get_violate_byte_en(mem_type_prot_t mem_type);

void esp_memprot_set_intr_matrix(mem_type_prot_t mem_type);
void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask);
void esp_memprot_set_prot_int(bool invoke_panic_handler, bool lock_feature, void *split_addr, uint32_t *mem_type_mask);

#ifdef __cplusplus
}
#endif
