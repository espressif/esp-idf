// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "test_utils.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_fatfs_common.h"
#include "esp_partition.h"
#include "ff.h"


static void test_setup(size_t max_files)
{
    extern const char fatfs_start[] asm("_binary_fatfs_img_start");
    extern const char fatfs_end[]   asm("_binary_fatfs_img_end");
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = max_files
    };
    const esp_partition_t* part = get_test_data_partition();

    TEST_ASSERT(part->size == (fatfs_end - fatfs_start - 1));

    spi_flash_mmap_handle_t mmap_handle;
    const void* mmap_ptr;
    TEST_ESP_OK(esp_partition_mmap(part, 0, part->size, SPI_FLASH_MMAP_DATA, &mmap_ptr, &mmap_handle));
    bool content_valid = memcmp(fatfs_start, mmap_ptr, part->size) == 0;
    spi_flash_munmap(mmap_handle);

    if (!content_valid) {
        printf("Copying fatfs.img into test partition...\n");
        esp_partition_erase_range(part, 0, part->size);
        for (int i = 0; i < part->size; i+= SPI_FLASH_SEC_SIZE) {
            ESP_ERROR_CHECK( esp_partition_write(part, i, fatfs_start + i, SPI_FLASH_SEC_SIZE) );
        }
    }

    TEST_ESP_OK(esp_vfs_fat_rawflash_mount("/spiflash", "flash_test", &mount_config));
}

static void test_teardown(void)
{
    TEST_ESP_OK(esp_vfs_fat_rawflash_unmount("/spiflash","flash_test"));
}

TEST_CASE("(raw) can read file", "[fatfs]")
{
    test_setup(5);
    FILE* f = fopen("/spiflash/hello.txt", "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[32] = { 0 };
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), cb);
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
    test_teardown();
}

TEST_CASE("(raw) can open maximum number of files", "[fatfs]")
{
    size_t max_files = FOPEN_MAX - 3; /* account for stdin, stdout, stderr */
    test_setup(max_files);

    FILE** files = calloc(max_files, sizeof(FILE*));
    for (size_t i = 0; i < max_files; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "/spiflash/f/%d.txt", i + 1);
        files[i] = fopen(name, "r");
        TEST_ASSERT_NOT_NULL(files[i]);
    }
    /* close everything and clean up */
    for (size_t i = 0; i < max_files; ++i) {
        fclose(files[i]);
    }
    free(files);
    test_teardown();

}


TEST_CASE("(raw) can lseek", "[fatfs]")
{
    test_setup(5);
    FILE* f = fopen("/spiflash/hello.txt", "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(0, fseek(f, 2, SEEK_CUR));
    TEST_ASSERT_EQUAL('l', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 4, SEEK_SET));
    TEST_ASSERT_EQUAL('o', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, -5, SEEK_END));
    TEST_ASSERT_EQUAL('r', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 3, SEEK_END));
    TEST_ASSERT_EQUAL(17, ftell(f));

    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    TEST_ASSERT_EQUAL(14, ftell(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_SET));
    test_teardown();
}

TEST_CASE("(raw) stat returns correct values", "[fatfs]")
{
    test_setup(5);
    struct tm tm;
    tm.tm_year = 2018 - 1900;
    tm.tm_mon = 5; // Note: month can be 0-11 & not 1-12
    tm.tm_mday = 13;
    tm.tm_hour = 11;
    tm.tm_min = 2;
    tm.tm_sec = 10;
    time_t t = mktime(&tm);
    printf("Reference time: %s", asctime(&tm));

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat("/spiflash/stat.txt", &st));

    time_t mtime = st.st_mtime;
    struct tm mtm;
    localtime_r(&mtime, &mtm);
    printf("File time: %s", asctime(&mtm));
    TEST_ASSERT(mtime > t);    // Modification time should be in future wrt ref time

    TEST_ASSERT(st.st_mode & S_IFREG);
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);

    memset(&st, 0, sizeof(st));
    TEST_ASSERT_EQUAL(0, stat("/spiflash", &st));
    TEST_ASSERT(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);

    test_teardown();
}



TEST_CASE("(raw) can opendir root directory of FS", "[fatfs]")
{
    test_setup(5);
    DIR* dir = opendir("/spiflash");
    TEST_ASSERT_NOT_NULL(dir);
    bool found = false;
    while (true) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }
        if (strcasecmp(de->d_name, "test_opd.txt") == 0) {
            found = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL(0, closedir(dir));

    test_teardown();
}
TEST_CASE("(raw) opendir, readdir, rewinddir, seekdir work as expected", "[fatfs]")
{
    test_setup(5);

    DIR* dir = opendir("/spiflash/dir");
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

    test_teardown();
}


typedef struct {
    const char* filename;
    size_t word_count;
    int seed;
    int val;
    SemaphoreHandle_t done;
    int result;
} read_test_arg_t;

#define READ_TEST_ARG_INIT(name, seed_, val_) \
        { \
            .filename = name, \
            .seed = seed_, \
            .word_count = 8000, \
            .val = val_, \
            .done = xSemaphoreCreateBinary() \
        }

static void read_task(void* param)
{
    read_test_arg_t* args = (read_test_arg_t*) param;
    FILE* f = fopen(args->filename, "rb");
    if (f == NULL) {
        args->result = ESP_ERR_NOT_FOUND;
        goto done;
    }

    srand(args->seed);
    for (size_t i = 0; i < args->word_count; ++i) {
        uint32_t rval;
        int cnt = fread(&rval, sizeof(rval), 1, f);
        if (cnt != 1 || rval != args->val) {
            ets_printf("E(r): i=%d, cnt=%d rval=%d val=%d\n\n", i, cnt, rval, args->val);
            args->result = ESP_FAIL;
            goto close;
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


TEST_CASE("(raw) multiple tasks can use same volume", "[fatfs]")
{
    test_setup(5);
    char names[4][64];
    for (size_t i = 0; i < 4; ++i) {
        snprintf(names[i], sizeof(names[i]), "/spiflash/ccrnt/%d.txt", i + 1);
    }

    read_test_arg_t args1 = READ_TEST_ARG_INIT(names[0], 1, 0x31313131);
    read_test_arg_t args2 = READ_TEST_ARG_INIT(names[1], 2, 0x32323232);
    read_test_arg_t args3 = READ_TEST_ARG_INIT(names[2], 3, 0x33333333);
    read_test_arg_t args4 = READ_TEST_ARG_INIT(names[3], 4, 0x34343434);

    const int cpuid_0 = 0;
    const int cpuid_1 = portNUM_PROCESSORS - 1;
    const int stack_size = 4096;

    printf("reading files 1.txt 2.txt 3.txt 4.txt \n");

    xTaskCreatePinnedToCore(&read_task, "r1", stack_size, &args1, 3, NULL, cpuid_1);
    xTaskCreatePinnedToCore(&read_task, "r2", stack_size, &args2, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_task, "r3", stack_size, &args3, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_task, "r4", stack_size, &args4, 3, NULL, cpuid_1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("1.txt done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("2.txt done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);
    xSemaphoreTake(args3.done, portMAX_DELAY);
    printf("3.txt done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args3.result);
    xSemaphoreTake(args4.done, portMAX_DELAY);
    printf("4.txt done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args4.result);

    vSemaphoreDelete(args1.done);
    vSemaphoreDelete(args2.done);
    vSemaphoreDelete(args3.done);
    vSemaphoreDelete(args4.done);
    test_teardown();
}

TEST_CASE("(raw) read speed test", "[fatfs][timeout=60]")
{
    test_setup(5);

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    const size_t file_size = 256 * 1024;
    const char* file = "/spiflash/256k.bin";

    test_fatfs_rw_speed(file, buf, 4 * 1024, file_size, false);
    test_fatfs_rw_speed(file, buf, 8 * 1024, file_size, false);
    test_fatfs_rw_speed(file, buf, 16 * 1024, file_size, false);

    free(buf);
    test_teardown();
}
