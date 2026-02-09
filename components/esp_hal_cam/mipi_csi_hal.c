/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/log.h"
#include "hal/mipi_csi_hal.h"
#include "hal/mipi_csi_ll.h"
#include "hal/mipi_csi_periph.h"

#define MHZ (1000 * 1000)

HAL_LOG_ATTR_TAG(TAG, "csi_hal");

void s_mipi_csi_hal_phy_write_register(mipi_csi_hal_context_t *hal, uint8_t reg_addr, uint8_t reg_val)
{
    mipi_csi_phy_ll_write_clock(hal->host_dev, 0, false);
    mipi_csi_phy_ll_write_reg_addr(hal->host_dev, reg_addr);
    mipi_csi_phy_ll_write_clock(hal->host_dev, 1, false);
    mipi_csi_phy_ll_write_clock(hal->host_dev, 0, false);
    mipi_csi_phy_ll_write_reg_val(hal->host_dev, reg_val);
    mipi_csi_phy_ll_write_clock(hal->host_dev, 1, false);
    mipi_csi_phy_ll_write_clock(hal->host_dev, 0, false);
}

void mipi_csi_hal_init(mipi_csi_hal_context_t *hal, const mipi_csi_hal_config_t *config)
{
    hal->bridge_dev = MIPI_CSI_BRG_LL_GET_HW(0);
    hal->host_dev = MIPI_CSI_HOST_LL_GET_HW(0);
    int csi_lane_rate = config->lane_bit_rate_mbps * MHZ;

    // reset PHY
    mipi_csi_phy_ll_enable_shutdown_input(hal->host_dev, true);
    mipi_csi_phy_ll_enable_reset_output(hal->host_dev, true);
    mipi_csi_host_ll_enable_reset_output(hal->host_dev, true);
    //reset reg addr to default value
    mipi_csi_phy_ll_write_reg_addr(hal->host_dev, 0x0);
    //reset reg val to default value
    mipi_csi_phy_ll_write_clock(hal->host_dev, 0, 1);
    mipi_csi_phy_ll_write_clock(hal->host_dev, 0, 0);

    uint8_t hs_freq_sel = 0;
    for (size_t i = 0; i < num_of_soc_mipi_csi_phy_pll_ranges; i++) {
        if ((csi_lane_rate / 1000000) >= soc_mipi_csi_phy_pll_ranges[i].start_mbps &&
                (csi_lane_rate / 1000000) <= soc_mipi_csi_phy_pll_ranges[i].end_mbps) {
            hs_freq_sel = soc_mipi_csi_phy_pll_ranges[i].hs_freq_range_sel;
            break;
        }
    }
    s_mipi_csi_hal_phy_write_register(hal, 0x44, hs_freq_sel << 1);
    HAL_LOGD(TAG, "CSI-DPHY lane_rate: %d Hz, hs_freq: 0x%x, lane_num: 0x%x", csi_lane_rate, hs_freq_sel, config->lanes_num);
    mipi_csi_phy_ll_enable_shutdown_input(hal->host_dev, false);
    mipi_csi_phy_ll_enable_reset_output(hal->host_dev, false);
    mipi_csi_host_ll_enable_reset_output(hal->host_dev, false);

    // Configure the host controller.
    // Configure the number of active lanes.
    mipi_csi_host_ll_set_active_lanes_num(hal->host_dev, config->lanes_num);
    /**
     * We ignore the virtual channel and virtual channel extension info, there is only one virtual channel.
     * VC is by default there, VCX needs enable or disable.
     * So we disable VCX here.
     */
    mipi_csi_host_ll_enable_virtual_channel_extension(hal->host_dev, false);
    // Set Scrambler.
    mipi_csi_host_ll_enable_scrambling(hal->host_dev, false);

    //CSI bridge
    //TODO: IDF-9126, make csi bridge updates into `mipi_csi_share_hw_ctrl.c` and share with ISP
    mipi_csi_brg_ll_set_intput_data_h_pixel_num(hal->bridge_dev, config->frame_height);
    mipi_csi_brg_ll_set_intput_data_v_row_num(hal->bridge_dev, config->frame_width);
    mipi_csi_brg_ll_set_flow_ctl_buf_afull_thrd(hal->bridge_dev, 960);
    mipi_csi_brg_ll_set_data_type_min(hal->bridge_dev, 0x12);
    mipi_csi_brg_ll_set_data_type_max(hal->bridge_dev, 0x2f);
    mipi_csi_brg_ll_set_output_byte_endian(hal->bridge_dev, config->byte_swap_en);
}
