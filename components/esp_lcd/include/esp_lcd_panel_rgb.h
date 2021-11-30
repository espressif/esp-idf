/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
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
    unsigned int vsync_front_porch; /*!< Vertical front porch, number of invalid lines between then end of frame and the next vsync */
    struct {
        unsigned int hsync_idle_low: 1;  /*!< The hsync signal is low in IDLE state */
        unsigned int vsync_idle_low: 1;  /*!< The vsync signal is low in IDLE state */
        unsigned int de_idle_high: 1;    /*!< The de signal is high in IDLE state */
        unsigned int pclk_active_neg: 1; /*!< The display will write data lines when there's a falling edge on PCLK */
        unsigned int pclk_idle_high: 1;  /*!< The PCLK stays at high level in IDLE phase */
    } flags;
} esp_lcd_rgb_timing_t;

/**
 * @brief Type of RGB LCD panel event data
 */
typedef struct {
} esp_lcd_rgb_panel_event_data_t;

/**
 * @brief Declare the prototype of the function that will be invoked when panel IO finishes transferring color data
 *
 * @param[in] panel LCD panel handle, returned from `esp_lcd_new_rgb_panel`
 * @param[in] edata Panel event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_rgb_panel_config_t`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_rgb_panel_frame_trans_done_cb_t)(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx);

/**
 * @brief LCD RGB panel configuration structure
 */
typedef struct {
    lcd_clock_source_t clk_src;   /*!< Clock source for the RGB LCD peripheral */
    esp_lcd_rgb_timing_t timings; /*!< RGB timing parameters */
    size_t data_width;            /*!< Number of data lines */
    int hsync_gpio_num;           /*!< GPIO used for HSYNC signal */
    int vsync_gpio_num;           /*!< GPIO used for VSYNC signal */
    int de_gpio_num;              /*!< GPIO used for DE signal, set to -1 if it's not used */
    int pclk_gpio_num;            /*!< GPIO used for PCLK signal */
    int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; /*!< GPIOs used for data lines */
    int disp_gpio_num; /*!< GPIO used for display control signal, set to -1 if it's not used */
    esp_lcd_rgb_panel_frame_trans_done_cb_t on_frame_trans_done; /*!< Callback invoked when one frame buffer has transferred done */
    void *user_ctx; /*!< User data which would be passed to on_frame_trans_done's user_ctx */
    struct {
        unsigned int disp_active_low: 1; /*!< If this flag is enabled, a low level of display control signal can turn the screen on; vice versa */
        unsigned int relax_on_idle: 1;   /*!< If this flag is enabled, the host won't refresh the LCD if nothing changed in host's frame buffer (this is usefull for LCD with built-in GRAM) */
        unsigned int fb_in_psram: 1;     /*!< If this flag is enabled, the frame buffer will be allocated from PSRAM preferentially */
    } flags;
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

#endif // SOC_LCD_RGB_SUPPORTED

#ifdef __cplusplus
}
#endif
