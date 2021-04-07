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

#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include_next "esp_efuse.h"

#define ESP_ERR_EFUSE                              0x1600                     /*!< Base error code for efuse api. */
#define ESP_OK_EFUSE_CNT                          (ESP_ERR_EFUSE + 0x01)      /*!< OK the required number of bits is set. */
#define ESP_ERR_EFUSE_CNT_IS_FULL                 (ESP_ERR_EFUSE + 0x02)      /*!< Error field is full. */
#define ESP_ERR_EFUSE_REPEATED_PROG               (ESP_ERR_EFUSE + 0x03)      /*!< Error repeated programming of programmed bits is strictly forbidden. */
#define ESP_ERR_CODING                            (ESP_ERR_EFUSE + 0x04)      /*!< Error while a encoding operation. */
#define ESP_ERR_NOT_ENOUGH_UNUSED_KEY_BLOCKS      (ESP_ERR_EFUSE + 0x05)      /*!< Error not enough unused key blocks available */

/**
 * @brief Type definition for an eFuse field
 */
typedef struct {
    esp_efuse_block_t   efuse_block: 8; /**< Block of eFuse */
    uint8_t             bit_start;      /**< Start bit [0..255] */
    uint16_t            bit_count;      /**< Length of bit field [1..-]*/
} esp_efuse_desc_t;

/**
 * @brief Type definition for ROM log scheme
 */
typedef enum {
    ESP_EFUSE_ROM_LOG_ALWAYS_ON,    /**< Always enable ROM logging */
    ESP_EFUSE_ROM_LOG_ON_GPIO_LOW,  /**< ROM logging is enabled when specific GPIO level is low during start up */
    ESP_EFUSE_ROM_LOG_ON_GPIO_HIGH, /**< ROM logging is enabled when specific GPIO level is high during start up */
    ESP_EFUSE_ROM_LOG_ALWAYS_OFF    /**< Disable ROM logging permanently */
} esp_efuse_rom_log_scheme_t;

/**
 * @brief   Reads bits from EFUSE field and writes it into an array.
 *
 * The number of read bits will be limited to the minimum value
 * from the description of the bits in "field" structure or "dst_size_bits" required size.
 * Use "esp_efuse_get_field_size()" function to determine the length of the field.
 *
 * @note Please note that reading in the batch mode does not show uncommitted changes.
 *
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
 * @brief Read a single bit eFuse field as a boolean value.
 *
 * @note The value must exist and must be a single bit wide. If there is any possibility of an error
 * in the provided arguments, call esp_efuse_read_field_blob() and check the returned value instead.
 *
 * @note If assertions are enabled and the parameter is invalid, execution will abort
 * @note Please note that reading in the batch mode does not show uncommitted changes.
 *
 * @param[in]  field          A pointer to the structure describing the fields of efuse.
 * @return
 *    - true: The field parameter is valid and the bit is set.
 *    - false: The bit is not set, or the parameter is invalid and assertions are disabled.
 *
 */
bool esp_efuse_read_field_bit(const esp_efuse_desc_t *field[]);

/**
 * @brief   Reads bits from EFUSE field and returns number of bits programmed as "1".
 *
 * If the bits are set not sequentially, they will still be counted.
 * @note Please note that reading in the batch mode does not show uncommitted changes.
 *
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
 * @brief Write a single bit eFuse field to 1
 *
 * For use with eFuse fields that are a single bit. This function will write the bit to value 1 if
 * it is not already set, or does nothing if the bit is already set.
 *
 * This is equivalent to calling esp_efuse_write_field_cnt() with the cnt parameter equal to 1,
 * except that it will return ESP_OK if the field is already set to 1.
 *
 * @param[in] field Pointer to the structure describing the efuse field.
 *
 * @return
 * - ESP_OK: The operation was successfully completed, or the bit was already set to value 1.
 * - ESP_ERR_INVALID_ARG: Error in the passed arugments, including if the efuse field is not 1 bit wide.
 */
esp_err_t esp_efuse_write_field_bit(const esp_efuse_desc_t* field[]);

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
 * @note Please note that reading in the batch mode does not show uncommitted changes.
 *
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
 * @note Please note that reading in the batch mode does not show uncommitted changes.
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

/**
 * @brief Permanently update values written to the efuse write registers
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

/**
 *  @brief Reset efuse write registers
 *
 * Efuse write registers are written to zero, to negate
 * any changes that have been staged here.
 *
 * @note This function is not threadsafe, if calling code updates
 * efuse values from multiple tasks then this is caller's
 * responsibility to serialise.
 */
void esp_efuse_reset(void);

#ifdef CONFIG_IDF_TARGET_ESP32
/**
 *  @brief Disable BASIC ROM Console via efuse
 *
 * By default, if booting from flash fails the ESP32 will boot a
 * BASIC console in ROM.
 *
 * Call this function (from bootloader or app) to permanently disable the console on this chip.
 *
 */
void esp_efuse_disable_basic_rom_console(void);
#endif


/**
 *  @brief Disable ROM Download Mode via eFuse
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

/**
 * @brief Set boot ROM log scheme via eFuse
 *
 * @note By default, the boot ROM will always print to console. This API can be called to set the log scheme only once per chip,
 *       once the value is changed from the default it can't be changed again.
 *
 * @param log_scheme Supported ROM log scheme
 * @return
 *      - ESP_OK If the eFuse was successfully burned, or had already been burned.
 *      - ESP_ERR_NOT_SUPPORTED (ESP32 only) This SoC is not capable of setting ROM log scheme
 *      - ESP_ERR_INVALID_STATE This eFuse is write protected or has been burned already
 */
esp_err_t esp_efuse_set_rom_log_scheme(esp_efuse_rom_log_scheme_t log_scheme);

#if SOC_SUPPORTS_SECURE_DL_MODE
/**
 *  @brief Switch ROM Download Mode to Secure Download mode via eFuse
 *
 * Permanently enables Secure Download mode. This mode limits the use of ROM Download Mode functions
 * to simple flash read, write and erase operations, plus a command to return a summary of currently
 * enabled security features.
 *
 * @note If Secure Download mode is already enabled, this function does nothing and returns success.
 *
 * @note Disabling the ROM Download Mode also disables Secure Download Mode.
 *
 * @return
 * - ESP_OK If the eFuse was successfully burned, or had already been burned.
 * - ESP_ERR_INVALID_STATE ROM Download Mode has been disabled via eFuse, so Secure Download mode is unavailable.
 */
esp_err_t esp_efuse_enable_rom_secure_download_mode(void);
#endif

/**
 *  @brief Write random data to efuse key block write registers
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

/**
 *  @brief Return secure_version from efuse field.
 * @return Secure version from efuse field
 */
uint32_t esp_efuse_read_secure_version(void);

/**
 *  @brief Check secure_version from app and secure_version and from efuse field.
 *
 * @param secure_version Secure version from app.
 * @return
 *          - True: If version of app is equal or more then secure_version from efuse.
 */
bool esp_efuse_check_secure_version(uint32_t secure_version);

/**
 *  @brief Write efuse field by secure_version value.
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

/**
 *  @brief Initializes variables: offset and size to simulate the work of an eFuse.
 *
 * Note: To simulate the work of an eFuse need to set CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE option
 * and to add in the partition.csv file a line `efuse_em, data, efuse,   ,   0x2000,`.
 *
 * @param[in] offset The starting address of the partition where the eFuse data will be located.
 * @param[in] size The size of the partition.
 */
void esp_efuse_init(uint32_t offset, uint32_t size);

/**
 *  @brief Set the batch mode of writing fields.
 *
 * This mode allows you to write the fields in the batch mode when need to burn several efuses at one time.
 * To enable batch mode call begin() then perform as usually the necessary operations
 * read and write and at the end call commit() to actually burn all written efuses.
 * The batch mode can be used nested. The commit will be done by the last commit() function.
 * The number of begin() functions should be equal to the number of commit() functions.
 *
 * @note Please note that reading in the batch mode does not show uncommitted changes.
 *
 * Note: If batch mode is enabled by the first task, at this time the second task cannot write/read efuses.
 * The second task will wait for the first task to complete the batch operation.
 *
 * \code{c}
 * // Example of using the batch writing mode.
 *
 * // set the batch writing mode
 * esp_efuse_batch_write_begin();
 *
 * // use any writing functions as usual
 * esp_efuse_write_field_blob(ESP_EFUSE_...);
 * esp_efuse_write_field_cnt(ESP_EFUSE_...);
 * esp_efuse_set_write_protect(EFUSE_BLKx);
 * esp_efuse_write_reg(EFUSE_BLKx, ...);
 * esp_efuse_write_block(EFUSE_BLKx, ...);
 * esp_efuse_write(ESP_EFUSE_1, 3);  // ESP_EFUSE_1 == 1, here we write a new value = 3. The changes will be burn by the commit() function.
 * esp_efuse_read_...(ESP_EFUSE_1);  // this function returns ESP_EFUSE_1 == 1 because uncommitted changes are not readable, it will be available only after commit.
 * ...
 *
 * // esp_efuse_batch_write APIs can be called recursively.
 * esp_efuse_batch_write_begin();
 * esp_efuse_set_write_protect(EFUSE_BLKx);
 * esp_efuse_batch_write_commit(); // the burn will be skipped here, it will be done in the last commit().
 *
 * ...
 *
 * // Write all of these fields to the efuse registers
 * esp_efuse_batch_write_commit();
 * esp_efuse_read_...(ESP_EFUSE_1);  // this function returns ESP_EFUSE_1 == 3.
 *
 * \endcode
 *
 * @return
 *          - ESP_OK: Successful.
 */
esp_err_t esp_efuse_batch_write_begin(void);

/**
 *  @brief Reset the batch mode of writing fields.
 *
 * It will reset the batch writing mode and any written changes.
 *
 * @return
 *          - ESP_OK: Successful.
 *          - ESP_ERR_INVALID_STATE: Tha batch mode was not set.
 */
esp_err_t esp_efuse_batch_write_cancel(void);

/**
 *  @brief Writes all prepared data for the batch mode.
 *
 * Must be called to ensure changes are written to the efuse registers.
 * After this the batch writing mode will be reset.
 *
 * @return
 *          - ESP_OK: Successful.
 *          - ESP_ERR_INVALID_STATE: The deferred writing mode was not set.
 */
esp_err_t esp_efuse_batch_write_commit(void);

/**
 *  @brief Checks that the given block is empty.
 *
 * @return
 *          - True: The block is empty.
 *          - False: The block is not empty or was an error.
 */
bool esp_efuse_block_is_empty(esp_efuse_block_t block);

/**
 * @brief Returns a read protection for the key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return True: The key block is read protected
 *         False: The key block is readable.
 */
bool esp_efuse_get_key_dis_read(esp_efuse_block_t block);

/**
 * @brief Sets a read protection for the key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_key_dis_read(esp_efuse_block_t block);

/**
 * @brief Returns a write protection for the key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return True: The key block is write protected
 *         False: The key block is writeable.
 */
bool esp_efuse_get_key_dis_write(esp_efuse_block_t block);

/**
 * @brief Sets a write protection for the key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_key_dis_write(esp_efuse_block_t block);

/**
 * @brief Returns true if the key block is unused, false otherwise.
 *
 * An unused key block is all zero content, not read or write protected,
 * and has purpose 0 (ESP_EFUSE_KEY_PURPOSE_USER)
 *
 * @param block key block to check.
 *
 * @return
 *         - True if key block is unused,
 *         - False if key block is used or the specified block index is not a key block.
 */
bool esp_efuse_key_block_unused(esp_efuse_block_t block);

#ifndef CONFIG_IDF_TARGET_ESP32

/**
 * @brief Type of key purpose
 */
typedef enum {
    ESP_EFUSE_KEY_PURPOSE_USER = 0,
    ESP_EFUSE_KEY_PURPOSE_RESERVED = 1,
    ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 = 2,
    ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 = 3,
    ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY = 4,
    ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL = 5,
    ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG = 6,
    ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE = 7,
    ESP_EFUSE_KEY_PURPOSE_HMAC_UP = 8,
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0 = 9,
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1 = 10,
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2 = 11,
    ESP_EFUSE_KEY_PURPOSE_MAX,
} esp_efuse_purpose_t;


/**
 * @brief Returns a pointer to a key purpose for an efuse key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * To get the value of this field use esp_efuse_read_field_blob() or esp_efuse_get_key_purpose().
 *
 * @return Pointer: If Successful returns a pointer to the corresponding efuse field otherwise NULL.
 */
const esp_efuse_desc_t **esp_efuse_get_purpose_field(esp_efuse_block_t block);

/**
 * @brief Returns a pointer to a key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return Pointer: If Successful returns a pointer to the corresponding efuse field otherwise NULL.
 */
const esp_efuse_desc_t** esp_efuse_get_key(esp_efuse_block_t block);

/**
 * @brief Returns the current purpose set for an efuse key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return
 *         - Value: If Successful, it returns the value of the purpose related to the given key block.
 *         - ESP_EFUSE_KEY_PURPOSE_MAX: Otherwise.
 */
esp_efuse_purpose_t esp_efuse_get_key_purpose(esp_efuse_block_t block);

/**
 * @brief Sets a key purpose for an efuse key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 * @param[in] purpose Key purpose.
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_key_purpose(esp_efuse_block_t block, esp_efuse_purpose_t purpose);

/**
 * @brief Returns a write protection of the key purpose field for an efuse key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return True: The key purpose is write protected.
 *         False: The key purpose is writeable.
 */
bool esp_efuse_get_keypurpose_dis_write(esp_efuse_block_t block);

/**
 * @brief Sets a write protection of the key purpose field for an efuse key block.
 *
 * @param[in] block A key block in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_keypurpose_dis_write(esp_efuse_block_t block);

/**
 * @brief Find a key block with the particular purpose set.
 *
 * @param[in] purpose Purpose to search for.
 * @param[out] block Pointer in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX which will be set to the key block if found.
 *                   Can be NULL, if only need to test the key block exists.
 *
 * @return
 *         - True: If found,
 *         - False: If not found (value at block pointer is unchanged).
 */
bool esp_efuse_find_purpose(esp_efuse_purpose_t purpose, esp_efuse_block_t *block);

/**
 * @brief Search for an unused key block and return the first one found.
 *
 * See esp_efuse_key_block_unused for a description of an unused key block.
 *
 * @return First unused key block, or EFUSE_BLK_KEY_MAX if no unused key block is found.
 */
esp_efuse_block_t esp_efuse_find_unused_key_block(void);

/**
 * @brief Return the number of unused efuse key blocks in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
 */
unsigned esp_efuse_count_unused_key_blocks(void);

/**
 * @brief Returns the status of the Secure Boot public key digest revocation bit.
 *
 * @param[in] num_digest The number of digest in range 0..2
 *
 * @return
 *         - True: If key digest is revoked,
 *         - False; If key digest is not revoked.
 */
bool esp_efuse_get_digest_revoke(unsigned num_digest);

/**
 * @brief Sets the Secure Boot public key digest revocation bit.
 *
 * @param[in] num_digest The number of digest in range 0..2
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_digest_revoke(unsigned num_digest);

/**
 * @brief Returns a write protection of the Secure Boot public key digest revocation bit.
 *
 * @param[in] num_digest The number of digest in range 0..2
 *
 * @return True: The revocation bit is write protected.
 *         False: The revocation bit is writeable.
 */
bool esp_efuse_get_write_protect_of_digest_revoke(unsigned num_digest);

/**
 * @brief Sets a write protection of the Secure Boot public key digest revocation bit.
 *
 * @param[in] num_digest The number of digest in range 0..2
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_set_write_protect_of_digest_revoke(unsigned num_digest);

/**
 * @brief Program a block of key data to an efuse block
 *
 * The burn of a key, protection bits, and a purpose happens in batch mode.
 *
 * @param[in] block Block to read purpose for. Must be in range EFUSE_BLK_KEY0 to EFUSE_BLK_KEY_MAX. Key block must be unused (esp_efuse_key_block_unused).
 * @param[in] purpose Purpose to set for this key. Purpose must be already unset.
 * @param[in] key Pointer to data to write.
 * @param[in] key_size_bytes Bytes length of data to write.
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_INVALID_STATE: Error in efuses state, unused block not found.
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_write_key(esp_efuse_block_t block, esp_efuse_purpose_t purpose, const void *key, size_t key_size_bytes);

/**
 * @brief Program keys to unused efuse blocks
 *
 * The burn of keys, protection bits, and purposes happens in batch mode.
 *
 * @param[in] purposes Array of purposes (purpose[number_of_keys]).
 * @param[in] keys Array of keys (uint8_t keys[number_of_keys][32]). Each key is 32 bytes long.
 * @param[in] number_of_keys The number of keys to write (up to 6 keys).
 *
 * @return
 *    - ESP_OK: Successful.
 *    - ESP_ERR_INVALID_ARG: Error in the passed arguments.
 *    - ESP_ERR_INVALID_STATE: Error in efuses state, unused block not found.
 *    - ESP_ERR_NOT_ENOUGH_UNUSED_KEY_BLOCKS: Error not enough unused key blocks available
 *    - ESP_ERR_EFUSE_REPEATED_PROG: Error repeated programming of programmed bits is strictly forbidden.
 *    - ESP_ERR_CODING: Error range of data does not match the coding scheme.
 */
esp_err_t esp_efuse_write_keys(esp_efuse_purpose_t purposes[], uint8_t keys[][32], unsigned number_of_keys);

#endif // not CONFIG_IDF_TARGET_ESP32

#ifdef __cplusplus
}
#endif
