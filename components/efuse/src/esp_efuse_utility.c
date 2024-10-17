/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse_utility.h"

#include "soc/efuse_periph.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

static const char *TAG = "efuse";

// This counter is used to implement independent read access for efuses.
// During the read operation, the counter should be unchanged and even.
// If it is not so, we must repeat the read to make sure that the burn operation does not affect the read data.
static volatile unsigned s_burn_counter = 0;

// Array for emulate efuse registers.
#ifdef CONFIG_EFUSE_VIRTUAL
uint32_t virt_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK];
#endif // CONFIG_EFUSE_VIRTUAL

extern const esp_efuse_range_addr_t range_read_addr_blocks[];
extern const esp_efuse_range_addr_t range_write_addr_blocks[];

static uint32_t get_mask(unsigned int bit_count, unsigned int shift);
static void write_reg(esp_efuse_block_t blk, unsigned int num_reg, uint32_t value);
static uint32_t fill_reg(int bit_start_in_reg, int bit_count_in_reg, uint8_t* blob, int* filled_bits_blob);
static uint32_t set_cnt_in_reg(int bit_start_in_reg, int bit_count_used_in_reg, uint32_t reg_masked, size_t* cnt);
static bool check_range_of_bits(esp_efuse_block_t blk, int offset_in_bits, int size_bits);

// This function processes the field by calling the passed function.
esp_err_t esp_efuse_utility_process(const esp_efuse_desc_t* field[], void* ptr, size_t ptr_size_bits, efuse_func_proc_t func_proc)
{
    esp_err_t err = ESP_OK;
    int bits_counter = 0;

    // get and check size.
    int field_len = esp_efuse_get_field_size(field);
    int req_size = (ptr_size_bits == 0) ? field_len : MIN(ptr_size_bits, field_len);

    unsigned count_before = s_burn_counter;
    for (const esp_efuse_desc_t* f = field[0]; (err == ESP_OK && req_size > bits_counter && f != NULL); ++f) {
        if (check_range_of_bits(f->efuse_block, f->bit_start, f->bit_count) == false) {
            ESP_EARLY_LOGE(TAG, "Range of data does not match the coding scheme");
            err = ESP_ERR_CODING;
            break;
        }

        int end_bit_in_block = (f->bit_start + f->bit_count - 1);
        int first_reg = f->bit_start / 32;
        int last_reg = end_bit_in_block / 32;
        int start_bit_in_reg = f->bit_start % 32;

        for (int num_reg = first_reg; num_reg <= last_reg; ++num_reg) {
            int first_bit_in_reg = (num_reg == first_reg) ? start_bit_in_reg % 32 : 0;
            int last_bit_in_reg = (num_reg == last_reg) ? end_bit_in_block % 32 : 31;
            int use_bit_in_reg = (last_bit_in_reg - first_bit_in_reg) + 1;
            if ((bits_counter + use_bit_in_reg) > req_size) { // Limits the length of the field.
                use_bit_in_reg = req_size - bits_counter;
            }
            int end_bit_in_reg = start_bit_in_reg + use_bit_in_reg - 1;
            ESP_EARLY_LOGD(TAG, "BLK%d REG%d [%d-%d], len=%d bits", (int)f->efuse_block, num_reg, start_bit_in_reg, end_bit_in_reg, use_bit_in_reg);

            err = func_proc(num_reg, f->efuse_block, start_bit_in_reg, use_bit_in_reg, ptr, &bits_counter);
            start_bit_in_reg = 0;

            if (err != ESP_OK || req_size <= bits_counter) {
                break;
            }
        }
    }
    unsigned count_after = s_burn_counter;
    if (err == ESP_OK &&
        (func_proc == esp_efuse_utility_fill_buff || func_proc == esp_efuse_utility_count_once) && // these functions are used for read APIs: read_field_blob and read_field_cnt
        (count_before != count_after || (count_after & 1) == 1)) {
        err = ESP_ERR_DAMAGED_READING;
    }
    assert(bits_counter <= req_size);
    return err;
}


// Read efuse register and write this value to array.
esp_err_t esp_efuse_utility_fill_buff(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* arr_out, int* bits_counter)
{
    uint8_t* blob = (uint8_t *) arr_out;
    uint32_t reg = esp_efuse_utility_read_reg(efuse_block, num_reg);
    uint64_t reg_of_aligned_bits = (reg >> bit_start) & get_mask(bit_count, 0);

    int shift_bit = (*bits_counter) % 8;
    if (shift_bit != 0) {
        blob[(*bits_counter) / 8] |= (uint8_t)(reg_of_aligned_bits << shift_bit);
        shift_bit = ((8 - shift_bit) < bit_count) ? (8 - shift_bit) : bit_count;
        (*bits_counter) += shift_bit;
        bit_count -= shift_bit;
    }

    int sum_shift = 0;
    while (bit_count > 0) {
        sum_shift += shift_bit;
        blob[(*bits_counter) / 8] |= (uint8_t)(reg_of_aligned_bits >> sum_shift);
        shift_bit = (bit_count > 8) ? 8 : bit_count;
        (*bits_counter) += shift_bit;
        bit_count -= shift_bit;
    };
    return ESP_OK;
}

// Count a set bits.
esp_err_t esp_efuse_utility_count_once(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* out_cnt, int* bits_counter)
{
    uint32_t reg = esp_efuse_utility_read_reg(efuse_block, num_reg);
    *((size_t *)out_cnt) += __builtin_popcount(reg & get_mask(bit_count, bit_start)); // Returns the number of 1-bits in reg.
    *bits_counter += bit_count;
    return ESP_OK;
}

// Fill registers from array for writing.
esp_err_t esp_efuse_utility_write_blob(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* arr_in, int* bits_counter)
{
    uint32_t reg_to_write = fill_reg(bit_start, bit_count, (uint8_t *)arr_in, bits_counter);
    return esp_efuse_utility_write_reg(efuse_block, num_reg, reg_to_write);
}

// fill registers with the required number of bits for writing.
esp_err_t esp_efuse_utility_write_cnt(unsigned int num_reg, esp_efuse_block_t efuse_block, int bit_start, int bit_count, void* cnt, int* bits_counter)
{
    esp_err_t err = ESP_OK;
    uint32_t reg = esp_efuse_utility_read_reg(efuse_block, num_reg);
    size_t* set_bits = (size_t*)cnt;
    uint32_t mask = get_mask(bit_count, bit_start);
    uint32_t reg_masked_bits = reg & mask;
    if ((reg_masked_bits ^ mask) != 0) {// register has free bits to set them to 1?
        uint32_t reg_to_write = set_cnt_in_reg(bit_start, bit_count, reg_masked_bits, set_bits);
        write_reg(efuse_block, num_reg, reg_to_write);
    }
    *bits_counter += bit_count;
    if ((*set_bits) == 0) {
        err = ESP_OK_EFUSE_CNT;
    }
    return err;
}

// Reset efuse write registers
void esp_efuse_utility_reset(void)
{
    ++s_burn_counter;
    esp_efuse_utility_clear_program_registers();
    ++s_burn_counter;
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        for (uintptr_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            REG_WRITE(addr_wr_block, 0);
        }
    }
}

void esp_efuse_utility_debug_dump_pending(void)
{
    for (int num_block = 0; num_block < EFUSE_BLK_MAX; num_block++) {
        for (uintptr_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            if (REG_READ(addr_wr_block) != 0) {
                esp_efuse_utility_debug_dump_single_block(num_block, false);
                break;
            }
        }
    }
}

esp_err_t esp_efuse_utility_burn_efuses(void)
{
    ++s_burn_counter;
#ifdef CONFIG_EFUSE_VIRTUAL_LOG_ALL_WRITES
    ESP_EARLY_LOGW(TAG, "Burn:");
    esp_efuse_utility_debug_dump_pending();
#endif
    esp_err_t err = esp_efuse_utility_burn_chip();
    ++s_burn_counter;
    return err;
}

// Erase the virt_blocks array.
void esp_efuse_utility_erase_virt_blocks(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    memset(virt_blocks, 0, sizeof(virt_blocks));
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_utility_write_efuses_to_flash();
#endif
#endif // CONFIG_EFUSE_VIRTUAL
}

// Fills the virt_blocks array by values from efuse_Rdata.
void esp_efuse_utility_update_virt_blocks(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    if (!esp_efuse_utility_load_efuses_from_flash()) {
#else
    if (1) {
#endif
        ESP_EARLY_LOGW(TAG, "[Virtual] Loading virtual efuse blocks from real efuses");
        for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
            int subblock = 0;
            for (uintptr_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4) {
                virt_blocks[num_block][subblock++] = REG_READ(addr_rd_block);
            }
            ESP_EARLY_LOGD(TAG, "[Virtual] virt_blocks[%d] is filled by EFUSE_BLOCK%d", num_block, num_block);
        }
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
        esp_efuse_utility_write_efuses_to_flash();
#endif
    }
#else
    ESP_EARLY_LOGI(TAG, "Emulate efuse is disabled");
#endif
}

// Prints efuse values for a single block
void esp_efuse_utility_debug_dump_single_block(int num_block, bool from_read)
{
    esp_rom_printf("%d) ", num_block);
    int num_reg = 0;
    if (from_read) {
        for (uintptr_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, num_reg++) {
#ifdef CONFIG_EFUSE_VIRTUAL
            esp_rom_printf("%08" PRIx32 " ", virt_blocks[num_block][num_reg]);
#else
            esp_rom_printf("%08" PRIx32 " ", REG_READ(addr_rd_block));
            (void) num_reg;
#endif
        }
    } else {
        for (uintptr_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4, num_reg++) {
            esp_rom_printf("%08" PRIx32 " ", REG_READ(addr_wr_block));
        }
    }
    esp_rom_printf("\n");
}

// Prints efuse values for all registers.
void esp_efuse_utility_debug_dump_blocks(void)
{
    esp_rom_printf("EFUSE_BLKx:\n");
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        esp_efuse_utility_debug_dump_single_block(num_block, true);
    }
    esp_rom_printf("\n");
}

// returns the number of array elements for placing these bits in an array with the length of each element equal to size_of_base.
int esp_efuse_utility_get_number_of_items(int bits, int size_of_base)
{
    return  bits / size_of_base + (bits % size_of_base > 0 ? 1 : 0);
}

// Writing efuse register with checking of repeated programming of programmed bits.
esp_err_t esp_efuse_utility_write_reg(esp_efuse_block_t efuse_block, unsigned int num_reg, uint32_t reg_to_write)
{
    esp_err_t err = ESP_OK;
    uint32_t reg = esp_efuse_utility_read_reg(efuse_block, num_reg);
    if (reg & reg_to_write) {
        ESP_EARLY_LOGE(TAG, "Repeated programming of programmed bits is strictly forbidden 0x%08" PRIx32, reg & reg_to_write);
        err = ESP_ERR_EFUSE_REPEATED_PROG;
    } else {
        write_reg(efuse_block, num_reg, reg_to_write);
    }
    return err;
}

// Reading efuse register.
uint32_t esp_efuse_utility_read_reg(esp_efuse_block_t blk, unsigned int num_reg)
{
    assert(blk >= 0 && blk < EFUSE_BLK_MAX);
    assert(num_reg <= (range_read_addr_blocks[blk].end - range_read_addr_blocks[blk].start) / sizeof(uint32_t));
    uint32_t value;
#ifdef CONFIG_EFUSE_VIRTUAL
    value = virt_blocks[blk][num_reg];
#else
    value = REG_READ(range_read_addr_blocks[blk].start + num_reg * 4);
#endif
    return value;
}

// Private functions

// writing efuse register.
static void write_reg(esp_efuse_block_t blk, unsigned int num_reg, uint32_t value)
{
    assert(blk >= 0 && blk < EFUSE_BLK_MAX);
    assert(num_reg <= (range_read_addr_blocks[blk].end - range_read_addr_blocks[blk].start) / sizeof(uint32_t));

    uintptr_t addr_wr_reg = range_write_addr_blocks[blk].start + num_reg * 4;
    uint32_t reg_to_write = REG_READ(addr_wr_reg) | value;
    // The register can be written in parts so we combine the new value with the one already available.
    REG_WRITE(addr_wr_reg, reg_to_write);
}

// return mask with required the number of ones with shift.
static uint32_t get_mask(unsigned int bit_count, unsigned int shift)
{
    uint32_t mask;
    if (bit_count != 32) {
        mask = (1 << bit_count) - 1;
    } else {
        mask = 0xFFFFFFFF;
    }
    return mask << shift;
}


// fill efuse register from array.
static uint32_t fill_reg(int bit_start_in_reg, int bit_count_in_reg, uint8_t* blob, int* filled_bits_blob)
{
    uint32_t reg_to_write = 0;
    uint32_t temp_blob_32;
    int shift_bit = (*filled_bits_blob) % 8;
    if (shift_bit != 0) {
        temp_blob_32 = blob[(*filled_bits_blob) / 8] >> shift_bit;
        shift_bit = ((8 - shift_bit) < bit_count_in_reg) ? (8 - shift_bit) : bit_count_in_reg;
        reg_to_write = temp_blob_32 & get_mask(shift_bit, 0);
        (*filled_bits_blob) += shift_bit;
        bit_count_in_reg -= shift_bit;
    }

    int shift_reg = shift_bit;
    while (bit_count_in_reg > 0) {
        temp_blob_32 = blob[(*filled_bits_blob) / 8];
        shift_bit = (bit_count_in_reg > 8) ? 8 : bit_count_in_reg;
        reg_to_write |= (temp_blob_32 & get_mask(shift_bit, 0)) << shift_reg;
        (*filled_bits_blob) += shift_bit;
        bit_count_in_reg -= shift_bit;
        shift_reg += 8;
    };
    return reg_to_write << bit_start_in_reg;
}

// sets a required count of bits as "1".
static uint32_t set_cnt_in_reg(int bit_start_in_reg, int bit_count_used_in_reg, uint32_t reg_masked, size_t* cnt)
{
    assert((bit_start_in_reg + bit_count_used_in_reg) <= 32);
    uint32_t reg_to_write = 0;
    for (int i = bit_start_in_reg; i < bit_start_in_reg + bit_count_used_in_reg; ++i) {
        if ((reg_masked & (1 << i)) == 0) {
            reg_to_write |= (1 << i);
            if (--(*cnt) == 0) {
                break;
            }
        }
    }
    return reg_to_write;
}

// check range of bits for any coding scheme.
static bool check_range_of_bits(esp_efuse_block_t blk, int offset_in_bits, int size_bits)
{
    int max_num_bit = offset_in_bits + size_bits;
    if (max_num_bit > 256) {
        return false;
    } else {
        ESP_EFUSE_FIELD_CORRESPONDS_CODING_SCHEME(blk, max_num_bit);
    }
    return true;
}

uintptr_t esp_efuse_utility_get_read_register_address(esp_efuse_block_t block)
{
    assert(block < EFUSE_BLK_MAX);
#ifdef CONFIG_EFUSE_VIRTUAL
    return (uintptr_t)&virt_blocks[block][0];
#else
    return range_read_addr_blocks[block].start;
#endif

}

#if defined(BOOTLOADER_BUILD) && defined(CONFIG_EFUSE_VIRTUAL) && !defined(CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH)
void esp_efuse_init_virtual_mode_in_ram(void)
{
    esp_efuse_utility_update_virt_blocks();
}
#endif

#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH

#include "../bootloader_flash/include/bootloader_flash_priv.h"

static uint32_t esp_efuse_flash_offset = 0;
static uint32_t esp_efuse_flash_size = 0;

void esp_efuse_init_virtual_mode_in_flash(uint32_t offset, uint32_t size)
{
    esp_efuse_flash_offset = offset;
    esp_efuse_flash_size = size;
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
}

void esp_efuse_utility_erase_efuses_in_flash(void)
{
    ESP_EARLY_LOGW(TAG, "[Virtual] Erasing eFuses in flash");

    if (esp_efuse_flash_offset == 0) {
        ESP_EARLY_LOGE(TAG, "[Virtual] no efuse partition in partition_table? (Flash is not updated)");
        abort();
    }
    esp_err_t err = bootloader_flash_erase_range(esp_efuse_flash_offset, esp_efuse_flash_size);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "[Virtual] Failed to erase flash. err = 0x%x", err);
        abort();
    }

    ESP_EARLY_LOGW(TAG, "[Virtual] Erase complete");
}

bool esp_efuse_utility_load_efuses_from_flash(void)
{
    ESP_EARLY_LOGW(TAG, "[Virtual] try loading efuses from flash: 0x%" PRIx32 " (offset)", esp_efuse_flash_offset);

    if (esp_efuse_flash_offset == 0) {
        ESP_EARLY_LOGE(TAG, "[Virtual] no efuse partition in partition_table? (Flash is not updated)");
        abort();
    }
    uint32_t efuses_in_flash[sizeof(virt_blocks)];

    esp_err_t err = bootloader_flash_read(esp_efuse_flash_offset, &efuses_in_flash, sizeof(efuses_in_flash), false);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "[Virtual] Can not read eFuse partition from flash (err=0x%x)", err);
        abort();
    }

    for (unsigned i = 0; i < sizeof(virt_blocks); ++i) {
        if (efuses_in_flash[i] != 0xFFFFFFFF) {
            ESP_EARLY_LOGW(TAG, "[Virtual] Loading virtual efuse blocks from flash");
            memcpy(virt_blocks, efuses_in_flash, sizeof(virt_blocks));
            return true;
        }
    }

    ESP_EARLY_LOGW(TAG, "[Virtual] no efuses found in flash");

    return false;
}

void esp_efuse_utility_write_efuses_to_flash(void)
{
    if (esp_efuse_flash_offset == 0) {
        ESP_EARLY_LOGE(TAG, "[Virtual] no efuse partition in partition_table? (Flash is not updated)");
        abort();
    }

    esp_err_t err = bootloader_flash_erase_range(esp_efuse_flash_offset, esp_efuse_flash_size);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "[Virtual] Failed to erase flash. err = 0x%x", err);
        abort();
    }

    err = bootloader_flash_write(esp_efuse_flash_offset, &virt_blocks, sizeof(virt_blocks), false);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "[Virtual] Failed to write eFuses to flash. err = 0x%x", err);
        abort();
    }
}
#endif // CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH

bool esp_efuse_utility_is_correct_written_data(esp_efuse_block_t block, unsigned r_data_len)
{
    uintptr_t* w_data = (uintptr_t*)range_write_addr_blocks[block].start;
    uintptr_t* r_data = (uintptr_t*)range_read_addr_blocks[block].start;

    bool correct_written_data = memcmp(w_data, r_data, r_data_len) == 0;
    if (correct_written_data) {
        ESP_LOGI(TAG, "BURN BLOCK%d - OK (write block == read block)", block);
        return true;
    }

    correct_written_data = true;
    for (unsigned i = 0; i < r_data_len / 4; i++) {
        if ((*(r_data + i) & *(w_data + i)) != *(w_data + i)) {
            correct_written_data = false;
            break;
        }
    }
    if (correct_written_data) {
        ESP_LOGI(TAG, "BURN BLOCK%d - OK (all write block bits are set)", block);
    } else {
        ESP_LOGE(TAG, "BURN BLOCK%d - ERROR (written bits != read bits)", block);
    }
    return correct_written_data;
}
