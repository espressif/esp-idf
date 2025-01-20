/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_clk_tree.h"
#include "esp_cache.h"
#include "mipi_dsi_priv.h"
#include "esp_async_fbcpy.h"
#include "esp_memory_utils.h"
#include "esp_private/dw_gdma.h"
#include "esp_private/esp_clk_tree_common.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/color_hal.h"

static const char *TAG = "lcd.dsi.dpi";

typedef struct esp_lcd_dpi_panel_t esp_lcd_dpi_panel_t;

static esp_err_t dpi_panel_del(esp_lcd_panel_t *panel);
static esp_err_t dpi_panel_init(esp_lcd_panel_t *panel);
static esp_err_t dpi_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);

struct esp_lcd_dpi_panel_t {
    esp_lcd_panel_t base;         // Base class of generic lcd panel
    esp_lcd_dsi_bus_handle_t bus; // DSI bus handle
    uint8_t virtual_channel;      // Virtual channel ID, index from 0
    uint8_t cur_fb_index;         // Current frame buffer index
    uint8_t num_fbs;              // Number of frame buffers
    uint8_t *fbs[DPI_PANEL_MAX_FB_NUM]; // Frame buffers
    uint32_t h_pixels;            // Horizontal pixels
    uint32_t v_pixels;            // Vertical pixels
    size_t fb_size;               // Frame buffer size, in bytes
    size_t bits_per_pixel;        // Bits per pixel
    lcd_color_format_t in_color_format;  // Input color format
    lcd_color_format_t out_color_format; // Output color format
    dw_gdma_channel_handle_t dma_chan;    // DMA channel
    dw_gdma_link_list_handle_t link_lists[DPI_PANEL_MAX_FB_NUM]; // DMA link list
    esp_async_fbcpy_handle_t fbcpy_handle; // Use DMA2D to do frame buffer copy
    SemaphoreHandle_t draw_sem;            // A semaphore used to synchronize the draw operations when DMA2D is used
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;          // Power management lock
#endif
    esp_lcd_dpi_panel_color_trans_done_cb_t on_color_trans_done; // Callback invoked when color data transfer has finished
    esp_lcd_dpi_panel_refresh_done_cb_t on_refresh_done; // Callback invoked when one refresh operation finished (kinda like a vsync end)
    void *user_ctx; // User context for the callback
};

IRAM_ATTR
static bool async_fbcpy_done_cb(esp_async_fbcpy_handle_t mcp, esp_async_fbcpy_event_data_t *event, void *cb_args)
{
    bool need_yield = false;
    esp_lcd_dpi_panel_t *dpi_panel = (esp_lcd_dpi_panel_t *)cb_args;

    // release the draw semaphore first
    BaseType_t task_woken = pdFALSE;
    xSemaphoreGiveFromISR(dpi_panel->draw_sem, &task_woken);
    if (task_woken == pdTRUE) {
        need_yield = true;
    }

    if (dpi_panel->on_color_trans_done) {
        if (dpi_panel->on_color_trans_done(&dpi_panel->base, NULL, dpi_panel->user_ctx)) {
            need_yield = true;
        }
    }

    return need_yield;
}

IRAM_ATTR
static bool dma_trans_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    bool yield_needed = false;
    esp_lcd_dpi_panel_t *dpi_panel = (esp_lcd_dpi_panel_t *)user_data;
    mipi_dsi_hal_context_t *hal = &dpi_panel->bus->hal;
    uint8_t fb_index = dpi_panel->cur_fb_index;
    dw_gdma_link_list_handle_t link_list = dpi_panel->link_lists[fb_index];

    // clear the interrupt status
    uint32_t error_status = mipi_dsi_brg_ll_get_interrupt_status(hal->bridge);
    mipi_dsi_brg_ll_clear_interrupt_status(hal->bridge, error_status);
    if (unlikely(error_status & MIPI_DSI_LL_EVENT_UNDERRUN)) {
        // when an underrun happens, the LCD display may already becomes blue
        // it's too late to recover the display, so we just print an error message
        // as a hint to the user that he should optimize the memory bandwidth (with AXI-ICM)
        ESP_DRAM_LOGE(TAG, "can't fetch data from external memory fast enough, underrun happens");
    }

    // restart the DMA transfer, keep refreshing the LCD
    dw_gdma_block_markers_t markers = {
        .is_valid = true,
        .is_last = true,
    };
    dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 0), markers);
    dw_gdma_channel_use_link_list(chan, link_list);
    dw_gdma_channel_enable_ctrl(chan, true);

    // the DMA descriptor is large enough to carry a whole frame buffer, so this event can also be treated as a fake "vsync end"
    if (dpi_panel->on_refresh_done) {
        if (dpi_panel->on_refresh_done(&dpi_panel->base, NULL, dpi_panel->user_ctx)) {
            yield_needed = true;
        }
    }
    return yield_needed;
}

// Please note, errors happened in this function is just propagated to the caller
// dpi_panel_del() is actually doing the error handling
static esp_err_t dpi_panel_create_dma_link(esp_lcd_dpi_panel_t *dpi_panel)
{
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
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    ESP_RETURN_ON_ERROR(dw_gdma_new_channel(&dma_alloc_config, &dma_chan), TAG, "create DMA channel failed");
    dpi_panel->dma_chan = dma_chan;

    // create DMA link lists
    dw_gdma_link_list_config_t link_list_config = {
        .num_items = DPI_PANEL_MIN_DMA_NODES_PER_LINK,
        .link_type = DW_GDMA_LINKED_LIST_TYPE_SINGLY,
    };
    for (int i = 0; i < dpi_panel->num_fbs; i++) {
        ESP_RETURN_ON_ERROR(dw_gdma_new_link_list(&link_list_config, &link_list), TAG, "create DMA link list failed");
        dpi_panel->link_lists[i] = link_list;
    }

    // register DMA ISR callbacks
    dw_gdma_event_callbacks_t dsi_dma_cbs = {
        .on_full_trans_done = dma_trans_done_cb,
    };
    ESP_RETURN_ON_ERROR(dw_gdma_channel_register_event_callbacks(dma_chan, &dsi_dma_cbs, dpi_panel), TAG, "register DMA callbacks failed");

    return ESP_OK;
}

esp_err_t esp_lcd_new_panel_dpi(esp_lcd_dsi_bus_handle_t bus, const esp_lcd_dpi_panel_config_t *panel_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_dpi_panel_t *dpi_panel = NULL;
    esp_async_fbcpy_handle_t fbcpy_ctx = NULL;
    ESP_RETURN_ON_FALSE(bus && panel_config && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(panel_config->virtual_channel < 4, ESP_ERR_INVALID_ARG, TAG, "invalid virtual channel %d", panel_config->virtual_channel);
    ESP_RETURN_ON_FALSE(panel_config->dpi_clock_freq_mhz, ESP_ERR_INVALID_ARG, TAG, "invalid DPI clock frequency %"PRIu32, panel_config->dpi_clock_freq_mhz);
#if !SOC_DMA2D_SUPPORTED
    ESP_RETURN_ON_FALSE(!panel_config->flags.use_dma2d, ESP_ERR_NOT_SUPPORTED, TAG, "DMA2D is not supported");
#endif // !SOC_DMA2D_SUPPORTED
    size_t num_fbs = panel_config->num_fbs;
    // if the user doesn't specify the number of frame buffers, then fallback to use one frame buffer
    if (num_fbs == 0) {
        num_fbs = 1;
    }
    ESP_RETURN_ON_FALSE(num_fbs <= DPI_PANEL_MAX_FB_NUM, ESP_ERR_INVALID_ARG, TAG, "num_fbs not within [1,%d]", DPI_PANEL_MAX_FB_NUM);

    size_t bits_per_pixel = 0;
    switch (panel_config->pixel_format) {
    case LCD_COLOR_PIXEL_FORMAT_RGB565:
        bits_per_pixel = 16;
        break;
    case LCD_COLOR_PIXEL_FORMAT_RGB666:
        // RGB data in the memory must be constructed in 6-6-6 (18 bits) for each pixel
        bits_per_pixel = 18;
        break;
    case LCD_COLOR_PIXEL_FORMAT_RGB888:
        bits_per_pixel = 24;
        break;
    }
    lcd_color_format_t in_color_format = COLOR_TYPE_ID(COLOR_SPACE_RGB, panel_config->pixel_format);
    // if user sets the in_color_format, it can override the pixel format setting
    if (panel_config->in_color_format) {
        color_space_pixel_format_t in_color_id = {
            .color_type_id = panel_config->in_color_format,
        };
        bits_per_pixel = color_hal_pixel_format_get_bit_depth(in_color_id);
        in_color_format = panel_config->in_color_format;
    }
    lcd_color_format_t out_color_format = in_color_format;
    if (panel_config->out_color_format) {
        out_color_format = panel_config->out_color_format;
    }
    ESP_RETURN_ON_FALSE(panel_config->video_timing.h_size * panel_config->video_timing.v_size * bits_per_pixel % 8 == 0,
                        ESP_ERR_INVALID_ARG, TAG, "frame buffer size not aligned to byte boundary");

    int bus_id = bus->bus_id;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    dpi_panel = heap_caps_calloc(1, sizeof(esp_lcd_dpi_panel_t), DSI_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(dpi_panel, ESP_ERR_NO_MEM, err, TAG, "no memory for DPI panel");
    dpi_panel->virtual_channel = panel_config->virtual_channel;
    dpi_panel->in_color_format = in_color_format;
    dpi_panel->out_color_format = out_color_format;
    dpi_panel->bus = bus;
    dpi_panel->num_fbs = num_fbs;

    // allocate frame buffer from PSRAM
    uint32_t cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    // DMA doesn't have requirement on the buffer alignment, but the cache does
    uint32_t alignment = cache_line_size;
    size_t fb_size = panel_config->video_timing.h_size * panel_config->video_timing.v_size * bits_per_pixel / 8;
    uint8_t *frame_buffer = NULL;
    for (int i = 0; i < num_fbs; i++) {
        frame_buffer = heap_caps_aligned_calloc(alignment, 1, fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(frame_buffer, ESP_ERR_NO_MEM, err, TAG, "no memory for frame buffer");
        dpi_panel->fbs[i] = frame_buffer;
        ESP_LOGD(TAG, "fb[%d] @%p", i, frame_buffer);
        // preset the frame buffer with black color
        // the frame buffer address alignment is ensured by `heap_caps_aligned_calloc`
        // while the value of the fb_size may not be aligned to the cache line size
        // but that's not a problem because the `heap_caps_aligned_calloc` internally allocated a buffer whose size is aligned up to the cache line size
        ESP_GOTO_ON_ERROR(esp_cache_msync(frame_buffer, fb_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                          err, TAG, "cache write back failed");
    }
    dpi_panel->fb_size = fb_size;
    dpi_panel->bits_per_pixel = bits_per_pixel;
    dpi_panel->h_pixels = panel_config->video_timing.h_size;
    dpi_panel->v_pixels = panel_config->video_timing.v_size;

#if SOC_DMA2D_SUPPORTED
    if (panel_config->flags.use_dma2d) {
        esp_async_fbcpy_config_t fbcpy_config = {};
        ESP_GOTO_ON_ERROR(esp_async_fbcpy_install(&fbcpy_config, &fbcpy_ctx), err, TAG, "install async memcpy 2d failed");
        dpi_panel->fbcpy_handle = fbcpy_ctx;
        dpi_panel->draw_sem = xSemaphoreCreateBinaryWithCaps(DSI_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(dpi_panel->draw_sem, ESP_ERR_NO_MEM, err, TAG, "no memory for draw semaphore");
        xSemaphoreGive(dpi_panel->draw_sem);
    }
#endif // SOC_DMA2D_SUPPORTED

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
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)dpi_clk_src, true), err, TAG, "clock source enable failed");
    // set the clock source, set the divider, and enable the dpi clock
    DSI_CLOCK_SRC_ATOMIC() {
        mipi_dsi_ll_set_dpi_clock_source(bus_id, dpi_clk_src);
        mipi_dsi_ll_set_dpi_clock_div(bus_id, dpi_div);
        mipi_dsi_ll_enable_dpi_clock(bus_id, true);
    }

#if CONFIG_PM_ENABLE
    // When MIPI DSI is working, we don't expect the clock source would be turned off
    // use CPU_MAX lock to ensure PSRAM bandwidth and usability during DFS
    esp_pm_lock_type_t pm_lock_type = ESP_PM_CPU_FREQ_MAX;
    ret  = esp_pm_lock_create(pm_lock_type, 0, "dsi_dpi", &dpi_panel->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create PM lock failed");
    esp_pm_lock_acquire(dpi_panel->pm_lock);
#endif

    // create DMA resources
    ESP_GOTO_ON_ERROR(dpi_panel_create_dma_link(dpi_panel), err, TAG, "initialize DMA link failed");

    mipi_dsi_host_ll_dpi_set_vcid(hal->host, panel_config->virtual_channel);
    mipi_dsi_hal_host_dpi_set_color_coding(hal, out_color_format, 0);
    // these signals define how the DPI interface interacts with the controller
    mipi_dsi_host_ll_dpi_set_timing_polarity(hal->host, false, false, false, false, false);

    if (panel_config->flags.disable_lp) {
        // configure the low-power transitions: defines the video periods which are NOT permitted to goto low-power
        mipi_dsi_host_ll_dpi_enable_lp_horizontal_timing(hal->host, false, false);
        mipi_dsi_host_ll_dpi_enable_lp_vertical_timing(hal->host, false, false, false, false);
        // commands are NOT transmitted in low-power mode
        mipi_dsi_host_ll_dpi_enable_lp_command(hal->host, false);
    } else {
        // configure the low-power transitions: defines the video periods which are permitted to goto low-power if the time available to do so
        mipi_dsi_host_ll_dpi_enable_lp_horizontal_timing(hal->host, true, true);
        mipi_dsi_host_ll_dpi_enable_lp_vertical_timing(hal->host, true, true, true, true);
        // commands are transmitted in low-power mode
        mipi_dsi_host_ll_dpi_enable_lp_command(hal->host, true);
    }
    // after sending a frame, the DSI device should return an ack
    mipi_dsi_host_ll_dpi_enable_frame_ack(hal->host, true);
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
    mipi_dsi_brg_ll_set_num_pixel_bits(hal->bridge, panel_config->video_timing.h_size * panel_config->video_timing.v_size * bits_per_pixel);
    mipi_dsi_brg_ll_set_underrun_discard_count(hal->bridge, panel_config->video_timing.h_size);
    // set input color space
    mipi_dsi_brg_ll_set_input_color_space(hal->bridge, COLOR_SPACE_TYPE(in_color_format));
    // use the DW_GDMA as the flow controller
    mipi_dsi_brg_ll_set_flow_controller(hal->bridge, MIPI_DSI_LL_FLOW_CONTROLLER_DMA);
    mipi_dsi_brg_ll_set_multi_block_number(hal->bridge, DPI_PANEL_MIN_DMA_NODES_PER_LINK);
    mipi_dsi_brg_ll_set_burst_len(hal->bridge, 256);
    mipi_dsi_brg_ll_set_empty_threshold(hal->bridge, 1024 - 256);
    // enable DSI bridge
    mipi_dsi_brg_ll_enable(hal->bridge, true);
    mipi_dsi_brg_ll_update_dpi_config(hal->bridge);

    dpi_panel->base.del = dpi_panel_del;
    dpi_panel->base.init = dpi_panel_init;
    dpi_panel->base.draw_bitmap = dpi_panel_draw_bitmap;
    *ret_panel = &dpi_panel->base;
    ESP_LOGD(TAG, "dpi panel created @%p", dpi_panel);
    return ESP_OK;
err:
    if (dpi_panel) {
        dpi_panel_del(&dpi_panel->base);
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
    // free memory
    if (dpi_panel->dma_chan) {
        dw_gdma_del_channel(dpi_panel->dma_chan);
    }
    for (int i = 0; i < DPI_PANEL_MAX_FB_NUM; i++) {
        if (dpi_panel->fbs[i]) {
            free(dpi_panel->fbs[i]);
        }
    }
    for (int i = 0; i < DPI_PANEL_MAX_FB_NUM; i++) {
        if (dpi_panel->link_lists[i]) {
            dw_gdma_del_link_list(dpi_panel->link_lists[i]);
        }
    }
    if (dpi_panel->fbcpy_handle) {
        esp_async_fbcpy_uninstall(dpi_panel->fbcpy_handle);
    }
    if (dpi_panel->draw_sem) {
        vSemaphoreDeleteWithCaps(dpi_panel->draw_sem);
    }
#if CONFIG_PM_ENABLE
    if (dpi_panel->pm_lock) {
        esp_pm_lock_release(dpi_panel->pm_lock);
        esp_pm_lock_delete(dpi_panel->pm_lock);
    }
#endif
    free(dpi_panel);
    return ESP_OK;
}

esp_err_t esp_lcd_dpi_panel_get_frame_buffer(esp_lcd_panel_handle_t panel, uint32_t fb_num, void **fb0, ...)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= dpi_panel->num_fbs, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");
    void **fb_itor = fb0;
    va_list args;
    va_start(args, fb0);
    for (uint32_t i = 0; i < fb_num; i++) {
        if (fb_itor) {
            *fb_itor = dpi_panel->fbs[i];
            fb_itor = va_arg(args, void **);
        }
    }
    va_end(args);
    return ESP_OK;
}

static esp_err_t dpi_panel_init(esp_lcd_panel_t *panel)
{
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    esp_lcd_dsi_bus_handle_t bus = dpi_panel->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;
    dw_gdma_channel_handle_t dma_chan = dpi_panel->dma_chan;
    dw_gdma_link_list_handle_t link_list = NULL;

    dw_gdma_block_transfer_config_t dma_transfer_config = {
        .src = {
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .dst = {
            .addr = MIPI_DSI_BRG_MEM_BASE,
            .burst_mode = DW_GDMA_BURST_MODE_FIXED,
            .burst_items = DW_GDMA_BURST_ITEMS_256,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .size = dpi_panel->fb_size * 8 / 64,
    };
    for (int i = 0; i < dpi_panel->num_fbs; i++) {
        link_list = dpi_panel->link_lists[i];
        dma_transfer_config.src.addr = (uint32_t)(dpi_panel->fbs[i]);
        dw_gdma_lli_config_transfer(dw_gdma_link_list_get_item(link_list, 0), &dma_transfer_config);
        dw_gdma_block_markers_t markers = {
            .is_valid = true,
            .is_last = true,
        };
        dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 0), markers);
    }

    // by default, we use the fb0 as the first working frame buffer
    dpi_panel->cur_fb_index = 0;
    link_list = dpi_panel->link_lists[0];
    dw_gdma_channel_use_link_list(dma_chan, link_list);
    // enable the DMA channel
    dw_gdma_channel_enable_ctrl(dma_chan, true);

    // enable the video mode
    mipi_dsi_host_ll_enable_video_mode(hal->host, true);
    // switch the clock lane to high speed mode
    mipi_dsi_host_ll_set_clock_lane_state(hal->host, MIPI_DSI_LL_CLOCK_LANE_STATE_AUTO);

    // enable the DPI output of the DSI bridge
    mipi_dsi_brg_ll_enable_dpi_output(hal->bridge, true);
    mipi_dsi_brg_ll_update_dpi_config(hal->bridge);

    // enable the underrun interrupt, we use this as a signal of bandwidth shortage
    // note, we opt to not install a dedicated interrupt handler just for this error condition, instead, we check it in the DMA callback
    mipi_dsi_brg_ll_enable_interrupt(hal->bridge, MIPI_DSI_LL_EVENT_UNDERRUN, true);

    return ESP_OK;
}

static esp_err_t dpi_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    uint8_t cur_fb_index = dpi_panel->cur_fb_index;
    uint8_t *frame_buffer = dpi_panel->fbs[cur_fb_index];
    uint8_t *draw_buffer = (uint8_t *)color_data;
    size_t fb_size = dpi_panel->fb_size;
    size_t bits_per_pixel = dpi_panel->bits_per_pixel;

    // clip to boundaries
    int h_res = dpi_panel->h_pixels;
    int v_res = dpi_panel->v_pixels;
    x_start = MAX(x_start, 0);
    x_end = MIN(x_end, h_res);
    y_start = MAX(y_start, 0);
    y_end = MIN(y_end, v_res);

    bool do_copy = false;
    uint8_t draw_buf_fb_index = 0;
    // check if the user draw buffer resides in any frame buffer's memory range
    // if so, we don't need to copy the data, just do cache write back
    if (draw_buffer >= dpi_panel->fbs[0] && draw_buffer < dpi_panel->fbs[0] + fb_size) {
        draw_buf_fb_index = 0;
    } else if (draw_buffer >= dpi_panel->fbs[1] && draw_buffer < dpi_panel->fbs[1] + fb_size) {
        draw_buf_fb_index = 1;
    } else if (draw_buffer >= dpi_panel->fbs[2] && draw_buffer < dpi_panel->fbs[2] + fb_size) {
        draw_buf_fb_index = 2;
    } else {
        do_copy = true;
    }

    if (!do_copy) { // no copy, just do cache memory write back
        ESP_LOGV(TAG, "draw buffer is in frame buffer memory range, do cache write back only");
        // only write back the LCD lines that updated by the draw buffer
        uint8_t *cache_sync_start = dpi_panel->fbs[draw_buf_fb_index] + (y_start * dpi_panel->h_pixels) * bits_per_pixel / 8;
        size_t cache_sync_size = (y_end - y_start) * dpi_panel->h_pixels * bits_per_pixel / 8;
        // the buffer to be flushed is still within the frame buffer, so even an unaligned address is OK
        esp_cache_msync(cache_sync_start, cache_sync_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

        dpi_panel->cur_fb_index = draw_buf_fb_index;
        // invoke the trans done callback
        if (dpi_panel->on_color_trans_done) {
            dpi_panel->on_color_trans_done(&dpi_panel->base, NULL, dpi_panel->user_ctx);
        }
    } else if (!dpi_panel->fbcpy_handle) { // copy by CPU
        ESP_LOGV(TAG, "copy draw buffer by CPU");
        const uint8_t *from = draw_buffer;
        uint8_t *to = frame_buffer + (y_start * dpi_panel->h_pixels + x_start) * bits_per_pixel / 8;
        uint32_t copy_bytes_per_line = (x_end - x_start) * bits_per_pixel / 8;
        uint32_t bytes_per_line = bits_per_pixel * dpi_panel->h_pixels / 8;
        // please note, we assume the user provided draw_buffer is compact,
        // but the destination is a sub-window of the frame buffer, so we need to skip the stride
        for (int y = y_start; y < y_end; y++) {
            memcpy(to, from, copy_bytes_per_line);
            to += bytes_per_line;
            from += copy_bytes_per_line;
        }
        uint8_t *cache_sync_start = frame_buffer + (y_start * dpi_panel->h_pixels) * bits_per_pixel / 8;
        size_t cache_sync_size = (y_end - y_start) * dpi_panel->h_pixels * bits_per_pixel / 8;
        // the buffer to be flushed is still within the frame buffer, so even an unaligned address is OK
        esp_cache_msync(cache_sync_start, cache_sync_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        // invoke the trans done callback
        if (dpi_panel->on_color_trans_done) {
            dpi_panel->on_color_trans_done(&dpi_panel->base, NULL, dpi_panel->user_ctx);
        }
    } else { // copy by DMA2D
        ESP_LOGV(TAG, "copy draw buffer by DMA2D");
        // ensure the previous draw operation is finished
        ESP_RETURN_ON_FALSE(xSemaphoreTake(dpi_panel->draw_sem, 0) == pdTRUE, ESP_ERR_INVALID_STATE,
                            TAG, "previous draw operation is not finished");

        // write back the user's draw buffer, so that the DMA can see the correct data
        // Note, the user draw buffer should be 1D array, and contiguous in memory, no stride
        size_t color_data_size = (x_end - x_start) * (y_end - y_start) * bits_per_pixel / 8;
        esp_cache_msync(draw_buffer, color_data_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

        esp_async_fbcpy_trans_desc_t fbcpy_trans_config = {
            .src_buffer = draw_buffer,
            .dst_buffer = (void *)frame_buffer,
            .src_buffer_size_x = x_end - x_start,
            .src_buffer_size_y = y_end - y_start,
            .dst_buffer_size_x = dpi_panel->h_pixels,
            .dst_buffer_size_y = dpi_panel->v_pixels,
            .src_offset_x = 0,
            .src_offset_y = 0,
            .dst_offset_x = x_start,
            .dst_offset_y = y_start,
            .copy_size_x = x_end - x_start,
            .copy_size_y = y_end - y_start,
            .pixel_format_unique_id = {
                .color_type_id = dpi_panel->in_color_format,
            }
        };
        ESP_RETURN_ON_ERROR(esp_async_fbcpy(dpi_panel->fbcpy_handle, &fbcpy_trans_config, async_fbcpy_done_cb, dpi_panel), TAG, "async memcpy failed");
    }

    return ESP_OK;
}

esp_err_t esp_lcd_dpi_panel_set_color_conversion(esp_lcd_panel_handle_t panel, const esp_lcd_color_conv_config_t *config)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
    esp_lcd_dsi_bus_handle_t bus = dpi_panel->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    if (dpi_panel->in_color_format == COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422)
            && COLOR_SPACE_TYPE(dpi_panel->out_color_format) == LCD_COLOR_SPACE_RGB) {
        // YUV422->RGB
        mipi_dsi_brg_ll_set_yuv_convert_std(hal->bridge, config->spec.yuv.conv_std);
        mipi_dsi_brg_ll_set_yuv422_pack_order(hal->bridge, config->spec.yuv.yuv422.in_pack_order);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "unsupported conversion mode");
    }
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
        // re-enable the DSI bridge to generate the DPI stream
        mipi_dsi_brg_ll_enable_dpi_output(hal->bridge, true);
        mipi_dsi_brg_ll_update_dpi_config(hal->bridge);
    }

    return ESP_OK;
}

esp_err_t esp_lcd_dpi_panel_register_event_callbacks(esp_lcd_panel_handle_t panel, const esp_lcd_dpi_panel_event_callbacks_t *cbs, void *user_ctx)
{
    ESP_RETURN_ON_FALSE(panel && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_lcd_dpi_panel_t *dpi_panel = __containerof(panel, esp_lcd_dpi_panel_t, base);
#if CONFIG_LCD_DSI_ISR_IRAM_SAFE
    if (cbs->on_color_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_color_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_color_trans_done callback not in IRAM");
    }
    if (cbs->on_refresh_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_refresh_done), ESP_ERR_INVALID_ARG, TAG, "on_refresh_done callback not in IRAM");
    }
    if (user_ctx) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_ctx), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE
    dpi_panel->on_color_trans_done = cbs->on_color_trans_done;
    dpi_panel->on_refresh_done = cbs->on_refresh_done;
    dpi_panel->user_ctx = user_ctx;

    return ESP_OK;
}
