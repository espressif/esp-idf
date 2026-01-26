/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/hal_utils.h"
#include "hal/mipi_csi_types.h"
#include "hal/mipi_csi_brg_ll.h"
#include "hal/mipi_csi_phy_ll.h"
#include "hal/mipi_csi_host_ll.h"
#include "hal/config.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_LL_HOST_CTLR_NUMS    1

/*---------------------------------------------------------------
                      CSI Bridge
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 * @param en               enable / disable
 */
static inline void mipi_csi_ll_enable_brg_module_clock(int csi_bridge_id, bool en)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_csi_brg_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_brg_module_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mipi_csi_ll_enable_brg_module_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 */
static inline void mipi_csi_ll_reset_brg_module_clock(int csi_bridge_id)
{
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_reset_brg_module_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mipi_csi_ll_reset_brg_module_clock(__VA_ARGS__); \
    } while(0)

/*---------------------------------------------------------------
                      CSI PHY
---------------------------------------------------------------*/
/**
 * @brief Set the clock source for the MIPI CSI D-PHY
 *
 * @param group_id Group ID
 * @param source Clock source
 */
static inline void mipi_csi_ll_set_phy_clock_source(int group_id, mipi_csi_phy_clock_source_t source)
{
    (void)group_id;
    switch (source) {
    case MIPI_CSI_PHY_CLK_SRC_PLL_F20M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 0;
        break;
    case MIPI_CSI_PHY_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 1;
        break;
    case MIPI_CSI_PHY_CLK_SRC_PLL_F25M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 2;
        break;
    default:
        abort();
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_set_phy_clock_source(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mipi_csi_ll_set_phy_clock_source(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable MIPI CSI PHY configuration clock
 *
 * @param group_id Group ID
 * @param en true to enable, false to disable
 */
static inline void mipi_csi_ll_enable_phy_config_clock(int group_id, bool en)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_cfg_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_phy_config_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mipi_csi_ll_enable_phy_config_clock(__VA_ARGS__); \
    } while(0)

/*---------------------------------------------------------------
                      CSI Host
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for MIPI CSI host
 *
 * @param group_id Group ID
 * @param en true to enable, false to disable
 */
static inline void _mipi_csi_ll_enable_host_bus_clock(int group_id, bool en)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_csi_host_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_host_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _mipi_csi_ll_enable_host_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the MIPI CSI host CLK
 *
 * @param group_id Group ID
 */
static inline void mipi_csi_ll_reset_host_clock(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_host = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_host = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_reset_host_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mipi_csi_ll_reset_host_clock(__VA_ARGS__); \
    } while(0)

#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300

/**
 * @brief Set RGB element order for input color data
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param rgb_format RGB element order: 0=RGB, 1=BGR, 2=RBG, 3=BRG, 4=GRB, 5=GBR
 *
 */
static inline void mipi_csi_brg_ll_set_input_rgb_format(csi_brg_dev_t *dev, uint32_t rgb_format)
{
    dev->host_cm_ctrl.csi_host_cm_rx_rgb_format = rgb_format;
}

/**
 * @brief Set the color format for the input color data
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param color_format Camera controller color format
 */
static inline void mipi_csi_brg_ll_set_input_color_format(csi_brg_dev_t *dev, cam_ctlr_color_t color_format)
{
    // Set format type
    if (color_format == CAM_CTLR_COLOR_RGB888) {
        dev->host_cm_ctrl.csi_host_cm_rx = 0;
        mipi_csi_brg_ll_set_input_rgb_format(dev, 0);  // Default: RGB order
    } else if (color_format == CAM_CTLR_COLOR_RGB565) {
        dev->host_cm_ctrl.csi_host_cm_rx = 1;
        mipi_csi_brg_ll_set_input_rgb_format(dev, 0);  // Default: RGB order
    } else if (color_format == CAM_CTLR_COLOR_YUV420) {
        dev->host_cm_ctrl.csi_host_cm_rx = 3;
    } else if (color_format == CAM_CTLR_COLOR_YUV422_YVYU ||
               color_format == CAM_CTLR_COLOR_YUV422_YUYV ||
               color_format == CAM_CTLR_COLOR_YUV422_UYVY ||
               color_format == CAM_CTLR_COLOR_YUV422_VYUY) {
        dev->host_cm_ctrl.csi_host_cm_rx = 2;
        // Set YUV422 packing order: YVYU=0, YUYV=1, VYUY=2, UYVY=3
        if (color_format == CAM_CTLR_COLOR_YUV422_YVYU) {
            dev->host_cm_ctrl.csi_host_cm_rx_yuv422_format = 0;
        } else if (color_format == CAM_CTLR_COLOR_YUV422_YUYV) {
            dev->host_cm_ctrl.csi_host_cm_rx_yuv422_format = 1;
        } else if (color_format == CAM_CTLR_COLOR_YUV422_VYUY) {
            dev->host_cm_ctrl.csi_host_cm_rx_yuv422_format = 2;
        } else if (color_format == CAM_CTLR_COLOR_YUV422_UYVY) {
            dev->host_cm_ctrl.csi_host_cm_rx_yuv422_format = 3;
        }
    } else {
        HAL_ASSERT(false && "Unsupported input color format");
    }
}

/**
 * @brief Enable or disable color mode conversion output
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param en true to enable, false to disable
 *
 */
static inline void mipi_csi_brg_ll_enable_color_conversion(csi_brg_dev_t *dev, bool en)
{
    dev->host_cm_ctrl.csi_host_cm_en = en;
}

/**
 * @brief Enable or disable color mode conversion bypass
 *
 * @param dev Pointer to the CSI bridge controller regihost_cm_ctrlster base address
 * @param bypass true to bypass (input directly to output), false to enable conversion
 *
 */
static inline void mipi_csi_brg_ll_set_color_mode_bypass(csi_brg_dev_t *dev, bool bypass)
{
    dev->host_cm_ctrl.csi_host_cm_bypass = bypass;
}

/**
 * @brief Set the color format for the output color data
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param color_format Camera controller color format
 */
static inline void mipi_csi_brg_ll_set_output_color_format(csi_brg_dev_t *dev, cam_ctlr_color_t color_format)
{
    if (color_format == CAM_CTLR_COLOR_RGB888) {
        dev->host_cm_ctrl.csi_host_cm_tx = 0;
    } else if (color_format == CAM_CTLR_COLOR_RGB565) {
        dev->host_cm_ctrl.csi_host_cm_tx = 1;
    } else if (color_format == CAM_CTLR_COLOR_YUV420) {
        dev->host_cm_ctrl.csi_host_cm_tx = 3;
    } else if (color_format == CAM_CTLR_COLOR_YUV422_YVYU ||
               color_format == CAM_CTLR_COLOR_YUV422_YUYV ||
               color_format == CAM_CTLR_COLOR_YUV422_UYVY ||
               color_format == CAM_CTLR_COLOR_YUV422_VYUY) {
        dev->host_cm_ctrl.csi_host_cm_tx = 2;
    } else {
        HAL_ASSERT(false && "Unsupported output color format");
    }
}
#else
static inline void mipi_csi_brg_ll_set_input_rgb_format(csi_brg_dev_t *dev, uint32_t rgb_format)
{
    //for compatibility
    (void)dev;
    (void)rgb_format;
}

static inline void mipi_csi_brg_ll_set_input_color_format(csi_brg_dev_t *dev, cam_ctlr_color_t color_format)
{
    //for compatibility
    (void)dev;
    (void)color_format;
}

static inline void mipi_csi_brg_ll_enable_color_conversion(csi_brg_dev_t *dev, bool en)
{
    //for compatibility
    (void)dev;
    (void)en;
}

static inline void mipi_csi_brg_ll_set_color_mode_bypass(csi_brg_dev_t *dev, bool bypass)
{
    //for compatibility
    (void)dev;
    (void)bypass;
}

static inline void mipi_csi_brg_ll_set_output_color_format(csi_brg_dev_t *dev, cam_ctlr_color_t color_format)
{
    //for compatibility
    (void)dev;
    (void)color_format;
}
#endif // HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300

#ifdef __cplusplus
}
#endif
