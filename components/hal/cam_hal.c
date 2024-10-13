/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/cam_ll.h"
#include "hal/cam_hal.h"
#include "soc/soc_caps.h"
#include "soc/cam_periph.h"

/**
 * @brief Configure line number to trigger interrupt
 *
 * @param hal CAM object data pointer
 * @param num line number
 *
 * @return None
 */
static void cam_hal_set_line_int_num(cam_hal_context_t *hal, uint32_t num)
{
    if (num > 0) {
        cam_ll_enable_hs_line_int(hal->hw, 1);
        cam_ll_set_line_int_num(hal->hw, num);
    } else {
        cam_ll_enable_hs_line_int(hal->hw, 0);
        cam_ll_set_line_int_num(hal->hw, 0);
    }
}

/**
 * @brief Configure V-SYNC filter threshold
 *
 * @param hal CAM object data pointer
 * @param num V-SYNC filter threshold
 *
 * @return None
 */
static void cam_hal_set_vsync_filter_num(cam_hal_context_t *hal, uint32_t num)
{
    if (num > 0) {
        cam_ll_enable_vsync_filter(hal->hw, 1);
        cam_ll_set_vsync_filter_thres(hal->hw, num);
    } else {
        cam_ll_enable_vsync_filter(hal->hw, 0);
        cam_ll_set_vsync_filter_thres(hal->hw, 0);
    }
}

/**
 * @brief Initialize CAM hardware
 *
 * @param hal    CAM object data pointer
 * @param config CAM configuration
 *
 * @return None
 */
void cam_hal_init(cam_hal_context_t *hal, const cam_hal_config_t *config)
{
    memset(hal, 0, sizeof(cam_hal_context_t));

    hal->hw = CAM_LL_GET_HW(0);

    cam_ll_enable_stop_signal(hal->hw, 0);
    cam_ll_swap_dma_data_byte_order(hal->hw, config->byte_swap_en);
    cam_ll_reverse_dma_data_bit_order(hal->hw, 0);
    cam_ll_enable_vsync_generate_eof(hal->hw, 1);

    cam_hal_set_line_int_num(hal, 0);
    cam_hal_set_vsync_filter_num(hal, 0);

    cam_ll_enable_invert_pclk(hal->hw, 0);
    cam_ll_set_input_data_width(hal->hw, 8);
    cam_ll_enable_invert_de(hal->hw, 0);
    cam_ll_enable_invert_vsync(hal->hw, 0);
    cam_ll_enable_invert_hsync(hal->hw, 0);
    cam_ll_set_vh_de_mode(hal->hw, 0); // Disable vh_de mode default
    cam_ll_enable_rgb_yuv_convert(hal->hw, 0); // bypass conv module default
}

/**
 * @brief De-initialize CAM hardware
 *
 * @note Stop stream before deinit
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_deinit(cam_hal_context_t *hal)
{
    cam_ll_stop(hal->hw);
    cam_ll_reset(hal->hw);
    cam_ll_fifo_reset(hal->hw);
}

/**
 * @brief Start CAM to receive frame data
 *
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_start_streaming(cam_hal_context_t *hal)
{
    cam_ll_reset(hal->hw);
    cam_ll_fifo_reset(hal->hw);

    cam_ll_start(hal->hw);
}

/**
 * @brief Stop CAM receiving frame data
 *
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_stop_streaming(cam_hal_context_t *hal)
{
    cam_ll_stop(hal->hw);
}
