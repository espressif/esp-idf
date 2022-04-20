// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_efuse_utility.h"

#include "soc/efuse_periph.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

static const char *TAG = "efuse";

// Array for emulate efuse registers.
#ifdef CONFIG_EFUSE_VIRTUAL
uint32_t virt_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK];

/* Call the update function to seed virtual efuses during initialization */
__attribute__((constructor)) void esp_efuse_utility_update_virt_blocks(void);
#endif

extern const esp_efuse_range_addr_t range_read_addr_blocks[];
extern const esp_efuse_range_addr_t range_write_addr_blocks[];

static int get_reg_num(int bit_start, int bit_count, int i_reg);
static int get_starting_bit_num_in_reg(int bit_start, int i_reg);
static uint32_t get_mask(unsigned int bit_count, unsigned int shift);
static int get_count_bits_in_reg(int bit_start, int bit_count, int i_reg);
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

    int i = 0;
    while (err == ESP_OK && req_size > bits_counter && field[i] != NULL) {
        if (check_range_of_bits(field[i]->efuse_block, field[i]->bit_start, field[i]->bit_count) == false) {
            ESP_LOGE(TAG, "Range of data does not match the coding scheme");
            err = ESP_ERR_CODING;
        }
        int i_reg = 0;
        int num_reg;
        while (err == ESP_OK && req_size > bits_counter &&
                (num_reg = get_reg_num(field[i]->bit_start, field[i]->bit_count, i_reg)) != -1) {

            int start_bit = get_starting_bit_num_in_reg(field[i]->bit_start, i_reg);
            int num_bits = get_count_bits_in_reg(field[i]->bit_start, field[i]->bit_count, i_reg);
            if ((bits_counter + num_bits) > req_size) { // Limits the length of the field.
                num_bits = req_size - bits_counter;
            }
            ESP_LOGD(TAG, "In EFUSE_BLK%d__DATA%d_REG is used %d bits starting with %d bit",
                    (int)field[i]->efuse_block, num_reg, num_bits, start_bit);
            err = func_proc(num_reg, field[i]->efuse_block, start_bit, num_bits, ptr, &bits_counter);
            ++i_reg;
        }
        i++;
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
    esp_efuse_utility_clear_program_registers();
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            REG_WRITE(addr_wr_block, 0);
        }
    }
}

// Erase the virt_blocks array.
void esp_efuse_utility_erase_virt_blocks(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    memset(virt_blocks, 0, sizeof(virt_blocks));
#endif
}

// Fills the virt_blocks array by values from efuse_Rdata.
void esp_efuse_utility_update_virt_blocks(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGI(TAG, "Loading virtual efuse blocks from real efuses");
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        int subblock = 0;
        for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4) {
            virt_blocks[num_block][subblock++] = REG_READ(addr_rd_block);
        }
        ESP_LOGD(TAG, "virt_blocks[%d] is filled by EFUSE_BLOCK%d", num_block, num_block);
    }
#else
    ESP_LOGI(TAG, "Emulate efuse is disabled");
#endif
}

// Prints efuse values for all registers.
#ifndef BOOTLOADER_BUILD
void esp_efuse_utility_debug_dump_blocks(void)
{
    printf("EFUSE_BLKx:\n");
#ifdef CONFIG_EFUSE_VIRTUAL
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        int num_reg = 0;
        printf("%d) ", num_block);
        for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, num_reg++) {
            printf("0x%08x ", virt_blocks[num_block][num_reg]);
        }
        printf("\n");
    }
#else
    for (int num_block = EFUSE_BLK0; num_block < EFUSE_BLK_MAX; num_block++) {
        printf("%d) ", num_block);
        for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4) {
            printf("0x%08x ", REG_READ(addr_rd_block));
        }
        printf("\n");
    }
#endif
    printf("\n");
}
#endif // BOOTLOADER_BUILD

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
        ESP_LOGE(TAG, "Repeated programming of programmed bits is strictly forbidden 0x%08x", reg & reg_to_write);
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
    unsigned int max_num_reg = (range_read_addr_blocks[blk].end - range_read_addr_blocks[blk].start) / sizeof(uint32_t);
    assert(num_reg <= max_num_reg);
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
    unsigned int max_num_reg = (range_read_addr_blocks[blk].end - range_read_addr_blocks[blk].start) / sizeof(uint32_t);
    assert(num_reg <= max_num_reg);
    uint32_t addr_wr_reg = range_write_addr_blocks[blk].start + num_reg * 4;
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

// return the register number in the array. return -1 if all registers for field was selected.
static int get_reg_num(int bit_start, int bit_count, int i_reg)
{
    int num_reg = i_reg + bit_start / 32;

    if (num_reg > (bit_start + bit_count - 1) / 32) {
        return -1;
    }

    return num_reg;
}

// returns the starting bit number in the register.
static int get_starting_bit_num_in_reg(int bit_start, int i_reg)
{
    return (i_reg == 0) ? bit_start % 32 : 0;
}

// Returns the number of bits in the register.
static int get_count_bits_in_reg(int bit_start, int bit_count, int i_reg)
{
    int ret_count = 0;
    int num_reg = 0;
    int last_used_bit = (bit_start + bit_count - 1);
    for (int num_bit = bit_start; num_bit <= last_used_bit; ++num_bit) {
        ++ret_count;
        if ((((num_bit + 1) % 32) == 0) || (num_bit == last_used_bit)) {
            if (i_reg == num_reg++) {
                return ret_count;
            }
            ret_count = 0;
        }
    }
    return 0;
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

bool esp_efuse_utility_is_correct_written_data(esp_efuse_block_t block, unsigned r_data_len)
{
    uint32_t* w_data = (uint32_t*)range_write_addr_blocks[block].start;
    uint32_t* r_data = (uint32_t*)range_read_addr_blocks[block].start;

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
