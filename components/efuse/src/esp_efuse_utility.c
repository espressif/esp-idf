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
#include "esp32/clk.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

static const char *TAG = "efuse";

#define COUNT_EFUSE_BLOCKS        4      /* The number of blocks. */
#define COUNT_EFUSE_REG_PER_BLOCK 8      /* The number of registers per block. */
#define EFUSE_CONF_WRITE          0x5A5A /* eFuse_pgm_op_ena, force no rd/wr disable. */
#define EFUSE_CONF_READ           0x5AA5 /* eFuse_read_op_ena, release force. */
#define EFUSE_CMD_PGM             0x02   /* Command to program. */
#define EFUSE_CMD_READ            0x01   /* Command to read. */

// Array for emulate efuse registers.
#ifdef CONFIG_EFUSE_VIRTUAL
static uint32_t virt_blocks[COUNT_EFUSE_BLOCKS][COUNT_EFUSE_REG_PER_BLOCK];

/* Call the update function to seed virtual efuses during initialization */
__attribute__((constructor)) void esp_efuse_utility_update_virt_blocks(void);

#endif

/**
 * @brief Structure range address by blocks
 */
typedef struct {
    uint32_t start;
    uint32_t end;
} esp_efuse_range_addr_t;

/*Range addresses to read blocks*/
static const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_BLK0_RDATA0_REG, EFUSE_BLK0_RDATA6_REG},    // range address of EFUSE_BLK0
    {EFUSE_BLK1_RDATA0_REG, EFUSE_BLK1_RDATA7_REG},    // range address of EFUSE_BLK1
    {EFUSE_BLK2_RDATA0_REG, EFUSE_BLK2_RDATA7_REG},    // range address of EFUSE_BLK2
    {EFUSE_BLK3_RDATA0_REG, EFUSE_BLK3_RDATA7_REG}     // range address of EFUSE_BLK3
};

/*Range addresses to write blocks*/
static const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {EFUSE_BLK0_WDATA0_REG, EFUSE_BLK0_WDATA6_REG},    // range address of EFUSE_BLK0
    {EFUSE_BLK1_WDATA0_REG, EFUSE_BLK1_WDATA7_REG},    // range address of EFUSE_BLK1
    {EFUSE_BLK2_WDATA0_REG, EFUSE_BLK2_WDATA7_REG},    // range address of EFUSE_BLK2
    {EFUSE_BLK3_WDATA0_REG, EFUSE_BLK3_WDATA7_REG}     // range address of EFUSE_BLK3
};

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
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            REG_WRITE(addr_wr_block, 0);
        }
    }
}

// Burn values written to the efuse write registers
void esp_efuse_utility_burn_efuses(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "Virtual efuses enabled: Not really burning eFuses");
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        if (scheme == EFUSE_CODING_SCHEME_3_4) {
            uint8_t buf[COUNT_EFUSE_REG_PER_BLOCK * 4] = { 0 };
            int i = 0;
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4, ++i) {
                *((uint32_t*)buf + i) = REG_READ(addr_wr_block);
            }
            int j = 0;
            uint32_t out_buf[COUNT_EFUSE_REG_PER_BLOCK] = { 0 };
            for (int k = 0; k < 4; ++k, ++j) {
                memcpy((uint8_t*)out_buf + j * 6, &buf[k * 8], 6);
            }
            for (int k = 0; k < COUNT_EFUSE_REG_PER_BLOCK; ++k) {
                REG_WRITE(range_write_addr_blocks[num_block].start + k * 4,  out_buf[k]);
            }
        }
        int subblock = 0;
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            virt_blocks[num_block][subblock++] |= REG_READ(addr_wr_block);
        }
    }
#else
    // Update Efuse timing configuration
    uint32_t apb_freq_mhz = esp_clk_apb_freq() / 1000000;
    uint32_t clk_sel0, clk_sel1, dac_clk_div;
    if (apb_freq_mhz <= 26) {
        clk_sel0 = 250;
        clk_sel1 = 255;
        dac_clk_div = 52;
    } else if (apb_freq_mhz <= 40) {
        clk_sel0 = 160;
        clk_sel1 = 255;
        dac_clk_div = 80;
    } else {
        clk_sel0 = 80;
        clk_sel1 = 128;
        dac_clk_div = 100;
    }
    REG_SET_FIELD(EFUSE_DAC_CONF_REG, EFUSE_DAC_CLK_DIV, dac_clk_div);
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL0, clk_sel0);
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL1, clk_sel1);
    // Permanently update values written to the efuse write registers
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_WRITE);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_PGM);
    while (REG_READ(EFUSE_CMD_REG) != 0) {};
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_READ);
    while (REG_READ(EFUSE_CMD_REG) != 0) {};
#endif
    esp_efuse_utility_reset();
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
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
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
void esp_efuse_utility_debug_dump_blocks(void)
{
    printf("EFUSE_BLKx:\n");
#ifdef CONFIG_EFUSE_VIRTUAL
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        int num_reg = 0;
        printf("%d) ", num_block);
        for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, num_reg++) {
            printf("0x%08x ", virt_blocks[num_block][num_reg]);
        }
        printf("\n");
    }
#else
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        printf("%d) ", num_block);
        for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4) {
            printf("0x%08x ", REG_READ(addr_rd_block));
        }
        printf("\n");
    }
#endif
    printf("\n");
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
    assert(blk >= 0 && blk <= 3);
    if (blk == 0) {
        assert(num_reg <= 6);
    } else {
        assert(num_reg <= 7);
    }

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
    assert(blk >= 0 && blk <= 3);
    if (blk == 0) {
        assert(num_reg <= 6);
    } else {
        assert(num_reg <= 7);
    }
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
    esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(blk);
    int max_num_bit = offset_in_bits + size_bits;
    if ((scheme == EFUSE_CODING_SCHEME_NONE   && max_num_bit > 256) ||
        (scheme == EFUSE_CODING_SCHEME_3_4    && max_num_bit > 192) ||
        (scheme == EFUSE_CODING_SCHEME_REPEAT && max_num_bit > 128)) {
        return false;
    }
    return true;
}

static bool read_w_data_and_check_fill(esp_efuse_block_t num_block, uint32_t *buf_w_data)
{
    bool blk_is_filled = false;
    int i = 0;
    for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4, ++i) {
        buf_w_data[i] = REG_READ(addr_wr_block);
        if (buf_w_data[i] != 0) {
            REG_WRITE(addr_wr_block, 0);
            blk_is_filled = true;
        }
    }
    return blk_is_filled;
}

static void read_r_data(esp_efuse_block_t num_block, uint32_t* buf_r_data)
{
    int i = 0;
    for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, ++i) {
        buf_r_data[i] = REG_READ(addr_rd_block);
    }
}

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, applies coding scheme and writes encoded values back to EFUSE_BLKx_WDATAx_REG.
esp_err_t esp_efuse_utility_apply_new_coding_scheme(void)
{
    uint8_t buf_w_data[COUNT_EFUSE_REG_PER_BLOCK * 4];
    uint8_t buf_r_data[COUNT_EFUSE_REG_PER_BLOCK * 4];
    uint32_t reg[COUNT_EFUSE_REG_PER_BLOCK];
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = 1; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        // check and apply a new coding scheme.
        if (scheme != EFUSE_CODING_SCHEME_NONE) {
            memset(buf_w_data, 0, sizeof(buf_w_data));
            memset((uint8_t*)reg, 0, sizeof(reg));
            if (read_w_data_and_check_fill(num_block, (uint32_t*)buf_w_data) == true) {
                read_r_data(num_block, (uint32_t*)buf_r_data);
                if (scheme == EFUSE_CODING_SCHEME_3_4) {
                    if (*((uint32_t*)buf_w_data + 6) != 0 || *((uint32_t*)buf_w_data + 7) != 0) {
                        return ESP_ERR_CODING;
                    }
                    for (int i = 0; i < 24; ++i) {
                        if (buf_w_data[i] != 0) {
                            int st_offset_buf = (i / 6) * 6;
                            // check that place is free.
                            for (int n = st_offset_buf; n < st_offset_buf + 6; ++n) {
                                if (buf_r_data[n] != 0) {
                                    ESP_LOGE(TAG, "Bits are not empty. Write operation is forbidden.");
                                    return ESP_ERR_CODING;
                                }
                            }

                            esp_err_t err = esp_efuse_apply_34_encoding(&buf_w_data[st_offset_buf], reg, 6);
                            if (err != ESP_OK) {
                                return err;
                            }

                            int num_reg = (st_offset_buf / 6) * 2;
                            for (int r = 0; r < 2; r++) {
                                REG_WRITE(range_write_addr_blocks[num_block].start + (num_reg + r) * 4, reg[r]);
                            }
                            i = st_offset_buf + 5;
                        }
                    }
                } else if (scheme == EFUSE_CODING_SCHEME_REPEAT) {
                    uint32_t* buf_32 = (uint32_t*)buf_w_data;
                    for (int i = 4; i < 8; ++i) {
                        if (*(buf_32 + i) != 0) {
                            return ESP_ERR_CODING;
                        }
                    }
                    for (int i = 0; i < 4; ++i) {
                        if (buf_32[i] != 0) {
                            REG_WRITE(range_write_addr_blocks[num_block].start + i * 4, buf_32[i]);
                            REG_WRITE(range_write_addr_blocks[num_block].start + (i + 4) * 4, buf_32[i]);
                        }
                    }
                }
            }
        }
    }
    return ESP_OK;
}
