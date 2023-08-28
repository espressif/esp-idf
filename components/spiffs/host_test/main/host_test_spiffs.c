/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#include "Mockqueue.h"

#include "esp_partition.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs_api.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(spiffs);

TEST_SETUP(spiffs)
{
    // CMock init for spiffs xSemaphore* use
    xQueueSemaphoreTake_IgnoreAndReturn(0);
    xQueueGenericSend_IgnoreAndReturn(0);
}

TEST_TEAR_DOWN(spiffs)
{
}

static void init_spiffs(spiffs *fs, uint32_t max_files)
{
    spiffs_config cfg = {};
    s32_t spiffs_res;
    u32_t flash_sector_size;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    TEST_ASSERT_NOT_NULL(partition);

    // Configure objects needed by SPIFFS
    esp_spiffs_t *user_data = (esp_spiffs_t *) calloc(1, sizeof(*user_data));
    user_data->partition = partition;
    fs->user_data = (void *)user_data;

    flash_sector_size = 4096;

    cfg.hal_erase_f = spiffs_api_erase;
    cfg.hal_read_f = spiffs_api_read;
    cfg.hal_write_f = spiffs_api_write;
    cfg.log_block_size = flash_sector_size;
    cfg.log_page_size = CONFIG_SPIFFS_PAGE_SIZE;
    cfg.phys_addr = 0;
    cfg.phys_erase_block = flash_sector_size;
    cfg.phys_size = partition->size;

    uint32_t work_sz = cfg.log_page_size * 2;
    uint8_t *work = (uint8_t *) malloc(work_sz);

    uint32_t fds_sz = max_files * sizeof(spiffs_fd);
    uint8_t *fds = (uint8_t *) malloc(fds_sz);

#if CONFIG_SPIFFS_CACHE
    uint32_t cache_sz = sizeof(spiffs_cache) + max_files * (sizeof(spiffs_cache_page)
                        + cfg.log_page_size);
    uint8_t *cache = (uint8_t *) malloc(cache_sz);
#else
    uint32_t cache_sz = 0;
    uint8_t cache = NULL;
#endif

    // Special mounting procedure: mount, format, mount as per
    // https://github.com/pellepl/spiffs/wiki/Using-spiffs
    spiffs_res = SPIFFS_mount(fs, &cfg, work, fds, fds_sz,
                              cache, cache_sz, spiffs_api_check);

    if (spiffs_res == SPIFFS_ERR_NOT_A_FS) {
        spiffs_res = SPIFFS_format(fs);
        TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);

        spiffs_res = SPIFFS_mount(fs, &cfg, work, fds, fds_sz,
                                  cache, cache_sz, spiffs_api_check);
    }

    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);
}

static void deinit_spiffs(spiffs *fs)
{
    SPIFFS_unmount(fs);

    free(fs->work);
    free(fs->user_data);
    free(fs->fd_space);

#if CONFIG_SPIFFS_CACHE
    free(fs->cache);
#endif
}

static void check_spiffs_files(spiffs *fs, const char *base_path, char *cur_path)
{
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(cur_path);

    if (len == 0) {
        strcpy(cur_path, base_path);
        len = strlen(base_path);
    }

    dir = opendir(cur_path);
    TEST_ASSERT_TRUE(dir != 0);

    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;

        char path[PATH_MAX] = { 0 };

        // Read the file from host FS
        strcpy(path, cur_path);
        strcat(path, "/");
        strcat(path, name);

        struct stat sb;
        stat(path, &sb);
        if (S_ISDIR(sb.st_mode)) {
            if (!strcmp(name, ".") || !strcmp(name, "..")) {
                continue;
            }
            cur_path[len] = '/';
            strcpy(cur_path + len + 1, name);
            check_spiffs_files(fs, base_path, cur_path);
            cur_path[len] = '\0';
        } else {
            FILE *f = fopen(path, "r");
            TEST_ASSERT_NOT_NULL(f);
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);

            char *f_contents = (char *) malloc(sz);
            TEST_ASSERT(fread(f_contents, 1, sz, f) == sz);
            fclose(f);

            s32_t spiffs_res;

            // Read the file from SPIFFS
            char *spiffs_path = path + strlen(base_path);
            spiffs_res = SPIFFS_open(fs, spiffs_path, SPIFFS_RDONLY, 0);

            TEST_ASSERT_TRUE(spiffs_res > SPIFFS_OK);

            spiffs_file fd = spiffs_res;

            spiffs_stat stat;
            spiffs_res = SPIFFS_stat(fs, spiffs_path, &stat);

            char *spiffs_f_contents = (char *) malloc(stat.size);
            spiffs_res = SPIFFS_read(fs, fd, spiffs_f_contents, stat.size);
            TEST_ASSERT_TRUE(spiffs_res == stat.size);

            // Compare the contents
            TEST_ASSERT_TRUE(sz == stat.size);

            bool same = memcmp(f_contents, spiffs_f_contents, sz) == 0;
            TEST_ASSERT_TRUE(same);

            free(f_contents);
            free(spiffs_f_contents);
        }
    }
    closedir(dir);
}

TEST(spiffs, format_disk_open_file_write_and_read_file)
{
    spiffs fs;
    s32_t spiffs_res;

    init_spiffs(&fs, 5);

    // Open test file
    spiffs_res = SPIFFS_open(&fs, "test.txt", SPIFFS_O_CREAT | SPIFFS_O_RDWR, 0);
    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);

    // Generate data
    spiffs_file file = spiffs_res;

    uint32_t data_count = 5000;
    uint32_t data_size = data_count * sizeof(uint32_t);

    char *data = (char *) malloc(data_size);
    char *read = (char *) malloc(data_size);

    for (uint32_t i = 0; i < data_size; i += sizeof(i)) {
        *((uint32_t *)(data + i)) = i;
    }

    // Write data to file
    spiffs_res = SPIFFS_write(&fs, file, (void *)data, data_size);
    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);
    TEST_ASSERT_TRUE(spiffs_res == data_size);

    // Set the file object pointer to the beginning
    spiffs_res = SPIFFS_lseek(&fs, file, 0, SPIFFS_SEEK_SET);
    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);

    // Read the file
    spiffs_res = SPIFFS_read(&fs, file, (void *)read, data_size);
    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);
    TEST_ASSERT_TRUE(spiffs_res == data_size);

    // Close the test file
    spiffs_res = SPIFFS_close(&fs, file);
    TEST_ASSERT_TRUE(spiffs_res >= SPIFFS_OK);

    TEST_ASSERT_TRUE(memcmp(data, read, data_size) == 0);

    deinit_spiffs(&fs);

    free(read);
    free(data);
}

TEST(spiffs, can_read_spiffs_image)
{
    spiffs fs;
    s32_t spiffs_res;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    TEST_ASSERT_NOT_NULL(partition);

    // Write the contents of the image file to partition
    FILE *img_file = fopen(BUILD_DIR "/image.bin", "r");
    TEST_ASSERT_NOT_NULL(img_file);

    fseek(img_file, 0, SEEK_END);
    long img_size = ftell(img_file);
    fseek(img_file, 0, SEEK_SET);

    char *img = (char *) malloc(img_size);
    TEST_ASSERT(fread(img, 1, img_size, img_file) == img_size);
    fclose(img_file);
    TEST_ASSERT_EQUAL(partition->size, img_size);

    esp_partition_erase_range(partition, 0, partition->size);
    esp_partition_write(partition, 0, img, img_size);

    free(img);

    // Mount the spiffs partition and init filesystem, using the contents of
    // the image file
    init_spiffs(&fs, 1024);

    // Check spiffs consistency
    spiffs_res = SPIFFS_check(&fs);
    TEST_ASSERT_TRUE(spiffs_res == SPIFFS_OK);

    char path_buf[PATH_MAX] = {0};

    // The image is created from the spiffs source directory. Compare the files in that
    // directory to the files read from the SPIFFS image.
    check_spiffs_files(&fs, BUILD_DIR "/../../spiffs", path_buf);

    deinit_spiffs(&fs);
}

TEST_GROUP_RUNNER(spiffs)
{
    RUN_TEST_CASE(spiffs, format_disk_open_file_write_and_read_file);
    RUN_TEST_CASE(spiffs, can_read_spiffs_image);
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(spiffs);
}

int main(int argc, char **argv)
{
    UNITY_MAIN_FUNC(run_all_tests);
    return 0;
}
