/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/param.h>
#include "hal/mipi_dsi_hal.h"
#include "hal/mipi_dsi_ll.h"

void mipi_dsi_hal_init(mipi_dsi_hal_context_t *hal, const mipi_dsi_hal_config_t *config)
{
    hal->host = &MIPI_DSI_HOST;
    hal->bridge = &MIPI_DSI_BRIDGE;
    hal->dpi2lane_clk_ratio = ((float)config->lane_byte_clk_hz) / config->dpi_clk_hz;
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
    const uint8_t *payload = param;
    // the payload size is the command size plus the parameter size
    uint32_t payload_size = command_bytes + param_size;

    // merge the command and some bytes of parameters into one 32-bit word
    uint32_t temp = command & ((1 << (8 * command_bytes)) - 1);
    uint16_t merged_size = MIN(4 - command_bytes, param_size);
    for (int i = 0; i < merged_size; i++) {
        temp |= payload[i] << (8 * (i + command_bytes));
    }
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

    uint8_t wc_msb = (payload_size >> 8) & 0xFF;
    uint8_t wc_lsb = payload_size & 0xFF;
    while (mipi_dsi_host_ll_gen_is_cmd_fifo_full(hal->host));
    mipi_dsi_host_ll_gen_set_packet_header(hal->host, vc, MIPI_DSI_DT_DCS_LONG_WRITE, wc_msb, wc_lsb);
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
    mipi_dsi_host_ll_dpi_enable_video_mode(hal->host, false);
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
            if ((counter + i) < buffer_size) {
                receive_buffer[counter + i] = (temp >> (8 * i)) & 0xFF;
            }
            counter++;
        }
    }
}

void mipi_dsi_hal_host_gen_read_dcs_command(mipi_dsi_hal_context_t *hal, uint8_t vc, uint32_t command, uint32_t command_bytes, void *ret_param, uint16_t param_buf_size)
{
    uint16_t header_data = command & ((1 << (8 * command_bytes)) - 1);
    mipi_dsi_hal_host_gen_read_short_packet(hal, vc, MIPI_DSI_DT_DCS_READ_0, header_data, ret_param, param_buf_size);
}

void mipi_dsi_hal_host_dpi_set_color_coding(mipi_dsi_hal_context_t *hal, lcd_color_rgb_pixel_format_t color_coding, uint32_t sub_config)
{
    mipi_dsi_host_ll_dpi_set_color_coding(hal->host, color_coding, sub_config);
    mipi_dsi_brg_ll_set_pixel_format(hal->bridge, color_coding, sub_config);
    // please note, we need to call bridge_update to make the new configuration take effect
}

void mipi_dsi_hal_host_dpi_set_horizontal_timing(mipi_dsi_hal_context_t *hal, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    mipi_dsi_host_ll_dpi_set_horizontal_timing(hal->host,
                                               hsw * hal->dpi2lane_clk_ratio,
                                               hbp * hal->dpi2lane_clk_ratio,
                                               active_width * hal->dpi2lane_clk_ratio,
                                               hfp * hal->dpi2lane_clk_ratio);
    mipi_dsi_brg_ll_set_horizontal_timing(hal->bridge, hsw, hbp, active_width, hfp);
    // please note, we need to call bridge_update to make the new configuration take effect
}

void mipi_dsi_hal_host_dpi_set_vertical_timing(mipi_dsi_hal_context_t *hal, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    mipi_dsi_host_ll_dpi_set_vertical_timing(hal->host, vsw, vbp, active_height, vfp);
    mipi_dsi_brg_ll_set_vertical_timing(hal->bridge, vsw, vbp, active_height, vfp);
    // please note, we need to call bridge_update to make the new configuration take effect
}
