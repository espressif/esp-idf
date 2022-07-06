/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

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
    unsigned int pclk_hz;           /*!< Frequency of pixel clock */
    unsigned int h_res;             /*!< Horizontal resolution, i.e. the number of pixels in a line */
    unsigned int v_res;             /*!< Vertical resolution, i.e. the number of lines in the frame  */
    unsigned int hsync_pulse_width; /*!< Horizontal sync width, unit: PCLK period */
    unsigned int hsync_back_porch;  /*!< Horizontal back porch, number of PCLK between hsync and start of line active data */
    unsigned int hsync_front_porch; /*!< Horizontal front porch, number of PCLK between the end of active data and the next hsync */
    unsigned int vsync_pulse_width; /*!< Vertical sync width, unit: number of lines */
    unsigned int vsync_back_porch;  /*!< Vertical back porch, number of invalid lines between vsync and start of frame */
    unsigned int vsync_front_porch; /*!< Vertical front porch, number of invalid lines between the end of frame and the next vsync */
    struct {
        unsigned int hsync_idle_low: 1;  /*!< The hsync signal is low in IDLE state */
        unsigned int vsync_idle_low: 1;  /*!< The vsync signal is low in IDLE state */
        unsigned int de_idle_high: 1;    /*!< The de signal is high in IDLE state */
        unsigned int pclk_active_neg: 1; /*!< Whether the display data is clocked out on the falling edge of PCLK */
        unsigned int pclk_idle_high: 1;  /*!< The PCLK stays at high level in IDLE phase */
    } flags;                             /*!< LCD RGB timing flags */
} esp_lcd_rgb_timing_t;

/**
 * @brief Type of RGB LCD panel event data
 */
typedef struct {
} esp_lcd_rgb_panel_event_data_t;

/**
 * @brief RGB LCD VSYNC event callback prototype
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel()`
 * @param[in] edata Panel event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_rgb_panel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_vsync_cb_t)(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx);

/**
 * @brief Prototype for function to re-fill a bounce buffer, rather than copying from the frame buffer
 *
 * @param[in] bounce_buf Bounce buffer to write data into
 * @param[in] pos_px How many pixels already were sent to the display in this frame, in other words,
 *                   at what pixel the routine should start putting data into bounce_buf
 * @param[in] len_bytes Length, in bytes, of the bounce buffer. Routine should fill this length fully.
 * @param[in] user_ctx Opaque pointer that was passed from `esp_lcd_rgb_panel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_bounce_buf_fill_cb_t)(esp_lcd_panel_handle_t panel, void *bounce_buf, int pos_px, int len_bytes, void *user_ctx);

/**
 * @brief Group of supported RGB LCD panel callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_LCD_RGB_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 */
typedef struct {
    esp_lcd_rgb_panel_vsync_cb_t on_vsync;                  /*!< VSYNC event callback */
    esp_lcd_rgb_panel_bounce_buf_fill_cb_t on_bounce_empty; /*!< Bounce buffer empty callback. */
} esp_lcd_rgb_panel_event_callbacks_t;

/**
 * @brief LCD RGB panel configuration structure
 */
typedef struct {
    lcd_clock_source_t clk_src;   /*!< Clock source for the RGB LCD peripheral */
    esp_lcd_rgb_timing_t timings; /*!< RGB timing parameters */
    size_t data_width;            /*!< Number of data lines */
    size_t sram_trans_align;      /*!< Alignment for framebuffer that allocated in SRAM */
    size_t psram_trans_align;     /*!< Alignment for framebuffer that allocated in PSRAM */
    int hsync_gpio_num;           /*!< GPIO used for HSYNC signal */
    int vsync_gpio_num;           /*!< GPIO used for VSYNC signal */
    int de_gpio_num;              /*!< GPIO used for DE signal, set to -1 if it's not used */
    int pclk_gpio_num;            /*!< GPIO used for PCLK signal */
    int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; /*!< GPIOs used for data lines */
    int disp_gpio_num; /*!< GPIO used for display control signal, set to -1 if it's not used */
    int bounce_buffer_size_px; /*!< If not-zero, the driver uses a bounce buffer in internal memory to DMA from. Value is in pixels. */
    struct {
        unsigned int disp_active_low: 1; /*!< If this flag is enabled, a low level of display control signal can turn the screen on; vice versa */
        unsigned int relax_on_idle: 1;   /*!< If this flag is enabled, the host won't refresh the LCD if nothing changed in host's frame buffer (this is usefull for LCD with built-in GRAM) */
        unsigned int fb_in_psram: 1;     /*!< If this flag is enabled, the frame buffer will be allocated from PSRAM preferentially */
        unsigned int bb_do_cache_invalidate:1; /*!< If this flag is enabled, in bounceback mode we'll do a cache invalidate on the read data, freeing the cache. Can be dangerous if data is written from other core. */
    } flags;                             /*!< LCD RGB panel configuration flags */
} esp_lcd_rgb_panel_config_t;

/**
 * @brief Create RGB LCD panel
 *
 * @param rgb_panel_config RGB panel configuration
 * @param ret_panel Returned LCD panel handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_ERR_NOT_FOUND     if no free RGB panel is available
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_rgb_panel(const esp_lcd_rgb_panel_config_t *rgb_panel_config, esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Register LCD RGB panel event callbacks
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel()`
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
 *       Usually you don't need to call this function to set the PCLK again, but in some cases, you may need to change the PCLK frequency.
 *       e.g. to slow down the PCLK frequency to reduce power consumption or to reduce the memory throughput.
 * @note This function doesn't cause the hardware to update the PCLK immediately but to record the new frequency and set a flag internally.
 *       Next time when start a new transaction, the driver will update the PCLK automatically.
 *
 * @param panel LCD panel handle, returned from `esp_lcd_new_rgb_panel()`
 * @param freq_hz Frequency of pixel clock, in Hz
 * @return
 *          - ESP_ERR_NOT_SUPPORTED   if frequency is unreachable
 *          - ESP_ERR_INVALID_ARG     if parameter panel is invalid
 *          - ESP_OK                  on success
 */
esp_err_t esp_rgb_panel_set_pclk(esp_lcd_panel_handle_t panel, uint32_t freq_hz);

#endif // SOC_LCD_RGB_SUPPORTED

#ifdef __cplusplus
}
#endif
