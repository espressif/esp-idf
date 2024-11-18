/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/param.h>
#include "hal/mipi_dsi_hal.h"
#include "hal/mipi_dsi_ll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "soc/mipi_dsi_periph.h"

void mipi_dsi_hal_init(mipi_dsi_hal_context_t *hal, const mipi_dsi_hal_config_t *config)
{
    hal->host = MIPI_DSI_LL_GET_HOST(config->bus_id);
    hal->bridge = MIPI_DSI_LL_GET_BRG(config->bus_id);
    // set the data lane number
    mipi_dsi_phy_ll_set_data_lane_number(hal->host, config->num_data_lanes);
    // power on the host controller and PHY
    mipi_dsi_host_ll_power_on_off(hal->host, true);
    mipi_dsi_phy_ll_power_on_off(hal->host, true);
    // reset the PHY and then enable the clock lane
    mipi_dsi_phy_ll_reset(hal->host);
    mipi_dsi_phy_ll_enable_clock_lane(hal->host, true);
    mipi_dsi_phy_ll_force_pll(hal->host, true);
}

void mipi_dsi_hal_deinit(mipi_dsi_hal_context_t *hal)
{
    // power off the host controller and PHY
    mipi_dsi_phy_ll_power_on_off(hal->host, false);
    mipi_dsi_host_ll_power_on_off(hal->host, false);
    hal->host = NULL;
    hal->bridge = NULL;
}

void mipi_dsi_hal_configure_phy_pll(mipi_dsi_hal_context_t *hal, uint32_t phy_clk_src_freq_hz, uint32_t lane_bit_rate_mbps)
{
    // Formula: f_vco = M/N * f_ref
    // where the M is Feedback Multiplication Ratio, N is Input Frequency Division Ratio
    uint32_t ref_freq_mhz = phy_clk_src_freq_hz / 1000 / 1000;
    uint32_t vco_freq_mhz = lane_bit_rate_mbps;
    uint8_t pll_N = 1;
    uint16_t pll_M = 0;
    // 5MHz <= f_ref/N <= 40MHz
    uint8_t min_N = MAX(1, ref_freq_mhz / 40);
    uint8_t max_N = ref_freq_mhz / 5;
    for (uint8_t n = min_N; n <= max_N; n++) {
        uint16_t m = vco_freq_mhz * n / ref_freq_mhz;
        // M must be even number
        if ((m & 0x01) == 0) {
            pll_M = m;
            pll_N = n;
            break;
        }
    }
    HAL_ASSERT(pll_M && pll_N);

    // search for the best PLL range
    uint8_t hs_freq_sel = 0;
    for (size_t i = 0; i < num_of_soc_mipi_dsi_phy_pll_ranges; i++) {
        if (lane_bit_rate_mbps >= soc_mipi_dsi_phy_pll_ranges[i].start_mbps &&
                lane_bit_rate_mbps <= soc_mipi_dsi_phy_pll_ranges[i].end_mbps) {
            hs_freq_sel = soc_mipi_dsi_phy_pll_ranges[i].hs_freq_range_sel;
            break;
        }
    }

    mipi_dsi_hal_phy_write_register(hal, 0x44, hs_freq_sel << 1);
    // make use of the N and M factors that configured in the 0x17 and 0x18
    mipi_dsi_hal_phy_write_register(hal, 0x19, 0x30);
    mipi_dsi_hal_phy_write_register(hal, 0x17, pll_N - 1);
    mipi_dsi_hal_phy_write_register(hal, 0x18, ((pll_M - 1) & 0x1F));
    mipi_dsi_hal_phy_write_register(hal, 0x18, 0x80 | (((pll_M - 1) >> 5) & 0x0F));
    // update the real lane bit rate
    hal->lane_bit_rate_mbps = ref_freq_mhz * pll_M / pll_N;
    HAL_LOGD("dsi_hal", "phy pll: ref=%" PRIu32 "Hz, lane_bit_rate=%" PRIu32 "Mbps, M=%" PRId16 ", N=%" PRId8 ", hsfreqrange=%" PRId8,
             phy_clk_src_freq_hz, hal->lane_bit_rate_mbps, pll_M, pll_N, hs_freq_sel);
}

void mipi_dsi_hal_phy_write_register(mipi_dsi_hal_context_t *hal, uint8_t reg_addr, uint8_t reg_val)
{
    // disable the test clear pin, enable the interface to write values to the PHY internal registers
    mipi_dsi_phy_ll_write_clock(hal->host, 0, false);
    // load PHY register address
    mipi_dsi_phy_ll_write_reg_addr(hal->host, reg_addr);
    // the address write operation is set on the falling edge of the test clock
    mipi_dsi_phy_ll_write_clock(hal->host, 1, false);
    mipi_dsi_phy_ll_write_clock(hal->host, 0, false);
    // load PHY register value
    mipi_dsi_phy_ll_write_reg_val(hal->host, reg_val);
    // the data write operation is set on the rising edge of the test clock
    mipi_dsi_phy_ll_write_clock(hal->host, 1, false);
    mipi_dsi_phy_ll_write_clock(hal->host, 0, false);
}

void mipi_dsi_hal_host_gen_write_dcs_command(mipi_dsi_hal_context_t *hal, uint8_t vc,
                                             uint32_t command, uint32_t command_bytes, const void *param, uint16_t param_size)
{
    mipi_dsi_data_type_t dt = 0;
    uint8_t pkt_hdr_msb = 0;
    uint8_t pkt_hdr_lsb = 0;
    const uint8_t *payload = param;
    // the payload size is the command size plus the parameter size
    uint32_t payload_size = command_bytes + param_size;

    // merge the command and some bytes of parameters into one 32-bit word
    uint32_t temp = command & ((1 << (8 * command_bytes)) - 1);
    uint16_t merged_size = MIN(4 - command_bytes, param_size);
    for (int i = 0; i < merged_size; i++) {
        temp |= payload[i] << (8 * (i + command_bytes));
    }

    if (payload_size > 2) {
        // write the first 32-bit word into FIFO
        while (mipi_dsi_host_ll_gen_is_write_fifo_full(hal->host));
        mipi_dsi_host_ll_gen_write_payload_fifo(hal->host, temp);

        // write the remaining parameters into FIFO
        payload += merged_size;
        uint32_t remain_size = param_size - merged_size;
        while (remain_size >= 4) {
            temp = *(uint32_t *)payload;
            while (mipi_dsi_host_ll_gen_is_write_fifo_full(hal->host));
            mipi_dsi_host_ll_gen_write_payload_fifo(hal->host, temp);
            payload += 4;
            remain_size -= 4;
        }
        if (remain_size) {
            temp = *(uint32_t *)payload;
            temp &= (1 << (8 * remain_size)) - 1;
            while (mipi_dsi_host_ll_gen_is_write_fifo_full(hal->host));
            mipi_dsi_host_ll_gen_write_payload_fifo(hal->host, temp);
        }

        dt = MIPI_DSI_DT_DCS_LONG_WRITE;
        pkt_hdr_msb = (payload_size >> 8) & 0xFF;
        pkt_hdr_lsb = payload_size & 0xFF;
    } else if (payload_size == 2) {
        dt = MIPI_DSI_DT_DCS_SHORT_WRITE_1;
        pkt_hdr_msb = (temp >> 8) & 0xFF;
        pkt_hdr_lsb = temp & 0xFF;
    } else if (payload_size == 1) {
        dt = MIPI_DSI_DT_DCS_SHORT_WRITE_0;
        pkt_hdr_msb = (temp >> 8) & 0xFF;
        pkt_hdr_lsb = temp & 0xFF;
    }

    // write the packet header
    while (mipi_dsi_host_ll_gen_is_cmd_fifo_full(hal->host));
    mipi_dsi_host_ll_gen_set_packet_header(hal->host, vc, dt, pkt_hdr_msb, pkt_hdr_lsb);
}

void mipi_dsi_hal_host_gen_write_short_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, uint16_t header_data)
{
    uint8_t msb = (header_data >> 8) & 0xFF;
    uint8_t lsb = header_data & 0xFF;
    while (mipi_dsi_host_ll_gen_is_cmd_fifo_full(hal->host));
    mipi_dsi_host_ll_gen_set_packet_header(hal->host, vc, dt, msb, lsb);
}

void mipi_dsi_hal_host_gen_write_long_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, const void *buffer, uint16_t buffer_size)
{
    const uint8_t *payload = buffer;
    uint32_t remain_size = buffer_size;
    uint32_t temp = 0;
    while (remain_size >= 4) {
        temp = *(uint32_t *)payload;
        while (mipi_dsi_host_ll_gen_is_write_fifo_full(hal->host));
        mipi_dsi_host_ll_gen_write_payload_fifo(hal->host, temp);
        payload += 4;
        remain_size -= 4;
    }
    if (remain_size) {
        temp = *(uint32_t *)payload;
        temp &= (1 << (8 * remain_size)) - 1;
        while (mipi_dsi_host_ll_gen_is_write_fifo_full(hal->host));
        mipi_dsi_host_ll_gen_write_payload_fifo(hal->host, temp);
    }

    uint8_t wc_msb = (buffer_size >> 8) & 0xFF;
    uint8_t wc_lsb = buffer_size & 0xFF;
    while (mipi_dsi_host_ll_gen_is_cmd_fifo_full(hal->host));
    mipi_dsi_host_ll_gen_set_packet_header(hal->host, vc, dt, wc_msb, wc_lsb);
}

void mipi_dsi_hal_host_gen_read_short_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, uint16_t header_data, void *ret_buffer, uint16_t buffer_size)
{
    uint8_t *receive_buffer = (uint8_t *)ret_buffer;
    // set the maximum returned data size, it should equal to the parameter size of the read command
    mipi_dsi_hal_host_gen_write_short_packet(hal, vc, MIPI_DSI_DT_SET_MAXIMUM_RETURN_PKT, buffer_size);
    // make sure command mode is on
    mipi_dsi_host_ll_enable_video_mode(hal->host, false);
    // make sure receiving is enabled
    mipi_dsi_host_ll_enable_bta(hal->host, true);
    // listen to the same virtual channel as the one sent to
    mipi_dsi_host_ll_gen_set_rx_vcid(hal->host, vc);
    mipi_dsi_hal_host_gen_write_short_packet(hal, vc, dt, header_data);
    while (mipi_dsi_host_ll_gen_is_read_cmd_busy(hal->host));
    // wait data to come into the fifo
    while (mipi_dsi_host_ll_gen_is_read_fifo_empty(hal->host));
    uint32_t temp = 0;
    uint32_t counter = 0;
    while (!mipi_dsi_host_ll_gen_is_read_fifo_empty(hal->host)) {
        temp = mipi_dsi_host_ll_gen_read_payload_fifo(hal->host);
        for (int i = 0; i < 4; i++) {
            if (counter < buffer_size) {
                receive_buffer[counter] = (temp >> (8 * i)) & 0xFF;
                counter++;
            }
        }
    }
}

void mipi_dsi_hal_host_gen_read_dcs_command(mipi_dsi_hal_context_t *hal, uint8_t vc, uint32_t command, uint32_t command_bytes, void *ret_param, uint16_t param_buf_size)
{
    uint16_t header_data = command & ((1 << (8 * command_bytes)) - 1);
    mipi_dsi_hal_host_gen_read_short_packet(hal, vc, MIPI_DSI_DT_DCS_READ_0, header_data, ret_param, param_buf_size);
}

void mipi_dsi_hal_host_dpi_set_color_coding(mipi_dsi_hal_context_t *hal, lcd_color_format_t color_coding, uint32_t sub_config)
{
    mipi_dsi_host_ll_dpi_set_color_coding(hal->host, color_coding, sub_config);
    mipi_dsi_brg_ll_set_pixel_format(hal->bridge, color_coding, sub_config);
}

void mipi_dsi_hal_host_dpi_set_horizontal_timing(mipi_dsi_hal_context_t *hal, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    float dpi2lane_clk_ratio = (float)hal->lane_bit_rate_mbps / hal->dpi_clock_freq_mhz / 8;
    mipi_dsi_host_ll_dpi_set_horizontal_timing(hal->host,
                                               hsw * dpi2lane_clk_ratio,
                                               hbp * dpi2lane_clk_ratio,
                                               active_width * dpi2lane_clk_ratio,
                                               hfp * dpi2lane_clk_ratio);
    mipi_dsi_brg_ll_set_horizontal_timing(hal->bridge, hsw, hbp, active_width, hfp);
}

void mipi_dsi_hal_host_dpi_set_vertical_timing(mipi_dsi_hal_context_t *hal, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    mipi_dsi_host_ll_dpi_set_vertical_timing(hal->host, vsw, vbp, active_height, vfp);
    mipi_dsi_brg_ll_set_vertical_timing(hal->bridge, vsw, vbp, active_height, vfp);
}

uint32_t mipi_dsi_hal_host_dpi_calculate_divider(mipi_dsi_hal_context_t *hal, uint32_t clk_src_mhz, uint32_t expect_dpi_clk_mhz)
{
    uint32_t div = clk_src_mhz / expect_dpi_clk_mhz;
    hal->dpi_clock_freq_mhz = clk_src_mhz / div;
    return div;
}
