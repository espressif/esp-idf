// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "diskio.h"
#include "ff.h"

static const char* hello_str = "Hello, World!\n";

#define HEAP_SIZE_CAPTURE()  \
    size_t heap_size = esp_get_free_heap_size();

#define HEAP_SIZE_CHECK(tolerance) \
    do {\
        size_t final_heap_size = esp_get_free_heap_size(); \
        if (final_heap_size < heap_size - tolerance) { \
            printf("Initial heap size: %d, final: %d, diff=%d\n", heap_size, final_heap_size, heap_size - final_heap_size); \
        } \
    } while(0)

static void create_file_with_text(const char* name, const char* text)
{
    FILE* f = fopen(name, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs(text, f) != EOF);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

TEST_CASE("can create and write file on sd card", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    create_file_with_text("/sdcard/hello.txt", hello_str);

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("can read file on sd card", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    FILE* f = fopen("/sdcard/hello.txt", "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[32];
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(hello_str), cb);
    TEST_ASSERT_EQUAL(0, strcmp(hello_str, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

static void speed_test(void* buf, size_t buf_size, size_t file_size, bool write)
{
    const size_t buf_count = file_size / buf_size;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = write,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    FILE* f = fopen("/sdcard/4mb.bin", (write) ? "wb" : "rb");
    TEST_ASSERT_NOT_NULL(f);

    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    for (size_t n = 0; n < buf_count; ++n) {
        if (write) {
            TEST_ASSERT_EQUAL(1, fwrite(buf, buf_size, 1, f));
        } else {
            if (fread(buf, buf_size, 1, f) != 1) {
                printf("reading at n=%d, eof=%d", n, feof(f));
                TEST_FAIL();
            }
        }
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);

    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());

    float t_s = tv_end.tv_sec - tv_start.tv_sec + 1e-6f * (tv_end.tv_usec - tv_start.tv_usec);
    printf("%s %d bytes (block size %d) in %.3fms (%.3f MB/s)\n",
            (write)?"Wrote":"Read", file_size, buf_size, t_s * 1e3,
                    (file_size / 1024 / 1024) / t_s);
}


TEST_CASE("read speed test", "[fatfs][ignore]")
{

    HEAP_SIZE_CAPTURE();
    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    const size_t file_size = 4 * 1024 * 1024;
    speed_test(buf, 4 * 1024, file_size, false);
    HEAP_SIZE_CHECK(0);
    speed_test(buf, 8 * 1024, file_size, false);
    HEAP_SIZE_CHECK(0);
    speed_test(buf, 16 * 1024, file_size, false);
    HEAP_SIZE_CHECK(0);
    free(buf);
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("write speed test", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    for (size_t i = 0; i < buf_size / 4; ++i) {
        buf[i] = esp_random();
    }
    const size_t file_size = 4 * 1024 * 1024;

    speed_test(buf, 4 * 1024, file_size, true);
    speed_test(buf, 8 * 1024, file_size, true);
    speed_test(buf, 16 * 1024, file_size, true);

    free(buf);

    HEAP_SIZE_CHECK(0);
}

TEST_CASE("can lseek", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    FILE* f = fopen("/sdcard/seek.txt", "wb+");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(11, fprintf(f, "0123456789\n"));
    TEST_ASSERT_EQUAL(0, fseek(f, -2, SEEK_CUR));
    TEST_ASSERT_EQUAL('9', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 3, SEEK_SET));
    TEST_ASSERT_EQUAL('3', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, -3, SEEK_END));
    TEST_ASSERT_EQUAL('8', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 3, SEEK_END));
    TEST_ASSERT_EQUAL(14, ftell(f));
    TEST_ASSERT_EQUAL(4, fprintf(f, "abc\n"));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    TEST_ASSERT_EQUAL(18, ftell(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_SET));
    char buf[20];
    TEST_ASSERT_EQUAL(18, fread(buf, 1, sizeof(buf), f));
    const char ref_buf[] = "0123456789\n\0\0\0abc\n";
    TEST_ASSERT_EQUAL_INT8_ARRAY(ref_buf, buf, sizeof(ref_buf) - 1);

    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("stat returns correct values", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    struct tm tm;
    tm.tm_year = 2016 - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 10;
    tm.tm_hour = 16;
    tm.tm_min = 30;
    tm.tm_sec = 0;
    time_t t = mktime(&tm);
    printf("Setting time: %s", asctime(&tm));
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    create_file_with_text("/sdcard/stat.txt", "foo\n");

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat("/sdcard/stat.txt", &st));
    time_t mtime = st.st_mtime;
    struct tm mtm;
    localtime_r(&mtime, &mtm);
    printf("File time: %s", asctime(&mtm));
    TEST_ASSERT(abs(mtime - t) < 2);    // fatfs library stores time with 2 second precision

    TEST_ASSERT(st.st_mode & S_IFREG);
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("unlink removes a file", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    create_file_with_text("/sdcard/unlink.txt", "unlink\n");

    TEST_ASSERT_EQUAL(0, unlink("/sdcard/unlink.txt"));

    TEST_ASSERT_NULL(fopen("/sdcard/unlink.txt", "r"));

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("link copies a file, rename moves a file", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    unlink("/sdcard/linkcopy.txt");
    unlink("/sdcard/link_dst.txt");
    unlink("/sdcard/link_src.txt");

    FILE* f = fopen("/sdcard/link_src.txt", "w+");
    TEST_ASSERT_NOT_NULL(f);
    char* str = "0123456789";
    for (int i = 0; i < 4000; ++i) {
        TEST_ASSERT_NOT_EQUAL(EOF, fputs(str, f));
    }
    TEST_ASSERT_EQUAL(0, fclose(f));

    TEST_ASSERT_EQUAL(0, link("/sdcard/link_src.txt", "/sdcard/linkcopy.txt"));

    FILE* fcopy = fopen("/sdcard/linkcopy.txt", "r");
    TEST_ASSERT_NOT_NULL(fcopy);
    TEST_ASSERT_EQUAL(0, fseek(fcopy, 0, SEEK_END));
    TEST_ASSERT_EQUAL(40000, ftell(fcopy));
    TEST_ASSERT_EQUAL(0, fclose(fcopy));

    TEST_ASSERT_EQUAL(0, rename("/sdcard/linkcopy.txt", "/sdcard/link_dst.txt"));
    TEST_ASSERT_NULL(fopen("/sdcard/linkcopy.txt", "r"));
    FILE* fdst = fopen("/sdcard/link_dst.txt", "r");
    TEST_ASSERT_NOT_NULL(fdst);
    TEST_ASSERT_EQUAL(0, fseek(fdst, 0, SEEK_END));
    TEST_ASSERT_EQUAL(40000, ftell(fdst));
    TEST_ASSERT_EQUAL(0, fclose(fdst));

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

typedef struct {
    const char* filename;
    bool write;
    size_t word_count;
    int seed;
    SemaphoreHandle_t done;
    int result;
} read_write_test_arg_t;

#define READ_WRITE_TEST_ARG_INIT(name, seed_) \
        { \
            .filename = name, \
            .seed = seed_, \
            .word_count = 8192, \
            .write = true, \
            .done = xSemaphoreCreateBinary() \
        }

static void read_write_task(void* param)
{
    read_write_test_arg_t* args = (read_write_test_arg_t*) param;
    FILE* f = fopen(args->filename, args->write ? "wb" : "rb");
    if (f == NULL) {
        args->result = ESP_ERR_NOT_FOUND;
        goto done;
    }

    srand(args->seed);
    for (size_t i = 0; i < args->word_count; ++i) {
        uint32_t val = rand();
        if (args->write) {
            int cnt = fwrite(&val, sizeof(val), 1, f);
            if (cnt != 1) {
                args->result = ESP_FAIL;
                goto close;
            }
        } else {
            uint32_t rval;
            int cnt = fread(&rval, sizeof(rval), 1, f);
            if (cnt != 1 || rval != val) {
                ets_printf("E: i=%d, cnt=%d rval=%d val=%d\n\n", i, cnt, rval, val);
                args->result = ESP_FAIL;
                goto close;
            }
        }
    }
    args->result = ESP_OK;

close:
    fclose(f);

done:
    xSemaphoreGive(args->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}


TEST_CASE("multiple tasks can use same volume", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    read_write_test_arg_t args1 = READ_WRITE_TEST_ARG_INIT("/sdcard/f1", 1);
    read_write_test_arg_t args2 = READ_WRITE_TEST_ARG_INIT("/sdcard/f2", 2);

    printf("writing f1 and f2\n");

    xTaskCreatePinnedToCore(&read_write_task, "rw1", 2048, &args1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", 2048, &args2, 3, NULL, 1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);

    args1.write = false;
    args2.write = false;
    read_write_test_arg_t args3 = READ_WRITE_TEST_ARG_INIT("/sdcard/f3", 3);
    read_write_test_arg_t args4 = READ_WRITE_TEST_ARG_INIT("/sdcard/f4", 4);

    printf("reading f1 and f2, writing f3 and f4\n");

    xTaskCreatePinnedToCore(&read_write_task, "rw3", 2048, &args3, 3, NULL, 1);
    xTaskCreatePinnedToCore(&read_write_task, "rw4", 2048, &args4, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw1", 2048, &args1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", 2048, &args2, 3, NULL, 1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);
    xSemaphoreTake(args3.done, portMAX_DELAY);
    printf("f3 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args3.result);
    xSemaphoreTake(args4.done, portMAX_DELAY);
    printf("f4 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args4.result);

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    vSemaphoreDelete(args1.done);
    vSemaphoreDelete(args2.done);
    vSemaphoreDelete(args3.done);
    vSemaphoreDelete(args4.done);
    vTaskDelay(10);
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("can create and remove directories", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    TEST_ASSERT_EQUAL(0, mkdir("/sdcard/dir1", 0755));
    struct stat st;
    TEST_ASSERT_EQUAL(0, stat("/sdcard/dir1", &st));
    TEST_ASSERT_TRUE(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(0, rmdir("/sdcard/dir1"));
    TEST_ASSERT_EQUAL(-1, stat("/sdcard/dir1", &st));

    TEST_ASSERT_EQUAL(0, mkdir("/sdcard/dir2", 0755));
    create_file_with_text("/sdcard/dir2/1.txt", "foo\n");
    TEST_ASSERT_EQUAL(0, stat("/sdcard/dir2", &st));
    TEST_ASSERT_TRUE(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(0, stat("/sdcard/dir2/1.txt", &st));
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);
    TEST_ASSERT_TRUE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(-1, rmdir("/sdcard/dir2"));
    TEST_ASSERT_EQUAL(0, unlink("/sdcard/dir2/1.txt"));
    TEST_ASSERT_EQUAL(0, rmdir("/sdcard/dir2"));

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}

TEST_CASE("opendir, readdir, rewinddir, seekdir work as expected", "[fatfs][ignore]")
{
    HEAP_SIZE_CAPTURE();
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    unlink("/sdcard/dir/inner/3.txt");
    rmdir("/sdcard/dir/inner");
    unlink("/sdcard/dir/2.txt");
    unlink("/sdcard/dir/1.txt");
    unlink("/sdcard/dir/boo.bin");
    rmdir("/sdcard/dir");

    TEST_ASSERT_EQUAL(0, mkdir("/sdcard/dir", 0755));
    create_file_with_text("/sdcard/dir/2.txt", "1\n");
    create_file_with_text("/sdcard/dir/1.txt", "1\n");
    create_file_with_text("/sdcard/dir/boo.bin", "\01\02\03");
    TEST_ASSERT_EQUAL(0, mkdir("/sdcard/dir/inner", 0755));
    create_file_with_text("/sdcard/dir/inner/3.txt", "3\n");

    DIR* dir = opendir("/sdcard/dir");
    TEST_ASSERT_NOT_NULL(dir);
    int count = 0;
    const char* names[4];
    while(count < 4) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }
        printf("found '%s'\n", de->d_name);
        if (strcasecmp(de->d_name, "1.txt") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "1.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "2.txt") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "2.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "inner") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_DIR);
            names[count] = "inner";
            ++count;
        } else if (strcasecmp(de->d_name, "boo.bin") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "boo.bin";
            ++count;
        } else {
            TEST_FAIL_MESSAGE("unexpected directory entry");
        }
    }
    TEST_ASSERT_EQUAL(count, 4);

    rewinddir(dir);
    struct dirent* de = readdir(dir);
    TEST_ASSERT_NOT_NULL(de);
    TEST_ASSERT_EQUAL(0, strcasecmp(de->d_name, names[0]));
    seekdir(dir, 3);
    de = readdir(dir);
    TEST_ASSERT_NOT_NULL(de);
    TEST_ASSERT_EQUAL(0, strcasecmp(de->d_name, names[3]));
    seekdir(dir, 1);
    de = readdir(dir);
    TEST_ASSERT_NOT_NULL(de);
    TEST_ASSERT_EQUAL(0, strcasecmp(de->d_name, names[1]));
    seekdir(dir, 2);
    de = readdir(dir);
    TEST_ASSERT_NOT_NULL(de);
    TEST_ASSERT_EQUAL(0, strcasecmp(de->d_name, names[2]));

    TEST_ASSERT_EQUAL(0, closedir(dir));

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
    HEAP_SIZE_CHECK(0);
}
