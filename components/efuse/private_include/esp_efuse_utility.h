/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_efuse.h"
#include "sdkconfig.h"
#include_next "esp_efuse_utility.h"

#define ESP_EFUSE_CHK(ret)       \
    do                           \
    {                            \
        if( ( err = (ret) ) != ESP_OK ) \
            goto err_exit;        \
    } while( 0 )


/**
 * @brief Structure range address by blocks
 */
typedef struct {
    uintptr_t start;
    uintptr_t end;
} esp_efuse_range_addr_t;

/**
 * @brief This is type of function that will handle the efuse field register.
 *
 * @param[in] num_reg          The register number in the block.
 * @param[in] efuse_block      Block number.
 * @param[in] bit_start        Start bit in the register.
 * @param[in] bit_count        The number of bits used in the register.
 * @param[in/out] arr          A pointer to an array or variable.
 * @param[in/out] bits_counter Counter bits.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - other efuse component errors.
 */
typedef esp_err_t (*efuse_func_proc_t) (unsigned int num_reg, esp_efuse_block_t efuse_block, int starting_bit_num_in_reg, int num_bits_used_in_reg, void* arr, int* bits_counter);

/**
 * @brief This function processes the field by calling the passed function.
 *
 * This function selects the field, checks the length, and calls the register processing function.
 * @param[in] field           A pointer to the structure describing the fields of efuse.
 * @param[in/out] ptr         A pointer to an array that is used to read / write from / to the efuse field.
 * @param[in] ptr_size_bits   The size of the data in bits for the efuse field. if = 0 then read all field bits.
 * @param[in] func_proc       This is the function that will handle the efuse fields.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - other efuse component errors.
 */
esp_err_t esp_efuse_utility_process(const esp_efuse_desc_t* field[], void* ptr, size_t ptr_size_bits, efuse_func_proc_t func_proc);

/**
 * @brief Write register with the required number of "1" bits.
 * @param[in/out] cnt      The number of bits you need to set in the field.
 */
esp_err_t esp_efuse_utility_write_cnt(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* cnt, int* bits_counter);

/**
 * @brief Fill registers from array for writing.
 * @param[in] arr_in       A pointer to an array in which the data for the writing.
 */
esp_err_t esp_efuse_utility_write_blob(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* arr_in, int* bits_counter);

/**
 * @brief Count a set bits in register.
 * @param[in/out] out_cnt  A pointer to size_t variable which will contain the number of "1" bits.
 */
esp_err_t esp_efuse_utility_count_once(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* out_cnt, int* bits_counter);

/**
 * @brief Read efuse register and write this value to array.
 * @param[out] arr_out     A pointer to array that will contain the result of reading.
 */
esp_err_t esp_efuse_utility_fill_buff(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* arr_out, int* bits_counter);

/**
 * @brief Burn values written to the efuse write registers.
 *
 * If CONFIG_EFUSE_VIRTUAL is set, writing will not be performed.
 * After the function is completed, the writing registers are cleared.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - ESP_FAIL: The operation was not successfully completed.
 */
esp_err_t esp_efuse_utility_burn_efuses(void);

/**
 * @brief Chip specific operations to perform the burn of values written to the efuse write registers.
 *
 * @note Use esp_efuse_utility_burn_efuses() to burn efuses.
 *
 * If CONFIG_EFUSE_VIRTUAL is set, writing will not be performed.
 * After the function is completed, the writing registers are cleared.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - ESP_FAIL: The operation was not successfully completed.
 */
esp_err_t esp_efuse_utility_burn_chip(void);

/**
 * @brief Chip specific operations to perform the burn of values written to the efuse write registers.
 *
 * If CONFIG_EFUSE_VIRTUAL is set, writing will not be performed.
 * After the function is completed, the writing registers are cleared.
 *
 * @param[in] ignore_coding_errors If this is true and any coding errors occur,
 *                                 they will be ignored and no further attempts
 *                                 will be made to correct them.
 * @param[in] verify_written_data  If this is true, then after burning it will check that all data is set correctly.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - ESP_FAIL: The operation was not successfully completed.
 */
esp_err_t esp_efuse_utility_burn_chip_opt(bool ignore_coding_errors, bool verify_written_data);

/**
 * @brief Returns the number of array elements for placing these "bits" in an array with the length of each element equal to "size_of_base".
 */
int esp_efuse_utility_get_number_of_items(int bits, int size_of_base);

/**
 * @brief Reading efuse register.
 */
uint32_t esp_efuse_utility_read_reg(esp_efuse_block_t blk, unsigned int num_reg);

/**
 * @brief Writing efuse register with checking of repeated programming of programmed bits.
 */
esp_err_t esp_efuse_utility_write_reg(esp_efuse_block_t efuse_block, unsigned int num_reg, uint32_t reg_to_write);

/* @brief Reset efuse write registers
 *
 * Efuse write registers are written to zero, to negate
 * any changes that have been staged here.
 */
void esp_efuse_utility_reset(void);

/**
 * @brief   Fills the virt_blocks array by values from efuse_Rdata.
 */
void esp_efuse_utility_update_virt_blocks(void);

/**
 * @brief   Prints efuse values for all registers.
 */
void esp_efuse_utility_debug_dump_blocks(void);

/**
 * @brief   Prints efuse values for a single block.
 * @param[in] num_block Index of efuse block.
 * @param[in] from_read Take data from:
 *                      true - read area (already burned efuses),
 *                      false - write area (not yet burned efuses, prepared for burn).
 */
void esp_efuse_utility_debug_dump_single_block(int num_block, bool from_read);

/**
 * @brief   Erase the virt_blocks array.
 */
void esp_efuse_utility_erase_virt_blocks(void);

/**
 * @brief   Apply coding_scheme to write registers.
 *
 * @return
 *         - ESP_OK: The operation was successfully completed.
 *         - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_utility_apply_new_coding_scheme(void);

/**
 * @brief   Checks eFuse errors in BLOCK0.
 *
 * @note Refers to ESP32-C3 only.
 *
 * It does a BLOCK0 check if eFuse EFUSE_ERR_RST_ENABLE is set.
 * If BLOCK0 has an error, it prints the error and returns ESP_FAIL, which should be treated as esp_restart.
 *
 * @return
 *         - ESP_OK: No errors in BLOCK0.
 *         - ESP_FAIL: Error in BLOCK0 requiring reboot.
 */
esp_err_t esp_efuse_utility_check_errors(void);

/**
 * @brief   Efuse read operation: copies data from physical efuses to efuse read registers.
 */
void esp_efuse_utility_clear_program_registers(void);

#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
/**
 * @brief   Writes eFuses to the efuse flash partition.
 *
 * Used only when CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH is set.
 */
void esp_efuse_utility_write_efuses_to_flash(void);

/**
 * @brief   Loads efuses from efuse flash partition.
 *
 * Used only when CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH is set.
 */
bool esp_efuse_utility_load_efuses_from_flash(void);

/**
 * @brief   Erase efuse flash partition.
 *
 * Used only when CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH is set.
 */
void esp_efuse_utility_erase_efuses_in_flash(void);
#endif

/**
 * @brief Return the address of a particular efuse block's first read register
 *
 * @param[in] block Index of efuse block to look up
 *
 * @return a numeric read register address of the first word in the block.
 */
uintptr_t esp_efuse_utility_get_read_register_address(esp_efuse_block_t block);

/**
 * @brief Checks the correctness of burned data in the given block.
 *
 * @note Internal use. Do not call it.
 *
 * @param[in] block Index of efuse block.
 * @param[in] r_data_len Block length for reading data in bytes (multiple of 4).
 *
 * @return True  - written data are correct.
 *         False - written data are incorrect.
 */
bool esp_efuse_utility_is_correct_written_data(esp_efuse_block_t block, unsigned r_data_len);

#ifdef __cplusplus
}
#endif
