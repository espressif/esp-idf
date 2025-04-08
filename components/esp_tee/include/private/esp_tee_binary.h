/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

/* Declarations used inside TEE binary, only */

#define portNUM_PROCESSORS (1)
#define configNUM_CORES    (portNUM_PROCESSORS)
#define TEE_SECURE_INUM    (14)

#define ESP_TEE_M2U_SWITCH_MAGIC  0xfedef

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)      (((addr) + (SOC_MMU_PAGE_SIZE) - 1) & ~((SOC_MMU_PAGE_SIZE) - 1))
#define ALIGN_DOWN_TO_MMU_PAGE_SIZE(addr)    ((addr) & ~((SOC_MMU_PAGE_SIZE) - 1))

#if ((CONFIG_SECURE_TEE_IRAM_SIZE) & (0xFF))
#error "CONFIG_SECURE_TEE_IRAM_SIZE must be 256-byte (0x100) aligned"
#endif

#if ((CONFIG_SECURE_TEE_DRAM_SIZE) & (0xFF))
#error "CONFIG_SECURE_TEE_DRAM_SIZE must be 256-byte (0x100) aligned"
#endif

#if ((CONFIG_SECURE_TEE_STACK_SIZE) & 0xF)
#error "CONFIG_SECURE_TEE_STACK_SIZE must be 16-byte (0x10) aligned"
#endif

#if ((CONFIG_SECURE_TEE_INTR_STACK_SIZE) & 0xF)
#error "CONFIG_SECURE_TEE_INTR_STACK_SIZE must be 16-byte (0x10) aligned"
#endif

/* TEE Secure Storage partition label and NVS namespace */
#define ESP_TEE_SEC_STG_PART_LABEL    "secure_storage"
#define ESP_TEE_SEC_STG_NVS_NAMESPACE "tee_sec_stg_ns"

/* NOTE: ESP32-C6 - TEE/REE memory regions */
/* TEE I/DRAM */
#define SOC_S_IRAM_START      (SOC_IRAM_LOW)
#define SOC_S_IRAM_END        (SOC_S_IRAM_START + CONFIG_SECURE_TEE_IRAM_SIZE)
#define SOC_S_DRAM_START      (SOC_S_IRAM_END)
#define SOC_S_DRAM_END        (SOC_S_IRAM_END + CONFIG_SECURE_TEE_DRAM_SIZE)
#define SOC_NS_IRAM_START     (SOC_S_DRAM_END)
/* TEE I/DROM */
#define SOC_S_IDROM_SIZE                   (CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE)
#define SOC_S_IDROM_MMU_PAGE_NUM           (SOC_S_IDROM_SIZE / SOC_MMU_PAGE_SIZE)
#define SOC_S_IROM_LOW                     (SOC_IROM_LOW)
#define SOC_S_IROM_HIGH                    (SOC_IROM_LOW + SOC_S_IDROM_SIZE)
#define SOC_S_DROM_LOW                     (SOC_DROM_LOW)
#define SOC_S_DROM_HIGH                    (SOC_DROM_LOW + SOC_S_IDROM_SIZE)
/* REE I/DRAM */
#define SOC_NS_IDRAM_START    (SOC_S_DRAM_END)
#define SOC_NS_IDRAM_END      (SOC_DIRAM_DRAM_HIGH)

#define SOC_MMU_TOTAL_SIZE                 (SOC_DRAM0_CACHE_ADDRESS_HIGH - SOC_DRAM0_CACHE_ADDRESS_LOW)
#define SOC_MMU_END_VADDR                  (SOC_DROM_LOW + SOC_MMU_TOTAL_SIZE)
#define SOC_S_MMU_MMAP_RESV_PAGE_NUM       (SOC_S_IDROM_MMU_PAGE_NUM + 1)
#define SOC_S_MMU_MMAP_RESV_START_VADDR    (SOC_MMU_END_VADDR - SOC_S_MMU_MMAP_RESV_PAGE_NUM * SOC_MMU_PAGE_SIZE)

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <stdarg.h>

/* Secure Service table */
typedef void (*secure_service_t)(void);
extern const secure_service_t tee_secure_service_table[];

/**
 * @brief TEE initialization function called by the bootloader at boot time.
 *        Performs secure system initialization before switching to the REE.
 *
 * @param ree_entry_addr  entry point to the App where TEE jump after completing secure initialization
 * @param ree_drom_addr   DROM address of the selected non-secure app for determining the running non-secure app partition
 * @param tee_boot_part   partition subtype of the active TEE partition
 */
void esp_tee_init(uint32_t ree_entry_addr, uint32_t ree_drom_addr, uint8_t tee_boot_part);

/**
 * @brief SoC-specific TEE secure initialization
 */
void esp_tee_soc_secure_sys_init(void);

/**
 * @brief Configure region protection through RISC-V PMP/PMA for TEE
 */
void esp_tee_configure_region_protection(void);

/**
 * @brief Configure APM protection for TEE
 */
void esp_tee_configure_apm_protection(void);

/**
 * @brief Switch to the REE app after TEE initialization is complete
 *
 * @param ree_entry_addr REE app entry address
 */
void esp_tee_switch_to_ree(uint32_t ree_entry_addr);

/**
 * @brief Secure service call entry point for the TEE binary.
 *        This function deciphers the call from the REE and
 *        dispatches the appropriate secure service API in the TEE.
 *
 * @param argc Number of arguments passed to the secure service API
 * @param ap   List of input arguments
 *
 * @return Return value from the secure service API
 */
int esp_tee_service_dispatcher(int argc, va_list ap);

#endif // ifndef __ASSEMBLER__

#ifdef __cplusplus
}
#endif
