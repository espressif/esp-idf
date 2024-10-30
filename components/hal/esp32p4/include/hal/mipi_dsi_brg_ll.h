/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "soc/mipi_dsi_bridge_struct.h"
#include "hal/mipi_dsi_types.h"
#include "hal/lcd_types.h"

#define MIPI_DSI_LL_GET_BRG(bus_id) (bus_id == 0 ? &MIPI_DSI_BRIDGE : NULL)
#define MIPI_DSI_LL_EVENT_UNDERRUN  (1 << 0)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MIPI_DSI_LL_FLOW_CONTROLLER_DMA,    ///< DMA controller as the flow controller
    MIPI_DSI_LL_FLOW_CONTROLLER_BRIDGE, ///< DSI bridge is the flow controller
} mipi_dsi_ll_flow_controller_t;

/**
 * @brief Force enable the register clock for the DSI bridge
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param en true to enable, false to disable
 */
static inline void mipi_dsi_brg_ll_force_enable_reg_clock(dsi_brg_dev_t *dev, bool en)
{
    dev->clk_en.clk_en = en;
}

/**
 * @brief Enable the DSI bridge
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param en True to enable, false to disable
 */
static inline void mipi_dsi_brg_ll_enable(dsi_brg_dev_t *dev, bool en)
{
    dev->en.dsi_en = en;
}

/**
 * @brief Enable DSI bridge interrupt for specific event mask
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param mask Event mask
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_brg_ll_enable_interrupt(dsi_brg_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear DSI bridge interrupt for specific event mask
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param mask Event mask
 */
__attribute__((always_inline))
static inline void mipi_dsi_brg_ll_clear_interrupt_status(dsi_brg_dev_t *dev, uint32_t mask)
{
    dev->int_clr.val = mask;
}

/**
 * @brief Get interrupt status for DSI bridge
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t mipi_dsi_brg_ll_get_interrupt_status(dsi_brg_dev_t *dev)
{
    return dev->int_st.val;
}

/**
 * @brief Set the number of 64-bit words in one dma burst transfer
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param burst_len Number of 64-bit words in one dma burst transfer
 */
static inline void mipi_dsi_brg_ll_set_burst_len(dsi_brg_dev_t *dev, uint32_t burst_len)
{
    dev->dma_req_cfg.dma_burst_len = burst_len;
}

/**
 * @brief Set the fifo empty threshold
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param threshold Threshold value
 */
static inline void mipi_dsi_brg_ll_set_empty_threshold(dsi_brg_dev_t *dev, uint32_t threshold)
{
    dev->raw_buf_almost_empty_thrd.dsi_raw_buf_almost_empty_thrd = threshold;
}

/**
 * @brief Set the number of pixel bits in total
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param num_pixel_bits Number of pixel bits, must be aligned to 64
 */
static inline void mipi_dsi_brg_ll_set_num_pixel_bits(dsi_brg_dev_t *dev, uint32_t num_pixel_bits)
{
    dev->raw_num_cfg.raw_num_total = num_pixel_bits / 64;
    // reload the value into internal counter
    dev->raw_num_cfg.raw_num_total_set = 1;
}

/**
 * @brief Set the threshold whether the dsi_bridge FIFO can receive one more 64-bit
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param threshold Threshold value
 */
static inline void mipi_dsi_brg_ll_credit_set_threshold(dsi_brg_dev_t *dev, uint32_t threshold)
{
    dev->raw_buf_credit_ctl.credit_thrd = threshold;
}

/**
 * @brief Set the threshold whether the dsi_bridge FIFO can receive one more DMA burst
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param threshold Threshold value
 */
static inline void mipi_dsi_brg_ll_credit_set_burst_threshold(dsi_brg_dev_t *dev, uint32_t threshold)
{
    dev->raw_buf_credit_ctl.credit_burst_thrd = threshold;
}

/**
 * @brief Reset the credit counter of the DSI bridge
 *
 * @note valid only when dsi_bridge is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 */
static inline void mipi_dsi_brg_ll_credit_reset(dsi_brg_dev_t *dev)
{
    dev->raw_buf_credit_ctl.credit_reset = 1;
}

/**
 * @brief Set the color coding for the bridge controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param color_coding Color coding value
 * @param sub_config Sub configuration
 */
static inline void mipi_dsi_brg_ll_set_pixel_format(dsi_brg_dev_t *dev, lcd_color_format_t color_coding, uint32_t sub_config)
{
    switch (color_coding) {
    case LCD_COLOR_FMT_RGB565:
        dev->pixel_type.raw_type = 2;
        break;
    case LCD_COLOR_FMT_RGB666:
        dev->pixel_type.raw_type = 1;
        break;
    case LCD_COLOR_FMT_RGB888:
        dev->pixel_type.raw_type = 0;
        break;
    default:
        // MIPI DSI host can only accept RGB data, no YUV data
        HAL_ASSERT(false);
        break;
    }
    dev->pixel_type.dpi_config = sub_config;
}

/**
 * @brief Set the color space for input color data
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param color_space Color space type
 */
static inline void mipi_dsi_brg_ll_set_input_color_space(dsi_brg_dev_t *dev, lcd_color_space_t color_space)
{
    switch (color_space) {
    case LCD_COLOR_SPACE_RGB:
        dev->pixel_type.data_in_type = 0;
        break;
    case LCD_COLOR_SPACE_YUV:
        dev->pixel_type.data_in_type = 1;
        break;
    default:
        abort();
    }
}

/**
 * @brief Set the vertical timing parameters for the bridge controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param vsw Vertical sync width
 * @param vbp Vertical back porch
 * @param active_height Active height
 * @param vfp Vertical front porch
 */
static inline void mipi_dsi_brg_ll_set_vertical_timing(dsi_brg_dev_t *dev, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    dev->dpi_v_cfg0.vdisp = active_height;
    dev->dpi_v_cfg0.vtotal = vsw + vbp + active_height + vfp;
    dev->dpi_v_cfg1.vsync = vsw;
    dev->dpi_v_cfg1.vbank = vbp;
}

/**
 * @brief Set the horizontal timing parameters for the bridge controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param hsw Horizontal sync width
 * @param hbp Horizontal back porch
 * @param active_width Active width
 * @param hfp Horizontal front porch
 */
static inline void mipi_dsi_brg_ll_set_horizontal_timing(dsi_brg_dev_t *dev, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    dev->dpi_h_cfg0.hdisp = active_width;
    dev->dpi_h_cfg0.htotal = hsw + hbp + active_width + hfp;
    dev->dpi_h_cfg1.hsync = hsw;
    dev->dpi_h_cfg1.hbank = hbp;
}

/**
 * @brief Set the under run discard count for the bridge controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param under_run_discard_count Under run discard count
 */
static inline void mipi_dsi_brg_ll_set_underrun_discard_count(dsi_brg_dev_t *dev, uint32_t under_run_discard_count)
{
    dev->dpi_misc_config.fifo_underrun_discard_vcnt = under_run_discard_count;
}

/**
 * @brief Enable the DPI output
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param en True to enable, false to disable
 */
static inline void mipi_dsi_brg_ll_enable_dpi_output(dsi_brg_dev_t *dev, bool en)
{
    dev->dpi_misc_config.dpi_en = en;
}

/**
 * @brief Update the DPI configuration of DSI bridge
 *
 * @param dev Pointer to the DSI bridge controller register base address
 */
static inline void mipi_dsi_brg_ll_update_dpi_config(dsi_brg_dev_t *dev)
{
    dev->dpi_config_update.dpi_config_update = 1;
}

/**
 * @brief Enable the refclk and cfg_clk of dsi host
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param en True to enable, false to disable
 */
static inline void mipi_dsi_brg_ll_enable_ref_clock(dsi_brg_dev_t *dev, bool en)
{
    dev->host_ctrl.dsi_cfg_ref_clk_en = en;
}

/**
 * @brief Set the flow controller of DMA transfer
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param controller Flow controller
 */
static inline void mipi_dsi_brg_ll_set_flow_controller(dsi_brg_dev_t *dev, mipi_dsi_ll_flow_controller_t controller)
{
    dev->dma_flow_ctrl.dsi_dma_flow_controller = controller;
}

/**
 * @brief Set the number of blocks when multi-block transfer is enabled
 *
 * @note only valid when DMAC is the flow controller
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param number Number of blocks
 */
static inline void mipi_dsi_brg_ll_set_multi_block_number(dsi_brg_dev_t *dev, uint32_t number)
{
    dev->dma_flow_ctrl.dma_flow_multiblk_num = number;
    dev->dma_frame_interval.dma_multiblk_en = number > 1;
}

/**
 * @brief Get the FIFO depth of the DSI bridge
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @return FIFO depth
 */
static inline uint32_t mipi_dsi_brg_ll_get_fifo_depth(dsi_brg_dev_t *dev)
{
    return dev->fifo_flow_status.raw_buf_depth;
}

/**
 * @brief Set the YUV-RGB conversion standard
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param std YUV-RGB conversion standard
 */
static inline void mipi_dsi_brg_ll_set_yuv_convert_std(dsi_brg_dev_t *dev, lcd_yuv_conv_std_t std)
{
    switch (std) {
    case LCD_YUV_CONV_STD_BT601:
        dev->yuv_cfg.protocol = 0;
        break;
    case LCD_YUV_CONV_STD_BT709:
        dev->yuv_cfg.protocol = 1;
        break;
    default:
        abort();
    }
}

/**
 * @brief Set the YUV422 packing order
 *
 * @param dev Pointer to the DSI bridge controller register base address
 * @param order YUV422 packing order
 */
static inline void mipi_dsi_brg_ll_set_yuv422_pack_order(dsi_brg_dev_t *dev, lcd_yuv422_pack_order_t order)
{
    switch (order) {
    case LCD_YUV422_PACK_ORDER_UYVY:
        dev->yuv_cfg.yuv422_format = 0;
        dev->yuv_cfg.yuv_pix_endian = 1;
        break;
    case LCD_YUV422_PACK_ORDER_VYUY:
        dev->yuv_cfg.yuv422_format = 1;
        dev->yuv_cfg.yuv_pix_endian = 1;
        break;
    case LCD_YUV422_PACK_ORDER_YUYV:
        dev->yuv_cfg.yuv422_format = 2;
        dev->yuv_cfg.yuv_pix_endian = 1;
        break;
    case LCD_YUV422_PACK_ORDER_YVYU:
        dev->yuv_cfg.yuv422_format = 3;
        dev->yuv_cfg.yuv_pix_endian = 1;
        break;
    default:
        abort();
    }
}

#ifdef __cplusplus
}
#endif
