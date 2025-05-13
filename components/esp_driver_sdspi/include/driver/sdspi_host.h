/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sd_protocol_types.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Handle representing an SD SPI device
typedef int sdspi_dev_handle_t;

#if CONFIG_IDF_TARGET_ESP32
#define SDSPI_DEFAULT_HOST SPI2_HOST
#define SDSPI_DEFAULT_DMA  SDSPI_DEFAULT_HOST
#elif CONFIG_IDF_TARGET_ESP32S2
#define SDSPI_DEFAULT_HOST SPI3_HOST
#define SDSPI_DEFAULT_DMA  SDSPI_DEFAULT_HOST
#else
#define SDSPI_DEFAULT_HOST SPI2_HOST
#define SDSPI_DEFAULT_DMA  SPI_DMA_CH_AUTO
#endif

/**
 * @brief Default sdmmc_host_t structure initializer for SD over SPI driver
 *
 * Uses SPI mode and max frequency set to 20MHz
 *
 * 'slot' should be set to an sdspi device initialized by `sdspi_host_init_device()`.
 */
#define SDSPI_HOST_DEFAULT() {\
    .flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG, \
    .slot = SDSPI_DEFAULT_HOST, \
    .max_freq_khz = SDMMC_FREQ_DEFAULT, \
    .io_voltage = 3.3f, \
    .driver_strength = SDMMC_DRIVER_STRENGTH_B, \
    .current_limit = SDMMC_CURRENT_LIMIT_200MA, \
    .init = &sdspi_host_init, \
    .set_bus_width = NULL, \
    .get_bus_width = NULL, \
    .set_bus_ddr_mode = NULL, \
    .set_card_clk = &sdspi_host_set_card_clk, \
    .set_cclk_always_on = NULL, \
    .do_transaction = &sdspi_host_do_transaction, \
    .deinit_p = &sdspi_host_remove_device, \
    .io_int_enable = &sdspi_host_io_int_enable, \
    .io_int_wait = &sdspi_host_io_int_wait, \
    .command_timeout_ms = 0, \
    .get_real_freq = &sdspi_host_get_real_freq, \
    .input_delay_phase = SDMMC_DELAY_PHASE_0, \
    .set_input_delay = NULL, \
    .set_input_delayline = NULL, \
    .dma_aligned_buffer = NULL, \
    .pwr_ctrl_handle = NULL, \
    .get_dma_info = NULL, \
    .check_buffer_alignment = sdspi_host_check_buffer_alignment, \
    .is_slot_set_to_uhs1 = NULL, \
}

/**
 * Extra configuration for SD SPI device.
 */
typedef struct {
    spi_host_device_t host_id; ///< SPI host to use, SPIx_HOST (see spi_types.h).
    gpio_num_t gpio_cs;        ///< GPIO number of CS signal
    gpio_num_t gpio_cd;        ///< GPIO number of card detect signal
    gpio_num_t gpio_wp;        ///< GPIO number of write protect signal
    gpio_num_t gpio_int;       ///< GPIO number of interrupt line (input) for SDIO card.
    bool gpio_wp_polarity;     /*!< GPIO write protect polarity
                                    0 means "active low", i.e. card is protected when the GPIO is low;
                                    1 means "active high", i.e. card is protected when GPIO is high. */
    uint16_t duty_cycle_pos;  ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
} sdspi_device_config_t;

#define SDSPI_SLOT_NO_CS          GPIO_NUM_NC      ///< indicates that card select line is not used
#define SDSPI_SLOT_NO_CD          GPIO_NUM_NC      ///< indicates that card detect line is not used
#define SDSPI_SLOT_NO_WP          GPIO_NUM_NC      ///< indicates that write protect line is not used
#define SDSPI_SLOT_NO_INT         GPIO_NUM_NC      ///< indicates that interrupt line is not used
#define SDSPI_IO_ACTIVE_LOW       0

/**
 * Macro defining default configuration of SD SPI device.
 */
#define SDSPI_DEVICE_CONFIG_DEFAULT() {\
    .host_id   = SDSPI_DEFAULT_HOST, \
    .gpio_cs   = GPIO_NUM_13, \
    .gpio_cd   = SDSPI_SLOT_NO_CD, \
    .gpio_wp   = SDSPI_SLOT_NO_WP, \
    .gpio_int  = GPIO_NUM_NC, \
    .gpio_wp_polarity = SDSPI_IO_ACTIVE_LOW, \
    .duty_cycle_pos = 0,\
}

/**
 * @brief Initialize SD SPI driver
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in future versions
 */
esp_err_t sdspi_host_init(void);

/**
* @brief Attach and initialize an SD SPI device on the specific SPI bus
*
* @note This function is not thread safe
*
* @note Initialize the SPI bus by `spi_bus_initialize()` before calling this function.
*
* @note The SDIO over sdspi needs an extra interrupt line. Call ``gpio_install_isr_service()`` before this function.
*
* @param dev_config pointer to device configuration structure
* @param out_handle Output of the handle to the sdspi device.

* @return
*      - ESP_OK on success
*      - ESP_ERR_INVALID_ARG if sdspi_host_init_device has invalid arguments
*      - ESP_ERR_NO_MEM if memory can not be allocated
*      - other errors from the underlying spi_master and gpio drivers
*/
esp_err_t sdspi_host_init_device(const sdspi_device_config_t* dev_config, sdspi_dev_handle_t* out_handle);

/**
 * @brief Remove an SD SPI device
 *
 * @param handle Handle of the SD SPI device
 * @return Always ESP_OK
 */
esp_err_t sdspi_host_remove_device(sdspi_dev_handle_t handle);

/**
 * @brief Send command to the card and get response
 *
 * This function returns when command is sent and response is received,
 * or data is transferred, or timeout occurs.
 *
 * @note This function is not thread safe w.r.t. init/deinit functions,
 *       and bus width/clock speed configuration functions. Multiple tasks
 *       can call sdspi_host_do_transaction as long as other sdspi_host_*
 *       functions are not called.
 *
 * @param handle    Handle of the sdspi device
 * @param cmdinfo   pointer to structure describing command and data to transfer
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TIMEOUT if response or data transfer has timed out
 *      - ESP_ERR_INVALID_CRC if response or data transfer CRC check has failed
 *      - ESP_ERR_INVALID_RESPONSE if the card has sent an invalid response
 */
esp_err_t sdspi_host_do_transaction(sdspi_dev_handle_t handle, sdmmc_command_t *cmdinfo);

/**
 * @brief Set card clock frequency
 *
 * Currently only integer fractions of 40MHz clock can be used.
 * For High Speed cards, 40MHz can be used.
 * For Default Speed cards, 20MHz can be used.
 *
 * @note This function is not thread safe
 *
 * @param host    Handle of the sdspi device
 * @param freq_khz  card clock frequency, in kHz
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in the future
 */
esp_err_t sdspi_host_set_card_clk(sdspi_dev_handle_t host, uint32_t freq_khz);

/**
 * @brief Calculate working frequency for specific device
 *
 * @param handle SDSPI device handle
 * @param[out] real_freq_khz output parameter to hold the calculated frequency (in kHz)
 *
 * @return
 *      - ESP_ERR_INVALID_ARG : ``handle`` is NULL or invalid or ``real_freq_khz`` parameter is NULL
 *      - ESP_OK : Success
 */
esp_err_t sdspi_host_get_real_freq(sdspi_dev_handle_t handle, int* real_freq_khz);

/**
 * @brief Release resources allocated using sdspi_host_init
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sdspi_host_init function has not been called
 */
esp_err_t sdspi_host_deinit(void);

/**
 * @brief Enable SDIO interrupt.
 *
 * @param handle    Handle of the sdspi device
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t sdspi_host_io_int_enable(sdspi_dev_handle_t handle);

/**
 * @brief Wait for SDIO interrupt until timeout.
 *
 * @param handle    Handle of the sdspi device
 * @param timeout_ticks Ticks to wait before timeout.
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t sdspi_host_io_int_wait(sdspi_dev_handle_t handle, TickType_t timeout_ticks);

/**
 * @brief Get the DMA memory information for the host driver
 *
 * @deprecated This API is deprecated
 *
 * @param[in]  slot          Not used
 * @param[out] dma_mem_info  DMA memory information structure
 * @return
 *        - ESP_OK:                ON success.
 *        - ESP_ERR_INVALID_ARG:   Invalid argument.
 */
esp_err_t sdspi_host_get_dma_info(int slot, esp_dma_mem_info_t *dma_mem_info) __attribute__((deprecated("This API is deprecated")));

/**
 * @brief Check if the buffer meets the alignment requirements
 *
 * @param[in]  slot slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param[in]  buf  buffer pointer
 * @param[in]  size buffer size
 *
 * @return
 *        True for aligned buffer, false for not aligned buffer
 */
bool sdspi_host_check_buffer_alignment(int slot, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif
