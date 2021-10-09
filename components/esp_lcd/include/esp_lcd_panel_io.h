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

typedef void *esp_lcd_spi_bus_handle_t;                       /*!< Type of LCD SPI bus handle */
typedef void *esp_lcd_i2c_bus_handle_t;                       /*!< Type of LCD I2C bus handle */
typedef struct esp_lcd_i80_bus_t *esp_lcd_i80_bus_handle_t;   /*!< Type of LCD intel 8080 bus handle */

/**
 * @brief Transmit LCD command and corresponding parameters
 *
 * @note Commands sent by this function are short, so they are sent using polling transactions.
 *       The function does not return before the command tranfer is completed.
 *       If any queued transactions sent by `esp_lcd_panel_io_tx_color()` are still pending when this function is called,
 *       this function will wait until they are finished and the queue is empty before sending the command(s).
 *
 * @param[in] io LCD panel IO handle, which is created by other factory API like `esp_lcd_new_panel_io_spi()`
 * @param[in] lcd_cmd The specific LCD command
 * @param[in] param Buffer that holds the command specific parameters, set to NULL if no parameter is needed for the command
 * @param[in] param_size Size of `param` in memory, in bytes, set to zero if no parameter is needed for the command
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size);

/**
 * @brief Transmit LCD RGB data
 *
 * @note This function will package the command and RGB data into a transaction, and push into a queue.
 *       The real transmission is performed in the background (DMA+interrupt).
 *       The caller should take care of the lifecycle of the `color` buffer.
 *       Recycling of color buffer should be done in the callback `on_color_trans_done()`.
 *
 * @param[in] io LCD panel IO handle, which is created by factory API like `esp_lcd_new_panel_io_spi()`
 * @param[in] lcd_cmd The specific LCD command
 * @param[in] color Buffer that holds the RGB color data
 * @param[in] color_size Size of `color` in memory, in bytes
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_panel_io_tx_color(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *color, size_t color_size);

/**
 * @brief Destory LCD panel IO handle (deinitialize panel and free all corresponding resource)
 *
 * @param[in] io LCD panel IO handle, which is created by factory API like `esp_lcd_new_panel_io_spi()`
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);

/**
 * @brief Type of LCD panel IO event data
 */
typedef struct {
} esp_lcd_panel_io_event_data_t;

/**
 * @brief Declare the prototype of the function that will be invoked when panel IO finishes transferring color data
 *
 * @param[in] panel_io LCD panel IO handle, which is created by factory API like `esp_lcd_new_panel_io_spi()`
 * @param[in] edata Panel IO event data, fed by driver
 * @param[in] user_ctx User data, passed from `esp_lcd_panel_io_xxx_config_t`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_panel_io_color_trans_done_cb_t)(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);

/**
 * @brief Panel IO configuration structure, for SPI interface
 */
typedef struct {
    int cs_gpio_num; /*!< GPIO used for CS line */
    int dc_gpio_num; /*!< GPIO used to select the D/C line, set this to -1 if the D/C line not controlled by manually pulling high/low GPIO */
    int spi_mode;    /*!< Traditional SPI mode (0~3) */
    unsigned int pclk_hz;    /*!< Frequency of pixel clock */
    size_t trans_queue_depth; /*!< Size of internal transaction queue */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data transfer has finished */
    void *user_ctx;    /*!< User private data, passed directly to on_color_trans_done's user_ctx */
    int lcd_cmd_bits;   /*!< Bit-width of LCD command */
    int lcd_param_bits; /*!< Bit-width of LCD parameter */
    struct {
        unsigned int dc_as_cmd_phase: 1; /*!< D/C line value is encoded into SPI transaction command phase */
        unsigned int dc_low_on_data: 1;  /*!< If this flag is enabled, DC line = 0 means transfer data, DC line = 1 means transfer command; vice versa */
        unsigned int octal_mode: 1;      /*!< transmit with octal mode (8 data lines), this mode is used to simulate Intel 8080 timing */
    } flags;
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

typedef struct {
    uint32_t dev_addr; /*!< I2C device address */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data transfer has finished */
    void *user_ctx; /*!< User private data, passed directly to on_color_trans_done's user_ctx */
    size_t control_phase_bytes; /*!< I2C LCD panel will encode control information (e.g. D/C seclection) into control phase, in several bytes */
    unsigned int dc_bit_offset; /*!< Offset of the D/C selection bit in control phase */
    int lcd_cmd_bits;           /*!< Bit-width of LCD command */
    int lcd_param_bits;         /*!< Bit-width of LCD parameter */
    struct {
        unsigned int dc_low_on_data: 1;  /*!< If this flag is enabled, DC line = 0 means transfer data, DC line = 1 means transfer command; vice versa */
    } flags;
} esp_lcd_panel_io_i2c_config_t;

/**
 * @brief Create LCD panel IO handle, for I2C interface
 *
 * @param[in] bus I2C bus handle
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_i2c(esp_lcd_i2c_bus_handle_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

#if SOC_LCD_I80_SUPPORTED
/**
 * @brief LCD Intel 8080 bus configuration structure
 */
typedef struct {
    int dc_gpio_num; /*!< GPIO used for D/C line */
    int wr_gpio_num; /*!< GPIO used for WR line */
    lcd_clock_source_t clk_src; /*!< Clock source for the I80 LCD peripheral */
    int data_gpio_nums[SOC_LCD_I80_BUS_WIDTH]; /*!< GPIOs used for data lines */
    size_t bus_width;          /*!< Number of data lines, 8 or 16 */
    size_t max_transfer_bytes; /*!< Maximum transfer size, this determines the length of internal DMA link */
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
 * @brief Destory Intel 8080 bus handle
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
    unsigned int pclk_hz;    /*!< Frequency of pixel clock */
    size_t trans_queue_depth; /*!< Transaction queue size, larger queue, higher throughput */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data was tranferred done */
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
    } flags;
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

#endif // SOC_LCD_I80_SUPPORTED

#ifdef __cplusplus
}
#endif
