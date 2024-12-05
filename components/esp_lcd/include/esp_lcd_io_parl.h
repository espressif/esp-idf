/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "soc/soc_caps.h"
#include "driver/parlio_types.h"

#define ESP_PARLIO_LCD_WIDTH_MAX 8 /*!< Maximum data width of parlio lcd interface */

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PARLIO_SUPPORTED
/**
 * @brief Parallel Panel IO configuration structure, for intel 8080 interface(8 data-lines) or SPI interface(1 data-lines)
 */
typedef struct {
    int dc_gpio_num; /*!< GPIO used for D/C line */
    int clk_gpio_num; /*!< GPIO used for CLK line */
    int cs_gpio_num;         /*!< GPIO used for CS line */
    int data_gpio_nums[ESP_PARLIO_LCD_WIDTH_MAX]; /*!< GPIOs used for data lines */
    size_t data_width;          /*!< Number of data lines, 1(SPI) or 8(I80) */
    uint32_t pclk_hz;        /*!< Frequency of pixel clock */
    parlio_clock_source_t clk_src; /*!< Clock source for the Parlio peripheral */
    size_t max_transfer_bytes; /*!< Maximum transfer size, this determines the length of internal DMA link */
    size_t dma_burst_size;     /*!< DMA burst size, in bytes */
    size_t trans_queue_depth; /*!< Transaction queue size, larger queue, higher throughput */
    int lcd_cmd_bits;   /*!< Bit-width of LCD command */
    int lcd_param_bits; /*!< Bit-width of LCD parameter */
    struct {
        unsigned int dc_cmd_level: 1;   /*!< Level of DC line in CMD phase */
        unsigned int dc_data_level: 1;  /*!< Level of DC line in DATA phase */
    } dc_levels; /*!< Each LCD device might have its own D/C control logic */
    struct {
        unsigned int cs_active_high: 1;     /*!< If set, a high level of CS line will select the device, otherwise, CS line is low level active */
    } flags;                                /*!< Panel IO config flags */
} esp_lcd_panel_io_parl_config_t;

/**
 * @brief Create LCD panel IO, for parlio interface
 *
 * @param[in] io_config IO configuration, for parlio interface
 * @param[out] ret_io Returned panel IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NOT_SUPPORTED if some configuration can't be satisfied, e.g. pixel clock out of the range
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_parl(const esp_lcd_panel_io_parl_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

/**
 * @brief Allocate a draw buffer that can be used by parlio interface LCD panel
 *
 * @note This function differs from the normal 'heap_caps_*' functions in that it can also automatically handle the alignment required by DMA burst, cache line size, etc.
 *
 * @param[in] io Panel IO handle, created by `esp_lcd_new_panel_io_parl()`
 * @param[in] size Size of memory to be allocated
 * @param[in] caps Bitwise OR of MALLOC_CAP_* flags indicating the type of memory desired for the allocation
 * @return Pointer to a new buffer of size 'size' with capabilities 'caps', or NULL if allocation failed
 */
void *esp_lcd_parlio_alloc_draw_buffer(esp_lcd_panel_io_handle_t io, size_t size, uint32_t caps);

#endif // SOC_PARLIO_SUPPORTED

#ifdef __cplusplus
}
#endif
