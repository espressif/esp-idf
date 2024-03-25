/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "esp_clk_tree.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "esp_private/gdma.h"
#include "driver/gpio.h"
#include "esp_bit_defs.h"
#include "esp_private/periph_ctrl.h"
#include "esp_psram.h"
#include "esp_lcd_common.h"
#include "esp_memory_utils.h"
#include "soc/lcd_periph.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "hal/gdma_ll.h"
#include "rom/cache.h"
#include "esp_cache.h"

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
#define LCD_RGB_INTR_ALLOC_FLAGS     (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define LCD_RGB_INTR_ALLOC_FLAGS     ESP_INTR_FLAG_INTRDISABLED
#endif

#define RGB_LCD_PANEL_MAX_FB_NUM         3 // maximum supported frame buffer number
#define RGB_LCD_PANEL_BOUNCE_BUF_NUM     2 // bounce buffer number
#define RGB_LCD_PANEL_DMA_LINKS_REPLICA  MAX(RGB_LCD_PANEL_MAX_FB_NUM, RGB_LCD_PANEL_BOUNCE_BUF_NUM)

#define RGB_PANEL_SWAP_XY  0
#define RGB_PANEL_MIRROR_Y 1
#define RGB_PANEL_MIRROR_X 2

typedef enum {
    ROTATE_MASK_SWAP_XY = BIT(RGB_PANEL_SWAP_XY),
    ROTATE_MASK_MIRROR_Y = BIT(RGB_PANEL_MIRROR_Y),
    ROTATE_MASK_MIRROR_X = BIT(RGB_PANEL_MIRROR_X),
} panel_rotate_mask_t;

static const char *TAG = "lcd_panel.rgb";

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
static esp_err_t lcd_rgb_panel_select_clock_src(esp_rgb_panel_t *panel, lcd_clock_source_t clk_src);
static esp_err_t lcd_rgb_panel_create_trans_link(esp_rgb_panel_t *panel);
static esp_err_t lcd_rgb_panel_configure_gpio(esp_rgb_panel_t *panel, const esp_lcd_rgb_panel_config_t *panel_config);
static void lcd_rgb_panel_start_transmission(esp_rgb_panel_t *rgb_panel);
static void lcd_default_isr_handler(void *args);

struct esp_rgb_panel_t {
    esp_lcd_panel_t base;  // Base class of generic lcd panel
    int panel_id;          // LCD panel ID
    lcd_hal_context_t hal; // Hal layer object
    size_t data_width;     // Number of data lines
    size_t fb_bits_per_pixel; // Frame buffer color depth, in bpp
    size_t num_fbs;           // Number of frame buffers
    size_t output_bits_per_pixel; // Color depth seen from the output data line. Default to fb_bits_per_pixel, but can be changed by YUV-RGB conversion
    size_t sram_trans_align;  // Alignment for framebuffer that allocated in SRAM
    size_t psram_trans_align; // Alignment for framebuffer that allocated in PSRAM
    int disp_gpio_num;     // Display control GPIO, which is used to perform action like "disp_off"
    intr_handle_t intr;    // LCD peripheral interrupt handle
    esp_pm_lock_handle_t pm_lock; // Power management lock
    size_t num_dma_nodes;  // Number of DMA descriptors that used to carry the frame buffer
    uint8_t *fbs[RGB_LCD_PANEL_MAX_FB_NUM]; // Frame buffers
    uint8_t cur_fb_index;  // Current frame buffer index
    uint8_t bb_fb_index;  // Current frame buffer index which used by bounce buffer
    size_t fb_size;        // Size of frame buffer, in bytes
    int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; // GPIOs used for data lines, we keep these GPIOs for action like "invert_color"
    uint32_t src_clk_hz;   // Peripheral source clock resolution
    esp_lcd_rgb_timing_t timings;   // RGB timing parameters (e.g. pclk, sync pulse, porch width)
    size_t bb_size;                 // Size of the bounce buffer, in bytes. If not-zero, the driver uses two bounce buffers allocated from internal memory
    int bounce_pos_px;              // Position in whatever source material is used for the bounce buffer, in pixels
    uint8_t *bounce_buffer[RGB_LCD_PANEL_BOUNCE_BUF_NUM]; // Pointer to the bounce buffers
    size_t bb_eof_count;            // record the number we received the DMA EOF event, compare with `expect_eof_count` in the VSYNC_END ISR
    size_t expect_eof_count;        // record the number of DMA EOF event we expected to receive
    gdma_channel_handle_t dma_chan; // DMA channel handle
    esp_lcd_rgb_panel_vsync_cb_t on_vsync; // VSYNC event callback
    esp_lcd_rgb_panel_bounce_buf_fill_cb_t on_bounce_empty; // callback used to fill a bounce buffer rather than copying from the frame buffer
    esp_lcd_rgb_panel_bounce_buf_finish_cb_t on_bounce_frame_finish; // callback used to notify when the bounce buffer finish copying the entire frame
    void *user_ctx;                 // Reserved user's data of callback functions
    int x_gap;                      // Extra gap in x coordinate, it's used when calculate the flush window
    int y_gap;                      // Extra gap in y coordinate, it's used when calculate the flush window
    portMUX_TYPE spinlock;          // to protect panel specific resource from concurrent access (e.g. between task and ISR)
    int lcd_clk_flags;              // LCD clock calculation flags
    int rotate_mask;                // panel rotate_mask mask, Or'ed of `panel_rotate_mask_t`
    struct {
        uint32_t disp_en_level: 1;       // The level which can turn on the screen by `disp_gpio_num`
        uint32_t stream_mode: 1;         // If set, the LCD transfers data continuously, otherwise, it stops refreshing the LCD when transaction done
        uint32_t fb_in_psram: 1;         // Whether the frame buffer is in PSRAM
        uint32_t need_update_pclk: 1;    // Whether to update the PCLK before start a new transaction
        uint32_t need_restart: 1;        // Whether to restart the LCD controller and the DMA
        uint32_t bb_invalidate_cache: 1; // Whether to do cache invalidation in bounce buffer mode
    } flags;
    dma_descriptor_t *dma_links[RGB_LCD_PANEL_DMA_LINKS_REPLICA]; // fbs[0] <-> dma_links[0], fbs[1] <-> dma_links[1], etc
    dma_descriptor_t dma_restart_node; // DMA descriptor used to restart the transfer
    dma_descriptor_t dma_nodes[];      // DMA descriptors pool
};

static esp_err_t lcd_rgb_panel_alloc_frame_buffers(const esp_lcd_rgb_panel_config_t *rgb_panel_config, esp_rgb_panel_t *rgb_panel)
{
    bool fb_in_psram = false;
    size_t psram_trans_align = rgb_panel_config->psram_trans_align ? rgb_panel_config->psram_trans_align : 64;
    size_t sram_trans_align = rgb_panel_config->sram_trans_align ? rgb_panel_config->sram_trans_align : 4;
    rgb_panel->psram_trans_align = psram_trans_align;
    rgb_panel->sram_trans_align = sram_trans_align;

    // alloc frame buffer
    if (rgb_panel->num_fbs > 0) {
        // fb_in_psram is only an option, if there's no PSRAM on board, we fallback to alloc from SRAM
        if (rgb_panel_config->flags.fb_in_psram) {
#if CONFIG_SPIRAM_USE_MALLOC || CONFIG_SPIRAM_USE_CAPS_ALLOC
            if (esp_psram_is_initialized()) {
                fb_in_psram = true;
            }
#endif
        }
        for (int i = 0; i < rgb_panel->num_fbs; i++) {
            if (fb_in_psram) {
                // the low level malloc function will help check the validation of alignment
                rgb_panel->fbs[i] = heap_caps_aligned_calloc(psram_trans_align, 1, rgb_panel->fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
                ESP_RETURN_ON_FALSE(rgb_panel->fbs[i], ESP_ERR_NO_MEM, TAG, "no mem for frame buffer");
                // calloc not only allocates but also zero's the buffer. We have to make sure this is
                // properly committed to the PSRAM, otherwise all sorts of visual corruption will happen.
                ESP_RETURN_ON_ERROR(esp_cache_msync(rgb_panel->fbs[i], rgb_panel->fb_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M), TAG, "cache write back failed");
            } else {
                rgb_panel->fbs[i] = heap_caps_aligned_calloc(sram_trans_align, 1, rgb_panel->fb_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
                ESP_RETURN_ON_FALSE(rgb_panel->fbs[i], ESP_ERR_NO_MEM, TAG, "no mem for frame buffer");
            }
        }
    }

    // alloc bounce buffer
    if (rgb_panel->bb_size) {
        for (int i = 0; i < RGB_LCD_PANEL_BOUNCE_BUF_NUM; i++) {
            // bounce buffer must come from SRAM
            rgb_panel->bounce_buffer[i] = heap_caps_aligned_calloc(sram_trans_align, 1, rgb_panel->bb_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
            ESP_RETURN_ON_FALSE(rgb_panel->bounce_buffer[i], ESP_ERR_NO_MEM, TAG, "no mem for bounce buffer");
        }
    }
    rgb_panel->cur_fb_index = 0;
    rgb_panel->bb_fb_index = 0;
    rgb_panel->flags.fb_in_psram = fb_in_psram;

    return ESP_OK;
}

static esp_err_t lcd_rgb_panel_destroy(esp_rgb_panel_t *rgb_panel)
{
    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_enable_clock(rgb_panel->hal.dev, false);
    }
    if (rgb_panel->panel_id >= 0) {
        PERIPH_RCC_RELEASE_ATOMIC(lcd_periph_signals.panels[rgb_panel->panel_id].module, ref_count) {
            if (ref_count == 0) {
                lcd_ll_enable_bus_clock(rgb_panel->panel_id, false);
            }
        }
        lcd_com_remove_device(LCD_COM_DEVICE_TYPE_RGB, rgb_panel->panel_id);
    }
    for (size_t i = 0; i < rgb_panel->num_fbs; i++) {
        if (rgb_panel->fbs[i]) {
            free(rgb_panel->fbs[i]);
        }
    }
    if (rgb_panel->bounce_buffer[0]) {
        free(rgb_panel->bounce_buffer[0]);
    }
    if (rgb_panel->bounce_buffer[1]) {
        free(rgb_panel->bounce_buffer[1]);
    }
    if (rgb_panel->dma_chan) {
        gdma_disconnect(rgb_panel->dma_chan);
        gdma_del_channel(rgb_panel->dma_chan);
    }
    if (rgb_panel->intr) {
        esp_intr_free(rgb_panel->intr);
    }
    if (rgb_panel->pm_lock) {
        esp_pm_lock_release(rgb_panel->pm_lock);
        esp_pm_lock_delete(rgb_panel->pm_lock);
    }
    free(rgb_panel);
    return ESP_OK;
}

esp_err_t esp_lcd_new_rgb_panel(const esp_lcd_rgb_panel_config_t *rgb_panel_config, esp_lcd_panel_handle_t *ret_panel)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    esp_rgb_panel_t *rgb_panel = NULL;
    ESP_RETURN_ON_FALSE(rgb_panel_config && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid parameter");
    size_t data_width = rgb_panel_config->data_width;
    ESP_RETURN_ON_FALSE((data_width >= 8) && (data_width <= SOC_LCD_RGB_DATA_WIDTH) && ((data_width & (data_width - 1)) == 0), ESP_ERR_INVALID_ARG,
                        TAG, "unsupported data width %d", data_width);
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->flags.double_fb && rgb_panel_config->flags.no_fb),
                        ESP_ERR_INVALID_ARG, TAG, "double_fb conflicts with no_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->num_fbs > 0 && rgb_panel_config->num_fbs != 2 && rgb_panel_config->flags.double_fb),
                        ESP_ERR_INVALID_ARG, TAG, "num_fbs conflicts with double_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->num_fbs > 0 && rgb_panel_config->flags.no_fb),
                        ESP_ERR_INVALID_ARG, TAG, "num_fbs conflicts with no_fb");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->flags.no_fb && rgb_panel_config->bounce_buffer_size_px == 0),
                        ESP_ERR_INVALID_ARG, TAG, "must set bounce buffer if there's no frame buffer");
    ESP_RETURN_ON_FALSE(!(rgb_panel_config->flags.refresh_on_demand && rgb_panel_config->bounce_buffer_size_px),
                        ESP_ERR_INVALID_ARG, TAG, "refresh on demand is not supported under bounce buffer mode");

    // determine number of framebuffers
    size_t num_fbs = 1;
    if (rgb_panel_config->flags.no_fb) {
        num_fbs = 0;
    } else if (rgb_panel_config->flags.double_fb) {
        num_fbs = 2;
    } else if (rgb_panel_config->num_fbs > 0) {
        num_fbs = rgb_panel_config->num_fbs;
    }
    ESP_RETURN_ON_FALSE(num_fbs <= RGB_LCD_PANEL_MAX_FB_NUM, ESP_ERR_INVALID_ARG, TAG, "too many frame buffers");

    // bpp defaults to the number of data lines, but for serial RGB interface, they're not equal
    // e.g. for serial RGB 8-bit interface, data lines are 8, whereas the bpp is 24 (RGB888)
    size_t fb_bits_per_pixel = data_width;
    if (rgb_panel_config->bits_per_pixel) { // override bpp if it's set
        fb_bits_per_pixel = rgb_panel_config->bits_per_pixel;
    }
    // calculate buffer size
    size_t fb_size = rgb_panel_config->timings.h_res * rgb_panel_config->timings.v_res * fb_bits_per_pixel / 8;
    size_t bb_size = rgb_panel_config->bounce_buffer_size_px * fb_bits_per_pixel / 8;
    size_t expect_bb_eof_count = 0;
    if (bb_size) {
        // we want the bounce can always end in the second buffer
        ESP_RETURN_ON_FALSE(fb_size % (2 * bb_size) == 0, ESP_ERR_INVALID_ARG, TAG,
                            "fb size must be even multiple of bounce buffer size");
        expect_bb_eof_count = fb_size / bb_size;
    }

    // calculate the number of DMA descriptors
    size_t num_dma_nodes = 0;
    if (bb_size) {
        // in bounce buffer mode, DMA is used to convey the bounce buffer, not the frame buffer.
        // frame buffer is copied to bounce buffer by CPU
        num_dma_nodes = (bb_size + DMA_DESCRIPTOR_BUFFER_MAX_SIZE - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
    } else {
        // Not bounce buffer mode, DMA descriptors need to fit the entire frame buffer
        num_dma_nodes = (fb_size + DMA_DESCRIPTOR_BUFFER_MAX_SIZE - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
    }

    // DMA descriptors must be placed in internal SRAM (requested by DMA)
    rgb_panel = heap_caps_calloc(1, sizeof(esp_rgb_panel_t) + num_dma_nodes * sizeof(dma_descriptor_t) * RGB_LCD_PANEL_DMA_LINKS_REPLICA,
                                 MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_GOTO_ON_FALSE(rgb_panel, ESP_ERR_NO_MEM, err, TAG, "no mem for rgb panel");
    rgb_panel->num_dma_nodes = num_dma_nodes;
    rgb_panel->num_fbs = num_fbs;
    rgb_panel->fb_size = fb_size;
    rgb_panel->bb_size = bb_size;
    rgb_panel->expect_eof_count = expect_bb_eof_count;
    rgb_panel->panel_id = -1;
    // register to platform
    int panel_id = lcd_com_register_device(LCD_COM_DEVICE_TYPE_RGB, rgb_panel);
    ESP_GOTO_ON_FALSE(panel_id >= 0, ESP_ERR_NOT_FOUND, err, TAG, "no free rgb panel slot");
    rgb_panel->panel_id = panel_id;

    // enable APB to access LCD registers
    PERIPH_RCC_ACQUIRE_ATOMIC(lcd_periph_signals.panels[panel_id].module, ref_count) {
        if (ref_count == 0) {
            lcd_ll_enable_bus_clock(panel_id, true);
            lcd_ll_reset_register(panel_id);
        }
    }

    // allocate frame buffers + bounce buffers
    ESP_GOTO_ON_ERROR(lcd_rgb_panel_alloc_frame_buffers(rgb_panel_config, rgb_panel), err, TAG, "alloc frame buffers failed");

    // initialize HAL layer, so we can call LL APIs later
    lcd_hal_init(&rgb_panel->hal, panel_id);
    // enable clock
    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_enable_clock(rgb_panel->hal.dev, true);
    }
    // set clock source
    ret = lcd_rgb_panel_select_clock_src(rgb_panel, rgb_panel_config->clk_src);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "set source clock failed");
    // reset peripheral and FIFO after we select a correct clock source
    lcd_ll_fifo_reset(rgb_panel->hal.dev);
    lcd_ll_reset(rgb_panel->hal.dev);
    // set minimal PCLK divider
    // A limitation in the hardware, if the LCD_PCLK == LCD_CLK, then the PCLK polarity can't be adjustable
    if (!(rgb_panel_config->timings.flags.pclk_active_neg || rgb_panel_config->timings.flags.pclk_idle_high)) {
        rgb_panel->lcd_clk_flags |= LCD_HAL_PCLK_FLAG_ALLOW_EQUAL_SYSCLK;
    }
    // install interrupt service, (LCD peripheral shares the interrupt source with Camera by different mask)
    int isr_flags = LCD_RGB_INTR_ALLOC_FLAGS | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
    ret = esp_intr_alloc_intrstatus(lcd_periph_signals.panels[panel_id].irq_id, isr_flags,
                                    (uint32_t)lcd_ll_get_interrupt_status_reg(rgb_panel->hal.dev),
                                    LCD_LL_EVENT_VSYNC_END, lcd_default_isr_handler, rgb_panel, &rgb_panel->intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install interrupt failed");
    lcd_ll_enable_interrupt(rgb_panel->hal.dev, LCD_LL_EVENT_VSYNC_END, false); // disable all interrupts
    lcd_ll_clear_interrupt_status(rgb_panel->hal.dev, UINT32_MAX); // clear pending interrupt

    // install DMA service
    rgb_panel->flags.stream_mode = !rgb_panel_config->flags.refresh_on_demand;
    rgb_panel->fb_bits_per_pixel = fb_bits_per_pixel;
    ret = lcd_rgb_panel_create_trans_link(rgb_panel);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install DMA failed");
    // configure GPIO
    ret = lcd_rgb_panel_configure_gpio(rgb_panel, rgb_panel_config);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "configure GPIO failed");
    // fill other rgb panel runtime parameters
    memcpy(rgb_panel->data_gpio_nums, rgb_panel_config->data_gpio_nums, sizeof(rgb_panel->data_gpio_nums));
    rgb_panel->timings = rgb_panel_config->timings;
    rgb_panel->data_width = rgb_panel_config->data_width;
    rgb_panel->output_bits_per_pixel = fb_bits_per_pixel; // by default, the output bpp is the same as the frame buffer bpp
    rgb_panel->disp_gpio_num = rgb_panel_config->disp_gpio_num;
    rgb_panel->flags.disp_en_level = !rgb_panel_config->flags.disp_active_low;
    rgb_panel->flags.bb_invalidate_cache = rgb_panel_config->flags.bb_invalidate_cache;
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
    if (callbacks->on_bounce_empty) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_bounce_empty), ESP_ERR_INVALID_ARG, TAG, "on_bounce_empty callback not in IRAM");
    }
    if (callbacks->on_bounce_frame_finish) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_bounce_frame_finish), ESP_ERR_INVALID_ARG, TAG, "on_bounce_frame_finish callback not in IRAM");
    }
    if (user_ctx) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_ctx), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE
    rgb_panel->on_vsync = callbacks->on_vsync;
    rgb_panel->on_bounce_empty = callbacks->on_bounce_empty;
    rgb_panel->on_bounce_frame_finish = callbacks->on_bounce_frame_finish;
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

esp_err_t esp_lcd_rgb_panel_get_frame_buffer(esp_lcd_panel_handle_t panel, uint32_t fb_num, void **fb0, ...)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= rgb_panel->num_fbs, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");
    void **fb_itor = fb0;
    va_list args;
    va_start(args, fb0);
    for (int i = 0; i < fb_num; i++) {
        if (fb_itor) {
            *fb_itor = rgb_panel->fbs[i];
            fb_itor = va_arg(args, void **);
        }
    }
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

esp_err_t esp_lcd_rgb_panel_set_yuv_conversion(esp_lcd_panel_handle_t panel, const esp_lcd_yuv_conv_config_t *config)
{
    ESP_RETURN_ON_FALSE(panel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    lcd_hal_context_t *hal = &rgb_panel->hal;
    bool en_conversion = config != NULL;

    // bits per pixel for different YUV sample
    const uint8_t bpp_yuv[] = {
        [LCD_YUV_SAMPLE_422] = 16,
        [LCD_YUV_SAMPLE_420] = 12,
        [LCD_YUV_SAMPLE_411] = 12,
    };

    if (en_conversion) {
        if (memcmp(&config->src, &config->dst, sizeof(config->src)) == 0) {
            ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "conversion source and destination are the same");
        }

        if (config->src.color_space == LCD_COLOR_SPACE_YUV && config->dst.color_space == LCD_COLOR_SPACE_RGB) { // YUV->RGB
            lcd_ll_set_convert_mode_yuv_to_rgb(hal->dev, config->src.yuv_sample);
            // Note, the RGB->YUV conversion only support RGB565
            rgb_panel->output_bits_per_pixel = 16;
        } else if (config->src.color_space == LCD_COLOR_SPACE_RGB && config->dst.color_space == LCD_COLOR_SPACE_YUV) { // RGB->YUV
            lcd_ll_set_convert_mode_rgb_to_yuv(hal->dev, config->dst.yuv_sample);
            rgb_panel->output_bits_per_pixel = bpp_yuv[config->dst.yuv_sample];
        } else if (config->src.color_space == LCD_COLOR_SPACE_YUV && config->dst.color_space == LCD_COLOR_SPACE_YUV) { // YUV->YUV
            lcd_ll_set_convert_mode_yuv_to_yuv(hal->dev, config->src.yuv_sample, config->dst.yuv_sample);
            rgb_panel->output_bits_per_pixel = bpp_yuv[config->dst.yuv_sample];
        } else {
            ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "unsupported conversion mode");
        }

        // set conversion standard
        lcd_ll_set_yuv_convert_std(hal->dev, config->std);
        // set conversion data width
        lcd_ll_set_convert_data_width(hal->dev, rgb_panel->data_width);
        // set color range
        lcd_ll_set_input_color_range(hal->dev, config->src.color_range);
        lcd_ll_set_output_color_range(hal->dev, config->dst.color_range);
    } else {
        // output bpp equals to frame buffer bpp
        rgb_panel->output_bits_per_pixel = rgb_panel->fb_bits_per_pixel;
    }

    // enable or disable RGB-YUV conversion
    lcd_ll_enable_rgb_yuv_convert(hal->dev, en_conversion);

    return ESP_OK;
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
    rgb_panel->timings.pclk_hz = lcd_hal_cal_pclk_freq(&rgb_panel->hal, rgb_panel->src_clk_hz, rgb_panel->timings.pclk_hz, rgb_panel->lcd_clk_flags, &lcd_clk_div);
    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_set_group_clock_coeff(rgb_panel->hal.dev, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator);
    }
    // pixel clock phase and polarity
    lcd_ll_set_clock_idle_level(rgb_panel->hal.dev, rgb_panel->timings.flags.pclk_idle_high);
    lcd_ll_set_pixel_clock_edge(rgb_panel->hal.dev, rgb_panel->timings.flags.pclk_active_neg);
    // enable RGB mode and set data width
    lcd_ll_enable_rgb_mode(rgb_panel->hal.dev, true);
    lcd_ll_set_dma_read_stride(rgb_panel->hal.dev, rgb_panel->data_width);
    lcd_ll_set_phase_cycles(rgb_panel->hal.dev, 0, 0, 1); // enable data phase only
    // number of data cycles is controlled by DMA buffer size
    lcd_ll_enable_output_always_on(rgb_panel->hal.dev, true);
    // configure HSYNC, VSYNC, DE signal idle state level
    lcd_ll_set_idle_level(rgb_panel->hal.dev, !rgb_panel->timings.flags.hsync_idle_low,
                          !rgb_panel->timings.flags.vsync_idle_low, rgb_panel->timings.flags.de_idle_high);
    // configure blank region timing
    lcd_ll_set_blank_cycles(rgb_panel->hal.dev, 1, 1); // RGB panel always has a front and back blank (porch region)
    lcd_ll_set_horizontal_timing(rgb_panel->hal.dev, rgb_panel->timings.hsync_pulse_width,
                                 rgb_panel->timings.hsync_back_porch, rgb_panel->timings.h_res * rgb_panel->output_bits_per_pixel / rgb_panel->data_width,
                                 rgb_panel->timings.hsync_front_porch);
    lcd_ll_set_vertical_timing(rgb_panel->hal.dev, rgb_panel->timings.vsync_pulse_width,
                               rgb_panel->timings.vsync_back_porch, rgb_panel->timings.v_res,
                               rgb_panel->timings.vsync_front_porch);
    // output hsync even in porch region
    lcd_ll_enable_output_hsync_in_porch_region(rgb_panel->hal.dev, true);
    // generate the hsync at the very beginning of line
    lcd_ll_set_hsync_position(rgb_panel->hal.dev, 0);
    // send next frame automatically in stream mode
    lcd_ll_enable_auto_next_frame(rgb_panel->hal.dev, rgb_panel->flags.stream_mode);
    // trigger interrupt on the end of frame
    lcd_ll_enable_interrupt(rgb_panel->hal.dev, LCD_LL_EVENT_VSYNC_END, true);
    // enable intr
    esp_intr_enable(rgb_panel->intr);
    // start transmission
    if (rgb_panel->flags.stream_mode) {
        lcd_rgb_panel_start_transmission(rgb_panel);
    }
    ESP_LOGD(TAG, "rgb panel(%d) start, pclk=%"PRIu32"Hz", rgb_panel->panel_id, rgb_panel->timings.pclk_hz);
    return ret;
}

__attribute__((always_inline))
static inline void copy_pixel_8bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
}

__attribute__((always_inline))
static inline void copy_pixel_16bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
    *to++ = *from++;
}

__attribute__((always_inline))
static inline void copy_pixel_24bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
}

#define COPY_PIXEL_CODE_BLOCK(_bpp)                                                               \
    switch (rgb_panel->rotate_mask)                                                               \
    {                                                                                             \
    case 0:                                                                                       \
    {                                                                                             \
        uint8_t *to = fb + (y_start * h_res + x_start) * bytes_per_pixel;                         \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            memcpy(to, from, copy_bytes_per_line);                                                \
            to += bytes_per_line;                                                                 \
            from += copy_bytes_per_line;                                                          \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + y_start * bytes_per_line;                                                \
    }                                                                                             \
    break;                                                                                        \
    case ROTATE_MASK_MIRROR_X:                                                                    \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            uint32_t index = (y * h_res + (h_res - 1 - x_start)) * bytes_per_pixel;               \
            for (size_t x = x_start; x < x_end; x++)                                              \
            {                                                                                     \
                copy_pixel_##_bpp##bpp(to + index, from);                                         \
                index -= bytes_per_pixel;                                                         \
                from += bytes_per_pixel;                                                          \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + y_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_MIRROR_Y:                                                                    \
    {                                                                                             \
        uint8_t *to = fb + ((v_res - 1 - y_start) * h_res + x_start) * bytes_per_pixel;           \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            memcpy(to, from, copy_bytes_per_line);                                                \
            to -= bytes_per_line;                                                                 \
            from += copy_bytes_per_line;                                                          \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - y_end) * bytes_per_line;                                        \
    }                                                                                             \
    break;                                                                                        \
    case ROTATE_MASK_MIRROR_X | ROTATE_MASK_MIRROR_Y:                                             \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            uint32_t index = ((v_res - 1 - y) * h_res + (h_res - 1 - x_start)) * bytes_per_pixel; \
            for (size_t x = x_start; x < x_end; x++)                                              \
            {                                                                                     \
                copy_pixel_##_bpp##bpp(to + index, from);                                         \
                index -= bytes_per_pixel;                                                         \
                from += bytes_per_pixel;                                                          \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - y_end) * bytes_per_line;                                        \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY:                                                                     \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = (x * h_res + y) * bytes_per_pixel;                                   \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + x_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_X:                                              \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = (x * h_res + h_res - 1 - y) * bytes_per_pixel;                       \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + x_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_Y:                                              \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = ((v_res - 1 - x) * h_res + y) * bytes_per_pixel;                     \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - x_end) * bytes_per_line;                                        \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_X | ROTATE_MASK_MIRROR_Y:                       \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = ((v_res - 1 - x) * h_res + h_res - 1 - y) * bytes_per_pixel;         \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - x_end) * bytes_per_line;                                        \
        break;                                                                                    \
    default:                                                                                      \
        break;                                                                                    \
    }

static esp_err_t rgb_panel_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    esp_rgb_panel_t *rgb_panel = __containerof(panel, esp_rgb_panel_t, base);
    ESP_RETURN_ON_FALSE(rgb_panel->num_fbs > 0, ESP_ERR_NOT_SUPPORTED, TAG, "no frame buffer installed");
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");

    // check if we need to copy the draw buffer (pointed by the color_data) to the driver's frame buffer
    bool do_copy = false;
    if (color_data == rgb_panel->fbs[0]) {
        rgb_panel->cur_fb_index = 0;
    } else if (color_data == rgb_panel->fbs[1]) {
        rgb_panel->cur_fb_index = 1;
    } else if (color_data == rgb_panel->fbs[2]) {
        rgb_panel->cur_fb_index = 2;
    } else {
        // we do the copy only if the color_data is different from either frame buffer
        do_copy = true;
    }

    // adjust the flush window by adding extra gap
    x_start += rgb_panel->x_gap;
    y_start += rgb_panel->y_gap;
    x_end += rgb_panel->x_gap;
    y_end += rgb_panel->y_gap;
    // round the boundary
    int h_res = rgb_panel->timings.h_res;
    int v_res = rgb_panel->timings.v_res;
    if (rgb_panel->rotate_mask & ROTATE_MASK_SWAP_XY) {
        x_start = MIN(x_start, v_res);
        x_end = MIN(x_end, v_res);
        y_start = MIN(y_start, h_res);
        y_end = MIN(y_end, h_res);
    } else {
        x_start = MIN(x_start, h_res);
        x_end = MIN(x_end, h_res);
        y_start = MIN(y_start, v_res);
        y_end = MIN(y_end, v_res);
    }

    int bytes_per_pixel = rgb_panel->fb_bits_per_pixel / 8;
    int pixels_per_line = rgb_panel->timings.h_res;
    uint32_t bytes_per_line = bytes_per_pixel * pixels_per_line;
    uint8_t *fb = rgb_panel->fbs[rgb_panel->cur_fb_index];
    size_t bytes_to_flush = v_res * h_res * bytes_per_pixel;
    uint8_t *flush_ptr = fb;

    if (do_copy) {
        // copy the UI draw buffer into internal frame buffer
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
    }

    // Note that if we use a bounce buffer, the data gets read by the CPU as well so no need to write back
    if (rgb_panel->flags.fb_in_psram && !rgb_panel->bb_size) {
        // CPU writes data to PSRAM through DCache, data in PSRAM might not get updated, so write back
        ESP_RETURN_ON_ERROR(esp_cache_msync(flush_ptr, bytes_to_flush, 0), TAG, "flush cache buffer failed");
    }

    if (!rgb_panel->bb_size) {
        if (rgb_panel->flags.stream_mode) {
            // the DMA will convey the new frame buffer next time
            for (int i = 0; i < RGB_LCD_PANEL_DMA_LINKS_REPLICA; i++) {
                rgb_panel->dma_nodes[rgb_panel->num_dma_nodes * (i + 1) - 1].next = rgb_panel->dma_links[rgb_panel->cur_fb_index];
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
            esp_rom_gpio_connect_out_signal(rgb_panel->data_gpio_nums[i], lcd_periph_signals.panels[panel_id].data_sigs[i],
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

static esp_err_t lcd_rgb_panel_configure_gpio(esp_rgb_panel_t *panel, const esp_lcd_rgb_panel_config_t *panel_config)
{
    int panel_id = panel->panel_id;
    // Set the number of output data lines
    lcd_ll_set_data_wire_width(panel->hal.dev, panel_config->data_width);
    // connect peripheral signals via GPIO matrix
    for (size_t i = 0; i < panel_config->data_width; i++) {
        if (panel_config->data_gpio_nums[i] >= 0) {
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->data_gpio_nums[i]], PIN_FUNC_GPIO);
            gpio_set_direction(panel_config->data_gpio_nums[i], GPIO_MODE_OUTPUT);
            esp_rom_gpio_connect_out_signal(panel_config->data_gpio_nums[i],
                                            lcd_periph_signals.panels[panel_id].data_sigs[i], false, false);
        }
    }
    if (panel_config->hsync_gpio_num >= 0) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->hsync_gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(panel_config->hsync_gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(panel_config->hsync_gpio_num,
                                        lcd_periph_signals.panels[panel_id].hsync_sig, false, false);
    }
    if (panel_config->vsync_gpio_num >= 0) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->vsync_gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(panel_config->vsync_gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(panel_config->vsync_gpio_num,
                                        lcd_periph_signals.panels[panel_id].vsync_sig, false, false);
    }
    // PCLK may not be necessary in some cases (i.e. VGA output)
    if (panel_config->pclk_gpio_num >= 0) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->pclk_gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(panel_config->pclk_gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(panel_config->pclk_gpio_num,
                                        lcd_periph_signals.panels[panel_id].pclk_sig, false, false);
    }
    // DE signal might not be necessary for some RGB LCD
    if (panel_config->de_gpio_num >= 0) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->de_gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(panel_config->de_gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(panel_config->de_gpio_num,
                                        lcd_periph_signals.panels[panel_id].de_sig, false, false);
    }
    // disp enable GPIO is optional
    if (panel_config->disp_gpio_num >= 0) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[panel_config->disp_gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(panel_config->disp_gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(panel_config->disp_gpio_num, SIG_GPIO_OUT_IDX, false, false);
    }
    return ESP_OK;
}

static esp_err_t lcd_rgb_panel_select_clock_src(esp_rgb_panel_t *panel, lcd_clock_source_t clk_src)
{
    // get clock source frequency
    uint32_t src_clk_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz),
                        TAG, "get clock source frequency failed");
    panel->src_clk_hz = src_clk_hz;
    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_select_clk_src(panel->hal.dev, clk_src);
    }

    // create pm lock based on different clock source
    // clock sources like PLL and XTAL will be turned off in light sleep
#if CONFIG_PM_ENABLE
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "rgb_panel", &panel->pm_lock), TAG, "create pm lock failed");
    // hold the lock during the whole lifecycle of RGB panel
    esp_pm_lock_acquire(panel->pm_lock);
    ESP_LOGD(TAG, "installed pm lock and hold the lock during the whole panel lifecycle");
#endif

    return ESP_OK;
}

static IRAM_ATTR bool lcd_rgb_panel_fill_bounce_buffer(esp_rgb_panel_t *panel, uint8_t *buffer)
{
    bool need_yield = false;
    int bytes_per_pixel = panel->fb_bits_per_pixel / 8;
    if (panel->num_fbs == 0) {
        if (panel->on_bounce_empty) {
            // We don't have a frame buffer here; we need to call a callback to refill the bounce buffer
            if (panel->on_bounce_empty(&panel->base, buffer, panel->bounce_pos_px, panel->bb_size, panel->user_ctx)) {
                need_yield = true;
            }
        }
    } else {
        // We do have frame buffer; copy from there.
        // Note: if the cache is disabled, and accessing the PSRAM by DCACHE will crash.
        memcpy(buffer, &panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel], panel->bb_size);
        if (panel->flags.bb_invalidate_cache) {
            // We don't need the bytes we copied from the psram anymore
            // Make sure that if anything happened to have changed (because the line already was in cache) we write the data back.
            esp_cache_msync(&panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel], (size_t)panel->bb_size, ESP_CACHE_MSYNC_FLAG_INVALIDATE);
        }
    }
    panel->bounce_pos_px += panel->bb_size / bytes_per_pixel;
    // If the bounce pos is larger than the frame buffer size, wrap around so the next isr starts pre-loading the next frame.
    if (panel->bounce_pos_px >= panel->fb_size / bytes_per_pixel) {
        panel->bounce_pos_px = 0;
        panel->bb_fb_index = panel->cur_fb_index;
        if (panel->on_bounce_frame_finish) {
            if (panel->on_bounce_frame_finish(&panel->base, NULL, panel->user_ctx)) {
                need_yield = true;
            }
        }
    }
    if (panel->num_fbs > 0) {
        // Preload the next bit of buffer from psram
        Cache_Start_DCache_Preload((uint32_t)&panel->fbs[panel->bb_fb_index][panel->bounce_pos_px * bytes_per_pixel],
                                   panel->bb_size, 0);
    }
    return need_yield;
}

// This is called in bounce buffer mode, when one bounce buffer has been fully sent to the LCD peripheral.
static IRAM_ATTR bool lcd_rgb_panel_eof_handler(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    esp_rgb_panel_t *panel = (esp_rgb_panel_t *)user_data;
    dma_descriptor_t *desc = (dma_descriptor_t *)event_data->tx_eof_desc_addr;
    // Figure out which bounce buffer to write to.
    // Note: what we receive is the *last* descriptor of this bounce buffer.
    int bb = (desc == &panel->dma_nodes[panel->num_dma_nodes - 1]) ? 0 : 1;
    portENTER_CRITICAL_ISR(&panel->spinlock);
    panel->bb_eof_count++;
    portEXIT_CRITICAL_ISR(&panel->spinlock);
    return lcd_rgb_panel_fill_bounce_buffer(panel, panel->bounce_buffer[bb]);
}

// If we restart GDMA, many pixels already have been transferred to the LCD peripheral.
// Looks like that has 16 pixels of FIFO plus one holding register.
#define LCD_FIFO_PRESERVE_SIZE_PX (LCD_LL_FIFO_DEPTH + 1)

static esp_err_t lcd_rgb_panel_create_trans_link(esp_rgb_panel_t *panel)
{
    for (int i = 0; i < RGB_LCD_PANEL_DMA_LINKS_REPLICA; i++) {
        panel->dma_links[i] = &panel->dma_nodes[panel->num_dma_nodes * i];
    }
    // chain DMA descriptors
    for (int i = 0; i < panel->num_dma_nodes * RGB_LCD_PANEL_DMA_LINKS_REPLICA; i++) {
        panel->dma_nodes[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        panel->dma_nodes[i].next = &panel->dma_nodes[i + 1];
    }

    if (panel->bb_size) {
        // loop end back to start
        panel->dma_nodes[panel->num_dma_nodes * RGB_LCD_PANEL_BOUNCE_BUF_NUM - 1].next = &panel->dma_nodes[0];
        // mount the bounce buffers to the DMA descriptors
        lcd_com_mount_dma_data(panel->dma_links[0], panel->bounce_buffer[0], panel->bb_size);
        lcd_com_mount_dma_data(panel->dma_links[1], panel->bounce_buffer[1], panel->bb_size);
    } else {
        if (panel->flags.stream_mode) {
            // circle DMA descriptors chain for each frame buffer
            for (int i = 0; i < RGB_LCD_PANEL_DMA_LINKS_REPLICA; i++) {
                panel->dma_nodes[panel->num_dma_nodes * (i + 1) - 1].next = &panel->dma_nodes[panel->num_dma_nodes * i];
            }
        } else {
            // one-off DMA descriptors chain
            for (int i = 0; i < RGB_LCD_PANEL_DMA_LINKS_REPLICA; i++) {
                panel->dma_nodes[panel->num_dma_nodes * (i + 1) - 1].next = NULL;
            }
        }
        // mount the frame buffer to the DMA descriptors
        for (size_t i = 0; i < panel->num_fbs; i++) {
            lcd_com_mount_dma_data(panel->dma_links[i], panel->fbs[i], panel->fb_size);
        }
    }

    // On restart, the data sent to the LCD peripheral needs to start LCD_FIFO_PRESERVE_SIZE_PX pixels after the FB start
    // so we use a dedicated DMA node to restart the DMA transaction
    // see also `lcd_rgb_panel_try_restart_transmission`
    memcpy(&panel->dma_restart_node, &panel->dma_nodes[0], sizeof(panel->dma_restart_node));
    int restart_skip_bytes = LCD_FIFO_PRESERVE_SIZE_PX * (panel->fb_bits_per_pixel / 8);
    uint8_t *p = (uint8_t *)panel->dma_restart_node.buffer;
    panel->dma_restart_node.buffer = &p[restart_skip_bytes];
    panel->dma_restart_node.dw0.length -= restart_skip_bytes;
    panel->dma_restart_node.dw0.size -= restart_skip_bytes;

    // alloc DMA channel and connect to LCD peripheral
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
#if SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AHB
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&dma_chan_config, &panel->dma_chan), TAG, "alloc DMA channel failed");
#elif SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AXI
    ESP_RETURN_ON_ERROR(gdma_new_axi_channel(&dma_chan_config, &panel->dma_chan), TAG, "alloc DMA channel failed");
#endif
    gdma_connect(panel->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_LCD, 0));
    gdma_transfer_ability_t ability = {
        .psram_trans_align = panel->psram_trans_align,
        .sram_trans_align = panel->sram_trans_align,
    };
    gdma_set_transfer_ability(panel->dma_chan, &ability);

    // we need to refill the bounce buffer in the DMA EOF interrupt, so only register the callback for bounce buffer mode
    if (panel->bb_size) {
        gdma_tx_event_callbacks_t cbs = {
            .on_trans_eof = lcd_rgb_panel_eof_handler,
        };
        gdma_register_tx_event_callbacks(panel->dma_chan, &cbs, panel);
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

    gdma_reset(panel->dma_chan);
    // restart the DMA by a special DMA node
    gdma_start(panel->dma_chan, (intptr_t)&panel->dma_restart_node);

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
    lcd_ll_fifo_reset(rgb_panel->hal.dev);

    // pre-fill bounce buffers if needed
    if (rgb_panel->bb_size) {
        rgb_panel->bounce_pos_px = 0;
        lcd_rgb_panel_fill_bounce_buffer(rgb_panel, rgb_panel->bounce_buffer[0]);
        lcd_rgb_panel_fill_bounce_buffer(rgb_panel, rgb_panel->bounce_buffer[1]);
    }

    // the start of DMA should be prior to the start of LCD engine
    gdma_start(rgb_panel->dma_chan, (intptr_t)rgb_panel->dma_links[rgb_panel->cur_fb_index]);
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
        rgb_panel->timings.pclk_hz = lcd_hal_cal_pclk_freq(&rgb_panel->hal, rgb_panel->src_clk_hz, rgb_panel->timings.pclk_hz, rgb_panel->lcd_clk_flags, &lcd_clk_div);
        LCD_CLOCK_SRC_ATOMIC() {
            lcd_ll_set_group_clock_coeff(rgb_panel->hal.dev, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator);
        }
    }
    portEXIT_CRITICAL_ISR(&rgb_panel->spinlock);
}

IRAM_ATTR static void lcd_default_isr_handler(void *args)
{
    esp_rgb_panel_t *rgb_panel = (esp_rgb_panel_t *)args;
    bool need_yield = false;

    uint32_t intr_status = lcd_ll_get_interrupt_status(rgb_panel->hal.dev);
    lcd_ll_clear_interrupt_status(rgb_panel->hal.dev, intr_status);
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
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
