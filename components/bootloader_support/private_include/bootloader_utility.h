/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "bootloader_config.h"
#include "esp_image_format.h"
#include "bootloader_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Load partition table.
 *
 * Parse partition table, get useful data such as location of
 * OTA data partition, factory app partition, and test app partition.
 *
 * @param[out] bs Bootloader state structure used to save read data.
 * @return        Return true if the partition table was successfully loaded and MD5 checksum is valid.
 */
bool bootloader_utility_load_partition_table(bootloader_state_t* bs);

/**
 * @brief Return the index of the selected boot partition.
 *
 * This is the preferred boot partition, as determined by the partition table &
 * any OTA sequence number found in OTA data.
 * This partition will only be booted if it contains a valid app image, otherwise load_boot_image() will search
 * for a valid partition using this selection as the starting point.
 *
 * @param[in] bs Bootloader state structure.
 * @return       Returns the index on success, INVALID_INDEX otherwise.
 */
int bootloader_utility_get_selected_boot_partition(const bootloader_state_t *bs);

/**
 * @brief Load and verify the TEE image from the selected partition
 *
 * @param bs Bootloader state structure
 */
void bootloader_utility_load_tee_image(const bootloader_state_t *bs);

/**
 * @brief Load the selected partition and start application.
 *
 * Start from partition 'start_index', if not bootable then work backwards to FACTORY_INDEX
 * (ie try any OTA slots in descending order and then the factory partition).
 * If still nothing, start from 'start_index + 1' and work up to highest numbered OTA partition.
 * If still nothing, try TEST_APP_INDEX.
 * Everything this function calls must be located in the iram_loader_seg segment.
 *
 * @param[in] bs Bootloader state structure.
 * @param[in] start_index The index from which the search for images begins.
 */
__attribute__((__noreturn__)) void bootloader_utility_load_boot_image(const bootloader_state_t *bs, int start_index);

#ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
/**
 * @brief Load that application which was worked before we go to the deep sleep.
 *
 * If chip supports the RTC memory:
 * Checks the reboot reason if it is the deep sleep and has a valid partition in the RTC memory
 * then try to load the application which was worked before we go to the deep sleep.
 *
 * If chip does not support the RTC memory:
 * Checks the reboot reason if it is the deep sleep then the partition table is read
 * to select and load an application which was worked before we go to the deep sleep.
 *
 */
void bootloader_utility_load_boot_image_from_deep_sleep(void);
#endif

/**
 * @brief Software reset the ESP32
 *
 * Bootloader code should call this in the case that it cannot proceed.
 *
 * It is not recommended to call this function from an app (if called, the app will abort).
 */
__attribute__((__noreturn__)) void bootloader_reset(void);

/**
 * @brief Do any cleanup before exiting the bootloader, before starting the app or resetting
 */
void bootloader_atexit(void);

#ifdef __cplusplus
}
#endif
