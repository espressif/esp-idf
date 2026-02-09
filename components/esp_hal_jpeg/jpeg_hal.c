/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/jpeg_hal.h"
#include "hal/jpeg_ll.h"

void jpeg_hal_init(jpeg_hal_context_t *hal)
{
    hal->dev = JPEG_LL_GET_HW();
}

void jpeg_hal_deinit(jpeg_hal_context_t *hal)
{
    hal->dev = NULL;
}

/*-----------------------------------------------------------------------*/
/* Config huffman code tables with a DHT segment                         */
/*-----------------------------------------------------------------------*/

static void jpeg_hal_create_minicode_tbl(uint8_t *huffbits, uint32_t *huffmin, uint32_t *tmp_huff)
{
    int total_len = 0;
    /* Re-build huffman code word table */
    for (int j = 0, i = 0, hc = 0; i < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; i++, hc <<= 1) {
        int b = huffbits[i];
        // If a codeword length does not exist, the configuration value is 0xFFFF.
        if (huffbits[i] == 0) {
            huffmin[i] = 0xffff;
            continue;
        }
        total_len += huffbits[i];
        while (b--) {
            tmp_huff[j++] = hc++;
        }

        if (huffbits[i] != 0) {
            huffmin[i] = tmp_huff[total_len - huffbits[i]];
            huffmin[i] <<= (15 - i);
        }
    }
}

void jpeg_hal_config_ac0_table(jpeg_hal_context_t *hal, uint8_t *huffbits, uint8_t *huffcode, uint32_t *tmp_huff)
{
    uint32_t huffmin[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {};
    jpeg_hal_create_minicode_tbl(huffbits, huffmin, tmp_huff);
    jpeg_ll_dht_ac0_write_codeword(hal->dev, huffbits, huffmin);
    jpeg_ll_dht_ac0_write_value(hal->dev, huffcode);
}

void jpeg_hal_config_ac1_table(jpeg_hal_context_t *hal, uint8_t *huffbits, uint8_t *huffcode, uint32_t *tmp_huff)
{
    uint32_t huffmin[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {};
    jpeg_hal_create_minicode_tbl(huffbits, huffmin, tmp_huff);
    jpeg_ll_dht_ac1_write_codeword(hal->dev, huffbits, huffmin);
    jpeg_ll_dht_ac1_write_value(hal->dev, huffcode);
}

void jpeg_hal_config_dc0_table(jpeg_hal_context_t *hal, uint8_t *huffbits, uint8_t *huffcode, uint32_t *tmp_huff)
{
    uint32_t huffmin[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {};
    jpeg_hal_create_minicode_tbl(huffbits, huffmin, tmp_huff);
    jpeg_ll_dht_dc0_write_codeword(hal->dev, huffbits, huffmin);
    jpeg_ll_dht_dc0_write_value(hal->dev, huffcode);
}

void jpeg_hal_config_dc1_table(jpeg_hal_context_t *hal, uint8_t *huffbits, uint8_t *huffcode, uint32_t *tmp_huff)
{
    uint32_t huffmin[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {};
    jpeg_hal_create_minicode_tbl(huffbits, huffmin, tmp_huff);
    jpeg_ll_dht_dc1_write_codeword(hal->dev, huffbits, huffmin);
    jpeg_ll_dht_dc1_write_value(hal->dev, huffcode);
}

jpeg_config_dht_table_t dht_func[DHT_TC_NUM][DHT_TH_NUM] = {
    {jpeg_hal_config_dc0_table, jpeg_hal_config_dc1_table},
    {jpeg_hal_config_ac0_table, jpeg_hal_config_ac1_table},
};

jpeg_config_frame_info_t sof_func[JPEG_COMPONENT_NUMBER_MAX] = {
    jpeg_ll_set_frame_info_component0,
    jpeg_ll_set_frame_info_component1,
    jpeg_ll_set_frame_info_component2,
    jpeg_ll_set_frame_info_component3,
};

jpeg_config_quantization_coefficient_t dqt_func[JPEG_COMPONENT_NUMBER_MAX] = {
    jpeg_ll_write_quantization_coefficient_t0,
    jpeg_ll_write_quantization_coefficient_t1,
    jpeg_ll_write_quantization_coefficient_t2,
    jpeg_ll_write_quantization_coefficient_t3,
};

void jpeg_hal_set_quantization_coefficient(jpeg_hal_context_t *hal, uint32_t *lqnr, uint32_t *cqnr)
{
    jpeg_ll_set_access_qnr_ram_mode(hal->dev, 1);
    jpeg_ll_luminance_qnr_table_id(hal->dev, 0);
    jpeg_ll_chrominance_qnr_table_id(hal->dev, 1);
    jpeg_ll_write_quantization_coefficient_t0(hal->dev, lqnr);
    jpeg_ll_write_quantization_coefficient_t1(hal->dev, cqnr);
    jpeg_ll_write_quantization_coefficient_t2(hal->dev, lqnr);
    jpeg_ll_write_quantization_coefficient_t3(hal->dev, cqnr);
}
