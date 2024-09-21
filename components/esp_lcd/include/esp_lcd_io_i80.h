/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "soc/soc_caps.h"

#define ESP_LCD_I80_BUS_WIDTH_MAX 16 /*!< Maximum width of I80 bus */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_i80_bus_t *esp_lcd_i80_bus_handle_t;   /*!< Type of LCD intel 8080 bus handle */

#if SOC_LCD_I80_SUPPORTED
/**
 * @brief LCD Intel 8080 bus configuration structure
 */
typedef struct {
    int dc_gpio_num; /*!< GPIO used for D/C line */
    int wr_gpio_num; /*!< GPIO used for WR line */
    lcd_clock_source_t clk_src; /*!< Clock source for the I80 LCD peripheral */
    int data_gpio_nums[ESP_LCD_I80_BUS_WIDTH_MAX]; /*!< GPIOs used for data lines */
    size_t bus_width;          /*!< Number of data lines, 8 or 16 */
    size_t max_transfer_bytes; /*!< Maximum transfer size, this determines the length of internal DMA link */
    union {
        size_t psram_trans_align __attribute__((deprecated)); /*!< DMA transfer alignment for data allocated from PSRAM */
        size_t dma_burst_size;     /*!< DMA burst size, in bytes */
    };
    size_t sram_trans_align __attribute__((deprecated)); /*!< DMA transfer alignment for data allocated from SRAM */
} esp_lcd_i80_bus_config_t;

/**
 * @brief Create Intel 8080 bus handle
 *
 * @param[in] bus_config Bus configuration
 * @param[out] ret_bus Returned bus handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_ERR_NOT_FOUND     if no free bus is available
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_i80_bus(const esp_lcd_i80_bus_config_t *bus_config, esp_lcd_i80_bus_handle_t *ret_bus);

/**
 * @brief Destroy Intel 8080 bus handle
 *
 * @param[in] bus Intel 8080 bus handle, created by `esp_lcd_new_i80_bus()`
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_INVALID_STATE if there still be some device attached to the bus
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_del_i80_bus(esp_lcd_i80_bus_handle_t bus);

/**
 * @brief Panel IO configuration structure, for intel 8080 interface
 */
typedef struct {
    int cs_gpio_num;         /*!< GPIO used for CS line, set to -1 will declaim exclusively use of I80 bus */
    uint32_t pclk_hz;        /*!< Frequency of pixel clock */
    size_t trans_queue_depth; /*!< Transaction queue size, larger queue, higher throughput */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data was transferred done */
    void *user_ctx;    /*!< User private data, passed directly to on_color_trans_done's user_ctx */
    int lcd_cmd_bits;   /*!< Bit-width of LCD command */
    int lcd_param_bits; /*!< Bit-width of LCD parameter */
    struct {
        unsigned int dc_idle_level: 1;  /*!< Level of DC line in IDLE phase */
        unsigned int dc_cmd_level: 1;   /*!< Level of DC line in CMD phase */
        unsigned int dc_dummy_level: 1; /*!< Level of DC line in DUMMY phase */
        unsigned int dc_data_level: 1;  /*!< Level of DC line in DATA phase */
    } dc_levels; /*!< Each i80 device might have its own D/C control logic */
    struct {
        unsigned int cs_active_high: 1;     /*!< If set, a high level of CS line will select the device, otherwise, CS line is low level active */
        unsigned int reverse_color_bits: 1; /*!< Reverse the data bits, D[N:0] -> D[0:N] */
        unsigned int swap_color_bytes: 1;   /*!< Swap adjacent two color bytes */
        unsigned int pclk_active_neg: 1;    /*!< The display will write data lines when there's a falling edge on WR signal (a.k.a the PCLK) */
        unsigned int pclk_idle_low: 1;      /*!< The WR signal (a.k.a the PCLK) stays at low level in IDLE phase */
    } flags;                                /*!< Panel IO config flags */
} esp_lcd_panel_io_i80_config_t;

/**
 * @brief Create LCD panel IO, for Intel 8080 interface
 *
 * @param[in] bus Intel 8080 bus handle, created by `esp_lcd_new_i80_bus()`
 * @param[in] io_config IO configuration, for i80 interface
 * @param[out] ret_io Returned panel IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NOT_SUPPORTED if some configuration can't be satisfied, e.g. pixel clock out of the range
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_i80(esp_lcd_i80_bus_handle_t bus, const esp_lcd_panel_io_i80_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

/**
 * @brief Allocate a draw buffer that can be used by I80 interfaced LCD panel
 *
 * @note This function differs from the normal 'heap_caps_*' functions in that it can also automatically handle the alignment required by DMA burst, cache line size, etc.
 *
 * @param[in] io Panel IO handle, created by `esp_lcd_new_panel_io_i80()`
 * @param[in] size Size of memory to be allocated
 * @param[in] caps Bitwise OR of MALLOC_CAP_* flags indicating the type of memory desired for the allocation
 * @return Pointer to a new buffer of size 'size' with capabilities 'caps', or NULL if allocation failed
 */
void *esp_lcd_i80_alloc_draw_buffer(esp_lcd_panel_io_handle_t io, size_t size, uint32_t caps);

#endif // SOC_LCD_I80_SUPPORTED

#ifdef __cplusplus
}
#endif
