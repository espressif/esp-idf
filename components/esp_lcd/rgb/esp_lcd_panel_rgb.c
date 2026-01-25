/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for gptimer driver
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "esp_clk_tree.h"
#include "hal/dma_types.h"
#include "esp_bit_defs.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_psram.h"
#include "esp_lcd_common.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "hal/lcd_periph.h"
#include "soc/io_mux_reg.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/color_hal.h"
#include "rgb_lcd_rotation_sw.h"

// hardware issue workaround
#if CONFIG_IDF_TARGET_ESP32S3
#define RGB_LCD_NEEDS_SEPARATE_RESTART_LINK 1
#endif

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
#define LCD_RGB_INTR_ALLOC_FLAGS     (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#define LCD_RGB_MEM_ALLOC_CAPS       (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define LCD_RGB_INTR_ALLOC_FLAGS     ESP_INTR_FLAG_INTRDISABLED
#define LCD_RGB_MEM_ALLOC_CAPS       MALLOC_CAP_DEFAULT
#endif

#if defined(SOC_GDMA_TRIG_PERIPH_LCD0_BUS) && (SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AHB)
#define LCD_GDMA_NEW_CHANNEL gdma_new_ahb_channel
#define LCD_GDMA_DESCRIPTOR_ALIGN 4
#elif defined(SOC_GDMA_TRIG_PERIPH_LCD0_BUS) && (SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AXI)
#define LCD_GDMA_NEW_CHANNEL gdma_new_axi_channel
#define LCD_GDMA_DESCRIPTOR_ALIGN 8
#else
#error "Unsupported GDMA bus type for RGB LCD"
#endif

#define RGB_LCD_PANEL_BOUNCE_BUF_NUM     2 // bounce buffer number

ESP_LOG_ATTR_TAG(TAG, "lcd.rgb");

typedef struct esp_rgb_panel_t esp_rgb_panel_t;

static esp_err_t rgb_panel_del(esp_lcd_panel_t *panel);
static esp_err_t rgb_panel_reset(esp_lcd_panel_t *panel);
static esp_err_t rgb_panel_init(esp_lcd_panel_t *panel);
static esp_err_t rgb_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t rgb_panel_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t rgb_panel_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t rgb_panel_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t rgb_panel_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t rgb_panel_disp_on_off(esp_lcd_panel_t *panel, bool off);
static esp_err_t lcd_rgb_panel_select_clock_src(esp_rgb_panel_t *rgb_panel, lcd_clock_source_t clk_src);
static esp_err_t lcd_rgb_create_dma_channel(esp_rgb_panel_t *rgb_panel);
static esp_err_t lcd_rgb_panel_init_trans_link(esp_rgb_panel_t *rgb_panel);
static esp_err_t lcd_rgb_panel_configure_gpio(esp_rgb_panel_t *rgb_panel, const esp_lcd_rgb_panel_config_t *panel_config);
static void lcd_rgb_panel_release_gpio(esp_rgb_panel_t *rgb_panel);
static void lcd_rgb_panel_start_transmission(esp_rgb_panel_t *rgb_panel);
static void rgb_lcd_default_isr_handler(void *args);

struct esp_rgb_panel_t {
    esp_lcd_panel_t base;  // Base class of generic lcd panel
    int panel_id;          // LCD panel ID
    lcd_hal_context_t hal; // Hal layer object
    size_t data_width;     // Number of data lines
    size_t num_fbs;        // Number of frame buffers
    lcd_color_format_t in_color_format;  // Input color format
    lcd_color_format_t out_color_format; // Output color format
    size_t fb_bits_per_pixel; // Frame buffer (the input buffer of the LCD controller) color depth, in bpp
    size_t dma_burst_size;    // DMA transfer burst size
    intr_handle_t intr;       // LCD peripheral interrupt handle
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock; // Power management lock
#endif
    size_t num_dma_nodes;  // Number of DMA descriptors that used to carry the frame buffer
    gdma_channel_handle_t dma_chan; // DMA channel handle
    gdma_link_list_handle_t dma_fb_links[ESP_RGB_LCD_PANEL_MAX_FB_NUM]; // DMA link lists for multiple frame buffers
    gdma_link_list_handle_t dma_bb_link; // DMA link list for bounce buffer
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
    gdma_link_list_handle_t dma_restart_link; // DMA link list for restarting the DMA
#endif
    uint8_t *fbs[ESP_RGB_LCD_PANEL_MAX_FB_NUM]; // Frame buffers
    uint8_t *bounce_buffer[RGB_LCD_PANEL_BOUNCE_BUF_NUM]; // Pointer to the bounce buffers
    size_t fb_size;        // Size of frame buffer, in bytes
    size_t bb_size;        // Size of the bounce buffer, in bytes. If not-zero, the driver uses two bounce buffers allocated from internal memory
    uint8_t cur_fb_index;  // Current frame buffer index
    uint8_t bb_fb_index;  // Current frame buffer index which used by bounce buffer
    size_t int_mem_align;  // DMA buffer alignment for internal memory
    size_t ext_mem_align;  // DMA buffer alignment for external memory
    gpio_num_t hsync_gpio_num;    // GPIO used for HSYNC signal
    gpio_num_t vsync_gpio_num;    // GPIO used for VSYNC signal
    gpio_num_t de_gpio_num;       // GPIO used for DE signal, set to -1 if it's not used
    gpio_num_t pclk_gpio_num;     // GPIO used for PCLK signal, set to -1 if it's not used
    gpio_num_t disp_gpio_num;     // GPIO used for display control signal, set to -1 if it's not used
    gpio_num_t data_gpio_nums[LCD_LL_GET(RGB_BUS_WIDTH)]; // GPIOs used for data lines, we keep these GPIOs for action like "invert_color"
    uint64_t gpio_reserve_mask; // GPIOs reserved by this panel, used to revoke the GPIO reservation when the panel is deleted
    uint32_t src_clk_hz;   // Peripheral source clock resolution
    esp_lcd_rgb_timing_t timings;   // RGB timing parameters (e.g. pclk, sync pulse, porch width)
    int bounce_pos_px;              // Position in whatever source material is used for the bounce buffer, in pixels
    size_t bb_eof_count;            // record the number we received the DMA EOF event, compare with `expect_eof_count` in the VSYNC_END ISR
    size_t expect_eof_count;        // record the number of DMA EOF event we expected to receive
    esp_lcd_rgb_panel_draw_buf_complete_cb_t on_color_trans_done; // draw buffer completes
    esp_lcd_rgb_panel_frame_buf_complete_cb_t on_frame_buf_complete; // callback used to notify when the bounce buffer finish copying the entire frame
    esp_lcd_rgb_panel_vsync_cb_t on_vsync; // VSYNC event callback
    esp_lcd_rgb_panel_bounce_buf_fill_cb_t on_bounce_empty; // callback used to fill a bounce buffer rather than copying from the frame buffer
    void *user_ctx;                 // Reserved user's data of callback functions
    int x_gap;                      // Extra gap in x coordinate, it's used when calculate the flush window
    int y_gap;                      // Extra gap in y coordinate, it's used when calculate the flush window
    portMUX_TYPE spinlock;          // to protect panel specific resource from concurrent access (e.g. between task and ISR)
    int rotate_mask;                // panel rotate_mask mask, Or'ed of `panel_rotate_mask_t`
    struct {
        uint32_t disp_en_level: 1;       // The level which can turn on the screen by `disp_gpio_num`
        uint32_t stream_mode: 1;         // If set, the LCD transfers data continuously, otherwise, it stops refreshing the LCD when transaction done
        uint32_t fb_in_psram: 1;         // Whether the frame buffer is in PSRAM
        uint32_t need_update_pclk: 1;    // Whether to update the PCLK before start a new transaction
        uint32_t need_restart: 1;        // Whether to restart the LCD controller and the DMA
        uint32_t fb_behind_cache: 1;     // Whether the frame buffer is behind the cache
        uint32_t bb_behind_cache: 1;     // Whether the bounce buffer is behind the cache
        uint32_t user_fb: 1;             // Whether the frame buffer is provided by user
    } flags;
};

static esp_err_t lcd_rgb_panel_alloc_frame_buffers(esp_rgb_panel_t *rgb_panel, const esp_lcd_rgb_panel_config_t *panel_config)
{
    bool fb_in_psram = rgb_panel->flags.fb_in_psram;

    // read the cache line size of internal and external memory, we use this information to check if the allocated memory is behind the cache
    uint32_t int_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    uint32_t ext_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);

    // alloc frame buffer
    uint8_t user_fb_count = 0;
    for (int i = 0; i < rgb_panel->num_fbs; i++) {
        if (panel_config->user_fbs[i] != NULL) {
            // Frame buffer is provided by user, no need to allocate
            rgb_panel->flags.user_fb = true;
            // Check user frame buffer DMA accessibility
            if (!esp_ptr_dma_capable(panel_config->user_fbs[i]) && !esp_ptr_dma_ext_capable(panel_config->user_fbs[i])) {
                ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "frame buffer %d is not DMA accessible", i);
            }
            // Check if user frame buffer is in PSRAM or internal memory
            if (esp_ptr_external_ram(panel_config->user_fbs[i])) {
                ESP_RETURN_ON_FALSE(((uintptr_t)panel_config->user_fbs[i] & (rgb_panel->ext_mem_align - 1)) == 0,
                                    ESP_ERR_INVALID_ARG, TAG, "frame buffer %d is not aligned to "PRIu32"", i, rgb_panel->ext_mem_align);
                rgb_panel->flags.fb_behind_cache = ext_mem_cache_line_size > 0;
            } else {
                ESP_RETURN_ON_FALSE(((uintptr_t)panel_config->user_fbs[i] & (rgb_panel->int_mem_align - 1)) == 0,
                                    ESP_ERR_INVALID_ARG, TAG, "frame buffer %d is not aligned to "PRIu32"", i, rgb_panel->int_mem_align);
                rgb_panel->flags.fb_behind_cache = int_mem_cache_line_size > 0;
            }
            rgb_panel->fbs[i] = (uint8_t *)panel_config->user_fbs[i];
            user_fb_count++;
        } else {
            // Allocate frame buffer by driver
            if (fb_in_psram) {
                // the allocated buffer is also aligned to the cache line size
                rgb_panel->fbs[i] = heap_caps_aligned_calloc(rgb_panel->ext_mem_align, 1, rgb_panel->fb_size,
                                                             MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
                ESP_RETURN_ON_FALSE(rgb_panel->fbs[i], ESP_ERR_NO_MEM, TAG, "no mem for frame buffer");
                rgb_panel->flags.fb_behind_cache = ext_mem_cache_line_size > 0;
            } else {
                rgb_panel->fbs[i] = heap_caps_aligned_calloc(rgb_panel->int_mem_align, 1, rgb_panel->fb_size,
                                                             MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
                ESP_RETURN_ON_FALSE(rgb_panel->fbs[i], ESP_ERR_NO_MEM, TAG, "no mem for frame buffer");
                rgb_panel->flags.fb_behind_cache = int_mem_cache_line_size > 0;
            }
        }

        // flush data from cache to the physical memory
        if (rgb_panel->flags.fb_behind_cache) {
            esp_cache_msync(rgb_panel->fbs[i], rgb_panel->fb_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        }
    }

    if (user_fb_count > 0 && user_fb_count != rgb_panel->num_fbs) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "user frame buffer count conflicts with num_fbs");
    }

    // alloc bounce buffer
    if (rgb_panel->bb_size) {
        for (int i = 0; i < RGB_LCD_PANEL_BOUNCE_BUF_NUM; i++) {
            // bounce buffer must be allocated from internal memory for performance
            rgb_panel->bounce_buffer[i] = heap_caps_aligned_calloc(rgb_panel->int_mem_align, 1, rgb_panel->bb_size,
                                                                   MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
            ESP_RETURN_ON_FALSE(rgb_panel->bounce_buffer[i], ESP_ERR_NO_MEM, TAG, "no mem for bounce buffer");
            if (int_mem_cache_line_size > 0) {
                // flush data from cache to the physical memory
                esp_cache_msync(rgb_panel->bounce_buffer[i], rgb_panel->bb_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
                rgb_panel->flags.bb_behind_cache = true;
            }
        }
    }
    rgb_panel->cur_fb_index = 0;
    rgb_panel->bb_fb_index = 0;

    return ESP_OK;
}

static esp_err_t lcd_rgb_panel_destroy(esp_rgb_panel_t *rgb_panel)
{
    // ensure the HW state machine is stopped
    lcd_ll_stop(rgb_panel->hal.dev);
    PERIPH_RCC_ATOMIC() {
        lcd_ll_enable_clock(rgb_panel->hal.dev, false);
    }
    if (rgb_panel->panel_id >= 0) {
        PERIPH_RCC_RELEASE_ATOMIC(soc_lcd_rgb_signals[rgb_panel->panel_id].module, ref_count) {
            if (ref_count == 0) {
                lcd_ll_enable_bus_clock(rgb_panel->panel_id, false);
            }
        }
        lcd_com_remove_device(LCD_COM_DEVICE_TYPE_RGB, rgb_panel->panel_id);
    }
    // release the GPIOs
    lcd_rgb_panel_release_gpio(rgb_panel);
    if (rgb_panel->dma_chan) {
        gdma_disconnect(rgb_panel->dma_chan);
        gdma_del_channel(rgb_panel->dma_chan);
    }
    for (size_t i = 0; i < ESP_RGB_LCD_PANEL_MAX_FB_NUM; i++) {
        if (rgb_panel->fbs[i] && !rgb_panel->flags.user_fb) {
            // Only free frame buffer if it was allocated by the driver
            free(rgb_panel->fbs[i]);
        }
        if (rgb_panel->dma_fb_links[i]) {
            gdma_del_link_list(rgb_panel->dma_fb_links[i]);
        }
    }
    for (int i = 0; i < RGB_LCD_PANEL_BOUNCE_BUF_NUM; i++) {
        if (rgb_panel->bounce_buffer[i]) {
            free(rgb_panel->bounce_buffer[i]);
        }
    }
    if (rgb_panel->dma_bb_link) {
        gdma_del_link_list(rgb_panel->dma_bb_link);
    }
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
    if (rgb_panel->dma_restart_link) {
        gdma_del_link_list(rgb_panel->dma_restart_link);
    }
#endif
    if (rgb_panel->intr) {
        esp_intr_free(rgb_panel->intr);
    }
#if CONFIG_PM_ENABLE
    if (rgb_panel->pm_lock) {
        esp_pm_lock_release(rgb_panel->pm_lock);
        esp_pm_lock_delete(rgb_panel->pm_lock);
    }
#endif
    free(rgb_panel);
    return ESP_OK;
}

esp_err_t esp_lcd_new_rgb_panel(const esp_lcd_rgb_panel_config_t *rgb_panel_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    esp_rgb_panel_t *rgb_panel = NULL;
    ESP_RETURN_ON_FALSE(rgb_panel_config && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid parameter");
    size_t data_width = rgb_panel_config->data_width;
    ESP_RETURN_ON_FALSE((data_width > 0) && (data_width <= LCD_LL_GET(RGB_BUS_WIDTH)) && ((data_width % 8) == 0), ESP_ERR_INVALID_ARG,
                        TAG, "unsupported data width %d", data_width);
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->flags.double_fb && rgb_panel_config->flags.no_fb),
                        ESP_ERR_INVALID_ARG, TAG, "double_fb conflicts with no_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->num_fbs > 0 && rgb_panel_config->num_fbs != 2 && rgb_panel_config->flags.double_fb),
                        ESP_ERR_INVALID_ARG, TAG, "num_fbs conflicts with double_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->num_fbs > 0 && rgb_panel_config->flags.no_fb),
                        ESP_ERR_INVALID_ARG, TAG, "num_fbs conflicts with no_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->flags.no_fb && rgb_panel_config->bounce_buffer_size_px == 0),
                        ESP_ERR_INVALID_ARG, TAG, "must set bounce buffer if there's no frame buffer");

    // determine number of framebuffers
    size_t num_fbs = 1;
    if (rgb_panel_config->flags.no_fb) {
        num_fbs = 0;
    } else if (rgb_panel_config->flags.double_fb) {
        num_fbs = 2;
    } else if (rgb_panel_config->num_fbs > 0) {
        num_fbs = rgb_panel_config->num_fbs;
    }
    ESP_RETURN_ON_FALSE(num_fbs <= ESP_RGB_LCD_PANEL_MAX_FB_NUM, ESP_ERR_INVALID_ARG, TAG, "too many frame buffers");

    // by default, we guess the input color format according to the data lines number, usually 16 lines means RGB565, 24 lines means RGB888
    lcd_color_format_t in_color_format = 0;
    if (rgb_panel_config->data_width == 24) {
        in_color_format = LCD_COLOR_FMT_RGB888;
    } else if (rgb_panel_config->data_width == 16) {
        in_color_format = LCD_COLOR_FMT_RGB565;
    }
    // override the color format if it's explicitly specified by the user
    if (rgb_panel_config->in_color_format) {
        in_color_format = rgb_panel_config->in_color_format;
    }
    ESP_RETURN_ON_FALSE(in_color_format != 0, ESP_ERR_INVALID_ARG, TAG, "cannot determine input color format");
    // if out_color_format is not specified, set it the same as in_color_format
    lcd_color_format_t out_color_format = rgb_panel_config->out_color_format ? rgb_panel_config->out_color_format : in_color_format;

    // calculate buffer size
    size_t fb_bits_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(in_color_format);
    size_t fb_size = rgb_panel_config->timings.h_res * rgb_panel_config->timings.v_res * fb_bits_per_pixel / 8;
    size_t bb_size = rgb_panel_config->bounce_buffer_size_px * fb_bits_per_pixel / 8;
    size_t expect_bb_eof_count = 0;
    if (bb_size) {
        ESP_RETURN_ON_FALSE(fb_size % bb_size == 0, ESP_ERR_INVALID_ARG, TAG, "frame buffer size must be multiple of bounce buffer size");
        expect_bb_eof_count = fb_size / bb_size;
    }

    // calculate the number of DMA descriptors
    size_t num_dma_nodes = 0;
    // allocate memory for rgb panel
    rgb_panel = heap_caps_calloc(1, sizeof(esp_rgb_panel_t), LCD_RGB_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(rgb_panel, ESP_ERR_NO_MEM, err, TAG, "no mem for rgb panel");
    rgb_panel->panel_id = -1;
    rgb_panel->num_dma_nodes = num_dma_nodes;
    rgb_panel->num_fbs = num_fbs;
    rgb_panel->fb_size = fb_size;
    rgb_panel->bb_size = bb_size;
    rgb_panel->fb_bits_per_pixel = fb_bits_per_pixel;
    rgb_panel->in_color_format = in_color_format;
    rgb_panel->out_color_format = out_color_format;
    rgb_panel->expect_eof_count = expect_bb_eof_count;
    // register to platform
    int panel_id = lcd_com_register_device(LCD_COM_DEVICE_TYPE_RGB, rgb_panel);
    ESP_GOTO_ON_FALSE(panel_id >= 0, ESP_ERR_NOT_FOUND, err, TAG, "no free rgb panel slot");
    rgb_panel->panel_id = panel_id;

    // enable APB to access LCD registers
    PERIPH_RCC_ACQUIRE_ATOMIC(soc_lcd_rgb_signals[panel_id].module, ref_count) {
        if (ref_count == 0) {
            lcd_ll_enable_bus_clock(panel_id, true);
            lcd_ll_reset_register(panel_id);
        }
    }

    // initialize HAL layer, so we can call LL APIs later
    lcd_hal_init(&rgb_panel->hal, panel_id);
    lcd_hal_context_t *hal = &rgb_panel->hal;
    // enable clock
    PERIPH_RCC_ATOMIC() {
        lcd_ll_enable_clock(hal->dev, true);
    }
    // set clock source
    ret = lcd_rgb_panel_select_clock_src(rgb_panel, rgb_panel_config->clk_src);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "set source clock failed");
    // reset peripheral and FIFO after we select a correct clock source
    lcd_ll_fifo_reset(hal->dev);
    lcd_ll_reset(hal->dev);
    // install interrupt service, (LCD peripheral shares the interrupt source with Camera by different mask)
    int isr_flags = LCD_RGB_INTR_ALLOC_FLAGS | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
    ret = esp_intr_alloc_intrstatus(soc_lcd_rgb_signals[panel_id].irq_id, isr_flags,
                                    (uint32_t)lcd_ll_get_interrupt_status_reg(hal->dev),
                                    LCD_LL_EVENT_RGB, rgb_lcd_default_isr_handler, rgb_panel, &rgb_panel->intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install interrupt failed");
    PERIPH_RCC_ATOMIC() {
        lcd_ll_enable_interrupt(hal->dev, LCD_LL_EVENT_RGB, false); // disable all interrupts
    }
    lcd_ll_clear_interrupt_status(hal->dev, UINT32_MAX); // clear pending interrupt

    // install DMA service
    rgb_panel->flags.stream_mode = !rgb_panel_config->flags.refresh_on_demand;
    rgb_panel->dma_burst_size = rgb_panel_config->dma_burst_size ? rgb_panel_config->dma_burst_size : 64;
    rgb_panel->flags.fb_in_psram = rgb_panel_config->flags.fb_in_psram;
    ESP_GOTO_ON_ERROR(lcd_rgb_create_dma_channel(rgb_panel), err, TAG, "install DMA failed");
    // allocate frame buffers + bounce buffers
    ESP_GOTO_ON_ERROR(lcd_rgb_panel_alloc_frame_buffers(rgb_panel, rgb_panel_config), err, TAG, "alloc frame buffers failed");
    // initialize DMA descriptor link
    ESP_GOTO_ON_ERROR(lcd_rgb_panel_init_trans_link(rgb_panel), err, TAG, "init DMA link failed");

    // configure GPIO
    ret = lcd_rgb_panel_configure_gpio(rgb_panel, rgb_panel_config);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "configure GPIO failed");
    // fill other rgb panel runtime parameters
    memcpy(rgb_panel->data_gpio_nums, rgb_panel_config->data_gpio_nums, sizeof(rgb_panel->data_gpio_nums));
    rgb_panel->disp_gpio_num = rgb_panel_config->disp_gpio_num;
    rgb_panel->de_gpio_num = rgb_panel_config->de_gpio_num;
    rgb_panel->hsync_gpio_num = rgb_panel_config->hsync_gpio_num;
    rgb_panel->vsync_gpio_num = rgb_panel_config->vsync_gpio_num;
    rgb_panel->pclk_gpio_num = rgb_panel_config->pclk_gpio_num;
    rgb_panel->timings = rgb_panel_config->timings;
    rgb_panel->data_width = rgb_panel_config->data_width;
    rgb_panel->flags.disp_en_level = !rgb_panel_config->flags.disp_active_low;
    rgb_panel->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    // fill function table
    rgb_panel->base.del = rgb_panel_del;
    rgb_panel->base.reset = rgb_panel_reset;
    rgb_panel->base.init = rgb_panel_init;
    rgb_panel->base.draw_bitmap = rgb_panel_draw_bitmap;
    rgb_panel->base.disp_on_off = rgb_panel_disp_on_off;
    rgb_panel->base.invert_color = rgb_panel_invert_color;
    rgb_panel->base.mirror = rgb_panel_mirror;
    rgb_panel->base.swap_xy = rgb_panel_swap_xy;
    rgb_panel->base.set_gap = rgb_panel_set_gap;
    // return base class
    *ret_panel = &(rgb_panel->base);
    ESP_LOGD(TAG, "new rgb panel(%d) @%p, num_fbs=%zu, fb_size=%zu, bb0 @%p, bb1 @%p, bb_size=%zu",
             rgb_panel->panel_id, rgb_panel, rgb_panel->num_fbs, rgb_panel->fb_size,
             rgb_panel->bounce_buffer[0], rgb_panel->bounce_buffer[1], rgb_panel->bb_size);
    for (size_t i = 0; i < rgb_panel->num_fbs; i++) {
        ESP_LOGD(TAG, "fb[%zu] @%p", i, rgb_panel->fbs[i]);
    }
    return ESP_OK;

err:
    if (rgb_panel) {
        lcd_rgb_panel_destroy(rgb_panel);
    }
    return ret;
}

esp_err_t esp_lcd_rgb_panel_register_event_callbacks(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_callbacks_t *callbacks, void *user_ctx)
{
    ESP_RETURN_ON_FALSE(panel && callbacks, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
    if (callbacks->on_vsync) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_vsync), ESP_ERR_INVALID_ARG, TAG, "on_vsync callback not in IRAM");
    }
    if (callbacks->on_color_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_color_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_color_trans_done callback not in IRAM");
    }
    if (callbacks->on_frame_buf_complete) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_frame_buf_complete), ESP_ERR_INVALID_ARG, TAG, "on_frame_buf_complete callback not in IRAM");
    }
    if (callbacks->on_bounce_empty) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_bounce_empty), ESP_ERR_INVALID_ARG, TAG, "on_bounce_empty callback not in IRAM");
    }
    if (user_ctx) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_ctx), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE
    rgb_panel->on_vsync = callbacks->on_vsync;
    rgb_panel->on_color_trans_done = callbacks->on_color_trans_done;
    rgb_panel->on_frame_buf_complete = callbacks->on_frame_buf_complete;
    rgb_panel->on_bounce_empty = callbacks->on_bounce_empty;
    rgb_panel->user_ctx = user_ctx;
    return ESP_OK;
}

esp_err_t esp_lcd_rgb_panel_set_pclk(esp_lcd_panel_handle_t panel, uint32_t freq_hz)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    // the pclk frequency will be updated in the `LCD_LL_EVENT_VSYNC_END` event handler
    portENTER_CRITICAL(&rgb_panel->spinlock);
    rgb_panel->flags.need_update_pclk = true;
    rgb_panel->timings.pclk_hz = freq_hz;
    portEXIT_CRITICAL(&rgb_panel->spinlock);
    return ESP_OK;
}

esp_err_t esp_lcd_rgb_panel_restart(esp_lcd_panel_handle_t panel)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(rgb_panel->flags.stream_mode, ESP_ERR_INVALID_STATE, TAG, "not in stream mode");

    // the underlying restart job will be done in the `LCD_LL_EVENT_VSYNC_END` event handler
    portENTER_CRITICAL(&rgb_panel->spinlock);
    rgb_panel->flags.need_restart = true;
    portEXIT_CRITICAL(&rgb_panel->spinlock);
    return ESP_OK;
}

static void esp_lcd_rgb_panel_get_frame_buffer_v(esp_rgb_panel_t *rgb_panel, uint32_t fb_num, void **fb0, va_list args)
{
    void **fb_itor = fb0;
    for (int i = 0; i < fb_num; i++) {
        if (fb_itor) {
            *fb_itor = rgb_panel->fbs[i];
            if (i < fb_num - 1) {  /* Only get next arg if we're not at the last iteration */
                fb_itor = va_arg(args, void **);
            }
        }
    }
}

esp_err_t esp_lcd_rgb_panel_get_frame_buffer(esp_lcd_panel_handle_t panel, uint32_t fb_num, void **fb0, ...)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= rgb_panel->num_fbs, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");

    va_list args = {0};
    va_start(args, fb0);
    esp_lcd_rgb_panel_get_frame_buffer_v(rgb_panel, fb_num, fb0, args);
    va_end(args);

    return ESP_OK;
}

esp_err_t esp_lcd_rgb_panel_refresh(esp_lcd_panel_handle_t panel)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(!rgb_panel->flags.stream_mode, ESP_ERR_INVALID_STATE, TAG, "refresh on demand is not enabled");
    lcd_rgb_panel_start_transmission(rgb_panel);
    return ESP_OK;
}

esp_err_t esp_lcd_rgb_panel_set_yuv_conversion(esp_lcd_panel_handle_t panel, const esp_lcd_color_conv_yuv_config_t *config)
{
    ESP_RETURN_ON_FALSE(panel && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    lcd_hal_context_t *hal = &rgb_panel->hal;

    lcd_ll_set_yuv2rgb_convert_mode(hal->dev, rgb_panel->in_color_format, rgb_panel->out_color_format);

    // set conversion standard
    lcd_ll_set_yuv_convert_std(hal->dev, config->conv_std);
    // set color range
    lcd_ll_set_input_color_range(hal->dev, config->in_color_range);
    lcd_ll_set_output_color_range(hal->dev, config->out_color_range);
    // set conversion data width
    lcd_ll_set_convert_data_width(hal->dev, rgb_panel->data_width);
    return ESP_OK;
}

void *esp_lcd_rgb_alloc_draw_buffer(esp_lcd_panel_handle_t panel, size_t size, uint32_t caps)
{
    ESP_RETURN_ON_FALSE(panel, NULL, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    void *buf = NULL;
    // alloc from external memory
    if (caps & MALLOC_CAP_SPIRAM) {
        buf = heap_caps_aligned_calloc(rgb_panel->ext_mem_align, 1, size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    } else {
        buf = heap_caps_aligned_calloc(rgb_panel->int_mem_align, 1, size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    }
    return buf;
}

static esp_err_t rgb_panel_del(esp_lcd_panel_t *panel)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    int panel_id = rgb_panel->panel_id;
    ESP_RETURN_ON_ERROR(lcd_rgb_panel_destroy(rgb_panel), TAG, "destroy rgb panel(%d) failed", panel_id);
    ESP_LOGD(TAG, "del rgb panel(%d)", panel_id);
    return ESP_OK;
}

static esp_err_t rgb_panel_reset(esp_lcd_panel_t *panel)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    lcd_ll_fifo_reset(rgb_panel->hal.dev);
    lcd_ll_reset(rgb_panel->hal.dev);
    return ESP_OK;
}

static esp_err_t rgb_panel_init(esp_lcd_panel_t *panel)
{
    esp_err_t ret = ESP_OK;
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    // set pixel clock frequency
    hal_utils_clk_div_t lcd_clk_div = {};
    rgb_panel->timings.pclk_hz = lcd_hal_cal_pclk_freq(&rgb_panel->hal, rgb_panel->src_clk_hz, rgb_panel->timings.pclk_hz, &lcd_clk_div);
    PERIPH_RCC_ATOMIC() {
        lcd_ll_set_group_clock_coeff(rgb_panel->hal.dev, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator);
    }
    // pixel clock phase and polarity
    lcd_ll_set_clock_idle_level(rgb_panel->hal.dev, rgb_panel->timings.flags.pclk_idle_high);
    lcd_ll_set_pixel_clock_edge(rgb_panel->hal.dev, rgb_panel->timings.flags.pclk_active_neg);
    // enable RGB mode and set data width
    lcd_ll_enable_rgb_mode(rgb_panel->hal.dev, true);
    lcd_ll_set_dma_read_stride(rgb_panel->hal.dev, rgb_panel->data_width);
    // enable conversion if the input color format is different from the output color format
    lcd_ll_enable_color_convert(rgb_panel->hal.dev, rgb_panel->in_color_format != rgb_panel->out_color_format);
    // enable data phase only
    lcd_ll_set_phase_cycles(rgb_panel->hal.dev, 0, 0, 1);
    // number of data cycles is controlled by DMA buffer size
    lcd_ll_enable_output_always_on(rgb_panel->hal.dev, true);
    // configure HSYNC, VSYNC, DE signal idle state level
    lcd_ll_set_idle_level(rgb_panel->hal.dev, !rgb_panel->timings.flags.hsync_idle_low,
                          !rgb_panel->timings.flags.vsync_idle_low, rgb_panel->timings.flags.de_idle_high);
    // configure blank region timing
    lcd_ll_set_blank_cycles(rgb_panel->hal.dev, 1, 1); // RGB panel always has a front and back blank (porch region)
    size_t out_bits_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(rgb_panel->out_color_format);
    lcd_ll_set_horizontal_timing(rgb_panel->hal.dev, rgb_panel->timings.hsync_pulse_width,
                                 rgb_panel->timings.hsync_back_porch, rgb_panel->timings.h_res * out_bits_per_pixel / rgb_panel->data_width,
                                 rgb_panel->timings.hsync_front_porch);
    lcd_ll_set_vertical_timing(rgb_panel->hal.dev, rgb_panel->timings.vsync_pulse_width,
                               rgb_panel->timings.vsync_back_porch, rgb_panel->timings.v_res,
                               rgb_panel->timings.vsync_front_porch);
    // output hsync even in porch region
    lcd_ll_enable_output_hsync_in_porch_region(rgb_panel->hal.dev, true);
    // generate the hsync at the very beginning of line
    lcd_ll_set_hsync_position(rgb_panel->hal.dev, 0);
    // in stream mode, after finish one frame, the LCD controller will ask for data automatically from the DMA
    // DMA should prepare the next frame data within porch region
    lcd_ll_enable_auto_next_frame(rgb_panel->hal.dev, rgb_panel->flags.stream_mode);
    PERIPH_RCC_ATOMIC() {
        // trigger interrupt on the end of frame
        lcd_ll_enable_interrupt(rgb_panel->hal.dev, LCD_LL_EVENT_RGB, true);
    }
    // enable intr
    esp_intr_enable(rgb_panel->intr);
    // start transmission
    if (rgb_panel->flags.stream_mode) {
        lcd_rgb_panel_start_transmission(rgb_panel);
    }
    ESP_LOGD(TAG, "rgb panel(%d) start, pclk=%"PRIu32"Hz", rgb_panel->panel_id, rgb_panel->timings.pclk_hz);
    return ret;
}

static esp_err_t rgb_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(rgb_panel->num_fbs > 0, ESP_ERR_NOT_SUPPORTED, TAG, "no frame buffer installed");
    esp_lcd_rgb_panel_draw_buf_complete_cb_t cb = rgb_panel->on_color_trans_done;

    uint8_t *draw_buffer = (uint8_t *)color_data;
    size_t fb_size = rgb_panel->fb_size;
    int h_res = rgb_panel->timings.h_res;
    int v_res = rgb_panel->timings.v_res;
    int bytes_per_pixel = rgb_panel->fb_bits_per_pixel / 8;
    uint32_t bytes_per_line = bytes_per_pixel * h_res;

    // adjust the flush window by adding extra gap
    x_start += rgb_panel->x_gap;
    y_start += rgb_panel->y_gap;
    x_end += rgb_panel->x_gap;
    y_end += rgb_panel->y_gap;

    // clip to boundaries
    if (rgb_panel->rotate_mask & ROTATE_MASK_SWAP_XY) {
        x_start = MAX(x_start, 0);
        x_end = MIN(x_end, v_res);
        y_start = MAX(y_start, 0);
        y_end = MIN(y_end, h_res);
    } else {
        x_start = MAX(x_start, 0);
        x_end = MIN(x_end, h_res);
        y_start = MAX(y_start, 0);
        y_end = MIN(y_end, v_res);
    }

    // check if we want to copy the draw buffer to the internal frame buffer
    bool draw_buf_copy_to_fb = true;
    uint8_t draw_buf_fb_index = 0;
    for (int i = 0; i < rgb_panel->num_fbs; i++) {
        if (draw_buffer >= rgb_panel->fbs[i] && draw_buffer < rgb_panel->fbs[i] + fb_size) {
            draw_buf_fb_index = i;
            draw_buf_copy_to_fb = false;
            break;
        }
    }

    if (draw_buf_copy_to_fb) {
        // sync the draw buffer with the frame buffer by CPU copy
        ESP_LOGV(TAG, "copy draw buffer to frame buffer by CPU");
        uint8_t *fb = rgb_panel->fbs[rgb_panel->cur_fb_index];
        size_t bytes_to_flush = v_res * bytes_per_line;
        uint8_t *flush_ptr = fb;

        const uint8_t *from = (const uint8_t *)color_data;
        uint32_t copy_bytes_per_line = (x_end - x_start) * bytes_per_pixel;
        size_t offset = y_start * copy_bytes_per_line + x_start * bytes_per_pixel;
        uint8_t *to = fb;
        if (1 == bytes_per_pixel) {
            COPY_PIXEL_CODE_BLOCK(8)
        } else if (2 == bytes_per_pixel) {
            COPY_PIXEL_CODE_BLOCK(16)
        } else if (3 == bytes_per_pixel) {
            COPY_PIXEL_CODE_BLOCK(24)
        }
        // do memory sync only when the frame buffer is mounted to the DMA link list and behind the cache
        if (!rgb_panel->bb_size && rgb_panel->flags.fb_behind_cache) {
            esp_cache_msync(flush_ptr, bytes_to_flush, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        }
        // after the draw buffer finished copying, notify the user to recycle the draw buffer
        if (cb) {
            cb(&rgb_panel->base, NULL, rgb_panel->user_ctx);
        }
    } else {
        ESP_LOGV(TAG, "draw buffer is part of the frame buffer");
        // the new frame buffer index is changed
        rgb_panel->cur_fb_index = draw_buf_fb_index;
        // when this function is called, the frame buffer already reflects the draw buffer changes
        // if the frame buffer is also mounted to the DMA, we need to do the sync between them
        if (!rgb_panel->bb_size && rgb_panel->flags.fb_behind_cache) {
            uint8_t *cache_sync_start = rgb_panel->fbs[draw_buf_fb_index] + (y_start * h_res) * bytes_per_pixel;
            size_t cache_sync_size = (y_end - y_start) * bytes_per_line;
            esp_cache_msync(cache_sync_start, cache_sync_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        }
        // after the draw buffer finished copying, notify the user to recycle the draw buffer
        if (cb) {
            cb(&rgb_panel->base, NULL, rgb_panel->user_ctx);
        }
    }

    if (!rgb_panel->bb_size) {
        if (rgb_panel->flags.stream_mode) {
            for (int i = 0; i < rgb_panel->num_fbs; i++) {
                // Note, because of DMA prefetch, there's possibility that the old frame buffer might be sent out again
                // it's hard to know the time when the new frame buffer starts
                gdma_link_concat(rgb_panel->dma_fb_links[i], -1, rgb_panel->dma_fb_links[rgb_panel->cur_fb_index], 0);
            }

        }
    }
    return ESP_OK;
}

static esp_err_t rgb_panel_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    int panel_id = rgb_panel->panel_id;
    // inverting the data line by GPIO matrix
    for (int i = 0; i < rgb_panel->data_width; i++) {
        if (rgb_panel->data_gpio_nums[i] >= 0) {
            esp_rom_gpio_connect_out_signal(rgb_panel->data_gpio_nums[i], soc_lcd_rgb_signals[panel_id].data_sigs[i],
                                            invert_color_data, false);
        }
    }
    return ESP_OK;
}

static esp_err_t rgb_panel_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    rgb_panel->rotate_mask &= ~(ROTATE_MASK_MIRROR_X | ROTATE_MASK_MIRROR_Y);
    rgb_panel->rotate_mask |= (mirror_x << RGB_PANEL_MIRROR_X | mirror_y << RGB_PANEL_MIRROR_Y);
    return ESP_OK;
}

static esp_err_t rgb_panel_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    rgb_panel->rotate_mask &= ~(ROTATE_MASK_SWAP_XY);
    rgb_panel->rotate_mask |= swap_axes << RGB_PANEL_SWAP_XY;
    return ESP_OK;
}

static esp_err_t rgb_panel_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    rgb_panel->x_gap = x_gap;
    rgb_panel->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t rgb_panel_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    if (rgb_panel->disp_gpio_num < 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (!on_off) { // turn off screen
        gpio_set_level(rgb_panel->disp_gpio_num, !rgb_panel->flags.disp_en_level);
    } else { // turn on screen
        gpio_set_level(rgb_panel->disp_gpio_num, rgb_panel->flags.disp_en_level);
    }
    return ESP_OK;
}

static esp_err_t lcd_rgb_panel_configure_gpio(esp_rgb_panel_t *rgb_panel, const esp_lcd_rgb_panel_config_t *panel_config)
{
    uint64_t gpio_reserve_mask = 0;
    int panel_id = rgb_panel->panel_id;
    // Set the number of output data lines
    lcd_ll_set_data_wire_width(rgb_panel->hal.dev, panel_config->data_width);
    // connect peripheral signals via GPIO matrix
    for (size_t i = 0; i < panel_config->data_width; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->data_gpio_nums[i])) {
            gpio_matrix_output(panel_config->data_gpio_nums[i],
                               soc_lcd_rgb_signals[panel_id].data_sigs[i], false, false);
            gpio_reserve_mask |= (1ULL << panel_config->data_gpio_nums[i]);
        }
    }
    if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->hsync_gpio_num)) {
        gpio_matrix_output(panel_config->hsync_gpio_num,
                           soc_lcd_rgb_signals[panel_id].hsync_sig, false, false);
        gpio_reserve_mask |= (1ULL << panel_config->hsync_gpio_num);
    }
    if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->vsync_gpio_num)) {
        gpio_matrix_output(panel_config->vsync_gpio_num,
                           soc_lcd_rgb_signals[panel_id].vsync_sig, false, false);
        gpio_reserve_mask |= (1ULL << panel_config->vsync_gpio_num);
    }
    // PCLK may not be necessary in some cases (i.e. VGA output)
    if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->pclk_gpio_num)) {
        gpio_matrix_output(panel_config->pclk_gpio_num,
                           soc_lcd_rgb_signals[panel_id].pclk_sig, false, false);
        gpio_reserve_mask |= (1ULL << panel_config->pclk_gpio_num);
    }
    // DE signal might not be necessary for some RGB LCD
    if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->de_gpio_num)) {
        gpio_matrix_output(panel_config->de_gpio_num,
                           soc_lcd_rgb_signals[panel_id].de_sig, false, false);
        gpio_reserve_mask |= (1ULL << panel_config->de_gpio_num);
    }
    // disp enable GPIO is optional, it is a general purpose output GPIO
    if (GPIO_IS_VALID_OUTPUT_GPIO(panel_config->disp_gpio_num)) {
        gpio_matrix_output(panel_config->disp_gpio_num,
                           soc_lcd_rgb_signals[panel_id].disp_sig, false, false);
        gpio_reserve_mask |= (1ULL << panel_config->disp_gpio_num);
    }

    // reserve the GPIOs
    uint64_t busy_mask = esp_gpio_reserve(gpio_reserve_mask);
    uint64_t conflict_mask = busy_mask & gpio_reserve_mask;
    for (; conflict_mask > 0;) {
        uint8_t pos = __builtin_ctzll(conflict_mask);
        conflict_mask &= ~(1ULL << pos);
        ESP_LOGW(TAG, "GPIO %d is not usable, maybe is reserved by others", pos);
    }

    rgb_panel->gpio_reserve_mask = gpio_reserve_mask;
    return ESP_OK;
}

static void lcd_rgb_panel_release_gpio(esp_rgb_panel_t *rgb_panel)
{
    if (rgb_panel->gpio_reserve_mask) {
        // disconnect the GPIOs from the LCD signals
        for (size_t i = 0; i < rgb_panel->data_width; i++) {
            if (rgb_panel->data_gpio_nums[i] >= 0) {
                gpio_output_disable(rgb_panel->data_gpio_nums[i]);
            }
        }
        if (rgb_panel->hsync_gpio_num >= 0) {
            gpio_output_disable(rgb_panel->hsync_gpio_num);
        }
        if (rgb_panel->vsync_gpio_num >= 0) {
            gpio_output_disable(rgb_panel->vsync_gpio_num);
        }
        // PCLK may not be necessary in some cases (i.e. VGA output)
        if (rgb_panel->pclk_gpio_num >= 0) {
            gpio_output_disable(rgb_panel->pclk_gpio_num);
        }
        // DE signal might not be necessary for some RGB LCD
        if (rgb_panel->de_gpio_num >= 0) {
            gpio_output_disable(rgb_panel->de_gpio_num);
        }
        // disp enable GPIO is optional, it is a general purpose output GPIO
        if (rgb_panel->disp_gpio_num >= 0) {
            gpio_output_disable(rgb_panel->disp_gpio_num);
        }
        // release the IOs so they can be used by others again
        esp_gpio_revoke(rgb_panel->gpio_reserve_mask);
    }
}

static esp_err_t lcd_rgb_panel_select_clock_src(esp_rgb_panel_t *rgb_panel, lcd_clock_source_t clk_src)
{
    // get clock source frequency
    uint32_t src_clk_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz),
                        TAG, "get clock source frequency failed");
    rgb_panel->src_clk_hz = src_clk_hz;
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true), TAG, "clock source enable failed");
    PERIPH_RCC_ATOMIC() {
        lcd_ll_select_clk_src(rgb_panel->hal.dev, clk_src);
    }

    // create pm lock based on different clock source
#if CONFIG_PM_ENABLE
    // clock sources like PLL and XTAL will be turned off in light sleep, so basically a NO_LIGHT_SLEEP lock is sufficient
    esp_pm_lock_type_t lock_type = ESP_PM_NO_LIGHT_SLEEP;
#if CONFIG_IDF_TARGET_ESP32P4
    // use CPU_MAX lock to ensure PSRAM bandwidth and usability during DFS
    lock_type = ESP_PM_CPU_FREQ_MAX;
#endif
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(lock_type, 0, "rgb_panel", &rgb_panel->pm_lock), TAG, "create pm lock failed");
    // hold the lock during the whole lifecycle of RGB panel
    esp_pm_lock_acquire(rgb_panel->pm_lock);
    ESP_LOGD(TAG, "installed pm lock and hold the lock during the whole panel lifecycle");
#endif

    return ESP_OK;
}

static IRAM_ATTR bool lcd_rgb_panel_fill_bounce_buffer(esp_rgb_panel_t *panel, uint8_t *buffer)
{
    bool need_yield = false;
    int bytes_per_pixel = panel->fb_bits_per_pixel / 8;
    if (unlikely(panel->num_fbs == 0)) {
        // driver doesn't maintain a frame buffer, so ask the user to fill the bounce buffer
        if (panel->on_bounce_empty) {
            if (panel->on_bounce_empty(&panel->base, buffer, panel->bounce_pos_px, panel->bb_size, panel->user_ctx)) {
                need_yield = true;
            }
        }
    } else {
        // copy partial frame buffer to the bounce buffer
        // Note: if the frame buffer is behind a cache, and the cache is disabled, crash would happen here when auto write back happens
        memcpy(buffer, &panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel], panel->bb_size);
    }
    // do memory sync if the bounce buffer is behind the cache
    if (panel->flags.bb_behind_cache) {
        esp_cache_msync(buffer, panel->bb_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    panel->bounce_pos_px += panel->bb_size / bytes_per_pixel;
    // If the bounce pos is larger than the frame buffer size, wrap around so the next isr starts pre-loading the next frame.
    if (panel->bounce_pos_px >= panel->fb_size / bytes_per_pixel) {
        panel->bounce_pos_px = 0;
        panel->bb_fb_index = panel->cur_fb_index;
        esp_lcd_rgb_panel_frame_buf_complete_cb_t cb = panel->on_frame_buf_complete;
        if (cb) {
            if (cb(&panel->base, NULL, panel->user_ctx)) {
                need_yield = true;
            }
        }
    }

    // Preload the next bit of buffer to the cache memory, this can improve the performance
    if (panel->num_fbs > 0 && panel->flags.fb_behind_cache) {
#if CONFIG_IDF_TARGET_ESP32S3
        Cache_Start_DCache_Preload((uint32_t)&panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel],
                                   panel->bb_size, 0);
#elif CONFIG_IDF_TARGET_ESP32P4
        Cache_Start_L2_Cache_Preload((uint32_t)&panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel],
                                     panel->bb_size, 0);
#else
#error "Unsupported target"
#endif
    }
    return need_yield;
}

static IRAM_ATTR bool lcd_rgb_panel_eof_handler(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    esp_rgb_panel_t *rgb_panel = (esp_rgb_panel_t *)user_data;

    if (rgb_panel->bb_size) {
        // in bounce buffer mode, the DMA EOF means time to fill the finished bounce buffer
        // Figure out which bounce buffer to write to
        portENTER_CRITICAL_ISR(&rgb_panel->spinlock);
        int bb = rgb_panel->bb_eof_count % RGB_LCD_PANEL_BOUNCE_BUF_NUM;
        rgb_panel->bb_eof_count++;
        portEXIT_CRITICAL_ISR(&rgb_panel->spinlock);
        need_yield = lcd_rgb_panel_fill_bounce_buffer(rgb_panel, rgb_panel->bounce_buffer[bb]);
    } else {
        // if not bounce buffer, the DMA EOF event means the end of a frame has been sent out to the LCD controller
        if (rgb_panel->on_frame_buf_complete) {
            if (rgb_panel->on_frame_buf_complete(&rgb_panel->base, NULL, rgb_panel->user_ctx)) {
                need_yield = true;
            }
        }
    }
    return need_yield;
}

static esp_err_t lcd_rgb_create_dma_channel(esp_rgb_panel_t *rgb_panel)
{
    // alloc DMA channel and connect to LCD peripheral
    gdma_channel_alloc_config_t dma_chan_config = {
#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(LCD_GDMA_NEW_CHANNEL(&dma_chan_config, &rgb_panel->dma_chan, NULL), TAG, "alloc DMA channel failed");
    gdma_connect(rgb_panel->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_LCD, 0));

    // configure DMA strategy
    gdma_strategy_config_t dma_strategy = {
        .eof_till_data_popped = false,
    };
    gdma_apply_strategy(rgb_panel->dma_chan, &dma_strategy);

    // configure DMA transfer
    gdma_transfer_config_t trans_cfg = {
        .max_data_burst_size = rgb_panel->dma_burst_size,
        .access_ext_mem = rgb_panel->flags.fb_in_psram,
    };
    ESP_RETURN_ON_ERROR(gdma_config_transfer(rgb_panel->dma_chan, &trans_cfg), TAG, "config DMA transfer failed");
    // get the memory alignment required by the DMA
    gdma_get_alignment_constraints(rgb_panel->dma_chan, &rgb_panel->int_mem_align, &rgb_panel->ext_mem_align);

    // register DMA EOF callback
    gdma_tx_event_callbacks_t cbs = {
        .on_trans_eof = lcd_rgb_panel_eof_handler,
    };
    ESP_RETURN_ON_ERROR(gdma_register_tx_event_callbacks(rgb_panel->dma_chan, &cbs, rgb_panel), TAG, "register DMA EOF callback failed");

    return ESP_OK;
}

#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
// If we restart GDMA, the data sent to the LCD peripheral needs to start LCD_FIFO_PRESERVE_SIZE_PX pixels after the FB start
// so we use a dedicated DMA link (called restart link) to restart the transaction
#define LCD_FIFO_PRESERVE_SIZE_PX (LCD_LL_FIFO_DEPTH + 1)
#endif

static esp_err_t lcd_rgb_panel_init_trans_link(esp_rgb_panel_t *rgb_panel)
{
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
    // the restart link shares the same buffer with the frame/bounce buffer but start from a different offset
    int restart_skip_bytes = LCD_FIFO_PRESERVE_SIZE_PX * (rgb_panel->fb_bits_per_pixel / 8);
#endif
    if (rgb_panel->bb_size) {
        // DMA is used to convey the bounce buffer
        size_t buffer_alignment = rgb_panel->int_mem_align;
        size_t num_dma_nodes_per_bounce_buffer = esp_dma_calculate_node_count(rgb_panel->bb_size, buffer_alignment, LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
        gdma_link_list_config_t link_cfg = {
            .item_alignment = LCD_GDMA_DESCRIPTOR_ALIGN,
            .num_items = num_dma_nodes_per_bounce_buffer * RGB_LCD_PANEL_BOUNCE_BUF_NUM,
            .flags = {
                .check_owner = true,
            }
        };
        ESP_RETURN_ON_ERROR(gdma_new_link_list(&link_cfg, &rgb_panel->dma_bb_link), TAG, "create bounce buffer DMA link failed");
        // mount bounce buffers to the DMA link list
        gdma_buffer_mount_config_t mount_cfgs[RGB_LCD_PANEL_BOUNCE_BUF_NUM] = {0};
        for (int i = 0; i < RGB_LCD_PANEL_BOUNCE_BUF_NUM; i++) {
            mount_cfgs[i].buffer = rgb_panel->bounce_buffer[i];
            mount_cfgs[i].buffer_alignment = buffer_alignment;
            mount_cfgs[i].length = rgb_panel->bb_size;
            mount_cfgs[i].flags.mark_eof = true; // we use the DMA EOF interrupt to copy the frame buffer (partially) to the bounce buffer
        }
        ESP_RETURN_ON_ERROR(gdma_link_mount_buffers(rgb_panel->dma_bb_link, 0, mount_cfgs, RGB_LCD_PANEL_BOUNCE_BUF_NUM, NULL),
                            TAG, "mount DMA bounce buffers failed");
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
        // create restart link
        gdma_link_list_config_t restart_link_cfg = {
            .item_alignment = LCD_GDMA_DESCRIPTOR_ALIGN,
            .num_items = 1, // the restart link only contains one node
            .flags = {
                .check_owner = true,
            }
        };
        ESP_RETURN_ON_ERROR(gdma_new_link_list(&restart_link_cfg, &rgb_panel->dma_restart_link), TAG, "create DMA restart link list failed");
        gdma_buffer_mount_config_t restart_buffer_mount_cfg = {
            .buffer = rgb_panel->bounce_buffer[0] + restart_skip_bytes,
            .buffer_alignment = buffer_alignment,
            .length = MIN(LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE, rgb_panel->bb_size) - restart_skip_bytes,
        };
        ESP_RETURN_ON_ERROR(gdma_link_mount_buffers(rgb_panel->dma_restart_link, 0, &restart_buffer_mount_cfg, 1, NULL),
                            TAG, "mount DMA restart buffer failed");

        // Magic here: we use the restart link to restart the bounce buffer link list, so concat them
        gdma_link_concat(rgb_panel->dma_restart_link, 0, rgb_panel->dma_bb_link, 1);
#endif
    } else {
        // DMA is used to convey the frame buffer
        size_t buffer_alignment = rgb_panel->flags.fb_in_psram ? rgb_panel->ext_mem_align : rgb_panel->int_mem_align;
        uint32_t num_dma_nodes = esp_dma_calculate_node_count(rgb_panel->fb_size, buffer_alignment, LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
        gdma_link_list_config_t link_cfg = {
            .item_alignment = LCD_GDMA_DESCRIPTOR_ALIGN,
            .num_items = num_dma_nodes,
            .flags = {
                .check_owner = true,
            },
        };
        gdma_buffer_mount_config_t mount_cfg = {
            .length = rgb_panel->fb_size,
            .flags = {
                .mark_final = rgb_panel->flags.stream_mode ? GDMA_FINAL_LINK_TO_DEFAULT : GDMA_FINAL_LINK_TO_NULL,
                .mark_eof = true,
            },
        };
        for (size_t i = 0; i < rgb_panel->num_fbs; i++) {
            ESP_RETURN_ON_ERROR(gdma_new_link_list(&link_cfg, &rgb_panel->dma_fb_links[i]), TAG, "create frame buffer DMA link failed");
            // mount bounce buffers to the DMA link list
            mount_cfg.buffer = rgb_panel->fbs[i];
            mount_cfg.buffer_alignment = buffer_alignment;
            ESP_RETURN_ON_ERROR(gdma_link_mount_buffers(rgb_panel->dma_fb_links[i], 0, &mount_cfg, 1, NULL),
                                TAG, "mount DMA frame buffer failed");
        }
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
        // create restart link
        gdma_link_list_config_t restart_link_cfg = {
            .item_alignment = LCD_GDMA_DESCRIPTOR_ALIGN,
            .num_items = 1, // the restart link only contains one node
            .flags = {
                .check_owner = true,
            }
        };
        ESP_RETURN_ON_ERROR(gdma_new_link_list(&restart_link_cfg, &rgb_panel->dma_restart_link), TAG, "create DMA restart link list failed");
        gdma_buffer_mount_config_t restart_buffer_mount_cfg = {
            .buffer = rgb_panel->fbs[0] + restart_skip_bytes,
            .buffer_alignment = buffer_alignment,
            .length = MIN(LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE, rgb_panel->fb_size) - restart_skip_bytes,
            .flags.bypass_buffer_align_check = true, // the restart buffer may doesn't match the buffer alignment but it doesn't really matter in this case
        };
        ESP_RETURN_ON_ERROR(gdma_link_mount_buffers(rgb_panel->dma_restart_link, 0, &restart_buffer_mount_cfg, 1, NULL),
                            TAG, "mount DMA restart buffer failed");

        // Magic here: we use the restart link to restart the frame buffer link list, so concat them
        gdma_link_concat(rgb_panel->dma_restart_link, 0, rgb_panel->dma_fb_links[0], 1);
#endif
    }

    return ESP_OK;
}

// reset the GDMA channel every VBlank to stop permanent desyncs from happening.
// Note that this fix can lead to single-frame desyncs itself, as in: if this interrupt
// is late enough, the display will shift as the LCD controller already read out the
// first data bytes, and resetting DMA will re-send those. However, the single-frame
// desync this leads to is preferable to the permanent desync that could otherwise
// happen. It's also not super-likely as this interrupt has the entirety of the VBlank
// time to reset DMA.
static IRAM_ATTR void lcd_rgb_panel_try_restart_transmission(esp_rgb_panel_t *panel)
{
    int bb_size_px = panel->bb_size / (panel->fb_bits_per_pixel / 8);
    bool do_restart = false;
#if CONFIG_LCD_RGB_RESTART_IN_VSYNC
    do_restart = true;
#else
    portENTER_CRITICAL_ISR(&panel->spinlock);
    if (panel->flags.need_restart) {
        panel->flags.need_restart = false;
        do_restart = true;
    }
    if (panel->bb_eof_count < panel->expect_eof_count) {
        do_restart = true;
    }
    panel->bb_eof_count = 0;
    portEXIT_CRITICAL_ISR(&panel->spinlock);
#endif // CONFIG_LCD_RGB_RESTART_IN_VSYNC

    if (!do_restart) {
        return;
    }

    if (panel->bb_size) {
        // Catch de-synced frame buffer and reset if needed.
        if (panel->bounce_pos_px > bb_size_px * 2) {
            panel->bounce_pos_px = 0;
        }
        // Pre-fill bounce buffer 0, if the EOF ISR didn't do that already
        if (panel->bounce_pos_px < bb_size_px) {
            lcd_rgb_panel_fill_bounce_buffer(panel, panel->bounce_buffer[0]);
        }
    }

    lcd_ll_fifo_reset(panel->hal.dev);
    gdma_reset(panel->dma_chan);
#if RGB_LCD_NEEDS_SEPARATE_RESTART_LINK
    // restart the DMA by a special DMA node
    gdma_start(panel->dma_chan, gdma_link_get_head_addr(panel->dma_restart_link));
#else
    if (panel->bb_size) {
        gdma_start(panel->dma_chan, gdma_link_get_head_addr(panel->dma_bb_link));
    } else {
        gdma_start(panel->dma_chan, gdma_link_get_head_addr(panel->dma_fb_links[panel->cur_fb_index]));
    }
#endif

    if (panel->bb_size) {
        // Fill 2nd bounce buffer while 1st is being sent out, if needed.
        if (panel->bounce_pos_px < bb_size_px * 2) {
            lcd_rgb_panel_fill_bounce_buffer(panel, panel->bounce_buffer[1]);
        }
    }

}

static void lcd_rgb_panel_start_transmission(esp_rgb_panel_t *rgb_panel)
{
    // reset FIFO of DMA and LCD, in case there remains old frame data
    gdma_reset(rgb_panel->dma_chan);
    lcd_ll_stop(rgb_panel->hal.dev);
    lcd_ll_reset(rgb_panel->hal.dev);
    lcd_ll_fifo_reset(rgb_panel->hal.dev);

    // pre-fill bounce buffers if needed
    if (rgb_panel->bb_size) {
        rgb_panel->bounce_pos_px = 0;
        lcd_rgb_panel_fill_bounce_buffer(rgb_panel, rgb_panel->bounce_buffer[0]);
        lcd_rgb_panel_fill_bounce_buffer(rgb_panel, rgb_panel->bounce_buffer[1]);
    }

    // the start of DMA should be prior to the start of LCD engine
    if (rgb_panel->bb_size) {
        gdma_start(rgb_panel->dma_chan, gdma_link_get_head_addr(rgb_panel->dma_bb_link));
    } else {
        gdma_start(rgb_panel->dma_chan, gdma_link_get_head_addr(rgb_panel->dma_fb_links[rgb_panel->cur_fb_index]));
    }
    // delay 1us is sufficient for DMA to pass data to LCD FIFO
    // in fact, this is only needed when LCD pixel clock is set too high
    esp_rom_delay_us(1);
    // start LCD engine
    lcd_ll_start(rgb_panel->hal.dev);
}

IRAM_ATTR static void lcd_rgb_panel_try_update_pclk(esp_rgb_panel_t *rgb_panel)
{
    hal_utils_clk_div_t lcd_clk_div = {};
    portENTER_CRITICAL_ISR(&rgb_panel->spinlock);
    if (unlikely(rgb_panel->flags.need_update_pclk)) {
        rgb_panel->flags.need_update_pclk = false;
        rgb_panel->timings.pclk_hz = lcd_hal_cal_pclk_freq(&rgb_panel->hal, rgb_panel->src_clk_hz, rgb_panel->timings.pclk_hz, &lcd_clk_div);
        PERIPH_RCC_ATOMIC() {
            lcd_ll_set_group_clock_coeff(rgb_panel->hal.dev, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator);
        }
    }
    portEXIT_CRITICAL_ISR(&rgb_panel->spinlock);
}

IRAM_ATTR static void rgb_lcd_default_isr_handler(void *args)
{
    esp_rgb_panel_t *rgb_panel = (esp_rgb_panel_t *)args;
    bool need_yield = false;

    // clear the interrupt status
    uint32_t intr_status = lcd_ll_get_interrupt_status(rgb_panel->hal.dev);
    lcd_ll_clear_interrupt_status(rgb_panel->hal.dev, intr_status);

#if LCD_LL_EVENT_UNDERRUN
    if (intr_status & LCD_LL_EVENT_UNDERRUN) {
        ESP_EARLY_LOGE(TAG, "LCD underrun");
    }
#endif

    // VSYNC event happened
    if (intr_status & LCD_LL_EVENT_VSYNC_END) {
        // call user registered callback
        if (rgb_panel->on_vsync) {
            if (rgb_panel->on_vsync(&rgb_panel->base, NULL, rgb_panel->user_ctx)) {
                need_yield = true;
            }
        }

        // check whether to update the PCLK frequency, it should be safe to update the PCLK frequency in the VSYNC interrupt
        lcd_rgb_panel_try_update_pclk(rgb_panel);

        if (rgb_panel->flags.stream_mode) {
            // check whether to restart the transmission
            lcd_rgb_panel_try_restart_transmission(rgb_panel);
        }

    }
    // yield if needed
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

#if CONFIG_LCD_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void rgb_lcd_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
