/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Generic-partition Block Device Layer (BDL) example
 *
 * Demonstrates partitioning a raw block device at run time and mounting two
 * different filesystems on the resulting sub-partitions:
 *
 *        +--------------+  +--------------+
 *        |   LittleFS   |  |    FATFS     |   file systems (VFS)
 *        +--------------+  +--------------+  +-----------------------+
 *        |              |  |             <-- | WL BDL (if SPI flash) |
 *        | generic-part |  +- - - - - - - +  +-----------------------+
 *        |     BDL      |  | generic-part |   esp_blockdev_generic_partition_get()
 *        |              |  |     BDL      |   (one slice per MBR partition entry)
 * +------+--------------+--+--------------+
 * | MBR  |        whole-disk BDL          |   esp_partition_get_blockdev() /
 * +------+                                |   sdmmc_get_blockdev()
 * +---------------------------------------+
 * |     SPI flash partition / SD card     |   physical storage
 * +---------------------------------------+
 *
 * Steps performed:
 *   1. Obtain a "whole-disk" BDL from either a SPI flash data partition or an
 *      SD/eMMC card accessed over SDMMC or SDSPI (selectable in menuconfig).
 *   2. Lay out and write an MBR partition table onto the whole disk using the
 *      `esp_ext_part_tables` managed component (fetched via idf_component.yml).
 *   3. Read the partition table back and, for each entry, create a
 *      generic-partition BDL that maps only that slice of the whole disk.
 *   4. Format (if needed) and mount FATFS on the FAT entry and LittleFS on the
 *      LittleFS entry, then perform a simple write/read on each.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "esp_blockdev.h"
#include "esp_blockdev/generic_partition.h"

#include "esp_ext_part_tables.h"
#include "esp_mbr.h"

#include "esp_vfs_fat.h"
#include "esp_littlefs.h"

#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
#include "esp_partition.h"
#include "wear_levelling.h"
#else
#include "sd_card_bdl.h"
#endif

static const char *TAG = "example";

#define WHOLE_DISK_PARTITION_LABEL "storage"
#define FAT_MOUNT_POINT            "/fat"
#define LITTLEFS_MOUNT_POINT       "/littlefs"

/* Keep SD/eMMC formatting bounded; SPI flash FAT uses the remaining space. */
#define LITTLEFS_PARTITION_SIZE    (1 * 1024 * 1024)
#define FAT_PARTITION_SIZE         (16 * 1024 * 1024)

/* The example lays out two data partitions on the whole disk. Their start
 * addresses (and the SPI flash FAT size) are computed by esp_ext_part_tables
 * during MBR generation - see build_and_write_partition_table(). */
enum {
    EXAMPLE_PART_LITTLEFS,  /* first slice, right after the MBR block */
    EXAMPLE_PART_FAT,
    EXAMPLE_PART_COUNT,
};

/* ---------------------------------------------------------------------- */
/* Whole-disk BDL creation / release (media specific)                     */
/* ---------------------------------------------------------------------- */

#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH

static esp_err_t obtain_whole_disk_bdl(esp_blockdev_handle_t *out)
{
    ESP_LOGI(TAG, "Whole disk: SPI flash data partition '%s'", WHOLE_DISK_PARTITION_LABEL);
    return esp_partition_get_blockdev(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY,
                                      WHOLE_DISK_PARTITION_LABEL, out);
}

static esp_err_t release_whole_disk_bdl(esp_blockdev_handle_t disk)
{
    return disk->ops->release(disk);
}

#else /* CONFIG_EXAMPLE_STORAGE_MEDIA_SDCARD */

static esp_err_t obtain_whole_disk_bdl(esp_blockdev_handle_t *out)
{
    /* Card setup (SDMMC or SDSPI, selectable in menuconfig) lives in sd_card_bdl.c. */
    return example_sd_card_bdl_create(out);
}

static esp_err_t release_whole_disk_bdl(esp_blockdev_handle_t disk)
{
    return example_sd_card_bdl_release(disk);
}

#endif

/* ---------------------------------------------------------------------- */
/* Partitioning                                                           */
/* ---------------------------------------------------------------------- */

static esp_err_t build_and_write_partition_table(esp_blockdev_handle_t disk)
{
    esp_ext_part_list_t part_list = {0};
    esp_err_t ret = ESP_OK;

    /* Describe the two partitions and let esp_ext_part_tables place them:
     * ESP_EXT_PART_FLAG_AUTO_ADDRESS makes the generator compute each start
     * address (after the previous entry, aligned to the requested alignment and
     * past the MBR sector), so `.address` is left unset. LittleFS gets a fixed
     * size; FATFS gets 16 MiB on SD/eMMC. On SPI flash, FATFS instead uses
     * ESP_EXT_PART_FLAG_FILL with `.size == 0` to take the remaining space. */
    esp_ext_part_list_item_t partitions[EXAMPLE_PART_COUNT] = {
        /* The LittleFS MBR entry (type 0xC3) stores the filesystem block size in
         * the CHS "hack" field, passed via `.extra` + ESP_EXT_PART_FLAG_EXTRA.
         * LittleFS in classic mode uses the BDL erase size as its block size. */
        [EXAMPLE_PART_LITTLEFS] = {
            .info = {
                .size = LITTLEFS_PARTITION_SIZE,
                .type = ESP_EXT_PART_TYPE_LITTLEFS,
                .extra = disk->geometry.erase_size,
                .flags = ESP_EXT_PART_FLAG_EXTRA | ESP_EXT_PART_FLAG_AUTO_ADDRESS,
                .label = NULL,
            },
        },
        [EXAMPLE_PART_FAT] = {
            .info = {
                .type = ESP_EXT_PART_TYPE_FAT32,
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
                .size = 0, /* FILL: sized to the rest of the disk by the generator */
                .flags = ESP_EXT_PART_FLAG_AUTO_ADDRESS | ESP_EXT_PART_FLAG_FILL,
#else
                .size = FAT_PARTITION_SIZE,
                .flags = ESP_EXT_PART_FLAG_AUTO_ADDRESS,
#endif
                .label = NULL,
            },
        },
    };

    /* Choose the partition-start alignment based on the storage medium:
     *   - SPI flash: 4 KiB, matching the flash erase (sector) size. A larger
     *     alignment would waste a big chunk of the small (few-MiB) flash disk.
     *   - SD/eMMC card: 1 MiB, matching how PC tools (fdisk/parted) align
     *     partitions to the card's erase/allocation unit for best performance.
     * Either value is >= the BDL erase size, so it also satisfies the
     * generic-partition BDL erase-alignment requirement.
     *
     * `total_size` is the whole-disk size: the generator needs it to size the
     * SPI flash FILL (FAT) partition and to run its overlap / off-disk checks.
     * (esp_ext_part_list_bdl_write() would auto-fill it from the device geometry
     * when left 0, but the example sets it explicitly for clarity.) */
    esp_mbr_generate_extra_args_t gen_args = {
        .total_size = disk->geometry.disk_size,
        .sector_size = ESP_EXT_PART_SECTOR_SIZE_512B,
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
        .alignment = ESP_EXT_PART_ALIGN_4KiB,
#else
        .alignment = ESP_EXT_PART_ALIGN_1MiB,
#endif
    };

    /* Insert in on-disk order so the MBR entries match the physical layout. */
    for (int i = 0; i < EXAMPLE_PART_COUNT; i++) {
        ESP_GOTO_ON_ERROR(esp_ext_part_list_insert(&part_list, &partitions[i]), cleanup, TAG, "insert partition entry %d", i);
    }

    /* esp_ext_part_list_bdl_write() writes the raw 512-byte MBR without erasing
     * first. On flash-like devices the target block must be erased beforehand. */
    if (disk->device_flags.erase_before_write) {
        ESP_GOTO_ON_ERROR(disk->ops->erase(disk, 0, disk->geometry.erase_size), cleanup, TAG, "erase MBR area");
    }

    ESP_GOTO_ON_ERROR(esp_ext_part_list_bdl_write(disk, &part_list, ESP_EXT_PART_LIST_SIGNATURE_MBR, &gen_args),
                      cleanup, TAG, "write MBR");

cleanup:
    esp_ext_part_list_deinit(&part_list);
    return ret;
}

/* ---------------------------------------------------------------------- */
/* Filesystem usage                                                       */
/* ---------------------------------------------------------------------- */

static void write_and_read_back(const char *path, const char *content);

static esp_err_t mount_fat(esp_blockdev_handle_t part)
{
    ESP_LOGI(TAG, "Mounting FATFS on the FAT partition BDL");
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        /* Use larger clusters to keep the FAT tables small. */
        .allocation_unit_size = 16 * 1024,
        .use_one_fat = false,
    };
    ESP_RETURN_ON_ERROR(esp_vfs_fat_bdl_mount(FAT_MOUNT_POINT, part, &mount_config), TAG, "FAT mount failed");
    return ESP_OK;
}

static esp_err_t mount_littlefs(esp_blockdev_handle_t part)
{
    ESP_LOGI(TAG, "Mounting LittleFS on the LittleFS partition BDL");
    const esp_vfs_littlefs_conf_t conf = {
        .base_path = LITTLEFS_MOUNT_POINT,
        .blockdev = part,
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    ESP_RETURN_ON_ERROR(esp_vfs_littlefs_register(&conf), TAG, "LittleFS mount failed");
    return ESP_OK;
}

/* ---------------------------------------------------------------------- */

void app_main(void)
{
    /* Step 1: obtain the whole-disk block device. */
    esp_blockdev_handle_t disk = NULL;
    ESP_ERROR_CHECK(obtain_whole_disk_bdl(&disk));
    ESP_LOGI(TAG, "Whole disk BDL: disk_size=%" PRIu64 ", read_size=%zu, write_size=%zu, erase_size=%zu",
        disk->geometry.disk_size, disk->geometry.read_size, disk->geometry.write_size, disk->geometry.erase_size);

    /* Step 2: lay out and write the MBR partition table. The partition start
     * addresses (and the SPI flash FAT size) are assigned by esp_ext_part_tables during
     * generation; the actual offsets are logged from the read-back below. */
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
    ESP_LOGI(TAG, "Layout: LittleFS = %u KiB (fixed), FAT = remainder of the disk",
             (unsigned)(LITTLEFS_PARTITION_SIZE / 1024));
#else
    ESP_LOGI(TAG, "Layout: LittleFS = %u KiB (fixed), FAT = %u KiB (fixed)",
             (unsigned)(LITTLEFS_PARTITION_SIZE / 1024), (unsigned)(FAT_PARTITION_SIZE / 1024));
#endif
    ESP_LOGI(TAG, "Writing MBR partition table to the whole disk");
    ESP_ERROR_CHECK(build_and_write_partition_table(disk));

    /* Step 3: read the partition table back and create a generic-partition BDL
     * for each entry, mounting the matching filesystem. */
    ESP_LOGI(TAG, "Reading MBR partition table back");
    esp_ext_part_list_t part_list = {0};
    esp_mbr_parse_extra_args_t parse_args = {
        .sector_size = ESP_EXT_PART_SECTOR_SIZE_512B,
        /* .match left zero (fn == NULL): keep every recognized partition (the two
         * we wrote). To have the parser drop non-mountable entries up front, set
         * .match = esp_ext_part_match_mountable() instead. */
    };
    ESP_ERROR_CHECK(esp_ext_part_list_bdl_read(disk, &part_list, ESP_EXT_PART_LIST_SIGNATURE_MBR, &parse_args));

    /* A LOSSY list means the parser could not represent every on-disk partition
     * (e.g. it was filtered, or the table held more than the list can hold). */
    if (part_list.flags & ESP_EXT_PART_LIST_FLAG_LOSSY) {
        ESP_LOGW(TAG, "Parsed partition list is LOSSY (some entries were dropped)");
    }

#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
    esp_blockdev_handle_t wl_part = NULL;
#endif
    esp_blockdev_handle_t fat_part = NULL;
    esp_blockdev_handle_t lfs_part = NULL;

    /* esp_ext_part_match_mountable() reports whether a partition holds a
     * filesystem this build can mount: FAT (always part of ESP-IDF) and LittleFS
     * when the LittleFS component is linked. esp_ext_part_tables detects LittleFS
     * via its build system (it defines ESP_EXT_PART_HAS_LITTLEFS when the
     * component is present), so the stock predicate works here without a custom
     * one. esp_ext_part_list_next_matching() then walks only the matching
     * (mountable) entries; any other partition types are skipped automatically. */
    esp_ext_part_match_t mountable = esp_ext_part_match_mountable();

    int index = 0;
    for (esp_ext_part_list_item_t *it = esp_ext_part_list_next_matching(NULL, &part_list, &mountable);
            it != NULL;
            it = esp_ext_part_list_next_matching(it, &part_list, &mountable), index++) {
        ESP_LOGI(TAG, "Mountable partition %d: type=%u, address=0x%08llx, size=0x%08llx",
                 index, (unsigned)it->info.type,
                 (unsigned long long)it->info.address, (unsigned long long)it->info.size);

        /* FAT and LittleFS need different mount calls. */
        switch (it->info.type) {
        case ESP_EXT_PART_TYPE_FAT12:
        case ESP_EXT_PART_TYPE_FAT16:
        case ESP_EXT_PART_TYPE_FAT32:
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
            ESP_ERROR_CHECK(esp_blockdev_generic_partition_get(disk, it->info.address, it->info.size, &wl_part));
            ESP_ERROR_CHECK(wl_get_blockdev(wl_part, &fat_part));
#else
            ESP_ERROR_CHECK(esp_blockdev_generic_partition_get(disk, it->info.address, it->info.size, &fat_part));
#endif
            ESP_ERROR_CHECK(mount_fat(fat_part));
            write_and_read_back(FAT_MOUNT_POINT "/hello.txt", "Hello from FATFS over a generic-partition BDL!");
            break;
        case ESP_EXT_PART_TYPE_LITTLEFS:
            ESP_ERROR_CHECK(esp_blockdev_generic_partition_get(disk, it->info.address, it->info.size, &lfs_part));
            ESP_ERROR_CHECK(mount_littlefs(lfs_part));
            write_and_read_back(LITTLEFS_MOUNT_POINT "/hello.txt", "Hello from LittleFS over a generic-partition BDL!");
            break;
        default:
            ESP_LOGW(TAG, "No mount handler for mountable type %u", (unsigned)it->info.type);
            break;
        }
    }
    /* Step 4: tear everything down. */

    esp_ext_part_list_deinit(&part_list);
    if (fat_part != NULL) {
        ESP_LOGI(TAG, "Unmounting FATFS");
        ESP_ERROR_CHECK(esp_vfs_fat_bdl_unmount(FAT_MOUNT_POINT, fat_part));
        /* FAT unmount does NOT release the BDL handle - the caller owns it. */
        ESP_ERROR_CHECK(fat_part->ops->release(fat_part));
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
        ESP_LOGI(TAG, "Releasing WL BDL");
        ESP_ERROR_CHECK(wl_part->ops->release(wl_part));
#endif
    }
    if (lfs_part != NULL) {
        ESP_LOGI(TAG, "Unmounting LittleFS");
        /* LittleFS unregister releases the BDL handle for us. */
        ESP_ERROR_CHECK(esp_vfs_littlefs_unregister_blockdev(lfs_part));
    }

    ESP_LOGI(TAG, "Releasing whole-disk BDL");
    ESP_ERROR_CHECK(release_whole_disk_bdl(disk));

    ESP_LOGI(TAG, "Done");
}

static void write_and_read_back(const char *path, const char *content)
{
    ESP_LOGI(TAG, "Writing '%s'", path);
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open %s for writing", path);
        return;
    }
    fprintf(f, "%s\n", content);
    fclose(f);

    FILE *fr = fopen(path, "r");
    if (fr == NULL) {
        ESP_LOGE(TAG, "Failed to open %s for reading", path);
        return;
    }
    char line[128] = {0};
    if (fgets(line, sizeof(line), fr) != NULL) {
        char *nl = strchr(line, '\n');
        if (nl) {
            *nl = '\0';
        }
        ESP_LOGI(TAG, "Read back from %s: '%s'", path, line);
    }
    fclose(fr);
}
