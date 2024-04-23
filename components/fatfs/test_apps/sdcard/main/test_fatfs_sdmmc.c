/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "ff.h"
#include "test_fatfs_common.h"
#include "soc/soc_caps.h"
#include "vfs_fat_internal.h"

#if CONFIG_IDF_TARGET_ESP32
#define SDSPI_MISO_PIN 2
#define SDSPI_MOSI_PIN 15
#define SDSPI_CLK_PIN  14
#define SDSPI_CS_PIN   13
#elif CONFIG_IDF_TARGET_ESP32S2
// Adapted for internal test board ESP-32-S3-USB-OTG-Ev-BOARD_V1.0 (with ESP32-S2-MINI-1 module)
#define SDSPI_MISO_PIN 37
#define SDSPI_MOSI_PIN 35
#define SDSPI_CLK_PIN  36
#define SDSPI_CS_PIN   34
#elif CONFIG_IDF_TARGET_ESP32C3
#define SDSPI_MISO_PIN 6
#define SDSPI_MOSI_PIN 4
#define SDSPI_CLK_PIN  5
#define SDSPI_CS_PIN   1
#define SPI_DMA_CHAN   SPI_DMA_CH_AUTO
#endif //CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3

#ifndef SPI_DMA_CHAN
#define SPI_DMA_CHAN   1
#endif //SPI_DMA_CHAN
#define SDSPI_HOST_ID  SPI2_HOST

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3)
// No runner
#include "driver/sdmmc_host.h"
const char* base_path = "/sdcard";

static void test_setup_sdmmc(sdmmc_card_t **out_card)
{
    sdmmc_card_t *card = NULL;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config, &card));
    *out_card = card;
}

static void test_teardown_sdmmc(sdmmc_card_t *card)
{
    TEST_ESP_OK(esp_vfs_fat_sdcard_unmount("/sdcard", card));
}

static const char* test_filename = "/sdcard/hello.txt";

TEST_CASE("Mount fails cleanly without card inserted", "[fatfs][ignore]")
{
    size_t heap_size;
    HEAP_SIZE_CAPTURE(heap_size);
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };

    for (int i = 0; i < 3; ++i) {
        printf("Initializing card, attempt %d\n", i);
        esp_err_t err = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL);
        printf("err=%d\n", err);
        TEST_ESP_ERR(ESP_ERR_TIMEOUT, err);
    }
    HEAP_SIZE_CHECK(heap_size, 0);
}

TEST_CASE("(SD) can format partition", "[fatfs][sdmmc][timeout=180]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    TEST_ESP_OK(esp_vfs_fat_sdcard_format("/sdcard", card));
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(test_filename);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can format partition with config", "[fatfs][sdmmc][timeout=180]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    vfs_fat_sd_ctx_t* ctx = get_vfs_fat_get_sd_ctx(card);
    TEST_ASSERT_NOT_NULL(ctx);

    esp_vfs_fat_mount_config_t format_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .use_one_fat = true,
    };
    TEST_ESP_OK(esp_vfs_fat_sdcard_format_cfg("/sdcard", card, &format_config));
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 1);

    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(test_filename);

    format_config.use_one_fat = false;
    TEST_ESP_OK(esp_vfs_fat_sdcard_format_cfg("/sdcard", card, &format_config));
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 2);

    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(test_filename);

    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can create and write file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can read file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(test_filename);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can read file with pread()", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_pread_file(test_filename);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) pwrite() works well", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_pwrite_file(test_filename);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) overwrite and append file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_overwrite_append(test_filename);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can lseek", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_lseek("/sdcard/seek.txt");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can truncate", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_truncate_file("/sdcard/truncate.txt", true);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can ftruncate", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_ftruncate_file("/sdcard/ftrunc.txt", true);
    test_teardown_sdmmc(card);
}

#if FF_USE_EXPAND
TEST_CASE("(SD) can esp_vfs_fat_create_contiguous_file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_create_contiguous_file("/sdcard", "/sdcard/expand.txt");
    test_teardown_sdmmc(card);
}
#endif

TEST_CASE("(SD) stat returns correct values", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_stat("/sdcard/stat.txt", "/sdcard");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) utime sets modification time", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_utime("/sdcard/utime.txt", "/sdcard");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) unlink removes a file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_unlink("/sdcard/unlink.txt");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) link copies a file, rename moves a file", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_link_rename("/sdcard/link");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can create and remove directories", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_mkdir_rmdir("/sdcard/dir");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) can opendir root directory of FS", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_can_opendir("/sdcard");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) opendir, readdir, rewinddir, seekdir work as expected", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_opendir_readdir_rewinddir("/sdcard/dir");
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) multiple tasks can use same volume", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_concurrent("/sdcard/f");
    test_teardown_sdmmc(card);
}

static void sdmmc_speed_test(void *buf, size_t buf_size, size_t file_size, bool write);

TEST_CASE("(SD) write/read speed test", "[fatfs][sdmmc]")
{
    size_t heap_size;
    HEAP_SIZE_CAPTURE(heap_size);

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
    const size_t file_size = 1 * 1024 * 1024;

    sdmmc_speed_test(buf, 4 * 1024, file_size, true);
    sdmmc_speed_test(buf, 8 * 1024, file_size, true);
    sdmmc_speed_test(buf, 16 * 1024, file_size, true);

    sdmmc_speed_test(buf, 4 * 1024, file_size, false);
    sdmmc_speed_test(buf, 8 * 1024, file_size, false);
    sdmmc_speed_test(buf, 16 * 1024, file_size, false);

    free(buf);

    HEAP_SIZE_CHECK(heap_size, 0);
}

static void sdmmc_speed_test(void *buf, size_t buf_size, size_t file_size, bool write)
{
    sdmmc_card_t *card = NULL;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = write,
        .max_files = 5,
        .allocation_unit_size = 64 * 1024
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card));

    test_fatfs_rw_speed("/sdcard/4mb.bin", buf, buf_size, file_size, write);

    TEST_ESP_OK(esp_vfs_fat_sdcard_unmount("/sdcard", card));
}

TEST_CASE("(SD) mount FAT partitions and readdir to get stat structure", "[fatfs][sdmmc]")
{
    char name_dir_file[64];
    char name_dir_stat[64] = {0};
    const char* dir_prefix = "/sdcard";
    int dir_prefix_len = strlen(dir_prefix);
    int file_num = 300;

    /* Mount FATFS in SD can WL at the same time. Create a file on each FS */
    sdmmc_card_t* card = NULL;
    test_setup_sdmmc(&card);
    TEST_ESP_OK(esp_vfs_fat_sdcard_format("/sdcard", card));

    //Create multiple files with text on sdcard. Each file size is 14 bytes
    //Total files created are file_num (300 in this case)
    //So directory size will be 300*14 bytes
    for(int i=0;i<file_num;i++) {
        snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_%d.bin", dir_prefix, i);
        test_fatfs_create_file_with_text(name_dir_file, fatfs_test_hello_str);
    }

    //Start the timer to get time needed to calculate the directory size
    int64_t start = esp_timer_get_time();
    DIR* dir = opendir(dir_prefix);
    TEST_ASSERT_NOT_NULL(dir);
    struct stat st;
    struct dirent* de;
    uint32_t dir_size = 0;

    // Call readdir before stat function and record the time needed to calculate the directory size
    while(1) {
        de = readdir(dir);
        if (!de) {
            break;
        }
        snprintf(name_dir_stat, dir_prefix_len+sizeof(de->d_name)+1, "%s/%s", dir_prefix, de->d_name);
        TEST_ASSERT_EQUAL(0, stat(name_dir_stat, &st));
        dir_size += st.st_size;
    }
    TEST_ASSERT_EQUAL(0, closedir(dir));
    int64_t end = esp_timer_get_time();
    int64_t total_time_readdir = end-start;
    printf("Time in us for calculating directory size by calling readdir first and then stat func:  %lld \n",total_time_readdir);
    printf("Size of the directory %s is %"PRIu32"Kb\n", dir_prefix, (dir_size/1000));
    TEST_ASSERT_EQUAL(file_num*strlen(fatfs_test_hello_str), dir_size); //each file size is 14 bytes

    // Call stat function directly without calling readdir and record the time needed to calculate the directory size
    dir_size = 0;
    start = esp_timer_get_time();
    for(int i=0;i<file_num;i++) {
        snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_%d.bin", dir_prefix, i);
        TEST_ASSERT_EQUAL(0, stat(name_dir_file, &st));
        dir_size += st.st_size;
    }
    end = esp_timer_get_time();
    int64_t total_time_stat = end-start;
    printf("Time in us for calculating directory size by calling stat func:  %lld \n",total_time_stat);
    printf("Size of the directory %s is %"PRIu32"Kb\n", dir_prefix, (dir_size/1000));
    printf("%d\n", strlen(fatfs_test_hello_str));
    TEST_ASSERT_EQUAL(file_num*strlen(fatfs_test_hello_str), dir_size); //each file size is 14 bytes

    for(int i=0;i<file_num;i++) {
        snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_%d.bin", dir_prefix,i);
        unlink(name_dir_file);
    }

    test_teardown_sdmmc(card);
}

typedef struct {
    const char *dir;
    const char *filename;
    const char *str;
    SemaphoreHandle_t sem;
} test_task_param_t;

static void test_task(void *param)
{
    DIR* dir = NULL;
    struct dirent* de = NULL;
    struct stat st;
    char name_dir_stat[64] = {0};
    const test_task_param_t *test_task_param = param;

    dir = opendir(test_task_param->dir);
    TEST_ASSERT_NOT_NULL(dir);
    while(1) {
        de = readdir(dir);
        if (!de) {
            break;
        }
        //Intentionally introduced a delay to ensure that the second task is triggered simultaneously.
        vTaskDelay(10 / portTICK_PERIOD_MS);
        snprintf(name_dir_stat, sizeof(test_task_param->dir)+sizeof(de->d_name), "%s/%s", test_task_param->dir, de->d_name);
        TEST_ASSERT_EQUAL(0, stat(name_dir_stat, &st));
        if (strcasecmp(de->d_name, test_task_param->filename) == 0) {
            TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);
            TEST_ASSERT_EQUAL(strlen(test_task_param->str), st.st_size);
        } else {
            TEST_FAIL_MESSAGE("unexpected directory entry");
        }
    }

    if (test_task_param->sem) {
        xSemaphoreGive(test_task_param->sem);
    }
    vTaskDelete(NULL);
}

TEST_CASE("(SD) mount two FAT partitions, SDMMC and WL, at the same time and readdir to get stat structure", "[fatfs][sdmmc]")
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    const char *dir_prefix[FF_VOLUMES] = {"/sdcard", "/spiflash"};
    const char *dir_filename[FF_VOLUMES] = {"sd.txt", "wl.txt"};
    const char* str[FF_VOLUMES] = {"this is sd\n", "this is spiflash\n"};
    const char* filename_sd = "/sdcard/sd.txt";
    const char* filename_wl = "/spiflash/wl.txt";

    /* Erase flash before the first use */
    const esp_partition_t *test_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    TEST_ASSERT_NOT_NULL(test_partition);
    esp_partition_erase_range(test_partition, 0, test_partition->size);

    /* Mount FATFS in SD can WL at the same time. Create a file on each FS */
    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &wl_handle));
    unlink(filename_sd);
    unlink(filename_wl);
    test_fatfs_create_file_with_text(filename_sd, str[0]);
    test_fatfs_create_file_with_text(filename_wl, str[1]);

    test_task_param_t test_task_param_sd = {
        .dir = dir_prefix[0],
        .filename = dir_filename[0],
        .str = str[0],
        .sem = xSemaphoreCreateBinary(),
    };

    test_task_param_t test_task_param_spiflash = {
        .dir = dir_prefix[1],
        .filename = dir_filename[1],
        .str = str[1],
        .sem = xSemaphoreCreateBinary(),
    };

    //Create two tasks with same priority to check file size on two different FAT partitions at the same time
    xTaskCreate(test_task, "test_task_1", 8*1024, (void *) &test_task_param_sd, 5, NULL);
    xTaskCreate(test_task, "test_task_2", 8*1024, (void *) &test_task_param_spiflash, 5, NULL);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_task_param_sd.sem, 1000 / portTICK_PERIOD_MS));
    vSemaphoreDelete(test_task_param_sd.sem);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_task_param_spiflash.sem, 1000 / portTICK_PERIOD_MS));
    vSemaphoreDelete(test_task_param_spiflash.sem);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle));
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) read two directories and get stat structure for respective file at the same time", "[fatfs][sdmmc]")
{
    char name_dir_file[64];
    char name_dir1_stat[64] = {0};
    char name_dir2_stat[64] = {0};
    const char* dir1_prefix = "/sdcard/dir1";
    const char* dir2_prefix = "/sdcard/dir2";
    int dir1_prefix_len = strlen(dir1_prefix);
    int dir2_prefix_len = strlen(dir2_prefix);
    const char* test_str1 = "Hello, World!\n";
    const char* test_str2 = "Hello, ESP Community\n";

    /* Mount FATFS in SD can WL at the same time. Create a file on each FS */
    sdmmc_card_t* card = NULL;
    test_setup_sdmmc(&card);

    TEST_ASSERT_EQUAL(0, mkdir(dir1_prefix, 0755));
    TEST_ASSERT_EQUAL(0, mkdir(dir2_prefix, 0755));

    snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_1.bin", dir1_prefix);
    test_fatfs_create_file_with_text(name_dir_file, test_str1);
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_1.bin", dir2_prefix);
    test_fatfs_create_file_with_text(name_dir_file, test_str2);

    DIR* dir1 = opendir(dir1_prefix);
    TEST_ASSERT_NOT_NULL(dir1);
    DIR* dir2 = opendir(dir2_prefix);
    TEST_ASSERT_NOT_NULL(dir2);
    struct dirent* de1;
    struct dirent* de2;
    struct stat st1;
    struct stat st2;

    while(1) {
        de1 = readdir(dir1);
        if (!de1) {
            break;
        }
        de2 = readdir(dir2);
        if (!de2) {
            break;
        }
        snprintf(name_dir1_stat, dir1_prefix_len+sizeof(de1->d_name)+1, "%s/%s", dir1_prefix, de1->d_name);
        snprintf(name_dir2_stat, dir2_prefix_len+sizeof(de2->d_name)+1, "%s/%s", dir2_prefix, de2->d_name);
        TEST_ASSERT_EQUAL(0, stat(name_dir1_stat, &st1));
        TEST_ASSERT_EQUAL(0, stat(name_dir2_stat, &st2));
        TEST_ASSERT_EQUAL(strlen(test_str1), st1.st_size); //size of dir1/boo_1.bin is 14
        TEST_ASSERT_EQUAL(strlen(test_str2), st2.st_size); //size of dir2/boo_1.bin is 21
    }
    TEST_ASSERT_EQUAL(0, closedir(dir1));
    TEST_ASSERT_EQUAL(0, closedir(dir2));

    snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_1.bin", dir1_prefix);
    unlink(name_dir_file);
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_1.bin", dir2_prefix);
    unlink(name_dir_file);
    rmdir(dir1_prefix);
    rmdir(dir2_prefix);

    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) mount two FAT partitions, SDMMC and WL, at the same time", "[fatfs][sdmmc]")
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    const char* filename_sd = "/sdcard/sd.txt";
    const char* filename_wl = "/spiflash/wl.txt";
    const char* str_sd = "this is sd\n";
    const char* str_wl = "this is spiflash\n";

    /* Erase flash before the first use */
    const esp_partition_t *test_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    TEST_ASSERT_NOT_NULL(test_partition);
    esp_partition_erase_range(test_partition, 0, test_partition->size);

    /* Mount FATFS in SD can WL at the same time. Create a file on each FS */
    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &wl_handle));
    unlink(filename_sd);
    unlink(filename_wl);
    test_fatfs_create_file_with_text(filename_sd, str_sd);
    test_fatfs_create_file_with_text(filename_wl, str_wl);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle));
    test_teardown_sdmmc(card);

    /* Check that the file "sd.txt" was created on FS in SD, and has the right data */
    test_setup_sdmmc(&card);
    TEST_ASSERT_NULL(fopen(filename_wl, "r"));
    FILE* f = fopen(filename_sd, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[64];
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf) - 1, f));
    TEST_ASSERT_EQUAL(0, strcmp(buf, str_sd));
    fclose(f);
    test_teardown_sdmmc(card);

    /* Check that the file "wl.txt" was created on FS in WL, and has the right data */
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &wl_handle));
    TEST_ASSERT_NULL(fopen(filename_sd, "r"));
    f = fopen(filename_wl, "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf) - 1, f));
    TEST_ASSERT_EQUAL(0, strcmp(buf, str_wl));
    fclose(f);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle));
}

/*
 * In FatFs menuconfig, set CONFIG_FATFS_API_ENCODING to UTF-8 and set the
 * Codepage to CP936 (Simplified Chinese) in order to run the following tests.
 * Ensure that the text editor is UTF-8 compatible when compiling these tests.
 */
#if defined(CONFIG_FATFS_API_ENCODING_UTF_8) && (CONFIG_FATFS_CODEPAGE == 936)

static const char* test_filename_utf_8 = "/sdcard/测试文件.txt";

TEST_CASE("(SD) can read file using UTF-8 encoded strings", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_create_file_with_text(test_filename_utf_8, fatfs_test_hello_str_utf);
    test_fatfs_read_file_utf_8(test_filename_utf_8);
    test_teardown_sdmmc(card);
}

TEST_CASE("(SD) opendir, readdir, rewinddir, seekdir work as expected using UTF-8 encoded strings", "[fatfs][ignore]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_opendir_readdir_rewinddir_utf_8("/sdcard/目录");
    test_teardown_sdmmc(card);
}
#endif // CONFIG_FATFS_API_ENCODING_UTF_8 && CONFIG_FATFS_CODEPAGE == 936

TEST_CASE("(SD) can get partition info", "[fatfs][sdmmc]")
{
    sdmmc_card_t *card = NULL;
    test_setup_sdmmc(&card);
    test_fatfs_info("/sdcard", "/sdcard/test.txt");
    test_teardown_sdmmc(card);
}

#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3)
