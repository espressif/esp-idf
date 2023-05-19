/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "bootloader_config.h"
#include "esp_image_format.h"
#include "bootloader_config.h"

/**
 * @brief Load partition table.
 *
 * Parse partition table, get useful data such as location of
 * OTA data partition, factory app partition, and test app partition.
 *
 * @param[out] bs Bootloader state structure used to save read data.
 * @return        Return true if the partition table was succesfully loaded and MD5 checksum is valid.
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
 * Checks the reboot reason if it is the deep sleep and has a valid partition in the RTC memory
 * then try to load the application which was worked before we go to the deep sleep.
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

/**
 * @brief Converts an array to a printable string.
 *
 * This function is useful for printing SHA-256 digest.
 * \code{c}
 * // Example of using. image_hash will be printed
 * #define HASH_LEN 32 // SHA-256 digest length
 * ...
 * char hash_print[HASH_LEN * 2 + 1];
 * hash_print[HASH_LEN * 2] = 0;
 * bootloader_sha256_hex_to_str(hash_print, image_hash, HASH_LEN);
 * ESP_LOGI(TAG, %s", hash_print);
 * \endcode

 * @param[out] out_str       Output string
 * @param[in]  in_array_hex  Pointer to input array
 * @param[in]  len           Length of input array
 *
 * @return   ESP_OK: Successful
 *           ESP_ERR_INVALID_ARG: Error in the passed arguments
 */
esp_err_t bootloader_sha256_hex_to_str(char *out_str, const uint8_t *in_array_hex, size_t len);

/**
 * @brief Debug log contents of a buffer as hexadecimal.
 *
 * @note - Only works if component log level is DEBUG or higher.
 *       - It will print at most 128 bytes from @c buffer.
 *
 * @param buffer Buffer to log
 * @param length Length of buffer in bytes. Maximum length 128 bytes.
 * @param label Label to print at beginning of log line.
 */
void bootloader_debug_buffer(const void *buffer, size_t length, const char *label);

/** @brief Generates the digest of the data between offset & offset+length.
 *
 * This function should be used when the size of the data is larger than 3.2MB.
 * The MMU capacity is 3.2MB (50 pages - 64KB each). This function generates the SHA-256
 * of the data in chunks of 3.2MB, considering the MMU capacity.
 *
 * @param[in]  flash_offset  Offset of the data in flash.
 * @param[in]  len           Length of data in bytes.
 * @param[out] digest        Pointer to buffer where the digest is written, if ESP_OK is returned.
 *
 * @return ESP_OK if secure boot digest is generated successfully.
 */
esp_err_t bootloader_sha256_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest);
