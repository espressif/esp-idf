/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/mipi_dsi_host_struct.h"
#include "hal/mipi_dsi_types.h"
#include "hal/lcd_types.h"

#define MIPI_DSI_LL_GET_HOST(bus_id) (bus_id == 0 ? &MIPI_DSI_HOST : NULL)

#define MIPI_DSI_LL_MAX_DPI_CLK_DIV 256

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MIPI DSI transmission speed mode
 */
typedef enum {
    MIPI_DSI_LL_TRANS_SPEED_HS, /*!< High speed transmission */
    MIPI_DSI_LL_TRANS_SPEED_LP, /*!< Low power transmission */
} mipi_dsi_ll_trans_speed_mode_t;

/**
 * @brief MIPI DSI clock lane state
 */
typedef enum {
    MIPI_DSI_LL_CLOCK_LANE_STATE_AUTO, /*!< Clock lane state is automatic controlled by the DSI host */
    MIPI_DSI_LL_CLOCK_LANE_STATE_HS,   /*!< Clock lane state is in high speed */
    MIPI_DSI_LL_CLOCK_LANE_STATE_LP,   /*!< Clock lane state is in low power */
} mipi_dsi_ll_clock_lane_state_t;

/**
 * @brief Color coding type (depth and pixel configuration)
 */
typedef enum {
    MIPI_DSI_LL_COLOR_CODE_16BIT_CONFIG1 =  0, // 16-bit configuration 1
    MIPI_DSI_LL_COLOR_CODE_16BIT_CONFIG2 =  1, // 16-bit configuration 2
    MIPI_DSI_LL_COLOR_CODE_16BIT_CONFIG3 =  2, // 16-bit configuration 3
    MIPI_DSI_LL_COLOR_CODE_18BIT_CONFIG1 =  3, // 18-bit configuration 1
    MIPI_DSI_LL_COLOR_CODE_18BIT_CONFIG2 =  4, // 18-bit configuration 2
    MIPI_DSI_LL_COLOR_CODE_24BIT         =  5, // 24-bit
} mipi_dsi_ll_color_coding_t;

/**
 * @brief MIPI DSI Video mode burst type
 */
typedef enum {
    MIPI_DSI_LL_VIDEO_NON_BURST_WITH_SYNC_PULSES, // Non-burst mode with sync pulses
    MIPI_DSI_LL_VIDEO_NON_BURST_WITH_SYNC_EVENTS, // Non-burst mode with sync events
    MIPI_DSI_LL_VIDEO_BURST_WITH_SYNC_PULSES,     // Burst mode with sync pulses
} mipi_dsi_ll_video_burst_type_t;

/**
 * @brief Set the DSI Host controller power state
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param True to turn on, False to turn off
 */
static inline void mipi_dsi_host_ll_power_on_off(dsi_host_dev_t *dev, bool on)
{
    dev->pwr_up.shutdownz = on;
}

/**
 * @brief Set the division factor for the Time Out clock
 *
 * @note The Time Out clock is the clock used as the timing unit in the configuration of HS->LP and LP->HS transition error.
 * @note Time out clock source is lane byte clock
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param div Division factor for the Time Out clock
 */
static inline void mipi_dsi_host_ll_set_timeout_clock_division(dsi_host_dev_t *dev, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->clkmgr_cfg, to_clk_division, div);
}

/**
 * @brief Set the division factor for the Escape clock
 *
 * @note TX Escape clock source is lane byte clock
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param div Division factor for the Escape clock
 */
static inline void mipi_dsi_host_ll_set_escape_clock_division(dsi_host_dev_t *dev, uint32_t div)
{
    HAL_ASSERT(div > 1 && div < 256);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->clkmgr_cfg, tx_esc_clk_division, div);
}

/**
 * @brief Set the timeout counts for various operations
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param hs_tx Timeout count for HS transmission, measured in TO_CLK_DIVISION cycles
 * @param lp_rx Timeout count for LP reception, measured in TO_CLK_DIVISION cycles
 * @param hs_rd Timeout count for HS read, measured in lane byte clock cycles
 * @param lp_rd Timeout count for LP read, measured in lane byte clock cycles
 * @param hs_wr Timeout count for HS write, measured in lane byte clock cycles
 * @param lp_wr Timeout count for LP write, measured in lane byte clock cycles
 * @param bta Timeout count for BTA, measured in lane byte clock cycles
 */
static inline void mipi_dsi_host_ll_set_timeout_count(dsi_host_dev_t *dev, uint32_t hs_tx, uint32_t lp_rx, uint32_t hs_rd, uint32_t lp_rd, uint32_t hs_wr, uint32_t lp_wr, uint32_t bta)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->to_cnt_cfg, hstx_to_cnt, hs_tx);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->to_cnt_cfg, lprx_to_cnt, lp_rx);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->hs_rd_timeout_cnt, hs_rd_to_cnt, hs_rd);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lp_rd_timeout_cnt, lp_rd_to_cnt, lp_rd);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->hs_wr_timeout_cnt, hs_wr_to_cnt, hs_wr);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->lp_wr_timeout_cnt, lp_wr_to_cnt, lp_wr);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->bta_timeout_cnt, bta_to_cnt, bta);
}

/**
 * @brief Set the state of the clock lane
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param state Clock lane state
 */
static inline void mipi_dsi_host_ll_set_clock_lane_state(dsi_host_dev_t *dev, mipi_dsi_ll_clock_lane_state_t state)
{
    switch (state) {
    case MIPI_DSI_LL_CLOCK_LANE_STATE_AUTO:
        dev->lpclk_ctrl.auto_clklane_ctrl = 1;
        dev->lpclk_ctrl.phy_txrequestclkhs = 1;
        break;
    case MIPI_DSI_LL_CLOCK_LANE_STATE_HS:
        dev->lpclk_ctrl.auto_clklane_ctrl = 0;
        dev->lpclk_ctrl.phy_txrequestclkhs = 1;
        break;
    case MIPI_DSI_LL_CLOCK_LANE_STATE_LP:
        dev->lpclk_ctrl.auto_clklane_ctrl = 0;
        dev->lpclk_ctrl.phy_txrequestclkhs = 0;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Enable / Disable video mode
 *
 * @note Commands can still be sent by the generic interface while in video mode
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_video_mode(dsi_host_dev_t *dev, bool en)
{
    dev->mode_cfg.cmd_video_mode = !en;
}

////////////////////////////////////////////DPI Interface///////////////////////////////

/**
 * @brief Set DPI virtual channel ID that will be indexed to the video mode packets
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param vcid Virtual channel ID
 */
static inline void mipi_dsi_host_ll_dpi_set_vcid(dsi_host_dev_t *dev, uint32_t vcid)
{
    dev->dpi_vcid.dpi_vcid = vcid;
}

/**
 * @brief Set DPI video color coding
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param color_coding Color coding value
 * @param sub_config Sub configuration value
 */
static inline void mipi_dsi_host_ll_dpi_set_color_coding(dsi_host_dev_t *dev, lcd_color_format_t color_coding, uint32_t sub_config)
{
    switch (color_coding) {
    case LCD_COLOR_FMT_RGB565:
        dev->dpi_color_coding.dpi_color_coding = MIPI_DSI_LL_COLOR_CODE_16BIT_CONFIG1 + sub_config;
        break;
    case LCD_COLOR_FMT_RGB666:
        dev->dpi_color_coding.dpi_color_coding = MIPI_DSI_LL_COLOR_CODE_18BIT_CONFIG1 + sub_config;
        break;
    case LCD_COLOR_FMT_RGB888:
        dev->dpi_color_coding.dpi_color_coding = MIPI_DSI_LL_COLOR_CODE_24BIT;
        break;
    default:
        // MIPI DSI bridge can only write RGB data to the DSI host controller, no YUV data
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Enable DPI loosely packetization video (used only when color depth = 18)
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_dpi_enable_loosely18_packet(dsi_host_dev_t *dev, bool en)
{
    dev->dpi_color_coding.loosely18_en = en;
}

/**
 * @brief Set DPI timing signal polarity
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param hsync_active_low If the HSYNC signal is active low
 * @param vsync_active_low If the VSYNC signal is active low
 * @param de_active_low If the DE signal is active low
 * @param shut_down_active_low If the SHUTDOWNZ signal is active low
 * @param color_mode_active_low If the COLORM active low
 */
static inline void mipi_dsi_host_ll_dpi_set_timing_polarity(dsi_host_dev_t *dev, bool hsync_active_low, bool vsync_active_low, bool de_active_low, bool shut_down_active_low, bool color_mode_active_low)
{
    dev->dpi_cfg_pol.hsync_active_low = hsync_active_low;
    dev->dpi_cfg_pol.vsync_active_low = vsync_active_low;
    dev->dpi_cfg_pol.dataen_active_low = de_active_low;
    dev->dpi_cfg_pol.shutd_active_low = shut_down_active_low;
    dev->dpi_cfg_pol.colorm_active_low = color_mode_active_low;
}

/**
 * @brief Enable frame BTA acknowledgement
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_dpi_enable_frame_ack(dsi_host_dev_t *dev, bool en)
{
    dev->vid_mode_cfg.frame_bta_ack_en = en;
}

/**
 * @brief Enable return to low power mode inside horizontal front/back porch periods when timing allows
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param en_hbp True to enable, False to disable
 * @param en_hfp True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_dpi_enable_lp_horizontal_timing(dsi_host_dev_t *dev, bool en_hbp, bool en_hfp)
{
    dev->vid_mode_cfg.lp_hbp_en = en_hbp;
    dev->vid_mode_cfg.lp_hfp_en = en_hfp;
}

/**
 * @brief Enable return to low power mode inside vertical timing periods (e.g. vbp) when timing allows
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param en_vsync True to enable, False to disable
 * @param en_vbp True to enable, False to disable
 * @param en_vfp True to enable, False to disable
 * @param en_vact True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_dpi_enable_lp_vertical_timing(dsi_host_dev_t *dev, bool en_vsync, bool en_vbp, bool en_vfp, bool en_vact)
{
    dev->vid_mode_cfg.lp_vsa_en = en_vsync;
    dev->vid_mode_cfg.lp_vbp_en = en_vbp;
    dev->vid_mode_cfg.lp_vfp_en = en_vfp;
    dev->vid_mode_cfg.lp_vact_en = en_vact;
}

/**
 * @brief Enable the command transmission in LP mode
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_dpi_enable_lp_command(dsi_host_dev_t *dev, bool enable)
{
    dev->vid_mode_cfg.lp_cmd_en = enable;
}

/**
 * @brief Set MIPI DSI video burst type
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param mode Video mode type
 */
static inline void mipi_dsi_host_ll_dpi_set_video_burst_type(dsi_host_dev_t *dev, mipi_dsi_ll_video_burst_type_t type)
{
    dev->vid_mode_cfg.vid_mode_type = type;
}

/**
 * @brief Set the kind of the pattern to be generated by the DSI Host controller
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param type Pattern type
 */
static inline void mipi_dsi_host_ll_dpi_set_pattern_type(dsi_host_dev_t *dev, mipi_dsi_pattern_type_t type)
{
    switch (type) {
    case MIPI_DSI_PATTERN_BAR_HORIZONTAL:
        dev->vid_mode_cfg.vpg_mode = 0;
        dev->vid_mode_cfg.vpg_orientation = 1;
        dev->vid_mode_cfg.vpg_en = 1;
        break;
    case MIPI_DSI_PATTERN_BAR_VERTICAL:
        dev->vid_mode_cfg.vpg_mode = 0;
        dev->vid_mode_cfg.vpg_orientation = 0;
        dev->vid_mode_cfg.vpg_en = 1;
        break;
    case MIPI_DSI_PATTERN_BER_VERTICAL:
        dev->vid_mode_cfg.vpg_mode = 1;
        dev->vid_mode_cfg.vpg_orientation = 0;
        dev->vid_mode_cfg.vpg_en = 1;
        break;
    case MIPI_DSI_PATTERN_NONE:
        dev->vid_mode_cfg.vpg_en = 0;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Set the number of bytes inside a null packet
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param packet_size Number of bytes inside a null packet, 0 means to disable the null packet
 */
static inline void mipi_dsi_host_ll_dpi_set_null_packet_size(dsi_host_dev_t *dev, uint32_t packet_size)
{
    dev->vid_null_size.vid_null_size = packet_size;
}

/**
 * @brief Se the number of chunks to be transmitted during a Line period
 *
 * @note A trunk is pair of video packet and null packet
 * @note The data in each chunk is set in `mipi_dsi_host_ll_dpi_set_video_packet_pixel_num`
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param trunk_num Number of chunks.  0 - video line is transmitted in a single packet
 *                                     1 - video line is transmitted in a single packet, followed by a null packet
 *                                     Others - Multiple chunks are used to transmit each video line
 */
static inline void mipi_dsi_host_ll_dpi_set_trunks_num(dsi_host_dev_t *dev, uint32_t trunks_num)
{
    dev->vid_num_chunks.vid_num_chunks = trunks_num;
}

/**
 * @brief Set the number of pixels in a signal video packet
 *
 * @note For 18-bit not loosely packed data types, the size must be a multiple of 4
 * @note For YUV data types, the size must be a multiple of 2
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param packet_pixels Number of pixels in a signal video packet
 */
static inline void mipi_dsi_host_ll_dpi_set_video_packet_pixel_num(dsi_host_dev_t *dev, uint32_t packet_pixels)
{
    dev->vid_pkt_size.vid_pkt_size = packet_pixels;
}

/**
 * @brief Set vertical timing parameters of video mode
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param vsw Vertical Synchronization Width, in lines
 * @param vbp Vertical Back Porch period, in lines
 * @param active_height Vertical active height, in lines
 * @param vfp Vertical Front Porch period, in lines
 */
static inline void mipi_dsi_host_ll_dpi_set_vertical_timing(dsi_host_dev_t *dev, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    dev->vid_vsa_lines.vsa_lines = vsw;
    dev->vid_vbp_lines.vbp_lines = vbp;
    dev->vid_vactive_lines.v_active_lines = active_height;
    dev->vid_vfp_lines.vfp_lines = vfp;
}

/**
 * @brief Set horizontal timing parameters of video mode
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param hsw Horizontal Synchronization Width, in lane byte clock cycles
 * @param hbp Horizontal Back Porch period, in lane byte clock cycles
 * @param active_width Horizontal active width, in lane byte clock cycles
 * @param hfp Horizontal Front Porch period, in lane byte clock cycles
 */
static inline void mipi_dsi_host_ll_dpi_set_horizontal_timing(dsi_host_dev_t *dev, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    dev->vid_hsa_time.vid_hsa_time = hsw;
    dev->vid_hbp_time.vid_hbp_time = hbp;
    // the vid_hline_time here is the overall time for each video line
    dev->vid_hline_time.vid_hline_time = active_width + hsw + hbp + hfp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Enable the tearing effect acknowledge
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_te_ack(dsi_host_dev_t *dev, bool en)
{
    dev->cmd_mode_cfg.tear_fx_en = en;
}

/**
 * @brief Enable to request an acknowledgement from the DSI device after sending a command or data packet
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_cmd_ack(dsi_host_dev_t *dev, bool enable)
{
    dev->cmd_mode_cfg.ack_rqst_en = enable;
}

/**
 * @brief Set the speed mode when transmitting DCS short write commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param num_of_params Number of parameters in the DCS command
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_dcs_short_wr_speed_mode(dsi_host_dev_t *dev, uint8_t num_of_params, mipi_dsi_ll_trans_speed_mode_t speed)
{
    switch (num_of_params) {
    case 0: // DCS short write command with no parameter
        dev->cmd_mode_cfg.dcs_sw_0p_tx = speed;
        break;
    case 1: // DCS short write command with one parameter
        dev->cmd_mode_cfg.dcs_sw_1p_tx = speed;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Set the speed mode when transmitting DCS long write commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_dcs_long_wr_speed_mode(dsi_host_dev_t *dev, mipi_dsi_ll_trans_speed_mode_t speed)
{
    dev->cmd_mode_cfg.dcs_lw_tx = speed;
}

/**
 * @brief Set the speed mode when transmitting DCS read commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param num_of_params Number of parameters in the DCS command
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_dcs_short_rd_speed_mode(dsi_host_dev_t *dev, uint8_t num_of_params, mipi_dsi_ll_trans_speed_mode_t speed)
{
    switch (num_of_params) {
    case 0: // DCS short read with zero parameter
        dev->cmd_mode_cfg.dcs_sr_0p_tx = speed;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set the speed mode when transmitting generic short write commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param num_of_params Number of parameters in the generic command
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_gen_short_wr_speed_mode(dsi_host_dev_t *dev, uint8_t num_of_params, mipi_dsi_ll_trans_speed_mode_t speed)
{
    switch (num_of_params) {
    case 0: // Generic short write command with no parameter
        dev->cmd_mode_cfg.gen_sw_0p_tx = speed;
        break;
    case 1: // Generic short write command with one parameter
        dev->cmd_mode_cfg.gen_sw_1p_tx = speed;
        break;
    case 2: // Generic short write command with two parameters
        dev->cmd_mode_cfg.gen_sw_2p_tx = speed;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Set the speed mode when transmitting generic long write commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_gen_long_wr_speed_mode(dsi_host_dev_t *dev, mipi_dsi_ll_trans_speed_mode_t speed)
{
    dev->cmd_mode_cfg.gen_lw_tx = speed;
}

/**
 * @brief Set the speed mode when transmitting generic short read commands
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param num_of_params Number of parameters in the generic command
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_gen_short_rd_speed_mode(dsi_host_dev_t *dev, uint8_t num_of_params, mipi_dsi_ll_trans_speed_mode_t speed)
{
    switch (num_of_params) {
    case 0: // Generic short read command with zero parameter
        dev->cmd_mode_cfg.gen_sr_0p_tx = speed;
        break;
    case 1: // Generic short read command with one parameter
        dev->cmd_mode_cfg.gen_sr_1p_tx = speed;
        break;
    case 2: // Generic short read command with two parameters
        dev->cmd_mode_cfg.gen_sr_2p_tx = speed;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set the speed mode for the "Maximum Return Packet Size" command
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param speed Speed mode
 */
static inline void mipi_dsi_host_ll_set_mrps_speed_mode(dsi_host_dev_t *dev, mipi_dsi_ll_trans_speed_mode_t speed)
{
    dev->cmd_mode_cfg.max_rd_pkt_size = speed;
}

/**
 * @brief Enable receive EoT packet
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_rx_eotp(dsi_host_dev_t *dev, bool en)
{
    dev->pckhdl_cfg.eotp_rx_en = en;
}

/**
 * @brief Enable transmit EoT packet
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable_in_hs_mode True to enable, False to disable
 * @param enable_in_lp_mode True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_tx_eotp(dsi_host_dev_t *dev, bool enable_in_hs_mode, bool enable_in_lp_mode)
{
    dev->pckhdl_cfg.eotp_tx_en = enable_in_hs_mode;
    dev->pckhdl_cfg.eotp_tx_lp_en = enable_in_lp_mode;
}

/**
 * @brief Enable the CRC check for the received packets
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_rx_crc(dsi_host_dev_t *dev, bool enable)
{
    dev->pckhdl_cfg.crc_rx_en = enable;
}

/**
 * @brief Enable the ECC check for the received packets
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_rx_ecc(dsi_host_dev_t *dev, bool enable)
{
    dev->pckhdl_cfg.ecc_rx_en = enable;
}

/**
 * @brief Enable the Bus Turn Around (BTA) request
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_host_ll_enable_bta(dsi_host_dev_t *dev, bool enable)
{
    dev->pckhdl_cfg.bta_en = enable;
}

/**
 * @brief Set the timing for low power commands sent while in video mode
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param in_vact largest packet size during VACT period, in bytes
 * @param out_vact largest packet size during non-VACT period (VSA,VBP,VFP), in bytes
 */
static inline void mipi_dsi_host_ll_set_lp_largest_cmd_packet_size(dsi_host_dev_t *dev, uint32_t in_vact, uint32_t out_vact)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->dpi_lp_cmd_tim, invact_lpcmd_time, in_vact);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->dpi_lp_cmd_tim, outvact_lpcmd_time, out_vact);
}

////////////////////////////////////////////Generic Interface///////////////////////////////

/**
 * @brief Set the header for new packets sent using the Generic interface
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param dt Data type
 * @param vc_id Virtual channel ID
 * @param ms_byte most significant byte of the word count for long packets or data1 for short packets
 * @param ls_byte least significant byte of the word count for long packets or data0 for short packets
 */
static inline void mipi_dsi_host_ll_gen_set_packet_header(dsi_host_dev_t *dev, uint8_t vc_id,
                                                          mipi_dsi_data_type_t dt, uint8_t ms_byte, uint8_t ls_byte)
{
    dev->gen_hdr.val = (ms_byte << 16) | (ls_byte << 8) | ((vc_id << 6) | dt);
}

/**
 * @brief Set the payload for packets sent using the generic interface
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param payload Payload data
 */
static inline void mipi_dsi_host_ll_gen_write_payload_fifo(dsi_host_dev_t *dev, uint32_t payload)
{
    dev->gen_pld_data.val = payload;
}

/**
 * @brief When using the generic interface, return the contents of READ responses
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return payload data
 */
static inline uint32_t mipi_dsi_host_ll_gen_read_payload_fifo(dsi_host_dev_t *dev)
{
    return dev->gen_pld_data.val;
}

/**
 * @brief Is the read command of the generic interface busy?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if busy, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_read_cmd_busy(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_rd_cmd_busy;
}

/**
 * @brief Is the read payload FIFO of the generic interface full?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if full, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_read_fifo_full(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_pld_r_full;
}

/**
 * @brief Is the read payload FIFO of the generic interface empty?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if empty, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_read_fifo_empty(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_pld_r_empty;
}

/**
 * @brief Is the write payload FIFO of generic interface full?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if full, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_write_fifo_full(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_pld_w_full;
}

/**
 * @brief Is the write payload FIFO of generic interface empty?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if empty, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_write_fifo_empty(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_pld_w_empty;
}

/**
 * @brief Is the command FIFO of generic interface full?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if full, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_cmd_fifo_full(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_cmd_full;
}

/**
 * @brief Is the command FIFO of generic interface empty?
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if empty, False if not
 */
static inline bool mipi_dsi_host_ll_gen_is_cmd_fifo_empty(dsi_host_dev_t *dev)
{
    return dev->cmd_pkt_status.gen_cmd_empty;
}

/**
 * @brief Set the ID of the virtual channel that for generic reading back
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param vcid Virtual channel ID
 */
static inline void mipi_dsi_host_ll_gen_set_rx_vcid(dsi_host_dev_t *dev, uint32_t vcid)
{
    dev->gen_vcid.gen_vcid_rx = vcid;
}

/**
 * @brief Set the ID of the virtual channel that for tear effect
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param vcid Virtual channel ID
 */
static inline void mipi_dsi_host_ll_gen_set_te_vcid(dsi_host_dev_t *dev, uint32_t vcid)
{
    dev->gen_vcid.gen_vcid_tear_auto = vcid;
}

/**
 * @brief Set the ID of the virtual channel that for automatically transmitting generic packets
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param vcid Virtual channel ID
 */
static inline void mipi_dsi_host_ll_gen_set_tx_vcid(dsi_host_dev_t *dev, uint32_t vcid)
{
    dev->gen_vcid.gen_vcid_tx_auto = vcid;
}

#ifdef __cplusplus

}
#endif
