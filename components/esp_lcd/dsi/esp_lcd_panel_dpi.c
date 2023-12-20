/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_clk_tree.h"
#include "esp_cache.h"
#include "mipi_dsi_priv.h"
#include "esp_private/dw_gdma.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

static const char *TAG = "lcd.dsi.dpi";

typedef struct esp_lcd_dpi_panel_t esp_lcd_dpi_panel_t;

static esp_err_t dpi_panel_del(esp_lcd_panel_t *panel);
static esp_err_t dpi_panel_init(esp_lcd_panel_t *panel);
static esp_err_t dpi_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);

struct esp_lcd_dpi_panel_t {
    esp_lcd_panel_t base;         // Base class of generic lcd panel
    esp_lcd_dsi_bus_handle_t bus; // DSI bus handle
    uint8_t virtual_channel;      // Virtual channel ID, index from 0
    void *frame_buffer;           // Frame buffer
    uint32_t h_pixels;            // Horizontal pixels
    uint32_t v_pixels;            // Vertical pixels
    size_t frame_buffer_size;     // Frame buffer size
    size_t bytes_per_pixel;       // Bytes per pixel
    dw_gdma_channel_handle_t dma_chan;    // DMA channel
    dw_gdma_link_list_handle_t link_list; // DMA link list
};

static bool dma_list_invalid_block_cb(dw_gdma_channel_handle_t chan, const dw_gdma_break_event_data_t *event_data, void *user_data)
{
    dw_gdma_lli_handle_t lli = event_data->invalid_lli;
    dw_gdma_block_markers_t markers = {
        .is_valid = true, // mark the block as valid so that the DMA can continue the transfer
    };
    dw_gdma_lli_set_block_markers(lli, markers);
    // after the item is marked as valid again, tell the DMA to continue the transfer
    dw_gdma_channel_continue(chan);
    return false;
}

static esp_err_t dpi_panel_create_dma_link(esp_lcd_dpi_panel_t *dpi_panel)
{
    esp_err_t ret = ESP_OK;
    dw_gdma_channel_handle_t dma_chan = NULL;
    dw_gdma_link_list_handle_t link_list = NULL;
    // sending image stream from memory to the DSI bridge
    dw_gdma_channel_alloc_config_t dma_alloc_config = {
        .src = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_LIST,
            .role = DW_GDMA_ROLE_MEM,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 5,
        },
        .dst = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_LIST,
            .role = DW_GDMA_ROLE_PERIPH_DSI,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 2,
        },
        .flow_controller = DW_GDMA_FLOW_CTRL_DST, // the DSI bridge as the DMA flow controller
        .chan_priority = 1,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_channel(&dma_alloc_config, &dma_chan), err, TAG, "create DMA channel failed");

    // create a linked list for the DMA channel
    dw_gdma_link_list_config_t link_list_config = {
        .num_items = 1, // Assume one link item can carry the whole image
        .link_type = DW_GDMA_LINKED_LIST_TYPE_CIRCULAR,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_link_list(&link_list_config, &link_list), err, TAG, "create DMA link list failed");

    // register DMA ISR callbacks
    dw_gdma_event_callbacks_t dsi_dma_cbs = {
        .on_invalid_block = dma_list_invalid_block_cb,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_channel_register_event_callbacks(dma_chan, &dsi_dma_cbs, NULL), err, TAG, "register DMA callbacks failed");

    dpi_panel->dma_chan = dma_chan;
    dpi_panel->link_list = link_list;
    return ESP_OK;
err:
    if (dma_chan) {
        dw_gdma_del_channel(dma_chan);
    }
    if (link_list) {
        dw_gdma_del_link_list(link_list);
    }
    return ret;
}

esp_err_t esp_lcd_new_panel_dpi(esp_lcd_dsi_bus_handle_t bus, const esp_lcd_dpi_panel_config_t *panel_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    void *frame_buffer = NULL;
    esp_lcd_dpi_panel_t *dpi_panel = NULL;
    ESP_RETURN_ON_FALSE(bus && panel_config && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(panel_config->virtual_channel < 4, ESP_ERR_INVALID_ARG, TAG, "invalid virtual channel %d", panel_config->virtual_channel);
    ESP_RETURN_ON_FALSE(panel_config->dpi_clock_freq_mhz, ESP_ERR_INVALID_ARG, TAG, "invalid DPI clock frequency %"PRIu32, panel_config->dpi_clock_freq_mhz);
    int bus_id = bus->bus_id;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    dpi_panel = heap_caps_calloc(1, sizeof(esp_lcd_dpi_panel_t), DSI_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(dpi_panel, ESP_ERR_NO_MEM, err, TAG, "no memory for DPI panel");
    dpi_panel->virtual_channel = panel_config->virtual_channel;
    dpi_panel->bus = bus;

    // allocate frame buffer from PSRAM
    size_t bytes_per_pixel = 0;
    switch (panel_config->pixel_format) {
    case LCD_COLOR_PIXEL_FORMAT_RGB565:
        bytes_per_pixel = 2;
        break;
    case LCD_COLOR_PIXEL_FORMAT_RGB666:
        bytes_per_pixel = 3;
        break;
    case LCD_COLOR_PIXEL_FORMAT_RGB888:
        bytes_per_pixel = 3;
        break;
    }
    uint32_t cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    // DMA doesn't have requirement on the buffer alignment, but the cache does
    uint32_t alignment = cache_line_size;
    size_t frame_buffer_size = panel_config->video_timing.h_size * panel_config->video_timing.v_size * bytes_per_pixel;
    frame_buffer = heap_caps_aligned_calloc(alignment, 1, frame_buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(frame_buffer, ESP_ERR_NO_MEM, err, TAG, "no memory for frame buffer");
    dpi_panel->frame_buffer = frame_buffer;
    dpi_panel->frame_buffer_size = frame_buffer_size;
    dpi_panel->bytes_per_pixel = bytes_per_pixel;
    dpi_panel->h_pixels = panel_config->video_timing.h_size;
    dpi_panel->v_pixels = panel_config->video_timing.v_size;
    // preset the frame buffer with black color
    ESP_GOTO_ON_ERROR(esp_cache_msync(frame_buffer, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M), err, TAG, "cache write back failed");

    // if the clock source is not assigned, fallback to the default clock source
    mipi_dsi_dpi_clock_source_t dpi_clk_src = panel_config->dpi_clk_src;
    if (dpi_clk_src == 0) {
        dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT;
    }
    // get the clock source frequency
    uint32_t dpi_clk_src_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(dpi_clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                                   &dpi_clk_src_freq_hz), err, TAG, "get clock source frequency failed");
    // divide the source clock to get the final DPI clock
    uint32_t dpi_div = mipi_dsi_hal_host_dpi_calculate_divider(hal, dpi_clk_src_freq_hz / 1000 / 1000, panel_config->dpi_clock_freq_mhz);
    // set the clock source, set the divider, and enable the dpi clock
    DSI_CLOCK_SRC_ATOMIC() {
        mipi_dsi_ll_set_dpi_clock_source(bus_id, dpi_clk_src);
        mipi_dsi_ll_set_dpi_clock_div(bus_id, dpi_div);
        mipi_dsi_ll_enable_dpi_clock(bus_id, true);
    }

    // create DMA resources
    ESP_GOTO_ON_ERROR(dpi_panel_create_dma_link(dpi_panel), err, TAG, "initialize DMA link failed");

    mipi_dsi_host_ll_dpi_set_vcid(hal->host, panel_config->virtual_channel);
    mipi_dsi_hal_host_dpi_set_color_coding(hal, panel_config->pixel_format, 0);
    // these signals define how the DPI interface interacts with the controller
    mipi_dsi_host_ll_dpi_set_timing_polarity(hal->host, false, false, false, false, false);
    // configure the low-power transitions: defines the video periods which are permitted to goto low-power if the time available to do so
    mipi_dsi_host_ll_dpi_enable_lp_horizontal_timing(hal->host, true, true);
    mipi_dsi_host_ll_dpi_enable_lp_vertical_timing(hal->host, true, true, true, true);
    // after sending a frame, the DSI device should return an ack
    mipi_dsi_host_ll_dpi_enable_frame_ack(hal->host, true);
    // commands are transmitted in low-power mode
    mipi_dsi_host_ll_dpi_enable_lp_command(hal->host, true);
    // using the burst mode because it's energy-efficient
    mipi_dsi_host_ll_dpi_set_video_burst_type(hal->host, MIPI_DSI_LL_VIDEO_BURST_WITH_SYNC_PULSES);
    // configure the size of the active lin period, measured in pixels
    mipi_dsi_host_ll_dpi_set_video_packet_pixel_num(hal->host, panel_config->video_timing.h_size);
    // disable multi-packets
    mipi_dsi_host_ll_dpi_set_trunks_num(hal->host, 0);
    // disable "null packets"
    mipi_dsi_host_ll_dpi_set_null_packet_size(hal->host, 0);
    // set horizontal and vertical timing configuration
    mipi_dsi_hal_host_dpi_set_horizontal_timing(hal, panel_config->video_timing.hsync_pulse_width,
                                                panel_config->video_timing.hsync_back_porch,
                                                panel_config->video_timing.h_size,
                                                panel_config->video_timing.hsync_front_porch);
    mipi_dsi_hal_host_dpi_set_vertical_timing(hal, panel_config->video_timing.vsync_pulse_width,
                                              panel_config->video_timing.vsync_back_porch,
                                              panel_config->video_timing.v_size,
                                              panel_config->video_timing.vsync_front_porch);
    mipi_dsi_brg_ll_set_num_pixel_bits(hal->bridge, panel_config->video_timing.h_size * panel_config->video_timing.v_size * 16);
    mipi_dsi_brg_ll_set_underrun_discard_count(hal->bridge, panel_config->video_timing.h_size);
    // let the DSI bridge as the DMA flow controller
    mipi_dsi_brg_ll_set_flow_controller(hal->bridge, MIPI_DSI_LL_FLOW_CONTROLLER_BRIDGE);
    mipi_dsi_brg_ll_set_burst_len(hal->bridge, 256);
    mipi_dsi_brg_ll_set_empty_threshold(hal->bridge, 1024 - 256);
    // enable DSI bridge
    mipi_dsi_brg_ll_enable(hal->bridge, true);
    mipi_dsi_brg_ll_update_dpi_config(hal->bridge);

    dpi_panel->base.del = dpi_panel_del;
    dpi_panel->base.init = dpi_panel_init;
    dpi_panel->base.draw_bitmap = dpi_panel_draw_bitmap;
    *ret_panel = &dpi_panel->base;
    ESP_LOGD(TAG, "new dpi panel @%p, fb@%p", dpi_panel, dpi_panel->frame_buffer);
    return ESP_OK;
err:
    if (dpi_panel) {
        dpi_panel_del(&dpi_panel->base);
    }
    if (frame_buffer) {
        free(frame_buffer);
    }
    return ret;
}

static esp_err_t dpi_panel_del(esp_lcd_panel_t *panel)
{
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    esp_lcd_dsi_bus_handle_t bus = dpi_panel->bus;
    int bus_id = bus->bus_id;
    mipi_dsi_hal_context_t *hal = &bus->hal;
    // disable the DPI clock
    DSI_CLOCK_SRC_ATOMIC() {
        mipi_dsi_ll_enable_dpi_clock(bus_id, false);
    }
    // disable the DSI bridge
    mipi_dsi_brg_ll_enable(hal->bridge, false);
    if (dpi_panel->dma_chan) {
        dw_gdma_del_channel(dpi_panel->dma_chan);
    }
    if (dpi_panel->frame_buffer) {
        free(dpi_panel->frame_buffer);
    }
    if (dpi_panel->link_list) {
        dw_gdma_del_link_list(dpi_panel->link_list);
    }
    free(dpi_panel);
    return ESP_OK;
}

static esp_err_t dpi_panel_init(esp_lcd_panel_t *panel)
{
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    esp_lcd_dsi_bus_handle_t bus = dpi_panel->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;
    dw_gdma_channel_handle_t dma_chan = dpi_panel->dma_chan;
    dw_gdma_link_list_handle_t link_list = dpi_panel->link_list;

    ESP_RETURN_ON_ERROR(dw_gdma_channel_use_link_list(dma_chan, link_list), TAG, "use DMA link list failed");
    dw_gdma_block_transfer_config_t dma_transfer_config = {
        .src = {
            .addr = (uint32_t)(dpi_panel->frame_buffer),
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .dst = {
            .addr = MIPI_DSI_MEM_BASE,
            .burst_mode = DW_GDMA_BURST_MODE_FIXED,
            .burst_items = DW_GDMA_BURST_ITEMS_256,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .size = dpi_panel->frame_buffer_size * 8 / 64,
    };
    dw_gdma_lli_config_transfer(dw_gdma_link_list_get_item(link_list, 0), &dma_transfer_config);
    dw_gdma_block_markers_t markers = {
        .is_valid = true,
    };
    dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 0), markers);
    dw_gdma_channel_enable_ctrl(dma_chan, true);

    // enable the video mode
    mipi_dsi_host_ll_enable_video_mode(hal->host, true);
    // switch the clock lane to high speed mode
    mipi_dsi_host_ll_set_clock_lane_state(hal->host, MIPI_DSI_LL_CLOCK_LANE_STATE_AUTO);

    // enable the DPI output of the DSI bridge
    mipi_dsi_brg_ll_enable_dpi_output(hal->bridge, true);
    mipi_dsi_brg_ll_update_dpi_config(hal->bridge);

    return ESP_OK;
}

static esp_err_t dpi_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    uint8_t *frame_buffer = dpi_panel->frame_buffer;
    size_t frame_buffer_size = dpi_panel->frame_buffer_size;

    // TODO: memory copy by 2D-DMA
    size_t bytes_per_pixel = dpi_panel->bytes_per_pixel;
    const uint8_t *from = (const uint8_t *)color_data;
    uint8_t *to = frame_buffer + (y_start * dpi_panel->h_pixels + x_start) * bytes_per_pixel;
    uint32_t copy_bytes_per_line = (x_end - x_start) * bytes_per_pixel;
    uint32_t bytes_per_line = bytes_per_pixel * dpi_panel->h_pixels;
    for (int y = y_start; y < y_end; y++) {
        memcpy(to, from, copy_bytes_per_line);
        to += bytes_per_line;
        from += copy_bytes_per_line;
    }
    ESP_RETURN_ON_ERROR(esp_cache_msync(frame_buffer, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M), TAG, "cache write back failed");

    return ESP_OK;
}

esp_err_t esp_lcd_dpi_panel_set_pattern(esp_lcd_panel_handle_t panel, mipi_dsi_pattern_type_t pattern)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    esp_lcd_dsi_bus_handle_t bus = dpi_panel->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    if (pattern != MIPI_DSI_PATTERN_NONE) {
        // stop the DSI bridge from generating the DPI stream
        mipi_dsi_brg_ll_enable_dpi_output(hal->bridge, false);
        mipi_dsi_brg_ll_update_dpi_config(hal->bridge);
    }

    // set the pattern type and enable the pattern generator for the DSI host controller
    mipi_dsi_host_ll_dpi_set_pattern_type(hal->host, pattern);

    if (pattern == MIPI_DSI_PATTERN_NONE) {
        // reenable the DSI bridge to generate the DPI stream
        mipi_dsi_brg_ll_enable_dpi_output(hal->bridge, true);
        mipi_dsi_brg_ll_update_dpi_config(hal->bridge);
    }

    return ESP_OK;
}
