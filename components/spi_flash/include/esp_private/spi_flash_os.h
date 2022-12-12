/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * System level MSPI APIs (private)
 */
/**
 * Currently the MSPI timing tuning related APIs are designed to be private.
 * Because:
 * 1. now we don't split SPI0 and SPI1, we don't have a component for SPI0, including PSRAM, Cache, etc..
 * 2. SPI0 and SPI1 are strongly coupling.
 *
 * In the future, we may consider creating a component for SPI0, and spi_flash component will only work on SPI1 (and it
 * can rely on SPI0). Therefore, we can put these APIs there.
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_rom_spiflash.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "hal/spi_flash_hal.h"
#include "spi_flash_override.h"
#include "soc/soc_caps.h"

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
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    ESP_MSPI_IO_DQS,
    ESP_MSPI_IO_D4,
    ESP_MSPI_IO_D5,
    ESP_MSPI_IO_D6,
    ESP_MSPI_IO_D7,
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE
#if CONFIG_SPIRAM
    ESP_MSPI_IO_CS1 /* cs for spi ram */
#endif
} esp_mspi_io_t;

/**
 * @brief To setup Flash chip
 */
esp_err_t spi_flash_init_chip_state(void);

/**
 * @brief Make MSPI work under 20Mhz, remove the timing tuning required delays.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_low_speed_mode(bool control_spi1);

/**
 * @brief Make MSPI work under the frequency as users set, may add certain delays to MSPI RX direction to meet timing requirements.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_high_speed_mode(bool control_spi1);

/**
 * @brief Switch MSPI into low speed mode / high speed mode.
 * @note This API is cache safe, it will freeze both D$ and I$ and restore them after MSPI is switched
 * @note For some of the MSPI high frequency settings (e.g. 80M DDR mode Flash or PSRAM), timing tuning is required.
 *       Certain delays will be added to the MSPI RX direction. When CPU clock switches from PLL to XTAL, should call
 *       this API first to enter MSPI low speed mode to remove the delays, and vice versa.
 */
void spi_timing_change_speed_mode_cache_safe(bool switch_down);

/**
 * @brief Tune MSPI flash timing to make it work under high frequency
 */
void spi_timing_flash_tuning(void);

/**
 * @brief Tune MSPI psram timing to make it work under high frequency
 */
void spi_timing_psram_tuning(void);

/**
 * @brief To initislize the MSPI pins
 */
void esp_mspi_pin_init(void);

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
 * @brief Get the knowledge if the MSPI timing is tuned or not
 */
bool spi_timing_is_tuned(void);

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
 * @brief Enable SPI flash high performance mode.
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

typedef enum {
    FLASH_WRAP_MODE_8B = 0,
    FLASH_WRAP_MODE_16B = 2,
    FLASH_WRAP_MODE_32B = 4,
    FLASH_WRAP_MODE_64B = 6,
    FLASH_WRAP_MODE_DISABLE = 1
} spi_flash_wrap_mode_t;

/**
 * @brief set wrap mode of flash
 *
 * @param mode: wrap mode support disable, 16 32, 64 byte
 *
 * @return esp_err_t : ESP_OK for successful.
 *
 */
esp_err_t spi_flash_wrap_set(spi_flash_wrap_mode_t mode);

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
