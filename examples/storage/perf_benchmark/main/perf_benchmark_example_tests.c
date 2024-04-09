/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_random.h"
#include "driver/sdmmc_types.h"
#include "sdmmc_cmd.h"
#include "wear_levelling.h"
#include "esp_log.h"
#include "esp_check.h"

#include "sdkconfig.h"
#include "perf_benchmark_example_tests.h"

#if CONFIG_EXAMPLE_USE_BYTES
#define UNIT_STRING "B"
#define UNIT_MULTIPLIER (1)
#else
#define UNIT_STRING "b"
#define UNIT_MULTIPLIER (8)
#endif

static void print_results(const char *name, double time, size_t size, int repeat_count)
{
    double average = time / repeat_count;
    double speed = (size / average) / (1024 * 1024) * (1000 * 1000);
    printf("[%-55s] (%dx) %8.3f ms %8.2f k" UNIT_STRING " %10.3f M" UNIT_STRING "/s\n",
           name, repeat_count, (float) average / 1000, (float)size * UNIT_MULTIPLIER / 1024, speed);
}

void spiflash_speed_test_raw_run(size_t repeat_count)
{
    const size_t buf_size = CONFIG_EXAMPLE_TARGET_RW_SIZE;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    assert(buf != NULL);
    esp_fill_random(buf, buf_size);

    struct timeval tv_start;
    struct timeval tv_end;
    struct timeval tv_diff;
    double time = 0;

    for (int i = 0; i < repeat_count; ++i) {
        gettimeofday(&tv_start, NULL);
        ESP_ERROR_CHECK(wl_write(s_wl_handle, 0, buf, buf_size));
        gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_start, &tv_diff);
        time += (tv_diff.tv_sec * 1000000 + tv_diff.tv_usec);
    }
    print_results("SPI Flash raw write", time, buf_size, repeat_count);

    time = 0;
    for (int i = 0; i < repeat_count; ++i) {
        gettimeofday(&tv_start, NULL);
        ESP_ERROR_CHECK(wl_read(s_wl_handle, 0, buf, buf_size));
        gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_start, &tv_diff);
        time += (tv_diff.tv_sec * 1000000 + tv_diff.tv_usec);
    }
    print_results("SPI Flash raw read", time, buf_size, repeat_count);

    free(buf);
}

static void run_fs_test(const char *name, const char* path, void *buf, size_t buf_size, bool is_write, bool cleanup, bool new_file, size_t repeat_count)
{
    struct timeval tv_start;
    struct timeval tv_end;
    struct timeval tv_diff;
    double time = 0;
    int fd = -1;
    const int flags = is_write ? O_CREAT | O_TRUNC | O_RDWR : O_RDONLY;

    if (!new_file) {
        fd = open(path, flags, 0666);
        assert(fd >= 0);
    }

    for (int i = 0; i < repeat_count; ++i) {
        if (new_file) {
            fd = open(path, flags, 0666);
            assert(fd >= 0);
        }

        gettimeofday(&tv_start, NULL);
        int ret = is_write ? write(fd, buf, buf_size) : read(fd, buf, buf_size);
        gettimeofday(&tv_end, NULL);

        if (ret < 0) {
            perror(is_write ? "write" : "read");
            close(fd);
            return;
        }

        if (ret != buf_size) {
            ESP_LOGE("fs_test",
                     "Could not %s full buffer size. Actually %s bytes: %d.",
                     is_write ? "write" : "read",
                     is_write ? "wrote" : "read",
                     ret);
        }

        timersub(&tv_end, &tv_start, &tv_diff);
        time += (tv_diff.tv_sec * 1000000 + tv_diff.tv_usec);

        if (new_file) {
            close(fd);
        }
    }

    if (!new_file) {
        close(fd);
    }

    if (cleanup) {
        unlink(path);
    }

    print_results(name, time, buf_size, repeat_count);
}

static void run_fs_tests(const char *base_path, const char *type, bool new_file, size_t repeat_count)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/test.bin", base_path);

    char name[256];
    strcpy(name, type);
    if (new_file)
        strcat(name, " (new file)");
    strcat(name, ": ");

    size_t name_prefix_len = strlen(name);

    const size_t less_than_target_size = CONFIG_EXAMPLE_TARGET_RW_SIZE / 1.2;
    const size_t exact_target_size = CONFIG_EXAMPLE_TARGET_RW_SIZE;
    const size_t more_than_target_size = CONFIG_EXAMPLE_TARGET_RW_SIZE * 1.2;

    const size_t tiny_size = 255;
    assert(tiny_size < less_than_target_size);

    const size_t buf_size = more_than_target_size;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    assert(buf != NULL);
    esp_fill_random(buf, buf_size);

    run_fs_test(strcat(name, "write target size"), path, buf, exact_target_size, true, false, new_file, repeat_count);
    name[name_prefix_len] = '\0';
    run_fs_test(strcat(name, "read target size"), path, buf, exact_target_size, false, true, new_file, repeat_count);
    name[name_prefix_len] = '\0';

    run_fs_test(strcat(name, "write more than target size"), path, buf, more_than_target_size, true, false, new_file, repeat_count);
    name[name_prefix_len] = '\0';
    run_fs_test(strcat(name, "read more than target size"), path, buf, more_than_target_size, false, true, new_file, repeat_count);
    name[name_prefix_len] = '\0';

    run_fs_test(strcat(name, "write less than target size"), path, buf, less_than_target_size, true, false, new_file, repeat_count);
    name[name_prefix_len] = '\0';
    run_fs_test(strcat(name, "read less than target size"), path, buf, less_than_target_size, false, true, new_file, repeat_count);
    name[name_prefix_len] = '\0';

    run_fs_test(strcat(name, "write tiny size"), path, buf, tiny_size, true, false, new_file, repeat_count);
    name[name_prefix_len] = '\0';
    run_fs_test(strcat(name, "read tiny size"), path, buf, tiny_size, false, true, new_file, repeat_count);
    name[name_prefix_len] = '\0';
    free(buf);
}

void spiflash_speed_test_spiffs_run(size_t repeat_count)
{
    run_fs_tests(FLASH_BASE_PATH, "SPI SPIFFS", true, repeat_count);
}

void spiflash_speed_test_fatfs_run(size_t repeat_count)
{
    run_fs_tests(FLASH_BASE_PATH, "SPI FATFS", true, repeat_count);
}

void spiflash_speed_test_littlefs_run(size_t repeat_count)
{
    run_fs_tests(FLASH_BASE_PATH, "SPI LittleFS", true, repeat_count);
}

void sdcard_speed_test_fatfs_run(size_t repeat_count)
{
    run_fs_tests(SD_BASE_PATH, "SD FATFS", true, repeat_count);
    run_fs_tests(SD_BASE_PATH, "SD FATFS", false, repeat_count);
}


void sdcard_speed_test_littlefs_run(size_t repeat_count)
{
    run_fs_tests(SD_BASE_PATH, "SD LittleFS", true, repeat_count);
    run_fs_tests(SD_BASE_PATH, "SD LittleFS", false, repeat_count);
}

void sdcard_speed_test_raw_run(sdmmc_card_t *card, size_t repeat_count)
{
    uint32_t sector_size = card->csd.sector_size;
    size_t sector_count = CONFIG_EXAMPLE_TARGET_RW_SIZE / sector_size;
    size_t subsection_sector_count = sector_count / 4;

    char *buf = (char *) calloc(1, sector_count * sector_size);
    assert(buf != NULL);

    struct timeval tv_start;
    struct timeval tv_end;
    struct timeval tv_diff;

    for(int i = 0; i < 4; i++) {
        const size_t count = subsection_sector_count * (i + 1);
        double time = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gettimeofday(&tv_start, NULL);
            ESP_ERROR_CHECK(sdmmc_write_sectors(card, buf, 0, sector_count));
            gettimeofday(&tv_end, NULL);
            timersub(&tv_end, &tv_start, &tv_diff);
            time += (tv_diff.tv_sec * 1000000 + tv_diff.tv_usec);
        }
        print_results("SD raw write", time, count * sector_size, repeat_count);

        time = 0;
        for (int i = 0; i < repeat_count; ++i) {
            gettimeofday(&tv_start, NULL);
            ESP_ERROR_CHECK(sdmmc_read_sectors(card, buf, 0, sector_count));
            gettimeofday(&tv_end, NULL);
            timersub(&tv_end, &tv_start, &tv_diff);
            time += (tv_diff.tv_sec * 1000000 + tv_diff.tv_usec);
        }
        print_results("SD raw read", time, count * sector_size, repeat_count);
    }
    free(buf);
}
