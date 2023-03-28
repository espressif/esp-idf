/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* INTERNAL API
 * generic interface to MMU memory protection features
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_attr.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//convenient constants for better code readabilty
#define RD_ENA                  true
#define RD_DIS                  false
#define WR_ENA                  true
#define WR_DIS                  false
#define EX_ENA                  true
#define EX_DIS                  false
#define RD_LOW_ENA              true
#define RD_LOW_DIS              false
#define WR_LOW_ENA              true
#define WR_LOW_DIS              false
#define EX_LOW_ENA              true
#define EX_LOW_DIS              false
#define RD_HIGH_ENA             true
#define RD_HIGH_DIS             false
#define WR_HIGH_ENA             true
#define WR_HIGH_DIS             false
#define EX_HIGH_ENA             true
#define EX_HIGH_DIS             false
#define PANIC_HNDL_ON           true
#define PANIC_HNDL_OFF          false
#define MEMPROT_LOCK            true
#define MEMPROT_UNLOCK          false
#define DEF_SPLIT_LINE          NULL

#define MEMPROT_INVALID_ADDRESS -1

//memory range types
typedef enum {
    MEMPROT_NONE =              0x00000000,
    MEMPROT_IRAM0_SRAM =        0x00000001, //0x40020000-0x4006FFFF, RWX
    MEMPROT_DRAM0_SRAM =        0x00000002, //0x3FFB0000-0x3FFFFFFF, RW
    MEMPROT_IRAM0_RTCFAST =     0x00000004, //0x40070000-0x40071FFF, RWX
    MEMPROT_DRAM0_RTCFAST =     0x00000008, //0x3FF9E000-0x3FF9FFFF, RW
    MEMPROT_PERI1_RTCSLOW =     0x00000010, //0x3F421000-0x3F423000, RW
    MEMPROT_PERI2_RTCSLOW_0 =   0x00000020, //0x50001000-0x50003000, RWX
    MEMPROT_PERI2_RTCSLOW_1 =   0x00000040, //0x60002000-0x60004000, RWX
    MEMPROT_ALL =               0xFFFFFFFF
} mem_type_prot_t;


/**
 * @brief Returns splitting address for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Splitting address for the memory region required.
 * The address is given by region-specific global symbol exported from linker script,
 * it is not read out from related configuration register.
 */
uint32_t * esp_memprot_get_split_addr(mem_type_prot_t mem_type);

/**
 * @brief Initializes illegal memory access control for required memory section.
 *
 * All memory access interrupts share ETS_MEMACCESS_ERR_INUM input channel, it is caller's
 * responsibility to properly detect actual intr. source as well as possible prioritization in case
 * of multiple source reported during one intr.handling routine run
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)\
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_intr_init(mem_type_prot_t mem_type);

/**
 * @brief Enable/disable the memory protection interrupt
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param enable enable/disable
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_intr_ena(mem_type_prot_t mem_type, bool enable);

/**
 * @brief Sets a request for clearing interrupt-on flag for specified memory region (register write)
 *
 * @note When called without actual interrupt-on flag set, subsequent occurrence of related interrupt is ignored.
 * Should be used only after the real interrupt appears, typically as the last step in interrupt handler's routine.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_clear_intr(mem_type_prot_t mem_type);

/**
 * @brief Detects which memory protection interrupt is active
 *
 * @note Check order
 *          MEMPROT_IRAM0_SRAM
 *          MEMPROT_IRAM0_RTCFAST
 *          MEMPROT_DRAM0_SRAM
 *          MEMPROT_DRAM0_RTCFAST
 *
 * @return Memory protection area type (see mem_type_prot_t enum)
 */
mem_type_prot_t  esp_memprot_get_active_intr_memtype(void);

/**
 * @brief Gets interrupt status register contents for specified memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param fault_reg_val Contents of status register
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_fault_reg(mem_type_prot_t mem_type, uint32_t *fault_reg_val);

/**
 * @brief Get details of given interrupt status
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param faulting_address Faulting address causing the interrupt [out]
 * @param op_type Operation being processed at the faulting address [out]
 *               IRAM0: 0 - read, 1 - write
 *               DRAM0: 0 - read, 1 - write
 * @param op_subtype Additional info for op_type [out]
 *               IRAM0: 0 - instruction segment access, 1 - data segment access
 *               DRAM0: 0 - non-atomic operation, 1 - atomic operation
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t  esp_memprot_get_fault_status(mem_type_prot_t mem_type, uint32_t **faulting_address, uint32_t *op_type, uint32_t *op_subtype);

/**
 * @brief Gets string representation of required memory region identifier
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return mem_type as string
 */
const char * esp_memprot_type_to_str(mem_type_prot_t mem_type);

/**
 * @brief Detects whether any of the interrupt locks is active (requires digital system reset to unlock)
 *
 * @return true/false
 */
bool esp_memprot_is_locked_any(void);

/**
 * @brief Sets lock for specified memory region.
 *
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_lock(mem_type_prot_t mem_type);

/**
 * @brief Gets lock status for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param locked Settings locked: true/false (locked/unlocked)
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_lock(mem_type_prot_t mem_type, bool *locked);

/**
 * @brief Gets permission control configuration register contents for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param conf_reg_val Permission control register contents
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_conf_reg(mem_type_prot_t mem_type, uint32_t *conf_reg_val);

/**
 * @brief Gets interrupt permission settings for unified management block
 *
 * Gets interrupt permission settings register contents for required memory region, returns settings for unified management blocks
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param perm_reg Permission settings register contents
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_uni_reg(mem_type_prot_t mem_type, uint32_t *perm_reg);

/**
 * @brief Gets interrupt permission settings for split management block
 *
 * Gets interrupt permission settings register contents for required memory region (unified management blocks)
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @return split_reg Unified management settings register contents
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_split_reg(mem_type_prot_t mem_type, uint32_t *split_reg);

/**
 * @brief Detects whether any of the memory protection interrupts is enabled
 *
 * @return true/false
 */
bool esp_memprot_is_intr_ena_any(void);

/**
 * @brief Gets interrupt-enabled flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param enable_bit Interrupt-enabled flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_intr_ena_bit(mem_type_prot_t mem_type, uint32_t *enable_bit);

/**
 * @brief Gets interrupt-active flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param intr_on_bit Interrupt-active flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure */
esp_err_t esp_memprot_get_intr_on_bit(mem_type_prot_t mem_type, uint32_t *intr_on_bit);

/**
 * @brief Gets interrupt-clear request flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param clear_bit Interrupt-clear request flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_intr_clr_bit(mem_type_prot_t mem_type, uint32_t *clear_bit);

/**
 * @brief Gets read permission value for specified block and memory region
 *
 * Returns read permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable to all memory types.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param read_bit Read permission value for required block
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_uni_block_read_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *read_bit);

/**
 * @brief Gets write permission value for specified block and memory region
 *
 * Returns write permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable to all memory types.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param write_bit Write permission value for required block
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_uni_block_write_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *write_bit);

/**
 * @brief Gets execute permission value for specified block and memory region
 *
 * Returns execute permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param exec_bit Execute permission value for required block
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_uni_block_exec_bit(mem_type_prot_t mem_type, uint32_t block, uint32_t *exec_bit);

/**
 * @brief Sets permissions for specified block in DRAM region
 *
 * Sets Read and Write permission for specified unified-management block (0-3) in given memory region.
 * Applicable only to DRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param write_perm Write permission flag
 * @param read_perm Read permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_uni_block_perm_dram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm);

/**
 * @brief Sets permissions for high and low memory segment in DRAM region
 *
 * Sets Read and Write permission for both low and high memory segments given by splitting address.
 * The splitting address must be equal to or higher then beginning of block 5
 * Applicable only to DRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param split_addr Address to split the memory region to lower and higher segment
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_prot_dram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr);

/**
 * @brief Sets permissions for specified block in IRAM region
 *
 * Sets Read, Write and Execute permission for specified unified-management block (0-3) in given memory region.
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (MEMPROT_IRAM0_SRAM)
 * @param block Memory block identifier (0-3)
 * @param write_perm Write permission flag
 * @param read_perm Read permission flag
 * @param exec_perm Execute permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 *         ESP_ERR_INVALID_ARG on incorrect block number
 */
esp_err_t esp_memprot_set_uni_block_perm_iram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm, bool exec_perm);

/**
 * @brief Sets permissions for high and low memory segment in IRAM region
 *
 * Sets Read, Write and Execute permission for both low and high memory segments given by splitting address.
 * The splitting address must be equal to or higher then beginning of block 5
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param split_addr Address to split the memory region to lower and higher segment
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_prot_iram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx);

/**
 * @brief Activates memory protection for all supported memory region types
 *
 * @note The feature is disabled when JTAG interface is connected
 *
 * @param invoke_panic_handler map mem.prot interrupt to ETS_MEMACCESS_ERR_INUM and thus invokes panic handler when fired ('true' not suitable for testing)
 * @param lock_feature sets LOCK bit, see esp_memprot_set_lock() ('true' not suitable for testing)
 * @param mem_type_mask holds a set of required memory protection types (bitmask built of mem_type_prot_t). NULL means default (MEMPROT_ALL in this version)
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature, uint32_t *mem_type_mask);

/**
 * @brief Get permission settings bits for IRAM0 split mgmt. Only IRAM0 memory types allowed
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_split_bits_iram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx);

/**
 * @brief Get permission settings bits for DRAM0 split mgmt. Only DRAM0 memory types allowed
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_split_bits_dram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr);

/**
 * @brief Sets permissions for high and low memory segment in PERIBUS1 region
 *
 * Sets Read and Write permission for both low and high memory segments given by splitting address.
 * Applicable only to PERIBUS1 memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param split_addr Address to split the memory region to lower and higher segment
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_set_prot_peri1(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr);

/**
 * @brief Get permission settings bits for PERIBUS1 split mgmt. Only PERIBUS1 memory types allowed
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_split_bits_peri1(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr);

/**
 * @brief Get permission settings bits for PERIBUS2 split mgmt. Only PERIBUS2 memory types allowed
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on failure
 */
esp_err_t esp_memprot_get_perm_split_bits_peri2(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx);

/**
 * @brief Configures the memory protection for high and low segment in PERIBUS2 region
 *
 * Sets Read Write permission for both low and high memory segments given by splitting address.
 * Applicable only to PERIBUS2 memory types
 *
 * @param mem_type Memory protection area type (MEMPROT_PERI2_RTCSLOW_0, MEMPROT_PERI2_RTCSLOW_1)
 * @param split_addr Address to split the memory region to lower and higher segment (32bit aligned)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 *         ESP_ERR_INVALID_STATE on splitting address out of PERIBUS2 range
 *         ESP_ERR_INVALID_SIZE on splitting address not 32-bit aligned
 */
esp_err_t esp_memprot_set_prot_peri2(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx);

/**
 * @brief Get permissions for specified memory type. Irrelevant bits are ignored
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on NULL lw/lr/lx/hw/hr/hx args
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_get_permissions(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx);

/**
 * @brief Get Read permission settings for low and high regions of given memory type
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lr Low segment Read permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on NULL lr/hr args
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_get_perm_read(mem_type_prot_t mem_type, bool *lr, bool *hr);

/**
 * @brief Get Write permission settings for low and high regions of given memory type
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lr Low segment Write permission flag
 * @param hr High segment Write permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on NULL lw/hw args
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_get_perm_write(mem_type_prot_t mem_type, bool *lw, bool *hw);

/**
 * @brief Get Execute permission settings for low and high regions of given memory type
 * Applicable only to IBUS-compatible memory types
 *
 * @param mem_type Memory protection area type (MEMPROT_IRAM0_SRAM, MEMPROT_IRAM0_RTCFAST, MEMPROT_PERI2_RTCSLOW_0, MEMPROT_PERI2_RTCSLOW_1)
 * @param lx Low segment Exec permission flag
 * @param hx High segment Exec permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on NULL lx/hx args
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_get_perm_exec(mem_type_prot_t mem_type, bool *lx, bool *hx);

/**
 * @brief Returns the lowest address in required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Required address or MEMPROT_INVALID_ADDRESS for invalid mem_type
 */
uint32_t esp_memprot_get_low_limit(mem_type_prot_t mem_type);

/**
 * @brief Returns the highest address in required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Required address or MEMPROT_INVALID_ADDRESS for invalid mem_type
 */
uint32_t esp_memprot_get_high_limit(mem_type_prot_t mem_type);

/**
 * @brief Sets READ permission bit for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lr Low segment Read permission flag
 * @param hr High segment Read permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_set_read_perm(mem_type_prot_t mem_type, bool lr, bool hr);

/**
 * @brief Sets WRITE permission bit for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lr Low segment Write permission flag
 * @param hr High segment Write permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_set_write_perm(mem_type_prot_t mem_type, bool lw, bool hw);

/**
 * @brief Sets EXECUTE permission bit for required memory region
 *
 * @param mem_type Memory protection area type (MEMPROT_IRAM0_SRAM, MEMPROT_IRAM0_RTCFAST, MEMPROT_PERI2_RTCSLOW_0, MEMPROT_PERI2_RTCSLOW_1)
 * @param lr Low segment Exec permission flag
 * @param hr High segment Exec permission flag
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_SUPPORTED on invalid mem_type
 */
esp_err_t esp_memprot_set_exec_perm(mem_type_prot_t mem_type, bool lx, bool hx);


#ifdef __cplusplus
}
#endif
