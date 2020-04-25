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

#ifndef _ESP_EFUSE_MANAGER_H_
#define _ESP_EFUSE_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"

#define ESP_ERR_EFUSE                              0x1600                     /*!< Base error code for efuse api. */
#define ESP_OK_EFUSE_CNT                          (ESP_ERR_EFUSE + 0x01)      /*!< OK the required number of bits is set. */
#define ESP_ERR_EFUSE_CNT_IS_FULL                 (ESP_ERR_EFUSE + 0x02)      /*!< Error field is full. */
#define ESP_ERR_EFUSE_REPEATED_PROG               (ESP_ERR_EFUSE + 0x03)      /*!< Error repeated programming of programmed bits is strictly forbidden. */
#define ESP_ERR_CODING                            (ESP_ERR_EFUSE + 0x04)      /*!< Error while a encoding operation. */

/**
 * @brief Type of eFuse blocks
 */
typedef enum {
    EFUSE_BLK0 = 0,     /**< Number of eFuse block. Reserved. */
    EFUSE_BLK1 = 1,     /**< Number of eFuse block. Used for Flash Encryption. If not using that Flash Encryption feature, they can be used for another purpose. */
    EFUSE_BLK2 = 2,     /**< Number of eFuse block. Used for Secure Boot. If not using that Secure Boot feature, they can be used for another purpose. */
    EFUSE_BLK3 = 3      /**< Number of eFuse block. Uses for the purpose of the user. */
} esp_efuse_block_t;

/**
 * @brief Type of coding scheme
 */
typedef enum {
    EFUSE_CODING_SCHEME_NONE    = 0,    /**< None */
    EFUSE_CODING_SCHEME_3_4     = 1,    /**< 3/4 coding */
    EFUSE_CODING_SCHEME_REPEAT  = 2,    /**< Repeat coding */
} esp_efuse_coding_scheme_t;

/**
* @brief Structure eFuse field
 */
typedef struct {
    esp_efuse_block_t   efuse_block: 8; /**< Block of eFuse */
    uint8_t             bit_start;      /**< Start bit [0..255] */
    uint16_t            bit_count;      /**< Length of bit field [1..-]*/
} esp_efuse_desc_t;

/**
 * @brief   Reads bits from EFUSE field and writes it into an array.
 *
 * The number of read bits will be limited to the minimum value
 * from the description of the bits in "field" structure or "dst_size_bits" required size.
 * Use "esp_efuse_get_field_size()" function to determine the length of the field.
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 * @param[out] dst            A pointer to array that will contain the result of reading.
 * @param[in]  dst_size_bits  The number of bits required to read.
 *                            If the requested number of bits is greater than the field,
 *                            the number will be limited to the field size.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 */
esp_err_t esp_efuse_read_field_blob(const esp_efuse_desc_t* field[], void* dst, size_t dst_size_bits);

/**
 * @brief   Reads bits from EFUSE field and returns number of bits programmed as "1".
 *
 * If the bits are set not sequentially, they will still be counted.
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 * @param[out] out_cnt        A pointer that will contain the number of programmed as "1" bits.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 */
esp_err_t esp_efuse_read_field_cnt(const esp_efuse_desc_t* field[], size_t* out_cnt);

/**
 * @brief   Writes array to EFUSE field.
 *
 * The number of write bits will be limited to the minimum value
 * from the description of the bits in "field" structure or "src_size_bits" required size.
 * Use "esp_efuse_get_field_size()" function to determine the length of the field.
 * After the function is completed, the writing registers are cleared.
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 * @param[in]  src            A pointer to array that contains the data for writing.
 * @param[in]  src_size_bits  The number of bits required to write.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_write_field_blob(const esp_efuse_desc_t* field[], const void* src, size_t src_size_bits);

/**
 * @brief   Writes a required count of bits as "1" to EFUSE field.
 *
 * If there are no free bits in the field to set the required number of bits to "1",
 * ESP_ERR_EFUSE_CNT_IS_FULL error is returned, the field will not be partially recorded.
 * After the function is completed, the writing registers are cleared.
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 * @param[in]  cnt            Required number of programmed as "1" bits.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_CNT_IS_FULL: Not all requested cnt bits is set.
 */
esp_err_t esp_efuse_write_field_cnt(const esp_efuse_desc_t* field[], size_t cnt);

/**
 * @brief   Sets a write protection for the whole block.
 *
 * After that, it is impossible to write to this block.
 * The write protection does not apply to block 0.
 * @param[in]  blk          Block number of eFuse. (EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3)
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_CNT_IS_FULL: Not all requested cnt bits is set.
 *    - ESP_ERR_NOT_SUPPORTED: The block does not support this command.
 */
esp_err_t esp_efuse_set_write_protect(esp_efuse_block_t blk);

/**
 * @brief   Sets a read protection for the whole block.
 *
 * After that, it is impossible to read from this block.
 * The read protection does not apply to block 0.
 * @param[in]  blk          Block number of eFuse. (EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3)
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_CNT_IS_FULL: Not all requested cnt bits is set.
 *    - ESP_ERR_NOT_SUPPORTED: The block does not support this command.
 */
esp_err_t esp_efuse_set_read_protect(esp_efuse_block_t blk);

/**
 * @brief   Returns the number of bits used by field.
 *
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 *
 * @return Returns the number of bits used by field.
 */
int esp_efuse_get_field_size(const esp_efuse_desc_t* field[]);

/**
 * @brief   Returns value of efuse register.
 *
 * This is a thread-safe implementation.
 * Example: EFUSE_BLK2_RDATA3_REG where (blk=2, num_reg=3)
 * @param[in]  blk     Block number of eFuse.
 * @param[in]  num_reg The register number in the block.
 *
 * @return Value of register
 */
uint32_t esp_efuse_read_reg(esp_efuse_block_t blk, unsigned int num_reg);

/**
 * @brief   Write value to efuse register.
 *
 * Apply a coding scheme if necessary.
 * This is a thread-safe implementation.
 * Example: EFUSE_BLK3_WDATA0_REG where (blk=3, num_reg=0)
 * @param[in]  blk     Block number of eFuse.
 * @param[in]  num_reg The register number in the block.
 * @param[in]  val     Value to write.
 *
 * @return
 *      - ESP_OK: The operation was successfully completed.
 *      - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 */
esp_err_t esp_efuse_write_reg(esp_efuse_block_t blk, unsigned int num_reg, uint32_t val);

/**
 * @brief   Return efuse coding scheme for blocks.
 *
 * Note: The coding scheme is applicable only to 1, 2 and 3 blocks. For 0 block, the coding scheme is always ``NONE``.
 *
 * @param[in]  blk     Block number of eFuse.
 * @return Return efuse coding scheme for blocks
 */
esp_efuse_coding_scheme_t esp_efuse_get_coding_scheme(esp_efuse_block_t blk);

/**
 * @brief   Read key to efuse block starting at the offset and the required size.
 *
 * @param[in]  blk             Block number of eFuse.
 * @param[in]  dst_key         A pointer to array that will contain the result of reading.
 * @param[in]  offset_in_bits  Start bit in block.
 * @param[in]  size_bits       The number of bits required to read.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_read_block(esp_efuse_block_t blk, void* dst_key, size_t offset_in_bits, size_t size_bits);

/**
 * @brief   Write key to efuse block starting at the offset and the required size.
 *
 * @param[in]  blk             Block number of eFuse.
 * @param[in]  src_key         A pointer to array that contains the key for writing.
 * @param[in]  offset_in_bits  Start bit in block.
 * @param[in]  size_bits       The number of bits required to write.
 *
 * @return
 *    - ESP_OK: The operation was successfully completed.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits
 */
esp_err_t esp_efuse_write_block(esp_efuse_block_t blk, const void* src_key, size_t offset_in_bits, size_t size_bits);

/**
 * @brief   Returns chip version from efuse
 *
 * @return chip version
 */
uint8_t esp_efuse_get_chip_ver(void);

/**
 * @brief   Returns chip package from efuse
 *
 * @return chip package
 */
uint32_t esp_efuse_get_pkg_ver(void);

/* @brief Permanently update values written to the efuse write registers
 *
 * After updating EFUSE_BLKx_WDATAx_REG registers with new values to
 * write, call this function to permanently write them to efuse.
 *
 * @note Setting bits in efuse is permanent, they cannot be unset.
 *
 * @note Due to this restriction you don't need to copy values to
 * Efuse write registers from the matching read registers, bits which
 * are set in the read register but unset in the matching write
 * register will be unchanged when new values are burned.
 *
 * @note This function is not threadsafe, if calling code updates
 * efuse values from multiple tasks then this is caller's
 * responsibility to serialise.
 *
 * After burning new efuses, the read registers are updated to match
 * the new efuse values.
 */
void esp_efuse_burn_new_values(void);

/* @brief Reset efuse write registers
 *
 * Efuse write registers are written to zero, to negate
 * any changes that have been staged here.
 *
 * @note This function is not threadsafe, if calling code updates
 * efuse values from multiple tasks then this is caller's
 * responsibility to serialise.
 */
void esp_efuse_reset(void);

/* @brief Disable BASIC ROM Console via efuse
 *
 * By default, if booting from flash fails the ESP32 will boot a
 * BASIC console in ROM.
 *
 * Call this function (from bootloader or app) to permanently
 * disable the console on this chip.
 */
void esp_efuse_disable_basic_rom_console(void);

/* @brief Encode one or more sets of 6 byte sequences into
 * 8 bytes suitable for 3/4 Coding Scheme.
 *
 * This function is only useful if the CODING_SCHEME efuse
 * is set to value 1 for 3/4 Coding Scheme.
 *
 * @param[in] in_bytes Pointer to a sequence of bytes to encode for 3/4 Coding Scheme. Must have length in_bytes_len. After being written to hardware, these bytes will read back as little-endian words.
 * @param[out] out_words Pointer to array of words suitable for writing to efuse write registers. Array must contain 2 words (8 bytes) for every 6 bytes in in_bytes_len. Can be a pointer to efuse write registers.
 * @param in_bytes_len. Length of array pointed to by in_bytes, in bytes. Must be a multiple of 6.
 *
 * @return ESP_ERR_INVALID_ARG if either pointer is null or in_bytes_len is not a multiple of 6. ESP_OK otherwise.
 */
esp_err_t esp_efuse_apply_34_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len);

/* @brief Disable ROM Download Mode via eFuse
 *
 * Permanently disables the ROM Download Mode feature. Once disabled, if the SoC is booted with
 * strapping pins set for ROM Download Mode then an error is printed instead.
 *
 * @note Not all SoCs support this option. An error will be returned if called on an ESP32
 * with a silicon revision lower than 3, as these revisions do not support this option.
 *
 * @note If ROM Download Mode is already disabled, this function does nothing and returns success.
 *
 * @return
 * - ESP_OK If the eFuse was successfully burned, or had already been burned.
 * - ESP_ERR_NOT_SUPPORTED (ESP32 only) This SoC is not capable of disabling UART download mode
 * - ESP_ERR_INVALID_STATE (ESP32 only) This eFuse is write protected and cannot be written
 */
esp_err_t esp_efuse_disable_rom_download_mode(void);

/* @brief Write random data to efuse key block write registers
 *
 * @note Caller is responsible for ensuring efuse
 * block is empty and not write protected, before calling.
 *
 * @note Behaviour depends on coding scheme: a 256-bit key is
 * generated and written for Coding Scheme "None", a 192-bit key
 * is generated, extended to 256-bits by the Coding Scheme,
 * and then writtten for 3/4 Coding Scheme.
 *
 * @note This function does not burn the new values, caller should
 * call esp_efuse_burn_new_values() when ready to do this.
 *
 * @param blk_wdata0_reg Address of the first data write register
 * in the block
 */
void esp_efuse_write_random_key(uint32_t blk_wdata0_reg);

/* @brief Return secure_version from efuse field.
 * @return Secure version from efuse field
 */
uint32_t esp_efuse_read_secure_version();

/* @brief Check secure_version from app and secure_version and from efuse field.
 *
 * @param secure_version Secure version from app.
 * @return
 *          - True: If version of app is equal or more then secure_version from efuse.
 */
bool esp_efuse_check_secure_version(uint32_t secure_version);

/* @brief Write efuse field by secure_version value.
 *
 * Update the secure_version value is available if the coding scheme is None.
 * Note: Do not use this function in your applications. This function is called as part of the other API.
 *
 * @param[in] secure_version Secure version from app.
 * @return
 *          - ESP_OK: Successful.
 *          - ESP_FAIL: secure version of app cannot be set to efuse field.
 *          - ESP_ERR_NOT_SUPPORTED: Anti rollback is not supported with the 3/4 and Repeat coding scheme.
 */
esp_err_t esp_efuse_update_secure_version(uint32_t secure_version);

/* @brief Initializes variables: offset and size to simulate the work of an eFuse.
 *
 * Note: To simulate the work of an eFuse need to set CONFIG_EFUSE_SECURE_VERSION_EMULATE option
 * and to add in the partition.csv file a line `efuse_em, data, efuse,   ,   0x2000,`.
 *
 * @param[in] offset The starting address of the partition where the eFuse data will be located.
 * @param[in] size The size of the partition.
 */
void esp_efuse_init(uint32_t offset, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // _ESP_EFUSE_MANAGER_H_
