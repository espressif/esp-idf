/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/jpeg_struct.h"
#include "hal/jpeg_types.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JPEG_LL_GET_HW(num)            (&JPEG)

typedef enum {
    JPEG_LL_DONE = (1 << 0),
    JPEG_LL_DCT_DONE = (1 << 11),
} jpeg_ll_codec_intr_t;

typedef enum {
    JPEG_LL_INTR_CID_ERR = (1 << 2),
    JPEG_LL_INTR_C_DHT_DC_ID = (1 << 3),
    JPEG_LL_INTR_C_DHT_AC_ID = (1 << 4),
    JPEG_LL_INTR_C_DQT_ID = (1 << 5),
    JPEG_LL_INTR_RST_UXP_ERR = (1 << 6),
    JPEG_LL_INTR_RST_CHECK_NON_ERR = (1 << 7),
    JPEG_LL_INTR_RST_CHECK_POS_ERR = (1 << 8),
    JPEG_LL_INTR_OUT_EOF = (1 << 9),
    JPEG_LL_INTR_SCAN_CHECK_NONE = (1 << 13),
    JPEG_LL_INTR_SCAN_POS_ERR = (1 << 14),
    JPEG_LL_INTR_UXP_DET = (1 << 15),
    JPEG_LL_INTR_DE_FRAME_EOF_ERR = (1 << 18),
    JPEG_LL_INTR_DE_FRAME_EOF_LACK = (1 << 19),
    JPEG_LL_INTR_SOS_UNMATCH_ERR = (1 << 20),
    JPEG_LL_INTR_MARKER_ERR_FST = (1 << 21),
    JPEG_LL_INTR_MARKER_ERR_OTHER = (1 << 22),
    JPEG_LL_INTR_UNDETECT = (1 << 23),
    JPEG_LL_INTR_DECODE_TIMEOUT = (1 << 24),
} jpeg_ll_decoder_intr_t;

#define JPEG_LL_DECODER_EVENT_INTR (JPEG_LL_INTR_CID_ERR | \
                                    JPEG_LL_INTR_C_DHT_DC_ID | \
                                    JPEG_LL_INTR_C_DHT_AC_ID | \
                                    JPEG_LL_INTR_C_DQT_ID | \
                                    JPEG_LL_INTR_RST_UXP_ERR | \
                                    JPEG_LL_INTR_RST_CHECK_NON_ERR | \
                                    JPEG_LL_INTR_RST_CHECK_POS_ERR | \
                                    JPEG_LL_INTR_OUT_EOF | \
                                    JPEG_LL_INTR_SCAN_CHECK_NONE | \
                                    JPEG_LL_INTR_SCAN_POS_ERR | \
                                    JPEG_LL_INTR_UXP_DET | \
                                    JPEG_LL_INTR_DE_FRAME_EOF_ERR | \
                                    JPEG_LL_INTR_DE_FRAME_EOF_LACK | \
                                    JPEG_LL_INTR_SOS_UNMATCH_ERR | \
                                    JPEG_LL_INTR_MARKER_ERR_FST | \
                                    JPEG_LL_INTR_MARKER_ERR_OTHER | \
                                    JPEG_LL_INTR_UNDETECT | \
                                    JPEG_LL_INTR_DECODE_TIMEOUT)


typedef enum {
    JPEG_LL_RLE_PARALLEL_ERR = (1 << 1),
    JPEG_LL_BS_LAST_BLOCK_EOF = (1 << 12),
    JPEG_LL_EN_FRAME_EOF_ERR = (1 << 16),
    JPEG_LL_EN_FRAME_EOF_LACK = (1 << 16),
} jpeg_ll_encoder_intr_t;

#define JPEG_LL_ENCODER_EVENT_INTR (JPEG_LL_RLE_PARALLEL_ERR | \
                                    JPEG_LL_EN_FRAME_EOF_ERR)

/**
 * @brief Enable the hardware clock for JPEG module
 *
 * @param enable True to enable; false to disable
 */
static inline void jpeg_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_jpeg_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define jpeg_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; jpeg_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the JPEG module
 */
static inline void jpeg_ll_reset_module_register(void)
{
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_jpeg = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_jpeg = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define jpeg_ll_reset_module_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; jpeg_ll_reset_module_register(__VA_ARGS__)

/**
 * @brief Write the numbers of 1~n codewords length sum of ac0 table and write the minimum codeword of code length
 * for 1~16 of ac0 table
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_bits_table Pointer to huffman table
 * @param minimum_code_table Pointer to minimum huffman code table
 */
static inline void jpeg_ll_dht_ac0_write_codeword(jpeg_dev_t *hw, uint8_t *huffman_bits_table, uint32_t *minimum_code_table)
{
    uint32_t element_number = 0;
    for (int idx = 0; idx < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; idx++) {
        element_number += (uint32_t)huffman_bits_table[idx];
        hw->dht_totlen_ac0.dht_totlen_ac0 = element_number;
        hw->dht_codemin_ac0.dht_codemin_ac0 = minimum_code_table[idx];
    }
}

/**
 * @brief Write the symbols corresponding to the decoded codeword for AC0 huffman table in FIFO mode
 *
 * @param hw Pointer to JPEG hardware.
 * @param huffman_value_table Pointer to huffman value table.
 */
static inline void jpeg_ll_dht_ac0_write_value(jpeg_dev_t *hw, uint8_t *huffman_value_table)
{
    for (int idx = 0; idx < JPEG_HUFFMAN_AC_VALUE_TABLE_LEN; idx++) {
        hw->dht_val_ac0.dht_val_ac0 = (uint32_t)huffman_value_table[idx];
    }
}

/**
 * @brief Write the numbers of 1~n codewords length sum of ac1 table and write the minimum codeword of code length
 * for 1~16 of ac1 table
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_bits_table Pointer to huffman table
 * @param minimum_code_table Pointer to minimum huffman code table
 */
static inline void jpeg_ll_dht_ac1_write_codeword(jpeg_dev_t *hw, uint8_t *huffman_bits_table, uint32_t *minimum_code_table)
{
    uint32_t element_number = 0;
    for (int idx = 0; idx < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; idx++) {
        element_number += (uint32_t)huffman_bits_table[idx];
        hw->dht_totlen_ac1.dht_totlen_ac1 = element_number;
        hw->dht_codemin_ac1.dht_codemin_ac1 = minimum_code_table[idx];
    }
}

/**
 * @brief Write the symbols corresponding to the decoded codeword for AC1 huffman table in FIFO mode
 *
 * @param hw Pointer to JPEG hardware.
 * @param huffman_value_table Pointer to huffman value table.
 */
static inline void jpeg_ll_dht_ac1_write_value(jpeg_dev_t *hw, uint8_t *huffman_value_table)
{
    for (int idx = 0; idx < JPEG_HUFFMAN_AC_VALUE_TABLE_LEN; idx++) {
        hw->dht_val_ac1.dht_val_ac1 = (uint32_t)huffman_value_table[idx];
    }
}

/**
 * @brief Write the numbers of 1~n codewords length sum of dc0 table and write the minimum codeword of code length
 * for 1~16 of dc0 table
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_bits_table Pointer to huffman table
 * @param minimum_code_table Pointer to minimum huffman code table
 */
static inline void jpeg_ll_dht_dc0_write_codeword(jpeg_dev_t *hw, uint8_t *huffman_bits_table, uint32_t *minimum_code_table)
{
    uint32_t element_number = 0;
    for (int idx = 0; idx < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; idx++) {
        element_number += (uint32_t)huffman_bits_table[idx];
        hw->dht_totlen_dc0.dht_totlen_dc0 = element_number;
        hw->dht_codemin_dc0.dht_codemin_dc0 = minimum_code_table[idx];
    }
}

/**
 * @brief Write the symbols corresponding to the decoded codeword for DC0 huffman table in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_value_table Pointer to huffman value table.
 */
static inline void jpeg_ll_dht_dc0_write_value(jpeg_dev_t *hw, uint8_t *huffman_value_table)
{
    for (int idx = 0; idx < JPEG_HUFFMAN_DC_VALUE_TABLE_LEN; idx++) {
        hw->dht_val_dc0.dht_val_dc0 = (uint32_t)huffman_value_table[idx];
    }
}

/**
 * @brief Write the numbers of 1~n codewords length sum of dc1 table and write the minimum codeword of code length
 * for 1~16 of dc1 table
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_bits_table Pointer to huffman table
 * @param minimum_code_table Pointer to minimum huffman code table
 */
static inline void jpeg_ll_dht_dc1_write_codeword(jpeg_dev_t *hw, uint8_t *huffman_bits_table, uint32_t *minimum_code_table)
{
    uint32_t element_number = 0;
    for (int idx = 0; idx < JPEG_HUFFMAN_BITS_LEN_TABLE_LEN; idx++) {
        element_number += (uint32_t)huffman_bits_table[idx];
        hw->dht_totlen_dc1.dht_totlen_dc1 = element_number;
        hw->dht_codemin_dc1.dht_codemin_dc1 = minimum_code_table[idx];
    }
}

/**
 * @brief Write the symbols corresponding to the decoded codeword for DC1 huffman table in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param huffman_value_table Pointer to huffman value table.
 */
static inline void jpeg_ll_dht_dc1_write_value(jpeg_dev_t *hw, uint8_t *huffman_value_table)
{
    for (int idx = 0; idx < JPEG_HUFFMAN_DC_VALUE_TABLE_LEN; idx++) {
        hw->dht_val_dc1.dht_val_dc1 = (uint32_t)huffman_value_table[idx];
    }
}

/**
 * @brief Configures the quantization coefficient table precision for the encoder
 *
 * @param hw Pointer to JPEG hardware
 * @param qnr_presition precision of quantization coefficient table
 */
static inline void jpeg_ll_set_qnr_presition(jpeg_dev_t *hw, uint8_t qnr_presition)
{
    hw->config.qnr_presition = qnr_presition;
}

/**
 * @brief Configures it to write quantization coefficient for table0 in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param quantization_table Pointer to quantization table.
 */
static inline void jpeg_ll_write_quantization_coefficient_t0(jpeg_dev_t *hw, uint32_t *quantization_table)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->dqt_info, t0_dqt_info, 0);
    for (int q = 0; q < JPEG_QUANTIZATION_TABLE_LEN; q++) {
        hw->t0qnr.t0_qnr_val = quantization_table[q];
    }
}

/**
 * @brief Configures it to write quantization coefficient for table1 in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param quantization_table Pointer to quantization table.
 */
static inline void jpeg_ll_write_quantization_coefficient_t1(jpeg_dev_t *hw, uint32_t *quantization_table)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->dqt_info, t1_dqt_info, 1);
    for (int q = 0; q < JPEG_QUANTIZATION_TABLE_LEN; q++) {
        hw->t1qnr.chrominance_qnr_val = quantization_table[q];
    }
}

/**
 * @brief Configures it to write quantization coefficient for table2 in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param quantization_table Pointer to quantization table.
 */
static inline void jpeg_ll_write_quantization_coefficient_t2(jpeg_dev_t *hw, uint32_t *quantization_table)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->dqt_info, t2_dqt_info, 2);
    for (int q = 0; q < JPEG_QUANTIZATION_TABLE_LEN; q++) {
        hw->t2qnr.t2_qnr_val = quantization_table[q];
    }
}

/**
 * @brief Configures it to write quantization coefficient for table3 in FIFO mode
 *
 * @param hw Pointer to JPEG hardware
 * @param quantization_table Pointer to quantization table.
 */
static inline void jpeg_ll_write_quantization_coefficient_t3(jpeg_dev_t *hw, uint32_t *quantization_table)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->dqt_info, t3_dqt_info, 3);
    for (int q = 0; q < JPEG_QUANTIZATION_TABLE_LEN; q++) {
        hw->t3qnr.t3_qnr_val = quantization_table[q];
    }
}

/**
 * @brief Configures image’s height
 *
 * @param hw Pointer to JPEG hardware
 * @param height height of image.
 */
static inline void jpeg_ll_set_picture_height(jpeg_dev_t *hw, uint32_t height)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->pic_size, va, height);
}

/**
 * @brief Configures image’s width
 *
 * @param hw Pointer to JPEG hardware
 * @param height width of image.
 */
static inline void jpeg_ll_set_picture_width(jpeg_dev_t *hw, uint32_t width)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->pic_size, ha, width);
}

/**
 * @brief Configure component0.
 *
 * @param hw Pointer to JPEG hardware
 * @param identifier ID of component 0.
 * @param horizontal_factor Configures the horizontal sampling factor of component 0.
 * @param vertical_factor Configures the vertical sampling factor of component 0.
 * @param qun_table_id Configures the selected quantization coefficient table ID for component 0.
 */
static inline void jpeg_ll_set_frame_info_component0(jpeg_dev_t *hw, uint8_t identifier, uint8_t horizontal_factor, uint8_t vertical_factor, uint8_t qun_table_id)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c0, c0_id, identifier);
    hw->c0.c0_x_factor = horizontal_factor;
    hw->c0.c0_y_factor = vertical_factor;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c0, c0_dqt_tbl_sel, qun_table_id);
}

/**
 * @brief Configure component1.
 *
 * @param hw Pointer to JPEG hardware
 * @param identifier ID of component 1.
 * @param horizontal_factor Configures the horizontal sampling factor of component 1.
 * @param vertical_factor Configures the vertical sampling factor of component 1.
 * @param qun_table_id Configures the selected quantization coefficient table ID for component 1.
 */
static inline void jpeg_ll_set_frame_info_component1(jpeg_dev_t *hw, uint8_t identifier, uint8_t horizontal_factor, uint8_t vertical_factor, uint8_t qun_table_id)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c1, c1_id, identifier);
    hw->c1.c1_x_factor = horizontal_factor;
    hw->c1.c1_y_factor = vertical_factor;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c1, c1_dqt_tbl_sel, qun_table_id);
}

/**
 * @brief Configure component2.
 *
 * @param hw Pointer to JPEG hardware
 * @param identifier ID of component 2.
 * @param horizontal_factor Configures the horizontal sampling factor of component 2.
 * @param vertical_factor Configures the vertical sampling factor of component 2.
 * @param qun_table_id Configures the selected quantization coefficient table ID for component 2.
 */
static inline void jpeg_ll_set_frame_info_component2(jpeg_dev_t *hw, uint8_t identifier, uint8_t horizontal_factor, uint8_t vertical_factor, uint8_t qun_table_id)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c2, c2_id, identifier);
    hw->c2.c2_x_factor = horizontal_factor;
    hw->c2.c2_y_factor = vertical_factor;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c2, c2_dqt_tbl_sel, qun_table_id);
}

/**
 * @brief Configure component3.
 *
 * @param hw Pointer to JPEG hardware
 * @param identifier ID of component 3.
 * @param horizontal_factor Configures the horizontal sampling factor of component 3.
 * @param vertical_factor Configures the vertical sampling factor of component 3.
 * @param qun_table_id Configures the selected quantization coefficient table ID for component 3.
 */
static inline void jpeg_ll_set_frame_info_component3(jpeg_dev_t *hw, uint8_t identifier, uint8_t horizontal_factor, uint8_t vertical_factor, uint8_t qun_table_id)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c3, c3_id, identifier);
    hw->c3.c3_x_factor = horizontal_factor;
    hw->c3.c3_y_factor = vertical_factor;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->c3, c3_dqt_tbl_sel, qun_table_id);
}

/**
 * @brief Get factor from component0
 *
 * @param hw Pointer to JPEG hardware
 * @param component_factor Pointer to component factor
 */
static inline void jpeg_ll_get_component0_factor(jpeg_dev_t *hw, jpeg_component_factor_t *component_factor)
{
    component_factor->horizontal = hw->c0.c0_x_factor;
    component_factor->vertical = hw->c0.c0_y_factor;
}

/**
 * @brief Get factor from component1
 *
 * @param hw Pointer to JPEG hardware
 * @param component_factor Pointer to component factor
 */
static inline void jpeg_ll_get_component1_factor(jpeg_dev_t *hw, jpeg_component_factor_t *component_factor)
{
    component_factor->horizontal = hw->c1.c1_x_factor;
    component_factor->vertical = hw->c1.c1_y_factor;
}

/**
 * @brief Configures whether or not to start compressing a new image
 *
 * @param hw Pointer to JPEG hardware
 */
static inline void jpeg_ll_process_start(jpeg_dev_t *hw)
{
    hw->config.jpeg_start = 1;
}

/**
 * @brief Configures whether or not pause the JPEG codec.
 *
 * @param hw Pointer to JPEG hardware
 * @param en true: pause the jpeg codec, false: resume.
 */
static inline void jpeg_ll_codec_pause(jpeg_dev_t *hw, bool en)
{
    hw->config.pause_en = en;
}


/**
 * @brief Apply soft reset, this will reset the hardware fsm and fifo.
 *
 * @param hw Pointer to JPEG hardware.
 */
static inline void jpeg_ll_soft_rst(jpeg_dev_t *hw)
{
    hw->config.soft_rst = 1;
    hw->config.soft_rst = 0;
}

/**
 * @brief Configures if the JPEG codec is working as an encoder or a decoder.
 *
 * @param hw Pointer to JPEG hardware.
 * @param mode Refer to `jpeg_codec_mode_t`
 */
static inline void jpeg_ll_set_codec_mode(jpeg_dev_t *hw, jpeg_codec_mode_t mode)
{
    uint8_t mode_val = 0;
    switch (mode) {
    case JPEG_CODEC_ENCODER:
        mode_val = 0;
        break;
    case JPEG_CODEC_DECODER:
        mode_val = 1;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    hw->config.mode = mode_val;
}

/**
 * @brief Configures whether or not to enable FIFO mode of configuring quantization coefficient tables.
 *
 * @param hw Pointer to JPEG hardware.
 * @param mode 0: Use non-fifo mode. 1: Use fifo mode.
 */
static inline void jpeg_ll_set_access_qnr_ram_mode(jpeg_dev_t *hw, uint8_t mode)
{
    hw->config.qnr_fifo_en = mode;
}

/**
 * @brief Configures the number of color components in image when decoding
 *
 * @param hw Pointer to JPEG hardware.
 * @param component_num Number of color components
 */
static inline void jpeg_ll_set_decode_component_num(jpeg_dev_t *hw, uint8_t component_num)
{
    // ESP32P4 only support to decode color component not larger than 4
    HAL_ASSERT(component_num <= 3);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->decode_conf, component_num, component_num);
}

/**
 * @brief Configures the format of the image to be compressed
 *
 * @param hw Pointer to JPEG hardware.
 * @param mode_sel Select sample mode, refer to `jpeg_sample_mode_t`.
 */
static inline void jpeg_ll_sample_mode_select(jpeg_dev_t *hw, jpeg_sample_mode_t mode_sel)
{
    uint8_t sample_sel = 0;
    switch (mode_sel) {
    case JPEG_SAMPLE_MODE_YUV444:
        sample_sel = 0;
        break;
    case JPEG_SAMPLE_MODE_YUV422:
        sample_sel = 1;
        break;
    case JPEG_SAMPLE_MODE_YUV420:
        sample_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    hw->config.sample_sel = sample_sel;
}

/**
 * @brief Configure whether or not to reverse the original image's pixel order.
 *
 * @param hw Pointer to JPEG hardware.
 * @param reverse_en 0: not revert; 1: revert.
 */
static inline void jpeg_ll_pixel_reverse(jpeg_dev_t *hw, bool reverse_en)
{
    hw->config.pixel_rev = reverse_en;
}

/**
 * @brief Configures whether or not to add EOI of “0xffd9” at the end of bitstream
 *
 * @param hw Pointer to JPEG hardware.
 * @param tailer_en 1: Add `0xffd9` at the end of bitstream.
 */
static inline void jpeg_ll_add_tail(jpeg_dev_t *hw, bool tailer_en)
{
    hw->config.tailer_en = tailer_en;
}

/**
 * @brief Configures whether or not to add “00” after “ff”
 *
 * @param hw Pointer to JPEG hardware.
 * @param ff_check_en 0: not add `00` after `ff`; 1: Add.
 */
static inline void jpeg_ll_enable_ff_check(jpeg_dev_t *hw, bool ff_check_en)
{
    hw->config.ff_check_en = ff_check_en;
}

/**
 * @brief Configures the luma quantization table ID for the encoder
 *
 * @param hw Pointer to JPEG hardware.
 * @param lqnr_id ID of luma quantization table.
 */
static inline void jpeg_ll_luminance_qnr_table_id(jpeg_dev_t *hw, uint8_t lqnr_id)
{
    hw->config.lqnr_tbl_sel = lqnr_id;
}

/**
 * @brief Configures the chroma quantization table ID for the encoder.
 *
 * @param hw Pointer to JPEG hardware.
 * @param cqnr_id ID of chroma quantization table.
 */
static inline void jpeg_ll_chrominance_qnr_table_id(jpeg_dev_t *hw, uint8_t cqnr_id)
{
    hw->config.cqnr_tbl_sel = cqnr_id;
}

/**
 * @brief Configures the restart interval in DRI segment when decoding
 *
 * @param hw Pointer to JPEG hardware.
 * @param restart_interval value of restart interval.
 */
static inline void jpeg_ll_set_restart_interval(jpeg_dev_t *hw, uint16_t restart_interval)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->decode_conf, restart_interval, restart_interval);
}

/**
 * @brief Get JPEG interrupt status register address
 *
 * @param hw Pointer to JPEG hardware.
 * @return address of interrupt status register.
 */
static inline volatile void *jpeg_ll_get_interrupt_status_reg(jpeg_dev_t *hw)
{
    return &hw->int_st;
}

/**
 * @brief Enable JPEG interrupt
 *
 * @param hw Pointer to JPEG hardware.
 */
static inline void jpeg_ll_enable_intr_mask(jpeg_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val |= mask;
}

/**
 * @brief Clear JPEG interrupt
 *
 * @param hw Pointer to JPEG hardware.
 */
static inline void jpeg_ll_clear_intr_mask(jpeg_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

/**
 * @brief Disable JPEG interrupt
 *
 * @param hw Pointer to JPEG hardware.
 */
static inline void jpeg_ll_disable_intr_mask(jpeg_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val &= (~mask);
}

/**
 * @brief Get JPEG interrupt status
 *
 * @param hw Pointer to JPEG hardware.
 */
static inline uint32_t jpeg_ll_get_intr_status(jpeg_dev_t *hw)
{
    return hw->int_st.val;
}

static inline void jpeg_ll_config_picture_pixel_format(jpeg_dev_t *hw, jpeg_enc_src_type_t pixel_format)
{
    uint8_t cs = 0;
    switch (pixel_format) {
    case JPEG_ENC_SRC_RGB888:
        cs = 0;
        break;
    case JPEG_ENC_SRC_YUV422:
        cs = 1;
        break;
    case JPEG_ENC_SRC_RGB565:
        cs = 2;
        break;
    case JPEG_ENC_SRC_GRAY:
        cs = 3;
        break;
    default:
        abort();
    }
    hw->config.color_space = cs;
}

#ifdef __cplusplus
}
#endif
