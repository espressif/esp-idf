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

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCD_RGB_SUPPORTED
/**
 * @brief LCD RGB timing structure
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
        unsigned int pclk_idle_low: 1;   /*!< The PCLK stays at low level in IDLE phase */
    } flags;
} esp_lcd_rgb_timing_t;

/**
 * @brief LCD RGB panel configuration structure
 */
typedef struct {
    esp_lcd_rgb_timing_t timings; /*!< RGB timing parameters */
    size_t data_width;            /*!< Number of data lines */
    int hsync_gpio_num;           /*!< GPIO used for HSYNC signal */
    int vsync_gpio_num;           /*!< GPIO used for VSYNC signal */
    int de_gpio_num;              /*!< GPIO used for DE signal, set to -1 if it's not used */
    int pclk_gpio_num;            /*!< GPIO used for PCLK signal */
    int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; /*!< GPIOs used for data lines */
    int disp_gpio_num; /*!< GPIO used for display control signal, set to -1 if it's not used */
    bool (*on_frame_trans_done)(esp_lcd_panel_handle_t panel, void *user_data); /*!< Callback, invoked when one frame buffer has transferred done */
    void *user_data; /*!< User data which would be passed to on_frame_trans_done's user_data */
    struct {
        unsigned int disp_active_low: 1; /*!< If this flag is enabled, a low level of display control signal can turn the screen on; vice versa */
        unsigned int relax_on_idle: 1;   /*!< If this flag is enabled, the host won't refresh the LCD if nothing changed in host's frame buffer (this is usefull for LCD with built-in GRAM) */
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
