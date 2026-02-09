/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OTA_OPS_H
#define _OTA_OPS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_app_desc.h"
#include "esp_bootloader_desc.h"
#include "esp_flash_partitions.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OTA_SIZE_UNKNOWN 0xffffffff /*!< Used for esp_ota_begin() if new image size is unknown */
#define OTA_WITH_SEQUENTIAL_WRITES 0xfffffffe /*!< Used for esp_ota_begin() if new image size is unknown and erase can be done in incremental manner (assuming write operation is in continuous sequence) */

#define ESP_ERR_OTA_BASE                         0x1500                     /*!< Base error code for ota_ops api */
#define ESP_ERR_OTA_PARTITION_CONFLICT           (ESP_ERR_OTA_BASE + 0x01)  /*!< Error if request was to write or erase the current running partition */
#define ESP_ERR_OTA_SELECT_INFO_INVALID          (ESP_ERR_OTA_BASE + 0x02)  /*!< Error if OTA data partition contains invalid content */
#define ESP_ERR_OTA_VALIDATE_FAILED              (ESP_ERR_OTA_BASE + 0x03)  /*!< Error if OTA app image is invalid */
#define ESP_ERR_OTA_SMALL_SEC_VER                (ESP_ERR_OTA_BASE + 0x04)  /*!< Error if the firmware has a secure version less than the running firmware. */
#define ESP_ERR_OTA_ROLLBACK_FAILED              (ESP_ERR_OTA_BASE + 0x05)  /*!< Error if flash does not have valid firmware in passive partition and hence rollback is not possible */
#define ESP_ERR_OTA_ROLLBACK_INVALID_STATE       (ESP_ERR_OTA_BASE + 0x06)  /*!< Error if current active firmware is still marked in pending validation state (ESP_OTA_IMG_PENDING_VERIFY), essentially first boot of firmware image post upgrade and hence firmware upgrade is not possible */


/**
 * @brief Opaque handle for an application OTA update
 *
 * esp_ota_begin() returns a handle which is then used for subsequent
 * calls to esp_ota_write() and esp_ota_end().
 */
typedef uint32_t esp_ota_handle_t;


/**
 * @brief   Commence an OTA update writing to the specified partition.

 * The specified partition is erased to the specified image size.
 *
 * If image size is not yet known, pass OTA_SIZE_UNKNOWN which will
 * cause the entire partition to be erased.
 *
 * On success, this function allocates memory that remains in use
 * until esp_ota_end() is called with the returned handle.
 *
 * Note: If the rollback option is enabled and the running application has the ESP_OTA_IMG_PENDING_VERIFY state then
 * it will lead to the ESP_ERR_OTA_ROLLBACK_INVALID_STATE error. Confirm the running app before to run download a new app,
 * use esp_ota_mark_app_valid_cancel_rollback() function for it (this should be done as early as possible when you first download a new application).
 *
 * Note: Rollback is applicable only for app type partitions.
 * Note: For Rollback - The OTA data slot corresponding to the last boot application partition will be invalidated.
 *
 * @param partition  Pointer to info for partition which will receive the OTA update. Required.
 *                   This is considered as the staging partition (where OTA is downloaded), be default this also considered as the final partition which supposed to be updated.
 *                   The final partition can be overwritten using esp_ota_set_final_partition() after calling esp_ota_begin() to relocate contents to the final destination partition.
 * @param image_size Size of new OTA app image. Partition will be erased in order to receive this size of image. If 0 or OTA_SIZE_UNKNOWN, the entire partition is erased.
 * @param out_handle On success, returns a handle which should be used for subsequent esp_ota_write() and esp_ota_end() calls.

 * @return
 *    - ESP_OK: OTA operation commenced successfully.
 *    - ESP_ERR_INVALID_ARG: partition or out_handle arguments were NULL, or partition doesn't point to an OTA app partition.
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_OTA_PARTITION_CONFLICT: Partition holds the currently running firmware, cannot update in place.
 *    - ESP_ERR_NOT_FOUND: Partition argument not found in partition table.
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: The OTA data partition contains invalid data.
 *    - ESP_ERR_INVALID_SIZE: Partition doesn't fit in configured flash size.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - ESP_ERR_OTA_ROLLBACK_INVALID_STATE: If the running app has not confirmed state. Before performing an update, the application must be valid.
 */
esp_err_t esp_ota_begin(const esp_partition_t* partition, size_t image_size, esp_ota_handle_t* out_handle);

/**
 * @brief   Resume an interrupted OTA update by continuing to write to the specified partition.
 *
 * This function is used when an OTA update was previously started and needs to be resumed after an interruption.
 * It continues the OTA process from the specified offset within the partition.
 *
 * Unlike esp_ota_begin(), this function does not erase the partition which receives the OTA update, but rather expects that part of the image
 * has already been written correctly, and it resumes writing from the given offset.
 *
 * @param partition Pointer to info for the partition which is receiving the OTA update. Required.
 * @param erase_size Specifies how much flash memory to erase before resuming OTA, depending on whether a sequential write or a bulk erase is being used.
 * @param image_offset Offset from where to resume the OTA process. Should be set to the number of bytes already written.
 * @param out_handle On success, returns a handle that should be used for subsequent esp_ota_write() and esp_ota_end() calls.
 *
 * @return
 *    - ESP_OK: OTA operation resumed successfully.
 *    - ESP_ERR_INVALID_ARG: partition, out_handle were NULL or image_offset arguments is negative, or partition doesn't point to an OTA app partition.
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_OTA_PARTITION_CONFLICT: Partition holds the currently running firmware, cannot update in place.
 *    - ESP_ERR_NOT_FOUND: Partition argument not found in partition table.
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: The OTA data partition contains invalid data.
 *    - ESP_ERR_INVALID_SIZE: Partition doesn't fit in configured flash size.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 */
esp_err_t esp_ota_resume(const esp_partition_t *partition, const size_t erase_size, const size_t image_offset, esp_ota_handle_t *out_handle);

/**
 * @brief Set the final destination partition for OTA update
 *
 * This function configures the specified final partition as the destination for the OTA update.
 * It also allows setting a flag to indicate if the image should be copied from the staging
 * partition to the final partition after the OTA update completes. Otherwise, copying will need
 * to be handled by custom code using esp_partition_copy().
 *
 * @note This can be called after esp_ota_begin() and before the OTA update has started (before esp_ota_write()).
 *
 * @param handle OTA update handle obtained from esp_ota_begin().
 * @param final Pointer to the final destination partition where the new image will be verified and potentially finalized.
 *              This partition must not be NULL.
 * @param finalize_with_copy Boolean flag indicating if the downloaded image should be copied
 *                           from the staging partition to the final partition upon completion.
 *                           Set to False if you intend to perform the final copy process manually later.
 *
 * @return
 *      - ESP_OK: final destination partition set successfully.
 *      - ESP_ERR_INVALID_STATE: Once the OTA update has started, changing the final destination partition is prohibited.
 *      - ESP_ERR_INVALID_ARG: Invalid arguments were passed (e.g., final partition is NULL).
 *      - ESP_ERR_NOT_FOUND: OTA handle not found or final partition verification failed.
 */
esp_err_t esp_ota_set_final_partition(esp_ota_handle_t handle, const esp_partition_t *final, bool finalize_with_copy);

/**
 * @brief   Write OTA update data to partition
 *
 * This function can be called multiple times as
 * data is received during the OTA operation. Data is written
 * sequentially to the partition.
 *
 * @param handle  Handle obtained from esp_ota_begin
 * @param data    Data buffer to write
 * @param size    Size of data buffer in bytes.
 *
 * @return
 *    - ESP_OK: Data was written to flash successfully, or size = 0
 *    - ESP_ERR_INVALID_ARG: handle is invalid.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: First byte of image contains invalid image magic byte.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - ESP_ERR_INVALID_SIZE: if write would go out of bounds of the partition
 *    - or one of error codes from lower-level flash driver.
 */
esp_err_t esp_ota_write(esp_ota_handle_t handle, const void* data, size_t size);

/**
 * @brief   Write OTA update data to partition at an offset
 *
 * This function can write data in non-contiguous manner.
 * If flash encryption is enabled, data should be 16 bytes aligned.
 *
 * @param handle  Handle obtained from esp_ota_begin
 * @param data    Data buffer to write
 * @param size    Size of data buffer in bytes
 * @param offset  Offset in flash partition
 *
 * @note While performing OTA, if the packets arrive out of order, esp_ota_write_with_offset() can be used to write data in non-contiguous manner.
 *       Use of esp_ota_write_with_offset() in combination with esp_ota_write() is not recommended.
 *
 * @return
 *    - ESP_OK: Data was written to flash successfully.
 *    - ESP_ERR_INVALID_ARG: handle is invalid.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 */
esp_err_t esp_ota_write_with_offset(esp_ota_handle_t handle, const void *data, size_t size, uint32_t offset);

/**
 * @brief Finish OTA update and validate newly written app image.
 *
 * @param handle  Handle obtained from esp_ota_begin().
 *
 * @note After calling esp_ota_end(), the handle is no longer valid and any memory associated with it is freed (regardless of result).
 * @note If either the final or staging partitions were for the bootloader, then at the end of this function,
 *       the bootloader is reset to its default offset: esp_image_bootloader_offset_set(ESP_PRIMARY_BOOTLOADER_OFFSET)
 *
 * If the finalize_with_copy option is set, the staging partition will be copied to the final partition at the end of this function.
 * Otherwise, copying will need to be handled by custom code using esp_partition_copy().
 *
 * @return
 *    - ESP_OK: Newly written OTA app image is valid.
 *    - ESP_ERR_NOT_FOUND: OTA handle was not found.
 *    - ESP_ERR_INVALID_ARG: Handle was never written to.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: OTA image is invalid (either not a valid app image, or - if secure boot is enabled - signature failed to verify.)
 *    - ESP_ERR_INVALID_STATE: If flash encryption is enabled, this result indicates an internal error writing the final encrypted bytes to flash.
 */
esp_err_t esp_ota_end(esp_ota_handle_t handle);

/**
 * @brief Abort OTA update, free the handle and memory associated with it.
 *
 * @param handle obtained from esp_ota_begin().
 *
 * @return
 *    - ESP_OK: Handle and its associated memory is freed successfully.
 *    - ESP_ERR_NOT_FOUND: OTA handle was not found.
 */
esp_err_t esp_ota_abort(esp_ota_handle_t handle);


/**
 * @brief Configure OTA data for a new boot partition
 *
 * Equivalent to esp_image_verify() followed by esp_ota_set_boot_partition_skip_validate().
 *
 * @note If this function returns ESP_OK, calling esp_restart() will boot the newly configured app partition.
 *
 * @param partition Pointer to info for partition containing app image to boot.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_ERR_INVALID_ARG: partition argument was NULL or didn't point to a valid OTA partition of type "app".
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Partition contained invalid app image. Also returned if secure boot is enabled and signature validation failed.
 *    - ESP_ERR_NOT_FOUND: OTA data partition not found.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash erase or write failed.
 */
esp_err_t esp_ota_set_boot_partition(const esp_partition_t* partition);

/**
 * @brief Configure OTA data for a new boot partition without validating the image
 *
 * @note If this function returns ESP_OK, calling esp_restart() will boot the newly configured app partition.
 *
 * @param partition Pointer to info for partition containing app image to boot.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_ERR_INVALID_ARG: partition argument was NULL or didn't point to a valid OTA partition of type "app".
 *    - ESP_ERR_NOT_FOUND: OTA data partition not found.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash erase or write failed.
 */
esp_err_t esp_ota_set_boot_partition_skip_validate(const esp_partition_t* partition);

/**
 * @brief Get partition info of currently configured boot app
 *
 * If esp_ota_set_boot_partition() has been called, the partition which was set by that function will be returned.
 *
 * If esp_ota_set_boot_partition() has not been called, the result is usually the same as esp_ota_get_running_partition().
 * The two results are not equal if the configured boot partition does not contain a valid app (meaning that the running partition
 * will be an app that the bootloader chose via fallback).
 *
 * If the OTA data partition is not present or not valid then the result is the first app partition found in the
 * partition table. In priority order, this means: the factory app, the first OTA app slot, or the test app partition.
 *
 * Note that there is no guarantee the returned partition is a valid app. Use esp_image_verify(ESP_IMAGE_VERIFY, ...) to verify if the
 * returned partition contains a bootable image.
 *
 * @return Pointer to info for partition structure, or NULL if partition table is invalid or a flash read operation failed. Any returned pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_ota_get_boot_partition(void);


/**
 * @brief Get partition info of currently running app
 *
 * This function is different to esp_ota_get_boot_partition() in that
 * it ignores any change of selected boot partition caused by
 * esp_ota_set_boot_partition(). Only the app whose code is currently
 * running will have its partition information returned.
 *
 * The partition returned by this function may also differ from esp_ota_get_boot_partition() if the configured boot
 * partition is somehow invalid, and the bootloader fell back to a different app partition at boot.
 *
 * @return Pointer to info for partition structure, or NULL if no partition is found or flash read operation failed. Returned pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_ota_get_running_partition(void);


/**
 * @brief Return the next OTA app partition which should be written with a new firmware.
 *
 * Call this function to find an OTA app partition which can be passed to esp_ota_begin().
 *
 * Finds next partition round-robin, starting from the current running partition.
 *
 * @param start_from If set, treat this partition info as describing the current running partition. Can be NULL, in which case esp_ota_get_running_partition() is used to find the currently running partition. The result of this function is never the same as this argument.
 *
 * @return Pointer to info for partition which should be updated next. NULL result indicates invalid OTA data partition, or that no eligible OTA app slot partition was found.
 *
 */
const esp_partition_t* esp_ota_get_next_update_partition(const esp_partition_t *start_from);

/**
 * @brief Returns esp_app_desc structure for app partition. This structure includes app version.
 *
 * Returns a description for the requested app partition.
 * @param[in] partition     Pointer to app partition. (only app partition)
 * @param[out] app_desc     Structure of info about app.
 * @return
 *  - ESP_OK                Successful.
 *  - ESP_ERR_NOT_FOUND     app_desc structure is not found. Magic word is incorrect.
 *  - ESP_ERR_NOT_SUPPORTED Partition is not application.
 *  - ESP_ERR_INVALID_ARG   Arguments is NULL or if partition's offset exceeds partition size.
 *  - ESP_ERR_INVALID_SIZE  Read would go out of bounds of the partition.
 *  - or one of error codes from lower-level flash driver.
 */
esp_err_t esp_ota_get_partition_description(const esp_partition_t *partition, esp_app_desc_t *app_desc);

/**
 * @brief Returns the description structure of the bootloader.
 *
 * @param[in] bootloader_partition Pointer to bootloader partition.
 *                                 If NULL, then the PRIMARY bootloader is used (the default location).
 *                                 offset = CONFIG_BOOTLOADER_OFFSET_IN_FLASH,
 *                                 size = CONFIG_PARTITION_TABLE_OFFSET - CONFIG_BOOTLOADER_OFFSET_IN_FLASH,
 * @param[out] desc     Structure of info about bootloader.
 * @return
 *  - ESP_OK                Successful.
 *  - ESP_ERR_NOT_FOUND     Description structure is not found in the bootloader image. Magic byte is incorrect.
 *  - ESP_ERR_INVALID_ARG   Arguments is NULL.
 *  - ESP_ERR_INVALID_SIZE  Read would go out of bounds of the partition.
 *  - or one of error codes from lower-level flash driver.
 */
esp_err_t esp_ota_get_bootloader_description(const esp_partition_t *bootloader_partition, esp_bootloader_desc_t *desc);

/**
 * @brief Invalidate the OTA data slot associated with the last boot application partition.
 *
 * This function erases the OTA data slot corresponding to the last boot application partition,
 * making the partition invalid for booting in future. The application partition itself
 * is not erased, preserving its contents.
 *
 * @return
 *      - ESP_OK: Successfully invalidated the OTA data slot.
 *      - ESP_FAIL: Failed to invalidate the OTA data slot (e.g., invalid parameters, no OTA data partition, or other errors).
 *      - Other error codes from `esp_partition_erase_range`.
 */
esp_err_t esp_ota_invalidate_inactive_ota_data_slot(void);

/**
 * @brief Returns number of ota partitions provided in partition table.
 *
 * @return
 *  - Number of OTA partitions
 */
uint8_t esp_ota_get_app_partition_count(void);

/**
 * @brief This function is called to indicate that the running app is working well.
 *
 * @return
 *  - ESP_OK: if successful.
 */
esp_err_t esp_ota_mark_app_valid_cancel_rollback(void);

/**
 * @brief This function is called to roll back to the previously workable app without reboot.
 *
 * Checks applications on a flash drive that can be booted in case of rollback.
 * If the flash does not have at least one app (except the running app) then rollback is not possible.
 * @return
 *  - ESP_OK: if successful.
 *  - ESP_FAIL: if not successful.
 *  - ESP_ERR_OTA_ROLLBACK_FAILED: The rollback is not possible because the available OTA partitions
 *    on the flash do not contain a valid application.
 */
esp_err_t esp_ota_mark_app_invalid_rollback(void);

/**
 * @brief This function is called to roll back to the previously workable app with reboot.
 *
 * Equivalent to calling esp_ota_mark_app_invalid_rollback(), and, if successful, followed by esp_restart().
 *
 * @return
 *  - ESP_FAIL: if not successful.
 *  - ESP_ERR_OTA_ROLLBACK_FAILED: The rollback is not possible due to flash does not have any apps.
 */
esp_err_t esp_ota_mark_app_invalid_rollback_and_reboot(void);

/**
 * @brief Returns last partition with invalid state (ESP_OTA_IMG_INVALID or ESP_OTA_IMG_ABORTED).
 *
 * @return Pointer to the last invalid partition, or NULL if no invalid partition is recorded.
 */
const esp_partition_t* esp_ota_get_last_invalid_partition(void);

/**
 * @brief Returns state for given partition.
 *
 * @param[in] partition  Pointer to partition.
 * @param[out] ota_state state of partition (if this partition has a record in otadata).
 * @return
 *        - ESP_OK:                 Successful.
 *        - ESP_ERR_INVALID_ARG:    partition or ota_state arguments were NULL.
 *        - ESP_ERR_NOT_SUPPORTED:  partition is not ota.
 *        - ESP_ERR_NOT_FOUND:      Partition table does not have otadata or state was not found for given partition.
 */
esp_err_t esp_ota_get_state_partition(const esp_partition_t *partition, esp_ota_img_states_t *ota_state);

/**
 * @brief Erase previous boot app partition and corresponding otadata select for this partition.
 *
 * When current app is marked to as valid then you can erase previous app partition.
 * @return
 *        - ESP_OK:   Successful, otherwise ESP_ERR.
 */
esp_err_t esp_ota_erase_last_boot_app_partition(void);

/**
 * @brief Checks applications on the slots which can be booted in case of rollback.
 *
 * These applications should be valid (marked in otadata as not UNDEFINED, INVALID or ABORTED and crc is good) and be able booted,
 * and secure_version of app >= secure_version of efuse (if anti-rollback is enabled).
 *
 * @return
 *        - True: Returns true if the slots have at least one app (except the running app).
 *        - False: The rollback is not possible.
 */
bool esp_ota_check_rollback_is_possible(void);

#if SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS > 1 && (CONFIG_SECURE_BOOT_V2_ENABLED || __DOXYGEN__)

/**
 * Secure Boot V2 public key indexes.
 */
typedef enum {
    SECURE_BOOT_PUBLIC_KEY_INDEX_0,     /*!< Points to the 0th index of the Secure Boot v2 public key */
    SECURE_BOOT_PUBLIC_KEY_INDEX_1,     /*!< Points to the 1st index of the Secure Boot v2 public key */
    SECURE_BOOT_PUBLIC_KEY_INDEX_2      /*!< Points to the 2nd index of the Secure Boot v2 public key */
} esp_ota_secure_boot_public_key_index_t;

/**
 * @brief Revokes the signature digest denoted by the given index. This should be called in the application only after the rollback logic otherwise the device may end up in unrecoverable state.
 *
 * Relevant for Secure boot v2 on ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2 where up to 3 key digests can be stored (Key \#N-1, Key \#N, Key \#N+1).
 * When a key used to sign an app is invalidated, an OTA update is to be sent with an app signed with at least one of the other two keys which has not been revoked already.
 * After successfully booting the OTA app should call this function to revoke Key \#N-1.
 *
 * @param index - The index of the signature block to be revoked
 *
 * @return
 *        - ESP_OK: If revocation is successful.
 *        - ESP_ERR_INVALID_ARG: If the index of the public key to be revoked is incorrect.
 *        - ESP_FAIL: If secure boot v2 has not been enabled.
 */
esp_err_t esp_ota_revoke_secure_boot_public_key(esp_ota_secure_boot_public_key_index_t index);
#endif /* SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS > 1 */

#ifdef __cplusplus
}
#endif

#endif /* OTA_OPS_H */
