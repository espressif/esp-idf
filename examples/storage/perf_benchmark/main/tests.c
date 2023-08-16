/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "esp_random.h"
#include "driver/sdmmc_types.h"
#include "sdmmc_cmd.h"
#include "wear_levelling.h"

#include "sdkconfig.h"
#include "tests.h"

static const char *TAG = "example_tests";

void spiflash_speed_test_raw_run(void);
void spiflash_speed_test_fs_run(void);
void sdcard_speed_test_raw_run(sdmmc_card_t *card);
void sdcard_speed_test_fatfs_run(void);

static void print_bench_output(struct timeval tv_start, struct timeval tv_end, size_t size, size_t start_offset, bool is_write)
{
    bool use_megabytes;
#ifdef CONFIG_EXAMPLE_USE_MEGABYTES
    use_megabytes = true;
#else
    use_megabytes = false;
#endif // CONFIG_EXAMPLE_USE_MEGABYTES

    float t_s = tv_end.tv_sec - tv_start.tv_sec + 1e-6f * (tv_end.tv_usec - tv_start.tv_usec);
    double io_speed = size / (1024.0f * 1024.0f * t_s);
    printf("%s %d bytes in %.3fms (%.3f%s) %s address %u\n",
            (is_write)?"Wrote":"Read", size, t_s * 1e3, (use_megabytes)?io_speed:io_speed*8.0,
            (use_megabytes)?"MiB/s":"Mib/s", (is_write)?"to":"from", start_offset);
}

static void test_rw_raw_spiflash_speed(wl_handle_t handle, size_t src_dest_addr, void *src_dest_buff, size_t size, bool is_write)
{
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    if (is_write) {
        wl_write(handle, src_dest_addr, src_dest_buff, size);
    } else {
        wl_read(handle, src_dest_addr, src_dest_buff, size);
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    print_bench_output(tv_start, tv_end, size, src_dest_addr, is_write);
}

static void test_rw_raw_sd_speed(sdmmc_card_t *card, char* buf, uint32_t start_sector, size_t sector_count, size_t sector_size, bool is_write)
{
    int file_size = sector_count * sector_size;
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    if (is_write) {
        ESP_ERROR_CHECK(sdmmc_write_sectors(card, buf, start_sector, sector_count));
    } else {
        ESP_ERROR_CHECK(sdmmc_read_sectors(card, buf, start_sector, sector_count));
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    print_bench_output(tv_start, tv_end, file_size, start_sector, is_write);
}

static void test_rw_fs_speed(const char* filename, void* buf, size_t buf_size, size_t file_size, bool is_write)
{
    bool use_megabytes;
#ifdef CONFIG_EXAMPLE_USE_MEGABYTES
    use_megabytes = true;
#else
    use_megabytes = false;
#endif // CONFIG_EXAMPLE_USE_MEGABYTES

    const size_t buf_count = file_size / buf_size;
    FILE* f = fopen(filename, (is_write) ? "wb" : "rb");

    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    for (size_t n = 0; n < buf_count; ++n) {
        if (is_write) {
            write(fileno(f), buf, buf_size);
        } else {
            if (read(fileno(f), buf, buf_size) != buf_size) {
                printf("reading at n=%d, eof=%d", n, feof(f));
            }
        }
    }
    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    fclose(f);

    float t_s = tv_end.tv_sec - tv_start.tv_sec + 1e-6f * (tv_end.tv_usec - tv_start.tv_usec);
    double io_speed = file_size / (1024.0f * 1024.0f * t_s);
    printf("%s %d bytes (buffer size %dB) in %.3fms (%.3f%s)\n",
            (is_write)?"Wrote":"Read", file_size, buf_size, t_s * 1e3,
            (use_megabytes)?io_speed:io_speed*8.0, (use_megabytes)?"MiB/s":"Mib/s");
}

void spiflash_speed_test_raw_run(void)
{
    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
    size_t file_size;

    ESP_LOGI(TAG, "Test with 128kiB file");
    file_size = 128 * 1024;

    test_rw_raw_spiflash_speed(s_wl_handle, 0, buf, file_size, true);

    free(buf);
}

static void spiflash_speed_test(void *buf, size_t buf_size, size_t file_size, bool write)
{
    test_rw_fs_speed(FLASH_BASE_PATH"/test.bin", buf, buf_size, file_size, write);
}

void spiflash_speed_test_fs_run(void)
{
    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
    size_t file_size;

    ESP_LOGI(TAG, "Test with 128kiB file");
    file_size = 128 * 1024;
    // Write
    spiflash_speed_test(buf, 4 * 1024, file_size, true);
    spiflash_speed_test(buf, 8 * 1024, file_size, true);
    spiflash_speed_test(buf, 16 * 1024, file_size, true);
    // Read
    spiflash_speed_test(buf, 4 * 1024, file_size, false);
    spiflash_speed_test(buf, 8 * 1024, file_size, false);
    spiflash_speed_test(buf, 16 * 1024, file_size, false);

    ESP_LOGI(TAG, "Test with 256kiB file");
    file_size *= 2;
    // Write
    spiflash_speed_test(buf, 4 * 1024, file_size, true);
    spiflash_speed_test(buf, 8 * 1024, file_size, true);
    spiflash_speed_test(buf, 16 * 1024, file_size, true);
    // Read
    spiflash_speed_test(buf, 4 * 1024, file_size, false);
    spiflash_speed_test(buf, 8 * 1024, file_size, false);
    spiflash_speed_test(buf, 16 * 1024, file_size, false);

    ESP_LOGI(TAG, "Test with 512kiB file");
    file_size *= 2;
    // Write
    spiflash_speed_test(buf, 4 * 1024, file_size, true);
    spiflash_speed_test(buf, 8 * 1024, file_size, true);
    spiflash_speed_test(buf, 16 * 1024, file_size, true);
    // Read
    spiflash_speed_test(buf, 4 * 1024, file_size, false);
    spiflash_speed_test(buf, 8 * 1024, file_size, false);
    spiflash_speed_test(buf, 16 * 1024, file_size, false);

    free(buf);
}

static void sd_speed_test(void *buf, size_t buf_size, size_t file_size, bool write)
{
    test_rw_fs_speed(SD_BASE_PATH"/test.bin", buf, buf_size, file_size, write);
}

void sdcard_speed_test_fatfs_run(void)
{
    const size_t buf_size = 32 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
    size_t file_size;

    ESP_LOGI(TAG, "Test with 1MiB file");
    file_size = 1 * 1024 * 1024;
    // Write
    sd_speed_test(buf, 4 * 1024, file_size, true);
    sd_speed_test(buf, 8 * 1024, file_size, true);
    sd_speed_test(buf, 16 * 1024, file_size, true);
    sd_speed_test(buf, 32 * 1024, file_size, true);
    // Read
    sd_speed_test(buf, 4 * 1024, file_size, false);
    sd_speed_test(buf, 8 * 1024, file_size, false);
    sd_speed_test(buf, 16 * 1024, file_size, false);
    sd_speed_test(buf, 32 * 1024, file_size, false);

    ESP_LOGI(TAG, "Test with 4MiB file");
    file_size = 4 * 1024 * 1024;
    // Write
    sd_speed_test(buf, 4 * 1024, file_size, true);
    sd_speed_test(buf, 8 * 1024, file_size, true);
    sd_speed_test(buf, 16 * 1024, file_size, true);
    sd_speed_test(buf, 32 * 1024, file_size, true);
    // Read
    sd_speed_test(buf, 4 * 1024, file_size, false);
    sd_speed_test(buf, 8 * 1024, file_size, false);
    sd_speed_test(buf, 16 * 1024, file_size, false);
    sd_speed_test(buf, 32 * 1024, file_size, false);

    ESP_LOGI(TAG, "Test with 16MiB file");
    file_size = 16 * 1024 * 1024;
    // Write
    sd_speed_test(buf, 4 * 1024, file_size, true);
    sd_speed_test(buf, 8 * 1024, file_size, true);
    sd_speed_test(buf, 16 * 1024, file_size, true);
    sd_speed_test(buf, 32 * 1024, file_size, true);
    // Read
    sd_speed_test(buf, 4 * 1024, file_size, false);
    sd_speed_test(buf, 8 * 1024, file_size, false);
    sd_speed_test(buf, 16 * 1024, file_size, false);
    sd_speed_test(buf, 32 * 1024, file_size, false);

    free(buf);
}

static uint32_t sd_random_start_sector(uint32_t sector_size)
{
    // Should be a little less than maximum number of sectors on a 512 MiB card
    // (512 Mib - 64 MiB) / sector_size (most probably 512)
    uint32_t upper_bound = (536870912 - 67108864) / sector_size;
    return (esp_random() % (upper_bound + 1));
}

void sdcard_speed_test_raw_run(sdmmc_card_t *card)
{
    size_t sector_count = 128;
    uint32_t sector_size = card->csd.sector_size;
    char *buf = (char *) calloc(1, sector_count * sector_size);
    assert(buf != NULL);
    uint32_t start_sector[3] = {sd_random_start_sector(sector_size), sd_random_start_sector(sector_size), sd_random_start_sector(sector_size)};
    for(uint8_t i = 0; i < 3; i++) {
        test_rw_raw_sd_speed(card, buf, start_sector[i], sector_count*(i+1), sector_size, true);
        test_rw_raw_sd_speed(card, buf, start_sector[i], sector_count*(i+1), sector_size, false);
    }
    free(buf);
}
