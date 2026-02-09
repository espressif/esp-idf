/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * System level MSPI APIs (private)
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_rom_spiflash.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#include "hal/spi_flash_hal.h"

#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"
#include "esp_flash_chips/spi_flash_override.h"

#ifdef __cplusplus
extern "C" {
#endif

// Type of MSPI IO
typedef enum {
    ESP_MSPI_IO_CLK = 0,
    ESP_MSPI_IO_Q,
    ESP_MSPI_IO_D,
    ESP_MSPI_IO_CS0, /* cs for spi flash */
    ESP_MSPI_IO_HD,
    ESP_MSPI_IO_WP,
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    ESP_MSPI_IO_DQS,
    ESP_MSPI_IO_D4,
    ESP_MSPI_IO_D5,
    ESP_MSPI_IO_D6,
    ESP_MSPI_IO_D7,
#endif // SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
#if CONFIG_SPIRAM
    ESP_MSPI_IO_CS1, /* cs for spi ram */
#endif
    ESP_MSPI_IO_MAX, /* Maximum IO MSPI occupied */
} esp_mspi_io_t;

/**
 * @brief To setup Flash chip
 */
esp_err_t spi_flash_init_chip_state(void);

/**
 * @brief To initislize the MSPI pins
 */
void esp_mspi_pin_init(void);

/**
 * @brief Reserve MSPI IOs
 */
void esp_mspi_pin_reserve(void);

/**
 * @brief Get the number of the GPIO corresponding to the given MSPI io
 *
 * @param[in] io  MSPI io
 *
 * @return MSPI IO number
 */
uint8_t esp_mspi_get_io(esp_mspi_io_t io);

/**
 * @brief Set SPI1 registers to make ROM functions work
 * @note This function is used for setting SPI1 registers to the state that ROM SPI functions work
 */
void spi_flash_set_rom_required_regs(void);

/**
 * @brief Initialize main flash
 * @param chip Pointer to main SPI flash(SPI1 CS0) chip to use..
 */
esp_err_t esp_flash_init_main(esp_flash_t *chip);

/**
 * @brief Should be only used by SPI1 Flash driver to know the necessary timing registers
 * @param out_timing_config Pointer to timing_tuning parameters.
 */
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config);

/**
 * @brief Get the knowledge if the Flash timing is tuned or not
 */
bool spi_flash_timing_is_tuned(void);

/**
 * @brief Set Flash chip specifically required MSPI register settings here
 */
void spi_flash_set_vendor_required_regs(void);

/**
 * @brief Judge whether need to reset flash when brownout.
 *        Set` flash_brownout_needs_reset` inside the function if really need reset.
 */
void spi_flash_needs_reset_check(void);

/**
 * @brief Set flag to reset flash. set when erase chip or program chip
 *
 * @param bool status. True if flash is eraing. False if flash is not erasing.
 *
 * @return None.
 */
void spi_flash_set_erasing_flag(bool status);

/**
 * @brief Judge whether need to reset flash when brownout.
 *
 * @return true if need reset, otherwise false.
 */
bool spi_flash_brownout_need_reset(void);

/**
 * @brief Check whether esp-chip supports 32bit address properly
 *
 * @return ESP_OK for supported, ESP_ERR_NOT_SUPPORTED for not supported
*/
esp_err_t esp_mspi_32bit_address_flash_feature_check(void);

#if CONFIG_SPI_FLASH_HPM_ON
/**
 * @brief Enable SPI flash high performance mode.
 *
 * @note 1. When `CONFIG_SPI_FLASH_HPM_ON` is True, caller can always call this function without taking whether the used
 *          frequency falls into the HPM range into consideration.
 *       2. However, caller shouldn't attempt to call this function on Octal flash. `CONFIG_SPI_FLASH_HPM_ON` may be
 *          True when `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT && !CONFIG_ESPTOOLPY_OCT_FLASH`
 *
 * @return ESP_OK if success.
 */
esp_err_t spi_flash_enable_high_performance_mode(void);

/**
 * @brief Get the flash dummy through this function
 *        This can be used when one flash has several dummy configurations to enable the high performance mode.
 * @note Don't forget to subtract one when assign to the register of mspi e.g. if the value you get is 4, (4-1=3) should be assigned to the register.
 *
 * @return Pointer to spi_flash_hpm_dummy_conf_t.
 */
const spi_flash_hpm_dummy_conf_t *spi_flash_hpm_get_dummy(void);

/**
 * @brief Used to judge whether flash works under HPM mode with dummy adjustment.
 *
 * @return true Yes, and work under HPM with adjusting dummy. Otherwise, false.
 */
bool spi_flash_hpm_dummy_adjust(void);
#endif //CONFIG_SPI_FLASH_HPM_ON

#if CONFIG_ESP_SLEEP_SET_FLASH_DPD

/**
 * @brief Get the duration of entering deep power-down mode.
 *
 * @return Entering deep power-down mode time(tDp), in microseconds.
 */
uint32_t spi_flash_dpd_get_enter_duration(void);

/**
 * @brief Get the duration of exiting deep power-down mode.
 *
 * @return Exiting deep power-down mode time(tRES1), in microseconds.
 */
uint32_t spi_flash_dpd_get_exit_duration(void);

/**
 * @brief Enable or disable SPI flash deep power-down mode.
 *
 * @param bool status. True: flash enable deep power-down mode. False: flash disable deep power-down mode.
 *
 * @note If using self-provided flash (not the chip’s factory-default flash), consult its datasheet to use this API safely.
 *
 * @return ESP_OK if success.
 */
esp_err_t spi_flash_enable_deep_power_down_mode(bool enable);
#endif

#if SOC_SPI_MEM_SUPPORT_WRAP
/**
 * @brief set wrap size of flash
 *
 * @param wrap_size: wrap mode support disable, 16 32, 64 byte
 *
 * @return esp_err_t : ESP_OK for successful.
 *
 */
esp_err_t spi_flash_wrap_enable(spi_flash_wrap_size_t wrap_size);

/**
 * @brief Probe flash wrap method
 *
 * @return esp_err_t: ESP_OK for success
 */
esp_err_t spi_flash_wrap_probe(void);

/**
 * @brief disable cache wrap
 */
esp_err_t spi_flash_wrap_disable(void);

/**
 * @brief Check whether flash and esp chip supports wrap mode.
 *
 * @param wrap_size wrap size.
 * @return true: wrap support, otherwise, false.
 */
bool spi_flash_support_wrap_size(uint32_t wrap_size);

#endif //SOC_SPI_MEM_SUPPORT_WRAP

/**
 * @brief SPI flash critical section enter function.
 *
 */
typedef void (*spi_flash_guard_start_func_t)(void);
/**
 * @brief SPI flash critical section exit function.
 */
typedef void (*spi_flash_guard_end_func_t)(void);

/**
 * Structure holding SPI flash access critical sections management functions.
 *
 * Flash API uses two types of flash access management functions:
 * 1) Functions which prepare/restore flash cache and interrupts before calling
 *    appropriate ROM functions (SPIWrite, SPIRead and SPIEraseBlock):
 *   - 'start' function should disables flash cache and non-IRAM interrupts and
 *      is invoked before the call to one of ROM function above.
 *   - 'end' function should restore state of flash cache and non-IRAM interrupts and
 *      is invoked after the call to one of ROM function above.
 *    These two functions are not recursive.
 *
 * Different versions of the guarding functions should be used depending on the context of
 * execution (with or without functional OS). In normal conditions when flash API is called
 * from task the functions use OS primitives. When there is no OS at all or when
 * it is not guaranteed that OS is functional (accessing flash from exception handler) these
 * functions cannot use OS primitives or even does not need them (multithreaded access is not possible).
 *
 * @note Structure and corresponding guard functions should not reside in flash.
 *       For example structure can be placed in DRAM and functions in IRAM sections.
 */
typedef struct {
    spi_flash_guard_start_func_t        start;      /**< critical section start function. */
    spi_flash_guard_end_func_t          end;        /**< critical section end function. */
} spi_flash_guard_funcs_t;


/**
 * @brief  Sets guard functions to access flash.
 *
 * @note Pointed structure and corresponding guard functions should not reside in flash.
 *       For example structure can be placed in DRAM and functions in IRAM sections.
 *
 * @param funcs pointer to structure holding flash access guard functions.
 */
void spi_flash_guard_set(const spi_flash_guard_funcs_t* funcs);

/**
 * @brief Get the guard functions used for flash access
 *
 * @return The guard functions that were set via spi_flash_guard_set(). These functions
 * can be called if implementing custom low-level SPI flash operations.
 */
const spi_flash_guard_funcs_t *spi_flash_guard_get(void);

/**
 * @brief Default OS-aware flash access guard functions
 */
extern const spi_flash_guard_funcs_t g_flash_guard_default_ops;

/**
 * @brief Non-OS flash access guard functions
 *
 * @note This version of flash guard functions is to be used when no OS is present or from panic handler.
 *       It does not use any OS primitives and IPC and implies that only calling CPU is active.
 */
extern const spi_flash_guard_funcs_t g_flash_guard_no_os_ops;

/**
 * @brief This function is used to re-initialize the flash mmap when using ROM flash
 * implementations.
 *
 * @note Only called in startup. User should not call this function.
 */
void spi_flash_rom_impl_init(void);



#ifdef __cplusplus
}
#endif
