/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/spi_flash_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct spi_flash_chip_t;
typedef struct spi_flash_chip_t spi_flash_chip_t;

/** @brief OS-level integration hooks for accessing flash chips inside a running OS
 *
 * This structure is used internally by the SPI flash driver and should not be used directly by applications.
 * It's defined here because some instances need to be allocated statically in the startup code.
 */
typedef struct esp_flash_os_functions_t {
    /**
     * Flags for start function
     */
    /** Limit CPU frequency during flash operations (ESP32-C5 only, 240MHz).
     */
    #define ESP_FLASH_START_FLAG_LIMIT_CPU_FREQ  BIT(0)
    /**
     * Called before commencing any flash operation. Does not need to be
     * recursive (ie is called at most once for each call to 'end').
     */
    esp_err_t (*start)(void *arg, uint32_t flags);

    /** Called after completing any flash operation. */
    esp_err_t (*end)(void *arg);

    /** Called before any erase/write operations to check whether the region is limited by the OS */
    esp_err_t (*region_protected)(void* arg, size_t start_addr, size_t size);

    /** Delay for at least 'us' microseconds. Called in between 'start' and 'end'. */
    esp_err_t (*delay_us)(void *arg, uint32_t us);

    /** Called for get temp buffer when buffer from application cannot be directly read into/write from. */
    void *(*get_temp_buffer)(void* arg, size_t reqest_size, size_t* out_size);

    /** Called for release temp buffer. */
    void (*release_temp_buffer)(void *arg, void *temp_buf);

    #define SPI_FLASH_YIELD_REQ_YIELD   BIT(0)
    #define SPI_FLASH_YIELD_REQ_SUSPEND BIT(1)

    /** Yield to other tasks. Called during erase operations.
     * @return ESP_OK means yield needs to be called (got an event to handle), while ESP_ERR_TIMEOUT means skip yield.*/
    esp_err_t (*check_yield)(void *arg, uint32_t chip_status, uint32_t* out_request);

    #define SPI_FLASH_YIELD_STA_RESUME  BIT(2)

    /** Yield to other tasks. Called during erase operations. */
    esp_err_t (*yield)(void *arg, uint32_t* out_status);

    /** Called for get system time. */
    int64_t (*get_system_time)(void *arg);

    #define SPI_FLASH_OS_IS_ERASING_STATUS_FLAG   BIT(0)

    /** Call to set flash operation status */
    void (*set_flash_op_status)(uint32_t op_status);

} esp_flash_os_functions_t;

/** @brief Structure to describe a SPI flash chip connected to the system.
 *
 * This structure is used internally by the SPI flash driver and should not be used directly by applications.
 * It's defined here because some instances need to be allocated statically in the startup code.
 */
struct esp_flash_t {
    spi_flash_host_inst_t* host;   ///< Pointer to hardware-specific "host_driver" structure. Must be initialized before used.
    const spi_flash_chip_t *chip_drv;   ///< Pointer to chip-model-specific "adapter" structure. If NULL, will be detected during initialisation.

    const esp_flash_os_functions_t *os_func;    ///< Pointer to os-specific hook structure. Call ``esp_flash_init_os_functions()`` to setup this field, after the host is properly initialized.
    void *os_func_data;                         ///< Pointer to argument for os-specific hooks. Left NULL and will be initialized with ``os_func``.

    esp_flash_io_mode_t read_mode; ///< Configured SPI flash read mode. Set before ``esp_flash_init`` is called.
    uint32_t size;                   ///< Size of SPI flash in bytes. If 0, size will be detected during initialisation. Note: this stands for the size in the binary image header. If you want to get the flash physical size, please call `esp_flash_get_physical_size`.
    uint32_t chip_id;               ///< Detected chip id.
    uint32_t busy             :1;   ///< This flag is used to verify chip's status.
    uint32_t hpm_dummy_ena    :1;   ///< This flag is used to verify whether flash works under HPM status.
    uint32_t reserved_flags   :30;  ///< reserved.
    int clock_source;               ///< Clock source for GPSPI.
};

#ifdef __cplusplus
}
#endif
