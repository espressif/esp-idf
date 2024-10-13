/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_flash_partitions.h"
#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

// Type of hold a GPIO in low state
typedef enum {
    GPIO_LONG_HOLD  = 1,    /*!< The long hold GPIO */
    GPIO_SHORT_HOLD = -1,   /*!< The short hold GPIO */
    GPIO_NOT_HOLD   = 0     /*!< If the GPIO input is not low */
} esp_comm_gpio_hold_t;

typedef enum {
    ESP_IMAGE_BOOTLOADER,
    ESP_IMAGE_APPLICATION
} esp_image_type;

/**
 * @brief Read ota_info partition and fill array from two otadata structures.
 *
 * @param[in]   ota_info It is a pointer to the OTA data partition.
 *                       The "otadata" partition (Type = "data" and SubType = "ota")
 *                       is defined in the CSV partition table.
 * @param[out]  two_otadata Pointer to array of OTA selection structure.
 * @return      - ESP_OK: On success
 *              - ESP_ERR_NOT_FOUND: Partition table does not have otadata partition
 *              - ESP_FAIL: On failure
 */
esp_err_t bootloader_common_read_otadata(const esp_partition_pos_t *ota_info, esp_ota_select_entry_t *two_otadata);

/**
 * @brief Calculate crc for the OTA data select.
 *
 * @param[in] s The OTA data select.
 * @return    Returns crc value.
 */
uint32_t bootloader_common_ota_select_crc(const esp_ota_select_entry_t *s);

/**
 * @brief Verifies the validity of the OTA data select
 *
 * @param[in] s The OTA data select.
 * @return    Returns true on valid, false otherwise.
 */
bool bootloader_common_ota_select_valid(const esp_ota_select_entry_t *s);

/**
 * @brief Returns true if OTADATA is not marked as bootable partition.
 *
 * @param[in] s The OTA data select.
 * @return    Returns true if OTADATA invalid, false otherwise.
 */
bool bootloader_common_ota_select_invalid(const esp_ota_select_entry_t *s);

/**
 * @brief Check if a GPIO input is held low for a long period, short period, or not
 * at all.
 *
 * This function will configure the specified GPIO as an input with internal pull-up enabled.
 *
 * If the GPIO input is held low continuously for delay_sec period then it is a long hold.
 * If the GPIO input is held low for less period then it is a short hold.
 *
 * @param[in] num_pin Number of the GPIO input.
 * @param[in] delay_sec Input must be driven low for at least this long, continuously.
 * @return esp_comm_gpio_hold_t Type of low level hold detected, if any.
 */
esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio(uint32_t num_pin, uint32_t delay_sec);

/**
 * @brief Check if a GPIO input is held low or high for a long period, short period, or not
 * at all.
 *
 * This function will configure the specified GPIO as an input with internal pull-up enabled.
 *
 * If the GPIO input is held at 'level' continuously for delay_sec period then it is a long hold.
 * If the GPIO input is held at 'level' for less period then it is a short hold.
 *
 * @param[in] num_pin Number of the GPIO input.
 * @param[in] delay_sec Input must be driven to 'level' for at least this long, continuously.
 * @param[in] level Input pin level to trigger on hold
 * @return esp_comm_gpio_hold_t Type of hold detected, if any.
 */
esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio_level(uint32_t num_pin, uint32_t delay_sec, bool level);


/**
 * @brief Erase the partition data that is specified in the transferred list.
 *
 * @param[in] list_erase String containing a list of cleared partitions. Like this "nvs, phy". The string must be null-terminal.
 * @param[in] ota_data_erase If true then the OTA data partition will be cleared (if there is it in partition table).
 * @return    Returns true on success, false otherwise.
 */
bool bootloader_common_erase_part_type_data(const char *list_erase, bool ota_data_erase);

/**
 * @brief Determines if the list contains the label
 *
 * @param[in] list  A string of names delimited by commas or spaces. Like this "nvs, phy, data". The string must be null-terminated.
 * @param[in] label The substring that will be searched in the list.
 * @return    Returns true if the list contains the label, false otherwise.
 */
bool bootloader_common_label_search(const char *list, char *label);

/**
 * @brief Configure default SPI pin modes and drive strengths
 *
 * @param drv GPIO drive level (determined by clock frequency)
 */
void bootloader_configure_spi_pins(int drv);

/**
 * @brief Calculates a sha-256 for a given partition or returns a appended digest.
 *
 * This function can be used to return the SHA-256 digest of application, bootloader and data partitions.
 * For apps with SHA-256 appended to the app image, the result is the appended SHA-256 value for the app image content.
 * The hash is verified before returning, if app content is invalid then the function returns ESP_ERR_IMAGE_INVALID.
 * For apps without SHA-256 appended to the image, the result is the SHA-256 of all bytes in the app image.
 * For other partition types, the result is the SHA-256 of the entire partition.
 *
 * @param[in]  address      Address of partition.
 * @param[in]  size         Size of partition.
 * @param[in]  type         Type of partition. For applications the type is 0, otherwise type is data.
 * @param[out] out_sha_256  Returned SHA-256 digest for a given partition.
 *
 * @return
 *          - ESP_OK: In case of successful operation.
 *          - ESP_ERR_INVALID_ARG: The size was 0 or the sha_256 was NULL.
 *          - ESP_ERR_NO_MEM: Cannot allocate memory for sha256 operation.
 *          - ESP_ERR_IMAGE_INVALID: App partition doesn't contain a valid app image.
 *          - ESP_FAIL: An allocation error occurred.
 */
esp_err_t bootloader_common_get_sha256_of_partition(uint32_t address, uint32_t size, int type, uint8_t *out_sha_256);

/**
 * @brief Returns the number of active otadata.
 *
 * @param[in] two_otadata Pointer on array from two otadata structures.
 *
 * @return The number of active otadata (0 or 1).
 *        - -1: If it does not have active otadata.
 */
int bootloader_common_get_active_otadata(esp_ota_select_entry_t *two_otadata);

/**
 * @brief Returns the number of active otadata.
 *
 * @param[in] two_otadata       Pointer on array from two otadata structures.
 * @param[in] valid_two_otadata Pointer on array from two bools. True means select.
 * @param[in] max               True - will select the maximum ota_seq number, otherwise the minimum.
 *
 * @return The number of active otadata (0 or 1).
 *        - -1: If it does not have active otadata.
 */
int bootloader_common_select_otadata(const esp_ota_select_entry_t *two_otadata, bool *valid_two_otadata, bool max);

/**
 * @brief Get chip package
 *
 * @return Chip package number
 */
uint32_t bootloader_common_get_chip_ver_pkg(void);

/**
 * @brief Check if the image (bootloader and application) has valid chip ID and revision
 *
 * @param[in] img_hdr: image header
 * @param[in] type: image type, bootloader or application
 * @return
 *      - ESP_OK: image and chip are matched well
 *      - ESP_FAIL: image doesn't match to the chip
 */
esp_err_t bootloader_common_check_chip_validity(const esp_image_header_t* img_hdr, esp_image_type type);

#if !CONFIG_IDF_TARGET_ESP32
/**
 * @brief Check the eFuse block revision
 *
 * @param[in] min_rev_full The required minimum revision of the eFuse block
 * @param[in] max_rev_full The required maximum revision of the eFuse block
 * @return
 *          - ESP_OK: The eFuse block revision is in the required range.
 *          - ESP_OK: DISABLE_BLK_VERSION_MAJOR has been set in the eFuse of the SoC. No requirements shall be checked at this time.
 *          - ESP_FAIL: The eFuse block revision of this chip does not match the requirement of the current image.
 */
esp_err_t bootloader_common_check_efuse_blk_validity(uint32_t min_rev_full, uint32_t max_rev_full);
#endif  // !CONFIG_IDF_TARGET_ESP32

/**
 * @brief Configure VDDSDIO, call this API to rise VDDSDIO to 1.9V when VDDSDIO regulator is enabled as 1.8V mode.
 */
void bootloader_common_vddsdio_configure(void);

#if CONFIG_BOOTLOADER_RESERVE_RTC_MEM
/**
 * @brief Returns partition from rtc_retain_mem
 *
 * Uses to get the partition of application which was worked before to go to the deep sleep.
 * This partition was stored in rtc_retain_mem.
 * Note: This function operates the RTC FAST memory which available only for PRO_CPU.
 *       Make sure that this function is used only PRO_CPU.
 *
 * @return partition: If rtc_retain_mem is valid.
 *        - NULL: If it is not valid.
 */
esp_partition_pos_t* bootloader_common_get_rtc_retain_mem_partition(void);

/**
 * @brief Update the partition and reboot_counter in rtc_retain_mem.
 *
 * This function saves the partition of application for fast booting from the deep sleep.
 * An algorithm uses this partition to avoid reading the otadata and does not validate an image.
 * Note: This function operates the RTC FAST memory which available only for PRO_CPU.
 *       Make sure that this function is used only PRO_CPU.
 *
 * @param[in] partition      App partition description. Can be NULL, in this case rtc_retain_mem.partition is not updated.
 * @param[in] reboot_counter If true then update reboot_counter.
 *
 */
void bootloader_common_update_rtc_retain_mem(esp_partition_pos_t* partition, bool reboot_counter);

/**
 * @brief Reset entire rtc_retain_mem.
 *
 * Note: This function operates the RTC FAST memory which available only for PRO_CPU.
 *       Make sure that this function is used only PRO_CPU.
 */
void bootloader_common_reset_rtc_retain_mem(void);

/**
 * @brief Returns reboot_counter from rtc_retain_mem
 *
 * The reboot_counter counts the number of reboots. Reset only when power is off.
 * The very first launch of the application will be from 1.
 * Overflow is not possible, it will stop at the value UINT16_MAX.
 * Note: This function operates the RTC FAST memory which available only for PRO_CPU.
 *       Make sure that this function is used only PRO_CPU.
 *
 * @return reboot_counter: 1..65535
 *         - 0: If rtc_retain_mem is not valid.
 */
uint16_t bootloader_common_get_rtc_retain_mem_reboot_counter(void);

/**
 * @brief Returns True if Factory reset has happened
 *
 * Reset the status after reading it.
 *
 * @return True: Factory reset has happened
 *         False: No Factory reset
 */
bool bootloader_common_get_rtc_retain_mem_factory_reset_state(void);

/**
 * @brief Sets Factory reset status
 */
void bootloader_common_set_rtc_retain_mem_factory_reset_state(void);

/**
 * @brief Returns rtc_retain_mem
 *
 * Note: This function operates the RTC FAST memory which available only for PRO_CPU.
 *       Make sure that this function is used only PRO_CPU.
 *
 * @return rtc_retain_mem
 */
rtc_retain_mem_t* bootloader_common_get_rtc_retain_mem(void);

#endif // CONFIG_BOOTLOADER_RESERVE_RTC_MEM

#ifdef __cplusplus
}
#endif
