/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_err.h"
#ifndef CONFIG_IDF_TARGET_LINUX
#include "sd_protocol_types.h"
#include "driver/sdspi_host.h"
#endif
#include "ff.h"
#include "wear_levelling.h"
#include "esp_blockdev.h"
#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Members marked with this attribute are internal to the fatfs component. They are part of
 * the public type only so that the type is complete and can be allocated by the user
 * (statically or dynamically); accessing them outside of fatfs produces a deprecation warning.
 */
#if defined(ESP_VFS_FAT_IMPL)
#define ESP_VFS_FAT_PRIVATE
#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) || (defined(__cplusplus) && __cplusplus >= 201402L)
#define ESP_VFS_FAT_PRIVATE [[deprecated("internal to the fatfs component")]]
#else
#define ESP_VFS_FAT_PRIVATE __attribute__((deprecated("internal to the fatfs component")))
#endif

typedef enum {
    FORMATTED_DURING_LAST_MOUNT = 1 << 0, // The FATFS partition was formatted during the last mount
} vfs_fat_x_ctx_flags_t;

typedef PARTITION esp_vfs_fat_pdrv_part_t;

/**
 * @note When the value of item is less than or equal to 100, it specifies the partition size in percentage of the entire drive space.
 *       When it is larger than 100, it specifies number of sectors. The partition map table is terminated by a zero,
 *       4th partition in MBR format or no remaining space for next allocation. If the specified size is larger than remaining space on the drive,
 *       the partition is truncated at end of the drive.
 *
 *       For example:
 *
 *       `{100, 0, 0, 0}` will create a single partition with 100% of the drive space.
 *       `{50, 50, 0, 0}` will create two partitions, first with 50% of the drive space and second with the remaining 50%.
 *       `{0x10000000, 0x10000000, 0x10000000, 0}` will create three partitions, each with a size of 256 MiB, leaving remaining space non-allocated.
 */
typedef LBA_t esp_vfs_fat_drive_divide_arr_t[4];

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
 *      - ESP_ERR_INVALID_STATE if a filesystem is already registered at this base path.
 *        If @p out_fs is not NULL, @p *out_fs is set to the existing FATFS object so callers
 *        can run f_mount (e.g. remount the same path).
 *      - ESP_ERR_NO_MEM if not enough memory or too many VFSes already registered
 */
esp_err_t esp_vfs_fat_register(const esp_vfs_fat_conf_t* conf, FATFS** out_fs);

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
    /**
     * Mount the filesystem in read-only mode.
     * When set to true, all write operations (open for write, mkdir, unlink,
     * rename, etc.) will fail with errno set to EROFS.
     * This flag is independent of the underlying storage — it can be used
     * with wear-levelled, raw, BDL, or SD card partitions.
     */
    bool read_only;
} esp_vfs_fat_mount_config_t;

#define VFS_FAT_MOUNT_DEFAULT_CONFIG() \
    { \
        .format_if_mount_failed = false, \
        .max_files = 5, \
        .allocation_unit_size = 0, \
        .disk_status_check_enable = false, \
        .use_one_fat = false, \
        .read_only = false, \
    }

// Compatibility definition
typedef esp_vfs_fat_mount_config_t esp_vfs_fat_sdmmc_mount_config_t;

/**
 * @brief Open file slot of a FATFS instance
 *
 * An array of these is supplied to esp_vfs_fat_init(). Its length is the maximum number
 * of files which can be open on the instance at the same time.
 */
typedef struct esp_vfs_fat_file {
    /* Present so the type is complete and can be allocated. Not part of the API. */
#ifndef __DOXYGEN__
    struct esp_vfs_fat_file_priv {
        FIL fil;
        uint32_t flags;
        uint8_t *buf;
#ifdef CONFIG_FATFS_USE_FASTSEEK
        DWORD clmt[CONFIG_FATFS_FAST_SEEK_BUFFER_SIZE];
#endif
    } priv ESP_VFS_FAT_PRIVATE;
#endif
} esp_vfs_fat_file_t;

/**
 * @brief FATFS instance context
 *
 * Allocated by the user (statically or dynamically) and initialized with esp_vfs_fat_init(),
 * or allocated and initialized by esp_vfs_fat_create(). All members are internal.
 */
typedef struct esp_vfs_fat_ctx {
    /* Present so the type is complete and can be allocated. Not part of the API. */
#ifndef __DOXYGEN__
    struct esp_vfs_fat_ctx_priv {
        FATFS fs;
        esp_vfs_fat_file_t *files;
        size_t max_files;
#if FF_USE_DYN_BUFFER
        uint8_t *heap_win;              /* Volume window allocated by the wrapper, or NULL if the caller provided sector_buffers */
        size_t heap_sector_size;        /* Size of each window the wrapper allocates; file windows are allocated on open */
#endif
#if FF_USE_LFN == 3
        void *heap_lfn_buffer;          /* LFN working buffer allocated by the wrapper, or NULL if the caller provided it */
#endif
        _lock_t lock;
        esp_blockdev_handle_t bdl;
        esp_vfs_fat_mount_config_t mount_config;
        vfs_fat_x_ctx_flags_t state;
        int vfs_flags;
        uint8_t pdrv;
        bool legacy;
        char fat_drive[8];
        char base_path[ESP_VFS_PATH_MAX + 1];
        char tmp_path_buf[FILENAME_MAX + 3];
        char tmp_path_buf2[FILENAME_MAX + 3];
#ifdef CONFIG_VFS_SUPPORT_DIR
        struct {
            struct vfs_fat_dir_t *slh_first;
        } open_dirs;
#endif
    } priv ESP_VFS_FAT_PRIVATE;
#endif
} esp_vfs_fat_ctx_t;

/** Largest sector size a FATFS instance can use; an upper bound for esp_vfs_fat_get_sector_size() */
#define ESP_VFS_FAT_MAX_SECTOR_SIZE ((size_t)FF_MAX_SS)

/**
 * Size of esp_vfs_fat_resources_t::sector_buffers for `max_files` file slots: one sector for
 * the volume plus, unless CONFIG_FATFS_PER_FILE_CACHE is disabled, one sector per file slot.
 * Zero without CONFIG_FATFS_USE_DYN_BUFFERS, where these buffers are part of the context and
 * the file slots.
 */
#if FF_USE_DYN_BUFFER && FF_FS_TINY
#define ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sector_size) ((size_t)(sector_size))
#elif FF_USE_DYN_BUFFER
#define ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sector_size) \
    ((size_t)(1 + (max_files)) * (size_t)(sector_size))
#else
#define ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sector_size) ((size_t)0)
#endif

/**
 * Size of esp_vfs_fat_resources_t::lfn_buffer. Zero unless CONFIG_FATFS_LFN_HEAP is selected;
 * with CONFIG_FATFS_LFN_STACK the buffer is on the stack and with CONFIG_FATFS_LFN_NONE there is none.
 */
#if FF_USE_LFN == 3
#define ESP_VFS_FAT_LFN_BUFFER_SIZE ((size_t)FF_LFN_WORK_SIZE)
#else
#define ESP_VFS_FAT_LFN_BUFFER_SIZE ((size_t)0)
#endif

/**
 * @brief Memory a FATFS instance uses besides its context
 *
 * Pass every buffer this configuration uses, or leave them all NULL. With NULL buffers the VFS
 * wrapper allocates them with the capabilities selected by CONFIG_FATFS_HEAP_CAPS: the volume
 * window and the LFN buffer for the life of the instance, and each file's sector window only
 * while that file is open. Buffers the caller supplies stay the caller's and must remain valid
 * until esp_vfs_fat_deinit(). A buffer whose size macro evaluates to 0 is ignored.
 *
 * Sector buffers are passed to the block device for reads and writes, so they must meet its
 * requirements, e.g. be DMA-capable and suitably aligned for an SD card. The start of each
 * sector inside `sector_buffers` keeps the alignment of `sector_buffers` for alignments up to
 * the sector size.
 */
typedef struct esp_vfs_fat_resources {
    esp_vfs_fat_file_t *files;      /*!< Open file slots; their number is the open file limit */
    size_t max_files;               /*!< Number of elements in `files`, at least 1 */
    void *sector_buffers;           /*!< ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sector size) bytes, or NULL */
    size_t sector_buffers_size;     /*!< Size of `sector_buffers` in bytes */
    void *lfn_buffer;               /*!< ESP_VFS_FAT_LFN_BUFFER_SIZE bytes, 2-byte aligned, or NULL */
    size_t lfn_buffer_size;         /*!< Size of `lfn_buffer` in bytes */
} esp_vfs_fat_resources_t;

/**
 * @brief Get the sector size a FATFS instance on a block device uses
 *
 * Use it to size esp_vfs_fat_resources_t::sector_buffers exactly; ESP_VFS_FAT_MAX_SECTOR_SIZE
 * is an upper bound usable in constant expressions.
 *
 * @param bdl_handle  block device
 *
 * @return sector size in bytes, or 0 if the block device geometry is not usable by FATFS
 */
size_t esp_vfs_fat_get_sector_size(esp_blockdev_handle_t bdl_handle);

/**
 * @brief Initialize a FATFS instance in user-provided memory
 *
 * Binds the block device to a FATFS drive and mounts the filesystem on it, formatting it
 * first if mounting fails and `mount_config->format_if_mount_failed` is set. The instance is
 * not registered with VFS; use esp_vfs_fat_get_fs() and esp_vfs_register_fs_instance() for
 * that, or hand the descriptor to another filesystem implementation.
 *
 * The context holds the instance and FATFS volume locks. With all buffers in `resources`
 * provided, initialization, formatting and opening, reading, writing, seeking and closing
 * files do not allocate. Directory iteration, rename, truncate and the contiguous-file
 * functions still allocate temporary objects.
 *
 * @param ctx           context to initialize; must stay valid until esp_vfs_fat_deinit()
 * @param resources     file slots and buffers; see esp_vfs_fat_resources_t
 * @param bdl_handle    block device providing the storage; owned by the caller and must
 *                      outlive the instance
 * @param mount_config  mount options; `max_files` is ignored in favor of `resources->max_files`
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is invalid
 *      - ESP_ERR_INVALID_SIZE if a provided buffer is too small
 *      - ESP_ERR_INVALID_STATE if the block device is already used by another instance
 *      - ESP_ERR_NO_MEM if all FATFS drives are in use or a buffer can not be allocated
 *      - ESP_FAIL if the filesystem can not be mounted
 */
esp_err_t esp_vfs_fat_init(esp_vfs_fat_ctx_t *ctx, const esp_vfs_fat_resources_t *resources,
                           esp_blockdev_handle_t bdl_handle, const esp_vfs_fat_mount_config_t *mount_config);

/**
 * @brief Unmount the filesystem and release the FATFS drive of an instance
 *
 * The instance must not be registered with VFS or used by another filesystem anymore.
 * The memory of `ctx` and of the file slots is not freed.
 *
 * @param ctx  context initialized with esp_vfs_fat_init()
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx is NULL
 */
esp_err_t esp_vfs_fat_deinit(esp_vfs_fat_ctx_t *ctx);

/**
 * @brief Allocate and initialize a FATFS instance
 *
 * Allocates the context together with `mount_config->max_files` file slots and calls
 * esp_vfs_fat_init() on them.
 *
 * @param bdl_handle    block device providing the storage; owned by the caller and must
 *                      outlive the instance
 * @param mount_config  mount options
 * @param[out] out_ctx  created instance
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - other errors from esp_vfs_fat_init()
 */
esp_err_t esp_vfs_fat_create(esp_blockdev_handle_t bdl_handle, const esp_vfs_fat_mount_config_t *mount_config,
                             esp_vfs_fat_ctx_t **out_ctx);

/**
 * @brief Deinitialize and free an instance created with esp_vfs_fat_create()
 *
 * @param ctx  context returned by esp_vfs_fat_create()
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx is NULL
 */
esp_err_t esp_vfs_fat_destroy(esp_vfs_fat_ctx_t *ctx);

/**
 * @brief Get the filesystem instance descriptor of a FATFS instance
 *
 * `ESP_VFS_FLAG_READONLY_FS` is set in the returned flags if the block device is read-only
 * or the instance was initialized with `read_only` set.
 *
 * @param ctx  initialized context
 *
 * @return descriptor for esp_vfs_register_fs_instance() or for use by another filesystem
 */
esp_vfs_fs_t esp_vfs_fat_get_fs(esp_vfs_fat_ctx_t *ctx);

/**
 * @brief Create a FATFS instance and register it with VFS
 *
 * Equivalent to esp_vfs_fat_create() followed by registering the instance at `base_path`.
 * The path-based functions (esp_vfs_fat_info() etc.) work on instances mounted this way.
 *
 * @param base_path     path where the filesystem is registered (e.g. "/data")
 * @param bdl_handle    block device providing the storage; owned by the caller and must
 *                      outlive the mount
 * @param mount_config  mount options
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is invalid
 *      - ESP_ERR_INVALID_STATE if the block device is already used by another instance
 *      - ESP_ERR_NO_MEM if memory can not be allocated or no free drives
 *      - ESP_FAIL if the filesystem can not be mounted
 *      - other errors from esp_vfs_register_fs_instance()
 */
esp_err_t esp_vfs_fat_mount(const char *base_path, esp_blockdev_handle_t bdl_handle,
                            const esp_vfs_fat_mount_config_t *mount_config);

/**
 * @brief Unregister and destroy an instance mounted with esp_vfs_fat_mount()
 *
 * The block device is not released.
 *
 * @param base_path  path passed to esp_vfs_fat_mount()
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if no instance was mounted at this path with esp_vfs_fat_mount()
 */
esp_err_t esp_vfs_fat_unmount(const char *base_path);

/**
 * @brief Get total and free space of a FATFS instance
 *
 * Same as esp_vfs_fat_info(), but works for instances which are not registered with VFS.
 *
 * @param ctx                   initialized context
 * @param[out] out_total_bytes  size of the file system, may be NULL
 * @param[out] out_free_bytes   free bytes available in the file system, may be NULL
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if ctx is NULL
 *      - ESP_FAIL on a FATFS error (saved in errno)
 */
esp_err_t esp_vfs_fat_ctx_info(esp_vfs_fat_ctx_t *ctx, uint64_t *out_total_bytes, uint64_t *out_free_bytes);

/**
 * @brief Create a file with contiguous space in a FATFS instance
 *
 * Same as esp_vfs_fat_create_contiguous_file(), with `path` relative to the root of the
 * instance (e.g. "/ABC.TXT").
 */
esp_err_t esp_vfs_fat_ctx_create_contiguous_file(esp_vfs_fat_ctx_t *ctx, const char *path, uint64_t size, bool alloc_now);

/**
 * @brief Test if a file in a FATFS instance is contiguous
 *
 * Same as esp_vfs_fat_test_contiguous_file(), with `path` relative to the root of the
 * instance (e.g. "/ABC.TXT").
 */
esp_err_t esp_vfs_fat_ctx_test_contiguous_file(esp_vfs_fat_ctx_t *ctx, const char *path, bool *is_contiguous);

#ifndef CONFIG_IDF_TARGET_LINUX
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
 * @param[out] out_card  pointer to the card information structure will be returned via this argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is NULL
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
 * @param[out] out_card Pointer to the card information structure will be returned via
 *                      this argument. It is suggested to hold this handle and use it to unmount the card later if
 *                      needed. Otherwise it's not suggested to use more than one card at the same time and unmount one
 *                      of them in your application.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is NULL
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
#endif

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
 * @brief Convenience function to mount a FatFS volume on a BDL (Block Device Layer) device
 *
 * The FatFS logical sector size is derived from BDL geometry as
 * LCM(FF_MIN_SS, read_size, write_size [, erase_size]).  erase_size is
 * included when it fits within FF_MAX_SS, making the sector erase-aligned
 * for NOR-style devices and page-aligned for NAND-style devices (where
 * the FTL/WL layer handles erase internally).
 *
 * The caller is responsible for constructing the BDL stack (e.g. partition BDL ->
 * WL BDL) before calling this function.  Read-only devices are detected
 * automatically.
 *
 * @param base_path     path where FATFS partition should be mounted (e.g. "/spiflash")
 * @param bdl_handle    BDL device handle providing the storage
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is invalid
 *      - ESP_ERR_NO_MEM if memory can not be allocated or no free drives
 *      - ESP_FAIL if partition can not be mounted
 */
esp_err_t esp_vfs_fat_bdl_mount(const char *base_path,
    esp_blockdev_handle_t bdl_handle,
    const esp_vfs_fat_mount_config_t *mount_config);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_bdl_mount
 *
 * @note This function does NOT release the BDL device handle — the caller owns
 *       the BDL stack lifecycle.
 *
 * @param base_path     path where partition was registered (e.g. "/spiflash")
 * @param bdl_handle    BDL device handle used during mount
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_bdl_mount hasn't been called
 */
esp_err_t esp_vfs_fat_bdl_unmount(const char *base_path, esp_blockdev_handle_t bdl_handle);

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
 *      - ESP_ERR_NO_MEM if memory allocation failed
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
 *      - ESP_ERR_NO_MEM if memory allocation failed
 *      - ESP_FAIL if another FRESULT error (saved in errno)
 */
esp_err_t esp_vfs_fat_test_contiguous_file(const char* base_path, const char* full_path, bool* is_contiguous);

/** @cond */
/**
 * @deprecated Please use `esp_vfs_fat_register` instead
 */
esp_err_t esp_vfs_fat_register_cfg(const esp_vfs_fat_conf_t* conf, FATFS** out_fs);

#ifndef CONFIG_IDF_TARGET_LINUX
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
#endif
/** @endcond */

#ifdef __cplusplus
}
#endif
