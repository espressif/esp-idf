/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int esp_lcd_spi_bus_handle_t;  /*!< Type of LCD SPI bus handle */

/**
 * @brief Panel IO configuration structure, for SPI interface
 */
typedef struct {
    int cs_gpio_num; /*!< GPIO used for CS line */
    int dc_gpio_num; /*!< GPIO used to select the D/C line, set this to -1 if the D/C line is not used */
    int spi_mode;    /*!< Traditional SPI mode (0~3) */
    unsigned int pclk_hz;    /*!< Frequency of pixel clock */
    size_t trans_queue_depth; /*!< Size of internal transaction queue */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data transfer has finished */
    void *user_ctx;    /*!< User private data, passed directly to on_color_trans_done's user_ctx */
    int lcd_cmd_bits;   /*!< Bit-width of LCD command */
    int lcd_param_bits; /*!< Bit-width of LCD parameter */
    uint8_t cs_ena_pretrans;        /*!< Amount of SPI bit-cycles the cs should be activated before the transmission (0-16) */
    uint8_t cs_ena_posttrans;       /*!< Amount of SPI bit-cycles the cs should stay active after the transmission (0-16) */
    struct {
        unsigned int dc_high_on_cmd: 1;  /*!< If enabled, DC level = 1 indicates command transfer */
        unsigned int dc_low_on_data: 1;  /*!< If enabled, DC level = 0 indicates color data transfer */
        unsigned int dc_low_on_param: 1; /*!< If enabled, DC level = 0 indicates parameter transfer */
        unsigned int octal_mode: 1;      /*!< transmit with octal mode (8 data lines), this mode is used to simulate Intel 8080 timing */
        unsigned int quad_mode: 1;       /*!< transmit with quad mode (4 data lines), this mode is useful when transmitting LCD parameters (Only use one line for command) */
        unsigned int sio_mode: 1;        /*!< Read and write through a single data line (MOSI) */
        unsigned int lsb_first: 1;       /*!< transmit LSB bit first */
        unsigned int cs_high_active: 1;  /*!< CS line is high active */
    } flags; /*!< Extra flags to fine-tune the SPI device */
} esp_lcd_panel_io_spi_config_t;

/**
 * @brief Create LCD panel IO handle, for SPI interface
 *
 * @param[in] bus SPI bus handle
 * @param[in] io_config IO configuration, for SPI interface
 * @param[out] ret_io Returned IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_spi(esp_lcd_spi_bus_handle_t bus, const esp_lcd_panel_io_spi_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

#ifdef __cplusplus
}
#endif
