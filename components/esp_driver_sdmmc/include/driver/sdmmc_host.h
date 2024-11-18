/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_default_configs.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Extra configuration for SDMMC peripheral slot
 */
typedef struct {
    gpio_num_t clk;         ///< GPIO number of CLK signal.
    gpio_num_t cmd;         ///< GPIO number of CMD signal.
    gpio_num_t d0;          ///< GPIO number of D0 signal.
    gpio_num_t d1;          ///< GPIO number of D1 signal.
    gpio_num_t d2;          ///< GPIO number of D2 signal.
    gpio_num_t d3;          ///< GPIO number of D3 signal.
    gpio_num_t d4;          ///< GPIO number of D4 signal. Ignored in 1- or 4- line mode.
    gpio_num_t d5;          ///< GPIO number of D5 signal. Ignored in 1- or 4- line mode.
    gpio_num_t d6;          ///< GPIO number of D6 signal. Ignored in 1- or 4- line mode.
    gpio_num_t d7;          ///< GPIO number of D7 signal. Ignored in 1- or 4- line mode.
    union {
        gpio_num_t gpio_cd;     ///< GPIO number of card detect signal
        gpio_num_t cd;          ///< GPIO number of card detect signal; shorter name.
    };
    union {
        gpio_num_t gpio_wp;     ///< GPIO number of write protect signal
        gpio_num_t wp;          ///< GPIO number of write protect signal; shorter name.
    };
    uint8_t width;          ///< Bus width used by the slot (might be less than the max width supported)
    uint32_t flags;         ///< Features used by this slot
#define SDMMC_SLOT_FLAG_INTERNAL_PULLUP  BIT(0)
    /**< Enable internal pullups on enabled pins. The internal pullups
     are insufficient however, please make sure external pullups are
     connected on the bus. This is for debug / example purpose only.
     */
#define SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH   BIT(1)
    /**< GPIO write protect polarity.
     * 0 means "active low", i.e. card is protected when the GPIO is low;
     * 1 means "active high", i.e. card is protected when GPIO is high.
     */
#define SDMMC_SLOT_FLAG_UHS1             BIT(2)
    /**< Enable UHS-I mode for this slot */
} sdmmc_slot_config_t;

/**
 * SD/MMC host state structure
 */
typedef struct {
    bool host_initialized;  ///< Whether the host is initialized
    int num_of_init_slots;  ///< Number of initialized slots
} sdmmc_host_state_t;

/**
 * @brief Initialize SDMMC host peripheral
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success or if sdmmc_host_init was already initialized with this function
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 */
esp_err_t sdmmc_host_init(void);

/**
 * @brief Initialize given slot of SDMMC peripheral
 *
 * On the ESP32, SDMMC peripheral has two slots:
 *  - Slot 0: 8-bit wide, maps to HS1_* signals in PIN MUX
 *  - Slot 1: 4-bit wide, maps to HS2_* signals in PIN MUX
 *
 * Card detect and write protect signals can be routed to
 * arbitrary GPIOs using GPIO matrix.
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param slot_config  additional configuration for the slot
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if host has not been initialized using sdmmc_host_init
 *      - ESP_ERR_INVALID_ARG if GPIO pins from slot_config are not valid
 */
esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t* slot_config);

/**
 * @brief Select bus width to be used for data transfer
 *
 * SD/MMC card must be initialized prior to this command, and a command to set
 * bus width has to be sent to the card (e.g. SD_APP_SET_BUS_WIDTH)
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param width  bus width (1, 4, or 8 for slot 0; 1 or 4 for slot 1)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if slot number or width is not valid
 */
esp_err_t sdmmc_host_set_bus_width(int slot, size_t width);

/**
 * @brief Get bus width configured in ``sdmmc_host_init_slot`` to be used for data transfer
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @return configured bus width of the specified slot.
 */
size_t sdmmc_host_get_slot_width(int slot);

/**
 * @brief Set card clock frequency
 *
 * Currently only integer fractions of 40MHz clock can be used.
 * For High Speed cards, 40MHz can be used.
 * For Default Speed cards, 20MHz can be used.
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param freq_khz  card clock frequency, in kHz
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in the future
 */
esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz);

/**
 * @brief Enable or disable DDR mode of SD interface
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param ddr_enabled  enable or disable DDR mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if DDR mode is not supported on this slot
 */
esp_err_t sdmmc_host_set_bus_ddr_mode(int slot, bool ddr_enabled);

/**
 * @brief Enable or disable always-on card clock
 * When cclk_always_on is false, the host controller is allowed to shut down
 * the card clock between the commands. When cclk_always_on is true, the clock
 * is generated even if no command is in progress.
 * @param slot  slot number
 * @param cclk_always_on  enable or disable always-on clock
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the slot number is invalid
 */
esp_err_t sdmmc_host_set_cclk_always_on(int slot, bool cclk_always_on);

/**
 * @brief Send command to the card and get response
 *
 * This function returns when command is sent and response is received,
 * or data is transferred, or timeout occurs.
 *
 * @note This function is not thread safe w.r.t. init/deinit functions,
 *       and bus width/clock speed configuration functions. Multiple tasks
 *       can call sdmmc_host_do_transaction as long as other sdmmc_host_*
 *       functions are not called.
 *
 * @attention Data buffer passed in cmdinfo->data must be in DMA capable memory and aligned to 4 byte boundary. If it's
 *            behind the cache, both cmdinfo->data and cmdinfo->buflen need to be aligned to cache line boundary.
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param cmdinfo   pointer to structure describing command and data to transfer
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TIMEOUT if response or data transfer has timed out
 *      - ESP_ERR_INVALID_CRC if response or data transfer CRC check has failed
 *      - ESP_ERR_INVALID_RESPONSE if the card has sent an invalid response
 *      - ESP_ERR_INVALID_SIZE if the size of data transfer is not valid in SD protocol
 *      - ESP_ERR_INVALID_ARG if the data buffer is not in DMA capable memory
 */
esp_err_t sdmmc_host_do_transaction(int slot, sdmmc_command_t* cmdinfo);

/**
 * @brief Enable IO interrupts
 *
 * This function configures the host to accept SDIO interrupts.
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @return returns ESP_OK, other errors possible in the future
 */
esp_err_t sdmmc_host_io_int_enable(int slot);

/**
 * @brief Block until an SDIO interrupt is received, or timeout occurs
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param timeout_ticks  number of RTOS ticks to wait for the interrupt
 * @return
 *  - ESP_OK on success (interrupt received)
 *  - ESP_ERR_TIMEOUT if the interrupt did not occur within timeout_ticks
 */
esp_err_t sdmmc_host_io_int_wait(int slot, TickType_t timeout_ticks);

/**
 * @brief Disable SDMMC host and release allocated resources gracefully
 *
 * @note If there are more than 1 active slots, this function will just decrease the reference count
 *       and won't actually disable the host until the last slot is disabled
 *
 * @note This function is not thread safe
 *
 * @param slot slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if SDMMC host has not been initialized
 *      - ESP_ERR_INVALID_ARG if invalid slot number is used
 */
esp_err_t sdmmc_host_deinit_slot(int slot);

/**
 * @brief Disable SDMMC host and release allocated resources forcefully
 *
 * @note This function will deinitialize the host immediately, regardless of the number of active slots
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if SDMMC host has not been initialized
 */
esp_err_t sdmmc_host_deinit(void);

/**
 * @brief Provides a real frequency used for an SD card installed on specific slot
 * of SD/MMC host controller
 *
 * This function calculates real working frequency given by current SD/MMC host
 * controller setup for required slot: it reads associated host and card dividers
 * from corresponding SDMMC registers, calculates respective frequency and stores
 * the value into the 'real_freq_khz' parameter
 *
 * @param slot slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param[out] real_freq_khz output parameter for the result frequency (in kHz)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG on real_freq_khz == NULL or invalid slot number used
 */
esp_err_t sdmmc_host_get_real_freq(int slot, int* real_freq_khz);

/**
 * @brief set input delay
 *
 * @note ESP32 doesn't support this feature, you will get an `ESP_ERR_NOT_SUPPORTED`
 *
 * - This API sets delay when the SDMMC Host samples the signal from the SD Slave.
 * - This API will check if the given `delay_phase` is valid or not.
 * - This API will print out the delay time, in picosecond (ps)
 *
 * @param slot         slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param delay_phase  delay phase, this API will convert the phase into picoseconds and print it out
 *
 * @return
 *        - ESP_OK:                ON success.
 *        - ESP_ERR_INVALID_ARG:   Invalid argument.
 *        - ESP_ERR_NOT_SUPPORTED: ESP32 doesn't support this feature.
 */
esp_err_t sdmmc_host_set_input_delay(int slot, sdmmc_delay_phase_t delay_phase);

/**
 * @brief Get the DMA memory information for the host driver
 *
 * @param[in]  slot slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param[out] dma_mem_info  DMA memory information structure
 * @return
 *        - ESP_OK:                ON success.
 *        - ESP_ERR_INVALID_ARG:   Invalid argument.
 */
esp_err_t sdmmc_host_get_dma_info(int slot, esp_dma_mem_info_t *dma_mem_info);

/**
 * @brief Check if the slot is set to uhs1 or not
 *
 * @param[in]  slot     Slot id
 * @param[out] is_uhs1  Is uhs1 or not
 *
 * @return
 *        - ESP_OK:                on success
 *        - ESP_ERR_INVALID_STATE: driver not in correct state
 */
esp_err_t sdmmc_host_is_slot_set_to_uhs1(int slot, bool *is_uhs1);

/**
 * @brief Get the state of SDMMC host
 *
 * @param[out] state output parameter for SDMMC host state structure
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG on invalid argument
 */
esp_err_t sdmmc_host_get_state(sdmmc_host_state_t* state);

#ifdef __cplusplus
}
#endif

#endif //SOC_SDMMC_HOST_SUPPORTED
