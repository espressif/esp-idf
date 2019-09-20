// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _ESP_EFUSE_H
#define _ESP_EFUSE_H

#include "soc/efuse_reg.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief   Returns chip version from efuse
 *
 * @return chip version
 */
uint8_t esp_efuse_get_chip_ver(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_EFUSE_H */

