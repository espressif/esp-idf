/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "soc/soc_caps.h"
#include "hal/lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCD_RGB_SUPPORTED
/**
 * @brief LCD RGB timing structure
 * @verbatim
 *                                                 Total Width
 *                             <--------------------------------------------------->
 *                       HSYNC width HBP             Active Width                HFP
 *                             <---><--><--------------------------------------><--->
 *                         ____    ____|_______________________________________|____|
 *                             |___|   |                                       |    |
 *                                     |                                       |    |
 *                         __|         |                                       |    |
 *            /|\    /|\  |            |                                       |    |
 *             | VSYNC|   |            |                                       |    |
 *             |Width\|/  |__          |                                       |    |
 *             |     /|\     |         |                                       |    |
 *             |  VBP |      |         |                                       |    |
 *             |     \|/_____|_________|_______________________________________|    |
 *             |     /|\     |         | / / / / / / / / / / / / / / / / / / / |    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *    Total    |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *    Height   |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |Active|      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |Heigh |      |         |/ / / / / / Active Display Area / / / /|    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 *             |     \|/_____|_________|_______________________________________|    |
 *             |     /|\     |                                                      |
 *             |  VFP |      |                                                      |
 *            \|/    \|/_____|______________________________________________________|
 * @endverbatim
 */
typedef struct {
    uint32_t pclk_hz;           /*!< Frequency of pixel clock */
    uint32_t h_res;             /*!< Horizontal resolution, i.e. the number of pixels in a line */
    uint32_t v_res;             /*!< Vertical resolution, i.e. the number of lines in the frame  */
    uint32_t hsync_pulse_width; /*!< Horizontal sync width, unit: PCLK period */
    uint32_t hsync_back_porch;  /*!< Horizontal back porch, number of PCLK between hsync and start of line active data */
    uint32_t hsync_front_porch; /*!< Horizontal front porch, number of PCLK between the end of active data and the next hsync */
    uint32_t vsync_pulse_width; /*!< Vertical sync width, unit: number of lines */
    uint32_t vsync_back_porch;  /*!< Vertical back porch, number of invalid lines between vsync and start of frame */
    uint32_t vsync_front_porch; /*!< Vertical front porch, number of invalid lines between the end of frame and the next vsync */
    struct {
        uint32_t hsync_idle_low: 1;  /*!< The hsync signal is low in IDLE state */
        uint32_t vsync_idle_low: 1;  /*!< The vsync signal is low in IDLE state */
        uint32_t de_idle_high: 1;    /*!< The de signal is high in IDLE state */
        uint32_t pclk_active_neg: 1; /*!< Whether the display data is clocked out on the falling edge of PCLK */
        uint32_t pclk_idle_high: 1;  /*!< The PCLK stays at high level in IDLE phase */
    } flags;                         /*!< LCD RGB timing flags */
} esp_lcd_rgb_timing_t;

/**
 * @brief Type of RGB LCD panel event data
 */
typedef struct {
} esp_lcd_rgb_panel_event_data_t;

/**
 * @brief RGB LCD VSYNC event callback prototype
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] edata Panel event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_rgb_panel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_vsync_cb_t)(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx);

/**
 * @brief Prototype for function to re-fill a bounce buffer, rather than copying from the frame buffer
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] bounce_buf Bounce buffer to write data into
 * @param[in] pos_px How many pixels already were sent to the display in this frame, in other words,
 *                   at what pixel the routine should start putting data into bounce_buf
 * @param[in] len_bytes Length, in bytes, of the bounce buffer. Routine should fill this length fully.
 * @param[in] user_ctx Opaque pointer that was passed from `esp_lcd_rgb_panel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_bounce_buf_fill_cb_t)(esp_lcd_panel_handle_t panel, void *bounce_buf, int pos_px, int len_bytes, void *user_ctx);

/**
 * @brief Prototype for the function to be called when the bounce buffer finish copying the entire frame.
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] edata Panel event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_rgb_panel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_bounce_buf_finish_cb_t)(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx);

/**
 * @brief Group of supported RGB LCD panel callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_LCD_RGB_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 */
typedef struct {
    esp_lcd_rgb_panel_vsync_cb_t on_vsync;                      /*!< VSYNC event callback */
    esp_lcd_rgb_panel_bounce_buf_fill_cb_t on_bounce_empty;     /*!< Bounce buffer empty callback. */
    esp_lcd_rgb_panel_bounce_buf_finish_cb_t on_bounce_frame_finish;  /*!< Bounce buffer finish callback. */
} esp_lcd_rgb_panel_event_callbacks_t;

/**
 * @brief LCD RGB panel configuration structure
 */
typedef struct {
    lcd_clock_source_t clk_src;   /*!< Clock source for the RGB LCD peripheral */
    esp_lcd_rgb_timing_t timings; /*!< RGB timing parameters, including the screen resolution */
    size_t data_width;            /*!< Number of data lines */
    size_t bits_per_pixel;        /*!< Frame buffer color depth, in bpp, specially, if set to zero, it will default to `data_width`.
                                       When using a Serial RGB interface, this value could be different from `data_width` */
    size_t num_fbs;               /*!< Number of screen-sized frame buffers that allocated by the driver. By default (set to either 0 or 1) only one frame buffer will be used. Maximum number of buffers are 3 */
    size_t bounce_buffer_size_px; /*!< If it's non-zero, the driver allocates two DRAM bounce buffers for DMA use.
                                       DMA fetching from DRAM bounce buffer is much faster than PSRAM frame buffer. */
    size_t sram_trans_align;      /*!< Alignment of buffers (frame buffer or bounce buffer) that allocated in SRAM */
    size_t psram_trans_align;     /*!< Alignment of buffers (frame buffer) that allocated in PSRAM */
    int hsync_gpio_num;           /*!< GPIO used for HSYNC signal */
    int vsync_gpio_num;           /*!< GPIO used for VSYNC signal */
    int de_gpio_num;              /*!< GPIO used for DE signal, set to -1 if it's not used */
    int pclk_gpio_num;            /*!< GPIO used for PCLK signal, set to -1 if it's not used */
    int disp_gpio_num;            /*!< GPIO used for display control signal, set to -1 if it's not used */
    int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; /*!< GPIOs used for data lines */
    struct {
        uint32_t disp_active_low: 1;     /*!< If this flag is enabled, a low level of display control signal can turn the screen on; vice versa */
        uint32_t refresh_on_demand: 1;   /*!< If this flag is enabled, the host only refresh the frame buffer when `esp_lcd_panel_draw_bitmap` is called.
                                              This is useful when the LCD screen has a GRAM and can refresh the LCD by itself. */
        uint32_t fb_in_psram: 1;         /*!< If this flag is enabled, the frame buffer will be allocated from PSRAM, preferentially */
        uint32_t double_fb: 1;           /*!< If this flag is enabled, the driver will allocate two screen sized frame buffer, same as num_fbs=2 */
        uint32_t no_fb: 1;               /*!< If this flag is enabled, the driver won't allocate frame buffer.
                                              Instead, user should fill in the bounce buffer manually in the `on_bounce_empty` callback */
        uint32_t bb_invalidate_cache: 1; /*!< If this flag is enabled, in bounce back mode we'll do a cache invalidate on the read data, freeing the cache.
                                              Can be dangerous if data is written from other core(s). */
    } flags;                             /*!< LCD RGB panel configuration flags */
} esp_lcd_rgb_panel_config_t;

/**
 * @brief Create RGB LCD panel
 *
 * @param[in] rgb_panel_config RGB panel configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *      - ESP_ERR_INVALID_ARG: Create RGB LCD panel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RGB LCD panel failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Create RGB LCD panel failed because some mandatory hardware resources are not found
 *      - ESP_OK: Create RGB LCD panel successfully
 */
esp_err_t esp_lcd_new_rgb_panel(const esp_lcd_rgb_panel_config_t *rgb_panel_config, esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Register LCD RGB panel event callbacks
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] callbacks Group of callback functions
 * @param[in] user_ctx User data, which will be passed to the callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t esp_lcd_rgb_panel_register_event_callbacks(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_callbacks_t *callbacks, void *user_ctx);

/**
 * @brief Set frequency of PCLK for RGB LCD panel
 *
 * @note The PCLK frequency is set in the `esp_lcd_rgb_timing_t` and gets configured during LCD panel initialization.
 *       Usually you don't need to call this function to set the PCLK again, but in some cases, you might want to change the PCLK frequency.
 *       e.g. slow down the PCLK frequency to reduce power consumption or to reduce the memory throughput during OTA.
 * @note This function doesn't cause the hardware to update the PCLK immediately but to record the new frequency and set a flag internally.
 *       Only in the next VSYNC event handler, will the driver attempt to update the PCLK frequency.
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] freq_hz Frequency of pixel clock, in Hz
 * @return
 *      - ESP_ERR_INVALID_ARG: Set PCLK frequency failed because of invalid argument
 *      - ESP_OK: Set PCLK frequency successfully
 */
esp_err_t esp_lcd_rgb_panel_set_pclk(esp_lcd_panel_handle_t panel, uint32_t freq_hz);

/**
 * @brief Restart the LCD transmission
 *
 * @note This function can be useful when the LCD controller is out of sync with the DMA because of insufficient bandwidth.
 *       To save the screen from a permanent shift, you can call this function to restart the LCD DMA.
 * @note This function doesn't restart the DMA immediately but to set a flag internally.
 *       Only in the next VSYNC event handler, will the driver attempt to do the restart job.
 * @note If CONFIG_LCD_RGB_RESTART_IN_VSYNC is enabled, you don't need to call this function manually,
 *       because the restart job will be done automatically in the VSYNC event handler.
 *
 * @param[in] panel panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @return
 *      - ESP_ERR_INVALID_ARG: Restart the LCD failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Restart the LCD failed because the LCD diver is working in refresh-on-demand mode
 *      - ESP_OK: Restart the LCD successfully
 */
esp_err_t esp_lcd_rgb_panel_restart(esp_lcd_panel_handle_t panel);

/**
 * @brief Get the address of the frame buffer(s) that allocated by the driver
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] fb_num Number of frame buffer(s) to get. This value must be the same as the number of the following parameters.
 * @param[out] fb0 Returned address of the frame buffer 0
 * @param[out] ... List of other frame buffer addresses
 * @return
 *      - ESP_ERR_INVALID_ARG: Get frame buffer address failed because of invalid argument
 *      - ESP_OK: Get frame buffer address successfully
 */
esp_err_t esp_lcd_rgb_panel_get_frame_buffer(esp_lcd_panel_handle_t panel, uint32_t fb_num, void **fb0, ...);

/**
 * @brief Manually trigger once transmission of the frame buffer to the LCD panel
 *
 * @note This function should only be called when the RGB panel is working under the `refresh_on_demand` mode.
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @return
 *      - ESP_ERR_INVALID_ARG: Start a refresh failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Start a refresh failed because the LCD panel is not created with the `refresh_on_demand` flag enabled.
 *      - ESP_OK: Start a refresh successfully
 */
esp_err_t esp_lcd_rgb_panel_refresh(esp_lcd_panel_handle_t panel);

/**
 * @brief LCD color conversion profile
 */
typedef struct {
    lcd_color_space_t color_space; /*!< Color space of the image */
    lcd_color_range_t color_range; /*!< Color range of the image */
    lcd_yuv_sample_t yuv_sample;   /*!< YUV sample format of the image */
} esp_lcd_color_conv_profile_t;

/**
 * @brief Configuration of YUG-RGB conversion
 */
typedef struct {
    lcd_yuv_conv_std_t std;           /*!< YUV conversion standard: BT601, BT709 */
    esp_lcd_color_conv_profile_t src; /*!< Color conversion profile of the input image */
    esp_lcd_color_conv_profile_t dst; /*!< Color conversion profile of the output image */
} esp_lcd_yuv_conv_config_t;

/**
 * @brief Configure how to convert the color format between RGB and YUV
 *
 * @note Pass in `config` as NULL will disable the RGB-YUV converter.
 * @note The hardware converter can only parse a "packed" storage format, while "planar" and "semi-planar" format is not supported.
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] config Configuration of RGB-YUV conversion
 * @return
 *      - ESP_ERR_INVALID_ARG: Configure RGB-YUV conversion failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Configure RGB-YUV conversion failed because the conversion mode is not supported by the hardware
 *      - ESP_OK: Configure RGB-YUV conversion successfully
 */
esp_err_t esp_lcd_rgb_panel_set_yuv_conversion(esp_lcd_panel_handle_t panel, const esp_lcd_yuv_conv_config_t *config);

#endif // SOC_LCD_RGB_SUPPORTED

#ifdef __cplusplus
}
#endif
