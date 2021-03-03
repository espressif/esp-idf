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
#include <stdint.h>

#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IRAM_SRAM_START
#define IRAM_SRAM_START             0x4037C000
#endif

#ifndef DRAM_SRAM_START
#define DRAM_SRAM_START             0x3FC7C000
#endif

#ifndef MAP_DRAM_TO_IRAM
#define MAP_DRAM_TO_IRAM(addr)       (addr - DRAM_SRAM_START + IRAM_SRAM_START)
#endif

#ifndef MAP_IRAM_TO_DRAM
#define MAP_IRAM_TO_DRAM(addr)       (addr - IRAM_SRAM_START + DRAM_SRAM_START)
#endif

typedef enum {
    MEMPROT_NONE =              0x00000000,
    MEMPROT_IRAM0_SRAM =        0x00000001,
    MEMPROT_DRAM0_SRAM =        0x00000002,
    MEMPROT_ALL =               0xFFFFFFFF
} mem_type_prot_t;

typedef enum {
    MEMPROT_SPLITLINE_NONE = 0,
    MEMPROT_IRAM0_DRAM0_SPLITLINE,
    MEMPROT_IRAM0_LINE_0_SPLITLINE,
    MEMPROT_IRAM0_LINE_1_SPLITLINE,
    MEMPROT_DRAM0_DMA_LINE_0_SPLITLINE,
    MEMPROT_DRAM0_DMA_LINE_1_SPLITLINE
} split_line_t;

typedef enum {
    MEMPROT_PMS_AREA_NONE = 0,
    MEMPROT_IRAM0_PMS_AREA_0,
    MEMPROT_IRAM0_PMS_AREA_1,
    MEMPROT_IRAM0_PMS_AREA_2,
    MEMPROT_IRAM0_PMS_AREA_3,
    MEMPROT_DRAM0_PMS_AREA_0,
    MEMPROT_DRAM0_PMS_AREA_1,
    MEMPROT_DRAM0_PMS_AREA_2,
    MEMPROT_DRAM0_PMS_AREA_3
} pms_area_t;

typedef enum
{
    MEMPROT_PMS_WORLD_0 = 0,
    MEMPROT_PMS_WORLD_1,
    MEMPROT_PMS_WORLD_2,
    MEMPROT_PMS_WORLD_INVALID = 0xFFFFFFFF
} pms_world_t;

typedef enum
{
    MEMPROT_PMS_OP_READ = 0,
    MEMPROT_PMS_OP_WRITE,
    MEMPROT_PMS_OP_FETCH,
    MEMPROT_PMS_OP_INVALID = 0xFFFFFFFF
} pms_operation_type_t;

/**
 * @brief Converts Memory protection type to string
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 */
const char *esp_memprot_mem_type_to_str(mem_type_prot_t mem_type);

/**
 * @brief Converts Split line type to string
 *
 * @param line_type Split line type (see split_line_t enum)
 */
const char *esp_memprot_split_line_to_str(split_line_t line_type);

/**
 * @brief Converts PMS Area type to string
 *
 * @param area_type PMS Area type (see pms_area_t enum)
 */
const char *esp_memprot_pms_to_str(pms_area_t area_type);

/**
 * @brief Returns PMS splitting address for given Split line type
 *
 * The value is taken from PMS configuration registers (IRam0 range)
 * For details on split lines see 'esp_memprot_set_prot_int' function description
 *
 * @param line_type Split line type (see split_line_t enum)
 *
 * @return appropriate split line address
 */
uint32_t *esp_memprot_get_split_addr(split_line_t line_type);

/**
 * @brief Returns default main IRAM/DRAM splitting address
 *
 * The address value is given by _iram_text_end global (IRam0 range)

 * @return Main I/D split line (IRam0_DRam0_Split_Addr)
 */
void *esp_memprot_get_default_main_split_addr(void);

/**
 * @brief Sets a lock for the main IRAM/DRAM splitting address
 *
 * Locks can be unlocked only by digital system reset
 */
void esp_memprot_set_split_line_lock(void);

/**
 * @brief Gets a lock status for the main IRAM/DRAM splitting address
 *
 * @return true/false (locked/unlocked)
 */
bool esp_memprot_get_split_line_lock(void);

/**
 * @brief Sets required split line address
 *
 * @param line_type Split line type (see split_line_t enum)
 * @param line_addr target address from a memory range relevant to given line_type (IRAM/DRAM)
 */
void esp_memprot_set_split_line(split_line_t line_type, const void *line_addr);

/**
 * @brief Sets a lock for PMS Area settings of required Memory type
 *
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 */
void esp_memprot_set_pms_lock(mem_type_prot_t mem_type);

/**
 * @brief Gets a lock status for PMS Area settings of required Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return true/false (locked/unlocked)
 */
bool esp_memprot_get_pms_lock(mem_type_prot_t mem_type);

/**
 * @brief Sets permissions for given PMS Area in IRam0 memory range (MEMPROT_IRAM0_SRAM)
 *
 * @param area_type IRam0 PMS Area type (see pms_area_t enum)
 * @param r Read permission flag
 * @param w Write permission flag
 * @param x Execute permission flag
 */
void esp_memprot_iram_set_pms_area(pms_area_t area_type, bool r, bool w, bool x);

/**
 * @brief Gets current permissions for given PMS Area in IRam0 memory range (MEMPROT_IRAM0_SRAM)
 *
 * @param area_type IRam0 PMS Area type (see pms_area_t enum)
 * @param r Read permission flag holder
 * @param w Write permission flag holder
 * @param x Execute permission flag holder
 */
void esp_memprot_iram_get_pms_area(pms_area_t area_type, bool *r, bool *w, bool *x);

/**
 * @brief Sets permissions for given PMS Area in DRam0 memory range (MEMPROT_DRAM0_SRAM)
 *
 * @param area_type DRam0 PMS Area type (see pms_area_t enum)
 * @param r Read permission flag
 * @param w Write permission flag
 */
void esp_memprot_dram_set_pms_area(pms_area_t area_type, bool r, bool w);

/**
 * @brief Gets current permissions for given PMS Area in DRam0 memory range (MEMPROT_DRAM0_SRAM)
 *
 * @param area_type DRam0 PMS Area type (see pms_area_t enum)
 * @param r Read permission flag holder
 * @param w Write permission flag holder
 */
void esp_memprot_dram_get_pms_area(pms_area_t area_type, bool *r, bool *w);

/**
 * @brief Sets a lock for PMS interrupt monitor settings of required Memory type
 *
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 */
void esp_memprot_set_monitor_lock(mem_type_prot_t mem_type);

/**
 * @brief Gets a lock status for PMS interrupt monitor settings of required Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return true/false (locked/unlocked)
 */
bool esp_memprot_get_monitor_lock(mem_type_prot_t mem_type);

/**
 * @brief Enable PMS violation interrupt monitoring of required Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 * @param enable/disable
 */
void esp_memprot_set_monitor_en(mem_type_prot_t mem_type, bool enable);

/**
 * @brief Gets enable/disable status for PMS interrupt monitor settings of required Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return true/false (enabled/disabled)
 */
bool esp_memprot_get_monitor_en(mem_type_prot_t mem_type);

/**
 * @brief Gets CPU ID for currently active PMS violation interrupt
 *
 * @return CPU ID (CPU_PRO for ESP32C3)
 */
int IRAM_ATTR esp_memprot_intr_get_cpuid(void);

/**
 * @brief Clears current interrupt ON flag for given Memory type
 *
 * Interrupt clearing happens in two steps:
 *      1. Interrupt CLR flag is set (to clear the interrupt ON status)
 *      2. Interrupt CLR flag is reset (to allow further monitoring)
 * This operation is non-atomic by PMS module design
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 */
void IRAM_ATTR esp_memprot_monitor_clear_intr(mem_type_prot_t mem_type);

/**
 * @brief Returns active PMS violation interrupt (if any)
 *
 * This function iterates through supported Memory type status registers
 * and returns the first interrupt-on flag. If none is found active,
 * MEMPROT_NONE is returned.
 * Order of checking (in current version):
 *      1. MEMPROT_IRAM0_SRAM
 *      2. MEMPROT_DRAM0_SRAM
 *
 * @return mem_type Memory protection type related to active interrupt found (see mem_type_prot_t enum)
 */
mem_type_prot_t IRAM_ATTR esp_memprot_get_active_intr_memtype(void);

/**
 * @brief Checks whether any violation interrupt is active
 *
 * @return true/false (yes/no)
 */
bool IRAM_ATTR esp_memprot_is_locked_any(void);

/**
 * @brief Checks whether any violation interrupt is enabled
 *
 * @return true/false (yes/no)
 */
bool IRAM_ATTR esp_memprot_is_intr_ena_any(void);

/**
 * @brief Checks whether any violation interrupt is enabled
 *
 * @return true/false (yes/no)
 */
bool IRAM_ATTR esp_memprot_get_violate_intr_on(mem_type_prot_t mem_type);

/**
 * @brief Returns the address which caused the violation interrupt (if any)
 *
 * The address is taken from appropriate PMS violation status register, based given Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return faulting address
 */
uint32_t IRAM_ATTR esp_memprot_get_violate_addr(mem_type_prot_t mem_type);

/**
 * @brief Returns the World identifier of the code causing the violation interrupt (if any)
 *
 * The value is taken from appropriate PMS violation status register, based given Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return World identifier (see pms_world_t enum)
 */
pms_world_t IRAM_ATTR esp_memprot_get_violate_world(mem_type_prot_t mem_type);

/**
 * @brief Returns Read or Write operation type which caused the violation interrupt (if any)
 *
 * The value (bit) is taken from appropriate PMS violation status register, based given Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return PMS operation type relevant to mem_type parameter (se pms_operation_type_t)
 */
pms_operation_type_t IRAM_ATTR esp_memprot_get_violate_wr(mem_type_prot_t mem_type);

/**
 * @brief Returns LoadStore flag of the operation type which caused the violation interrupt (if any)
 *
 * The value (bit) is taken from appropriate PMS violation status register, based given Memory type
 * Effective only on IRam0 access
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return true/false (LoadStore bit on/off)
 */
bool IRAM_ATTR esp_memprot_get_violate_loadstore(mem_type_prot_t mem_type);

/**
 * @brief Returns byte-enables for the address which caused the violation interrupt (if any)
 *
 * The value is taken from appropriate PMS violation status register, based given Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return byte-enables
 */
uint32_t IRAM_ATTR esp_memprot_get_violate_byte_en(mem_type_prot_t mem_type);

/**
 * @brief Returns raw contents of DRam0 status register 1
 *
 * @return 32-bit register value
 */
uint32_t IRAM_ATTR esp_memprot_get_dram_status_reg_1(void);

/**
 * @brief Returns raw contents of DRam0 status register 2
 *
 * @return 32-bit register value
 */
uint32_t IRAM_ATTR esp_memprot_get_dram_status_reg_2(void);

/**
 * @brief Returns raw contents of IRam0 status register
 *
 * @return 32-bit register value
 */
uint32_t IRAM_ATTR esp_memprot_get_iram_status_reg(void);

/**
 * @brief Register PMS violation interrupt in global interrupt matrix for given Memory type
 *
 * Memory protection components uses specific interrupt number, see ETS_MEMPROT_ERR_INUM
 * The registration makes the panic-handler routine being called when the interrupt appears
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 */
void esp_memprot_set_intr_matrix(mem_type_prot_t mem_type);

/**
 * @brief Convenient routine for setting the PMS defaults
 *
 * Called on application startup, depending on CONFIG_ESP_SYSTEM_MEMPROT_FEATURE Kconfig settings
 * For implementation details see 'esp_memprot_set_prot_int' description
 *
 * @param invoke_panic_handler register all interrupts for panic handling (true/false)
 * @param lock_feature lock the defaults to prevent further PMS settings changes (true/false)
 * @param mem_type_mask 32-bit field of specific PMS parts to configure (see 'esp_memprot_set_prot_int')
 */
void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask);

/**
 * @brief Internal routine for setting the PMS defaults
 *
 * Called on application startup from within 'esp_memprot_set_prot'. Allows setting a specific splitting address
 * (main I/D split line) - see the parameter 'split_addr'. If the 'split_addr' equals to NULL, default I/D split line
 * is used (&_iram_text_end) and all the remaining lines share the same address.
 * The function sets all the split lines and PMS areas to the same space,
 * ie there is a single instruction space and single data space at the end.
 * The PMS split lines and permission areas scheme described below:
 *
 *                            DRam0/DMA                                     IRam0
 *                              -----------------------------------------------
 *                    ...       |                 IRam0_PMS_0                 |
 *               DRam0_PMS_0   -----------------------------------------------    IRam0_line1_Split_addr
 *                    ...       |                 IRam0_PMS_1                 |
 *                    ...       -----------------------------------------------   IRam0_line0_Split_addr
 *                              |                 IRam0_PMS_2                 |
 *                              ===============================================   IRam0_DRam0_Split_addr (main I/D)
 *                              |                 DRam0_PMS_1                 |
 * DRam0_DMA_line0_Split_addr   -----------------------------------------------       ...
 *                              |                 DRam0_PMS_2                 |       ...
 * DRam0_DMA_line1_Split_addr   -----------------------------------------------   IRam0_PMS_3
 *                              |                 DRam0_PMS_3                 |       ...
 *                              -----------------------------------------------
 *
 * Default settings provided by 'esp_memprot_set_prot_int' are as follows:
 *
 *                            DRam0/DMA                                     IRam0
 *                              -----------------------------------------------
 *                              |   IRam0_PMS_0 = IRam0_PMS_1 = IRam0_PMS_2   |
 *                              |                 DRam0_PMS_0                 |   IRam0_line1_Split_addr
 * DRam0_DMA_line0_Split_addr   |                                             |             =
 *               =              ===============================================   IRam0_line0_Split_addr
 * DRam0_DMA_line1_Split_addr   |                                             |             =
 *                              |   DRam0_PMS_1 = DRam0_PMS_2 = DRam0_PMS_3   |   IRam0_DRam0_Split_addr (main I/D)
 *                              |                 IRam0_PMS_3                 |
 *                              -----------------------------------------------
 *
 * Once the memprot feature is locked, it can be unlocked only by digital system reset
 *
 * @param invoke_panic_handler register all the violation interrupts for panic handling (true/false)
 * @param lock_feature lock the defaults to prevent further PMS settings changes (true/false)
 * @param split_addr specific main I/D adrees or NULL to use default ($_iram_text_end)
 * @param mem_type_mask 32-bit field of specific PMS parts to configure (members of mem_type_prot_t)
 */
void esp_memprot_set_prot_int(bool invoke_panic_handler, bool lock_feature, void *split_addr, uint32_t *mem_type_mask);

/**
 * @brief Returns raw contents of PMS interrupt monitor register for given Memory type
 *
 * @param mem_type Memory protection type (see mem_type_prot_t enum)
 *
 * @return 32-bit register value
 */
uint32_t esp_memprot_get_monitor_enable_reg(mem_type_prot_t mem_type);

#ifdef __cplusplus
}
#endif
