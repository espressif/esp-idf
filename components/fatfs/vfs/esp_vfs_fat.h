/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stddef.h>
#include "esp_err.h"
#include "sd_protocol_types.h"
#include "driver/sdspi_host.h"
#include "ff.h"
#include "wear_levelling.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for esp_vfs_fat_register
 */
typedef struct {
    const char* base_path; /*!< Path prefix where FATFS should be registered, */
    const char* fat_drive; /*!< FATFS drive specification; if only one drive is used, can be an empty string. */
    size_t max_files;      /*!< Maximum number of files which can be open at the same time. */
} esp_vfs_fat_conf_t;

/**
 * @brief Register FATFS with VFS component
 *
 * This function registers given FAT drive in VFS, at the specified base path.
 * Input arguments are held in esp_vfs_fat_conf_t structure.
 * If only one drive is used, fat_drive argument can be an empty string.
 * Refer to FATFS library documentation on how to specify FAT drive.
 * This function also allocates FATFS structure which should be used for f_mount
 * call.
 *
 * @note This function doesn't mount the drive into FATFS, it just connects
 *       POSIX and C standard library IO function with FATFS. You need to mount
 *       desired drive into FATFS separately.
 *
 * @param conf  pointer to esp_vfs_fat_conf_t configuration structure
 * @param[out] out_fs  pointer to FATFS structure which can be used for FATFS f_mount call is returned via this argument.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_register was already called
 *      - ESP_ERR_NO_MEM if not enough memory or too many VFSes already registered
 */
esp_err_t esp_vfs_fat_register_cfg(const esp_vfs_fat_conf_t* conf, FATFS** out_fs);

/**
 * @brief Un-register FATFS from VFS
 *
 * @note FATFS structure returned by esp_vfs_fat_register is destroyed after
 *       this call. Make sure to call f_mount function to unmount it before
 *       calling esp_vfs_fat_unregister_ctx.
 *       Difference between this function and the one above is that this one
 *       will release the correct drive, while the one above will release
 *       the last registered one
 *
 * @param base_path     path prefix where FATFS is registered. This is the same
 *                      used when esp_vfs_fat_register was called
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if FATFS is not registered in VFS
 */
esp_err_t esp_vfs_fat_unregister_path(const char* base_path);


/**
 * @brief Configuration arguments for esp_vfs_fat_sdmmc_mount and esp_vfs_fat_spiflash_mount_rw_wl functions
 */
typedef struct {
    /**
     * If FAT partition can not be mounted, and this parameter is true,
     * create partition table and format the filesystem.
     */
    bool format_if_mount_failed;
    int max_files;                  ///< Max number of open files
    /**
     * If format_if_mount_failed is set, and mount fails, format the card
     * with given allocation unit size. Must be a power of 2, between sector
     * size and 128 * sector size.
     * For SD cards, sector size is always 512 bytes. For wear_levelling,
     * sector size is determined by CONFIG_WL_SECTOR_SIZE option.
     *
     * Using larger allocation unit size will result in higher read/write
     * performance and higher overhead when storing small files.
     *
     * Setting this field to 0 will result in allocation unit set to the
     * sector size.
     */
    size_t allocation_unit_size;
    /**
     * Enables real ff_disk_status function implementation for SD cards
     * (ff_sdmmc_status). Possibly slows down IO performance.
     *
     * Try to enable if you need to handle situations when SD cards
     * are not unmounted properly before physical removal
     * or you are experiencing issues with SD cards.
     *
     * Doesn't do anything for other memory storage media.
     */
    bool disk_status_check_enable;
    /**
     * Use 1 FAT (File Allocation Tables) instead of 2.
     * This decreases reliability, but makes more space available
     * (usually only one sector).
     * Note that this option has effect only when the filesystem is formatted.
     * When mounting an already-formatted partition, the actual number of FATs
     * may be different.
     */
    bool use_one_fat;
} esp_vfs_fat_mount_config_t;

#define VFS_FAT_MOUNT_DEFAULT_CONFIG() \
    { \
        .format_if_mount_failed = false, \
        .max_files = 5, \
        .allocation_unit_size = 0, \
        .disk_status_check_enable = false, \
        .use_one_fat = false, \
    }

// Compatibility definition
typedef esp_vfs_fat_mount_config_t esp_vfs_fat_sdmmc_mount_config_t;

/**
 * @brief Convenience function to get FAT filesystem on SD card registered in VFS
 *
 * This is an all-in-one function which does the following:
 * - initializes SDMMC driver or SPI driver with configuration in host_config
 * - initializes SD card with configuration in slot_config
 * - mounts FAT partition on SD card using FATFS library, with configuration in mount_config
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 * For real world applications, developers should implement the logic of
 * probing SD card, locating and mounting partition, and registering FATFS in VFS,
 * with proper error checking and handling of exceptional conditions.
 *
 * @note Use this API to mount a card through SDSPI is deprecated. Please call
 *       `esp_vfs_fat_sdspi_mount()` instead for that case.
 *
 * @param base_path     path where partition should be registered (e.g. "/sdcard")
 * @param host_config   Pointer to structure describing SDMMC host. When using
 *                      SDMMC peripheral, this structure can be initialized using
 *                      SDMMC_HOST_DEFAULT() macro. When using SPI peripheral,
 *                      this structure can be initialized using SDSPI_HOST_DEFAULT()
 *                      macro.
 * @param slot_config   Pointer to structure with slot configuration.
 *                      For SDMMC peripheral, pass a pointer to sdmmc_slot_config_t
 *                      structure initialized using SDMMC_SLOT_CONFIG_DEFAULT.
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 * @param[out] out_card  if not NULL, pointer to the card information structure will be returned via this argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
    const sdmmc_host_t* host_config,
    const void* slot_config,
    const esp_vfs_fat_mount_config_t* mount_config,
    sdmmc_card_t** out_card);

/**
 * @brief Convenience function to get FAT filesystem on SD card registered in VFS
 *
 * This is an all-in-one function which does the following:
 * - initializes an SPI Master device based on the SPI Master driver with configuration in
 *   slot_config, and attach it to an initialized SPI bus.
 * - initializes SD card with configuration in host_config_input
 * - mounts FAT partition on SD card using FATFS library, with configuration in mount_config
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 * For real world applications, developers should implement the logic of
 * probing SD card, locating and mounting partition, and registering FATFS in VFS,
 * with proper error checking and handling of exceptional conditions.
 *
 * @note This function try to attach the new SD SPI device to the bus specified in host_config.
 *       Make sure the SPI bus specified in `host_config->slot` have been initialized by
 *       `spi_bus_initialize()` before.
 *
 * @param base_path     path where partition should be registered (e.g. "/sdcard")
 * @param host_config_input Pointer to structure describing SDMMC host. This structure can be
 *                          initialized using SDSPI_HOST_DEFAULT() macro.
 * @param slot_config   Pointer to structure with slot configuration.
 *                      For SPI peripheral, pass a pointer to sdspi_device_config_t
 *                      structure initialized using SDSPI_DEVICE_CONFIG_DEFAULT().
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 * @param[out] out_card If not NULL, pointer to the card information structure will be returned via
 *                      this argument. It is suggested to hold this handle and use it to unmount the card later if
 *                      needed. Otherwise it's not suggested to use more than one card at the same time and unmount one
 *                      of them in your application.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdspi_mount(const char* base_path,
                                  const sdmmc_host_t* host_config_input,
                                  const sdspi_device_config_t* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_sdmmc_mount
 *
 * @deprecated Use `esp_vfs_fat_sdcard_unmount()` instead.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount hasn't been called
 */
esp_err_t esp_vfs_fat_sdmmc_unmount(void) __attribute__((deprecated("Please use esp_vfs_fat_sdcard_unmount instead")));

/**
 * @brief Unmount an SD card from the FAT filesystem and release resources acquired using
 *        `esp_vfs_fat_sdmmc_mount()` or `esp_vfs_fat_sdspi_mount()`
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the card argument is unregistered
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount hasn't been called
 */
esp_err_t esp_vfs_fat_sdcard_unmount(const char* base_path, sdmmc_card_t *card);

/**
 * @brief Format FAT filesystem with given configuration
 *
 * @note
 * This API should be only called when the FAT is already mounted.
 *
 * @param base_path  Path where partition should be registered (e.g. "/sdcard")
 * @param card       Pointer to the card handle, which should be initialised by calling `esp_vfs_fat_sdspi_mount` first
 * @param cfg        Pointer to structure with extra parameters for formatting FATFS (only relevant fields are used).
 *                   If NULL, the previous configuration will be used.
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_STATE: FAT partition isn't mounted, call esp_vfs_fat_sdmmc_mount or esp_vfs_fat_sdspi_mount first
 *        - ESP_ERR_NO_MEM: if memory can not be allocated
 *        - ESP_FAIL: fail to format it, or fail to mount back
 */
esp_err_t esp_vfs_fat_sdcard_format_cfg(const char *base_path, sdmmc_card_t *card, esp_vfs_fat_mount_config_t *cfg);

/**
 * @brief Format FAT filesystem
 *
 * @note
 * This API should be only called when the FAT is already mounted.
 *
 * @param base_path  Path where partition should be registered (e.g. "/sdcard")
 * @param card       Pointer to the card handle, which should be initialised by calling `esp_vfs_fat_sdspi_mount` first
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_STATE: FAT partition isn't mounted, call esp_vfs_fat_sdmmc_mount or esp_vfs_fat_sdspi_mount first
 *        - ESP_ERR_NO_MEM: if memory can not be allocated
 *        - ESP_FAIL: fail to format it, or fail to mount back
 */
esp_err_t esp_vfs_fat_sdcard_format(const char *base_path, sdmmc_card_t *card);

/**
 * @brief Convenience function to initialize FAT filesystem in SPI flash and register it in VFS
 *
 * This is an all-in-one function which does the following:
 *
 * - finds the partition with defined partition_label. Partition label should be
 *   configured in the partition table.
 * - initializes flash wear levelling library on top of the given partition
 * - mounts FAT partition using FATFS library on top of flash wear levelling
 *   library
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 *
 * @param base_path        path where FATFS partition should be mounted (e.g. "/spiflash")
 * @param partition_label  label of the partition which should be used
 * @param mount_config     pointer to structure with extra parameters for mounting FATFS
 * @param[out] wl_handle   wear levelling driver handle
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain FATFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount_rw_wl was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from wear levelling library, SPI flash driver, or FATFS drivers
 */
esp_err_t esp_vfs_fat_spiflash_mount_rw_wl(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config,
    wl_handle_t* wl_handle);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_spiflash_mount_rw_wl
 *
 * @param base_path  path where partition should be registered (e.g. "/spiflash")
 * @param wl_handle  wear levelling driver handle returned by esp_vfs_fat_spiflash_mount_rw_wl
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount_rw_wl hasn't been called
 */
esp_err_t esp_vfs_fat_spiflash_unmount_rw_wl(const char* base_path, wl_handle_t wl_handle);

/**
 * @brief Format FAT filesystem with given configuration
 *
 * @note
 * This API can be called when the FAT is mounted / not mounted.
 * If this API is called when the FAT isn't mounted (by calling esp_vfs_fat_spiflash_mount_rw_wl),
 * this API will first mount the FAT then format it, then restore back to the original state.
 *
 * @param base_path        Path where partition should be registered (e.g. "/spiflash")
 * @param partition_label  Label of the partition which should be used
 * @param cfg              Pointer to structure with extra parameters for formatting FATFS (only relevant fields are used).
 *                         If NULL and mounted the previous configuration will be used.
 *                         If NULL and unmounted the default configuration will be used.
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_NO_MEM: if memory can not be allocated
 *        - Other errors from esp_vfs_fat_spiflash_mount_rw_wl
 */
esp_err_t esp_vfs_fat_spiflash_format_cfg_rw_wl(const char* base_path, const char* partition_label, esp_vfs_fat_mount_config_t *cfg);

/**
 * @brief Format FAT filesystem
 *
 * @note
 * This API can be called when the FAT is mounted / not mounted.
 * If this API is called when the FAT isn't mounted (by calling esp_vfs_fat_spiflash_mount_rw_wl),
 * this API will first mount the FAT then format it, then restore back to the original state.
 *
 * @param base_path        Path where partition should be registered (e.g. "/spiflash")
 * @param partition_label  Label of the partition which should be used
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_NO_MEM: if memory can not be allocated
 *        - Other errors from esp_vfs_fat_spiflash_mount_rw_wl
 */
esp_err_t esp_vfs_fat_spiflash_format_rw_wl(const char* base_path, const char* partition_label);

/**
 * @brief Convenience function to initialize read-only FAT filesystem and register it in VFS
 *
 * This is an all-in-one function which does the following:
 *
 * - finds the partition with defined partition_label. Partition label should be
 *   configured in the partition table.
 * - mounts FAT partition using FATFS library
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * @note Wear levelling is not used when FAT is mounted in read-only mode using this function.
 *
 * @param base_path        path where FATFS partition should be mounted (e.g. "/spiflash")
 * @param partition_label  label of the partition which should be used
 * @param mount_config     pointer to structure with extra parameters for mounting FATFS
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain FATFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount_ro was already called for the same partition
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SPI flash driver, or FATFS drivers
 */
esp_err_t esp_vfs_fat_spiflash_mount_ro(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_spiflash_mount_ro
 *
 * @param base_path  path where partition should be registered (e.g. "/spiflash")
 * @param partition_label  label of partition to be unmounted
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount_ro hasn't been called
 */
esp_err_t esp_vfs_fat_spiflash_unmount_ro(const char* base_path, const char* partition_label);

/**
 * @brief Get information for FATFS partition
 *
 * @param base_path  Base path of the partition examined (e.g. "/spiflash")
 * @param[out] out_total_bytes  Size of the file system
 * @param[out] out_free_bytes   Free bytes available in the file system
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if partition not found
 *      - ESP_FAIL if another FRESULT error (saved in errno)
 */
esp_err_t esp_vfs_fat_info(const char* base_path, uint64_t* out_total_bytes, uint64_t* out_free_bytes);

/**
 * @brief Create a file with contiguous space at given path
 *
 * @note The file cannot exist before calling this function (or the file size has to be 0)
 *       For more information see documentation for `f_expand` from FATFS library
 *
 * @param base_path  Base path of the partition examined (e.g. "/spiflash")
 * @param full_path  Full path of the file (e.g. "/spiflash/ABC.TXT")
 * @param size       File size expanded to, number of bytes in size to prepare or allocate for the file
 * @param alloc_now  True == allocate space now, false == prepare to allocate -- see `f_expand` from FATFS
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if invalid arguments (e.g. any of arguments are NULL or size lower or equal to 0)
 *      - ESP_ERR_INVALID_STATE if partition not found
 *      - ESP_FAIL if another FRESULT error (saved in errno)
 */
esp_err_t esp_vfs_fat_create_contiguous_file(const char* base_path, const char* full_path, uint64_t size, bool alloc_now);

/**
 * @brief Test if a file is contiguous in the FAT filesystem
 *
 * @param base_path  Base path of the partition examined (e.g. "/spiflash")
 * @param full_path  Full path of the file (e.g. "/spiflash/ABC.TXT")
 * @param[out] is_contiguous  True == allocate space now, false == prepare to allocate -- see `f_expand` from FATFS
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if invalid arguments (e.g. any of arguments are NULL)
 *      - ESP_ERR_INVALID_STATE if partition not found
 *      - ESP_FAIL if another FRESULT error (saved in errno)
 */
esp_err_t esp_vfs_fat_test_contiguous_file(const char* base_path, const char* full_path, bool* is_contiguous);

/** @cond */
/**
 * @deprecated Please use `esp_vfs_fat_register_cfg` instead
 */
esp_err_t esp_vfs_fat_register(const char* base_path, const char* fat_drive,
        size_t max_files, FATFS** out_fs);

/**
 * @deprecated Please use `esp_vfs_fat_spiflash_mount_rw_wl` instead
 */
esp_err_t esp_vfs_fat_spiflash_mount(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config,
    wl_handle_t* wl_handle)
    __attribute__((deprecated("esp_vfs_fat_spiflash_mount is deprecated, please use esp_vfs_fat_spiflash_mount_rw_wl instead")));

/**
 * @deprecated Please use `esp_vfs_fat_spiflash_unmount_rw_wl` instead
 */
esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle)
    __attribute__((deprecated("esp_vfs_fat_spiflash_unmount is deprecated, please use esp_vfs_fat_spiflash_unmount_rw_wl instead")));

/**
 * @deprecated Please use `esp_vfs_fat_spiflash_mount_ro` instead
 */
esp_err_t esp_vfs_fat_rawflash_mount(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config)
    __attribute__((deprecated("esp_vfs_fat_rawflash_mount is deprecated, please use esp_vfs_fat_spiflash_mount_ro instead")));

/**
 * @deprecated Please use `esp_vfs_fat_spiflash_unmount_ro` instead
 */
esp_err_t esp_vfs_fat_rawflash_unmount(const char* base_path, const char* partition_label)
    __attribute__((deprecated("esp_vfs_fat_rawflash_unmount is deprecated, please use esp_vfs_fat_spiflash_unmount_ro instead")));
/** @endcond */

#ifdef __cplusplus
}
#endif
