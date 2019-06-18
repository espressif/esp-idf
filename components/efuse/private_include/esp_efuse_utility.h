// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_efuse.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/esp_efuse_utility.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/esp_efuse_utility.h"
#endif

/**
 * @brief Structure range address by blocks
 */
typedef struct {
    uint32_t start;
    uint32_t end;
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
 */
void esp_efuse_utility_burn_efuses(void);

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
esp_err_t esp_efuse_utility_write_reg(unsigned int num_reg, esp_efuse_block_t efuse_block, uint32_t reg_to_write);

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
 * @brief   Efuse read operation: copies data from physical efuses to efuse read registers.
 */
void esp_efuse_utility_clear_program_registers(void);

#ifdef __cplusplus
}
#endif
