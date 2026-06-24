/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include "esp_partition.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <stdio.h>
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"
#include "esp_partition.h"
#include "soc/soc_caps.h"
#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
#include "hal/spimem_flash_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the 'flash_test' custom data partition
 * defined in the custom partition table.
 *
 * @return partition handle
 */
const esp_partition_t *get_test_flash_partition(void);

#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
// Forward declaration of the flash test handle structure
typedef struct flash_test_handle_s flash_test_handle_t;

// Event data structure for flash test (currently empty, can be expanded as needed)
typedef struct {
} flash_test_event_data_t;

// Callback type definition for handling flash test events
typedef bool (*flash_test_callback_t)(flash_test_handle_t *handle, const flash_test_event_data_t *event_data, void *arg);

// Structure to hold event callbacks for flash test
typedef struct {
    flash_test_callback_t auto_suspend_done;         // Callback triggered when auto suspend is complete
} flash_test_event_callbacks_t;

// Definition of the flash test handle structure
struct flash_test_handle_s {
    spi_mem_dev_t *dev;
    flash_test_callback_t auto_suspend_done;         // Callback for auto suspend event
    uint32_t suspend_times;                          // Counter for the number of suspend operations
    intr_handle_t intr_handle;                       // Handle for the interrupt associated with flash test
    uint32_t operation_address;                      // flash operation address.
    uint32_t operation_size;                         // flash operation size.
};

/**
 * @brief Initialize the flash test system.
 *
 * This function sets up the flash test handle and necessary resources.
 *
 * @param[out] test_handle Pointer to the pointer that will hold the initialized handle.
 * @return esp_err_t ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t spi_flash_suspend_test_init(flash_test_handle_t **test_handle);

/**
 * @brief Deinitialize the flash test system.
 *
 * This function releases the resources associated with the flash test handle.
 *
 * @param[in] handle Pointer to the flash test handle to deinitialize.
 * @return esp_err_t ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t spi_flash_suspend_test_deinit(flash_test_handle_t *handle);

/**
 * @brief Finds the last partition in the flash memory.
 *
 * This function retrieves the last partition available for testing purposes.
 *
 * @return const esp_partition_t* Pointer to the last partition, or NULL if none found.
 */
const esp_partition_t *spi_flash_suspend_test_find_last_partition(void);

/**
 * @brief Invalidates the flash cache.
 */
void spi_flash_suspend_test_invalidate_cache(void);

#endif

#ifdef __cplusplus
}
#endif
