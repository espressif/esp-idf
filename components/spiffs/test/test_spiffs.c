// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_partition.h"

const char* spiffs_test_hello_str = "Hello, World!\n";
const char* spiffs_test_partition_label = "flash_test";

void test_spiffs_create_file_with_text(const char* name, const char* text)
{
    FILE* f = fopen(name, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs(text, f) != EOF);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_spiffs_overwrite_append(const char* filename)
{
    /* Create new file with 'aaaa' */
    test_spiffs_create_file_with_text(filename, "aaaa");

    /* Append 'bbbb' to file */
    FILE *f_a = fopen(filename, "a");
    TEST_ASSERT_NOT_NULL(f_a);
    TEST_ASSERT_NOT_EQUAL(EOF, fputs("bbbb", f_a));
    TEST_ASSERT_EQUAL(0, fclose(f_a));

    /* Read back 8 bytes from file, verify it's 'aaaabbbb' */
    char buf[10] = { 0 };
    FILE *f_r = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f_r);
    TEST_ASSERT_EQUAL(8, fread(buf, 1, 8, f_r));
    TEST_ASSERT_EQUAL_STRING_LEN("aaaabbbb", buf, 8);

    /* Be sure we're at end of file */
    TEST_ASSERT_EQUAL(0, fread(buf, 1, 8, f_r));

    TEST_ASSERT_EQUAL(0, fclose(f_r));

    /* Overwrite file with 'cccc' */
    test_spiffs_create_file_with_text(filename, "cccc");

    /* Verify file now only contains 'cccc' */
    f_r = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f_r);
    bzero(buf, sizeof(buf));
    TEST_ASSERT_EQUAL(4, fread(buf, 1, 8, f_r)); // trying to read 8 bytes, only expecting 4
    TEST_ASSERT_EQUAL_STRING_LEN("cccc", buf, 4);
    TEST_ASSERT_EQUAL(0, fclose(f_r));
}

void test_spiffs_read_file(const char* filename)
{
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[32] = { 0 };
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(spiffs_test_hello_str), cb);
    TEST_ASSERT_EQUAL(0, strcmp(spiffs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_spiffs_open_max_files(const char* filename_prefix, size_t files_count)
{
    FILE** files = calloc(files_count, sizeof(FILE*));
    for (size_t i = 0; i < files_count; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "%s_%d.txt", filename_prefix, i);
        files[i] = fopen(name, "w");
        TEST_ASSERT_NOT_NULL(files[i]);
    }
    /* close everything and clean up */
    for (size_t i = 0; i < files_count; ++i) {
        fclose(files[i]);
    }
    free(files);
}

void test_spiffs_lseek(const char* filename)
{
    FILE* f = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(11, fprintf(f, "0123456789\n"));
    TEST_ASSERT_EQUAL(0, fseek(f, -2, SEEK_CUR));
    TEST_ASSERT_EQUAL('9', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 3, SEEK_SET));
    TEST_ASSERT_EQUAL('3', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, -3, SEEK_END));
    TEST_ASSERT_EQUAL('8', fgetc(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    TEST_ASSERT_EQUAL(11, ftell(f));
    TEST_ASSERT_EQUAL(4, fprintf(f, "abc\n"));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    TEST_ASSERT_EQUAL(15, ftell(f));
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_SET));
    char buf[20];
    TEST_ASSERT_EQUAL(15, fread(buf, 1, sizeof(buf), f));
    const char ref_buf[] = "0123456789\nabc\n";
    TEST_ASSERT_EQUAL_INT8_ARRAY(ref_buf, buf, sizeof(ref_buf) - 1);

    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_spiffs_stat(const char* filename)
{
    test_spiffs_create_file_with_text(filename, "foo\n");
    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT(st.st_mode & S_IFREG);
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);
}

void test_spiffs_unlink(const char* filename)
{
    test_spiffs_create_file_with_text(filename, "unlink\n");

    TEST_ASSERT_EQUAL(0, unlink(filename));

    TEST_ASSERT_NULL(fopen(filename, "r"));
}

void test_spiffs_rename(const char* filename_prefix)
{
    char name_dst[64];
    char name_src[64];
    snprintf(name_dst, sizeof(name_dst), "%s_dst.txt", filename_prefix);
    snprintf(name_src, sizeof(name_src), "%s_src.txt", filename_prefix);

    unlink(name_dst);
    unlink(name_src);

    FILE* f = fopen(name_src, "w+");
    TEST_ASSERT_NOT_NULL(f);
    const char* str = "0123456789";
    for (int i = 0; i < 400; ++i) {
        TEST_ASSERT_NOT_EQUAL(EOF, fputs(str, f));
    }
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL(0, rename(name_src, name_dst));
    TEST_ASSERT_NULL(fopen(name_src, "r"));
    FILE* fdst = fopen(name_dst, "r");
    TEST_ASSERT_NOT_NULL(fdst);
    TEST_ASSERT_EQUAL(0, fseek(fdst, 0, SEEK_END));
    TEST_ASSERT_EQUAL(4000, ftell(fdst));
    TEST_ASSERT_EQUAL(0, fclose(fdst));
}

void test_spiffs_can_opendir(const char* path)
{
    char name_dir_file[64];
    const char * file_name = "test_opd.txt";
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/%s", path, file_name);
    unlink(name_dir_file);
    test_spiffs_create_file_with_text(name_dir_file, "test_opendir\n");
    DIR* dir = opendir(path);
    TEST_ASSERT_NOT_NULL(dir);
    bool found = false;
    while (true) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }
        if (strcasecmp(de->d_name, file_name) == 0) {
            found = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL(0, closedir(dir));
    unlink(name_dir_file);
}

void test_spiffs_opendir_readdir_rewinddir(const char* dir_prefix)
{
    char name_dir_inner_file[64];
    char name_dir_inner[64];
    char name_dir_file3[64];
    char name_dir_file2[64];
    char name_dir_file1[64];

    snprintf(name_dir_inner_file, sizeof(name_dir_inner_file), "%s/inner/3.txt", dir_prefix);
    snprintf(name_dir_inner, sizeof(name_dir_inner), "%s/inner", dir_prefix);
    snprintf(name_dir_file3, sizeof(name_dir_file2), "%s/boo.bin", dir_prefix);
    snprintf(name_dir_file2, sizeof(name_dir_file2), "%s/2.txt", dir_prefix);
    snprintf(name_dir_file1, sizeof(name_dir_file1), "%s/1.txt", dir_prefix);

    unlink(name_dir_inner_file);
    rmdir(name_dir_inner);
    unlink(name_dir_file1);
    unlink(name_dir_file2);
    unlink(name_dir_file3);
    rmdir(dir_prefix);

    test_spiffs_create_file_with_text(name_dir_file1, "1\n");
    test_spiffs_create_file_with_text(name_dir_file2, "2\n");
    test_spiffs_create_file_with_text(name_dir_file3, "\01\02\03");
    test_spiffs_create_file_with_text(name_dir_inner_file, "3\n");

    DIR* dir = opendir(dir_prefix);
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
        } else if (strcasecmp(de->d_name, "inner/3.txt") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "inner/3.txt";
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
}

void test_spiffs_readdir_many_files(const char* dir_prefix)
{
    const int n_files = 40;
    const int n_folders = 4;
    unsigned char file_count[n_files * n_folders];
    memset(file_count, 0, sizeof(file_count)/sizeof(file_count[0]));
    char file_name[ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN];

    /* clean stale files before the test */
    DIR* dir = opendir(dir_prefix);
    if (dir) {
        while (true) {
            struct dirent* de = readdir(dir);
            if (!de) {
                break;
            }
            int len = snprintf(file_name, sizeof(file_name), "%s/%s", dir_prefix, de->d_name);
            assert(len < sizeof(file_name));
            unlink(file_name);
        }
    }

    /* create files */
    for (int d = 0; d < n_folders; ++d) {
        printf("filling directory %d\n", d);
        for (int f = 0; f < n_files; ++f) {
            snprintf(file_name, sizeof(file_name), "%s/%d/%d.txt", dir_prefix, d, f);
            test_spiffs_create_file_with_text(file_name, file_name);
        }
    }

    /* list files */
    for (int d = 0; d < n_folders; ++d) {
        printf("listing files in directory %d\n", d);
        snprintf(file_name, sizeof(file_name), "%s/%d", dir_prefix, d);
        dir = opendir(file_name);
        TEST_ASSERT_NOT_NULL(dir);
        while (true) {
            struct dirent* de = readdir(dir);
            if (!de) {
                break;
            }
            int file_id;
            TEST_ASSERT_EQUAL(1, sscanf(de->d_name, "%d.txt", &file_id));
            file_count[file_id + d * n_files]++;
        }
        closedir(dir);
    }

    /* check that all created files have been seen */
    for (int d = 0; d < n_folders; ++d) {
        printf("checking that all files have been found in directory %d\n", d);
        for (int f = 0; f < n_files; ++f) {
            TEST_ASSERT_EQUAL(1, file_count[f + d * n_files]);
        }
    }
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
            .word_count = 4096, \
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
                ets_printf("E(w): i=%d, cnt=%d val=%d\n\n", i, cnt, val);
                args->result = ESP_FAIL;
                goto close;
            }
        } else {
            uint32_t rval;
            int cnt = fread(&rval, sizeof(rval), 1, f);
            if (cnt != 1) {
                ets_printf("E(r): i=%d, cnt=%d rval=%d\n\n", i, cnt, rval);
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

void test_spiffs_concurrent(const char* filename_prefix)
{
    char names[4][64];
    for (size_t i = 0; i < 4; ++i) {
        snprintf(names[i], sizeof(names[i]), "%s%d", filename_prefix, i + 1);
        unlink(names[i]);
    }

    read_write_test_arg_t args1 = READ_WRITE_TEST_ARG_INIT(names[0], 1);
    read_write_test_arg_t args2 = READ_WRITE_TEST_ARG_INIT(names[1], 2);

    printf("writing f1 and f2\n");
    const int cpuid_0 = 0;
    const int cpuid_1 = portNUM_PROCESSORS - 1;
    xTaskCreatePinnedToCore(&read_write_task, "rw1", 2048, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", 2048, &args2, 3, NULL, cpuid_1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);

    args1.write = false;
    args2.write = false;
    read_write_test_arg_t args3 = READ_WRITE_TEST_ARG_INIT(names[2], 3);
    read_write_test_arg_t args4 = READ_WRITE_TEST_ARG_INIT(names[3], 4);

    printf("reading f1 and f2, writing f3 and f4\n");

    xTaskCreatePinnedToCore(&read_write_task, "rw3", 2048, &args3, 3, NULL, cpuid_1);
    xTaskCreatePinnedToCore(&read_write_task, "rw4", 2048, &args4, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw1", 2048, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", 2048, &args2, 3, NULL, cpuid_1);

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

    vSemaphoreDelete(args1.done);
    vSemaphoreDelete(args2.done);
    vSemaphoreDelete(args3.done);
    vSemaphoreDelete(args4.done);
}


static void test_setup()
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = spiffs_test_partition_label,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    TEST_ESP_OK(esp_vfs_spiffs_register(&conf));
}

static void test_teardown()
{
    TEST_ESP_OK(esp_vfs_spiffs_unregister(spiffs_test_partition_label));
}

TEST_CASE("can initialize SPIFFS in erased partition", "[spiffs]")
{
    const esp_partition_t* part = get_test_data_partition();
    TEST_ASSERT_NOT_NULL(part);
    TEST_ESP_OK(esp_partition_erase_range(part, 0, part->size));
    test_setup();
    size_t total = 0, used = 0;
    TEST_ESP_OK(esp_spiffs_info(spiffs_test_partition_label, &total, &used));
    printf("total: %d, used: %d\n", total, used);
    TEST_ASSERT_EQUAL(0, used);
    test_teardown();
}

TEST_CASE("can format mounted partition", "[spiffs]")
{
    // Mount SPIFFS, create file, format, check that the file does not exist.
    const esp_partition_t* part = get_test_data_partition();
    TEST_ASSERT_NOT_NULL(part);
    test_setup();
    const char* filename = "/spiffs/hello.txt";
    test_spiffs_create_file_with_text(filename, spiffs_test_hello_str);
    esp_spiffs_format(part->label);
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NULL(f);
    test_teardown();
}

TEST_CASE("can format unmounted partition", "[spiffs]")
{
    // Mount SPIFFS, create file, unmount. Format. Mount again, check that
    // the file does not exist.
    const esp_partition_t* part = get_test_data_partition();
    TEST_ASSERT_NOT_NULL(part);
    test_setup();
    const char* filename = "/spiffs/hello.txt";
    test_spiffs_create_file_with_text(filename, spiffs_test_hello_str);
    test_teardown();
    esp_spiffs_format(part->label);
    // Don't use test_setup here, need to mount without formatting
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = spiffs_test_partition_label,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    TEST_ESP_OK(esp_vfs_spiffs_register(&conf));
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NULL(f);
    test_teardown();
}

TEST_CASE("can create and write file", "[spiffs]")
{
    test_setup();
    test_spiffs_create_file_with_text("/spiffs/hello.txt", spiffs_test_hello_str);
    test_teardown();
}

TEST_CASE("can read file", "[spiffs]")
{
    test_setup();
    test_spiffs_create_file_with_text("/spiffs/hello.txt", spiffs_test_hello_str);
    test_spiffs_read_file("/spiffs/hello.txt");
    test_teardown();
}

TEST_CASE("can open maximum number of files", "[spiffs]")
{
    size_t max_files = FOPEN_MAX - 3; /* account for stdin, stdout, stderr */
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = spiffs_test_partition_label,
        .format_if_mount_failed = true,
        .max_files = max_files
    };
    TEST_ESP_OK(esp_vfs_spiffs_register(&conf));
    test_spiffs_open_max_files("/spiffs/f", max_files);
    TEST_ESP_OK(esp_vfs_spiffs_unregister(spiffs_test_partition_label));
}

TEST_CASE("overwrite and append file", "[spiffs]")
{
    test_setup();
    test_spiffs_overwrite_append("/spiffs/hello.txt");
    test_teardown();
}

TEST_CASE("can lseek", "[spiffs]")
{
    test_setup();
    test_spiffs_lseek("/spiffs/seek.txt");
    test_teardown();
}


TEST_CASE("stat returns correct values", "[spiffs]")
{
    test_setup();
    test_spiffs_stat("/spiffs/stat.txt");
    test_teardown();
}

TEST_CASE("unlink removes a file", "[spiffs]")
{
    test_setup();
    test_spiffs_unlink("/spiffs/unlink.txt");
    test_teardown();
}

TEST_CASE("rename moves a file", "[spiffs]")
{
    test_setup();
    test_spiffs_rename("/spiffs/move");
    test_teardown();
}

TEST_CASE("can opendir root directory of FS", "[spiffs]")
{
    test_setup();
    test_spiffs_can_opendir("/spiffs");
    test_teardown();
}

TEST_CASE("opendir, readdir, rewinddir, seekdir work as expected", "[spiffs]")
{
    test_setup();
    test_spiffs_opendir_readdir_rewinddir("/spiffs/dir");
    test_teardown();
}

TEST_CASE("readdir with large number of files", "[spiffs][timeout=30]")
{
    test_setup();
    test_spiffs_readdir_many_files("/spiffs/dir2");
    test_teardown();
}

TEST_CASE("multiple tasks can use same volume", "[spiffs]")
{
    test_setup();
    test_spiffs_concurrent("/spiffs/f");
    test_teardown();
}

#ifdef CONFIG_SPIFFS_USE_MTIME
TEST_CASE("mtime is updated when file is opened", "[spiffs]")
{
    /* Open a file, check that mtime is set correctly */
    const char* filename = "/spiffs/time";
    test_setup();
    time_t t_before_create = time(NULL);
    test_spiffs_create_file_with_text(filename, "\n");
    time_t t_after_create = time(NULL);

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    printf("mtime=%d\n", (int) st.st_mtime);
    TEST_ASSERT(st.st_mtime >= t_before_create
             && st.st_mtime <= t_after_create);

    /* Wait a bit, open again, check that mtime is updated */
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    time_t t_before_open = time(NULL);
    FILE *f = fopen(filename, "a");
    time_t t_after_open = time(NULL);
    TEST_ASSERT_EQUAL(0, fstat(fileno(f), &st));
    printf("mtime=%d\n", (int) st.st_mtime);
    TEST_ASSERT(st.st_mtime >= t_before_open
             && st.st_mtime <= t_after_open);
    fclose(f);

    /* Wait a bit, open for reading, check that mtime is not updated */
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    time_t t_before_open_ro = time(NULL);
    f = fopen(filename, "r");
    TEST_ASSERT_EQUAL(0, fstat(fileno(f), &st));
    printf("mtime=%d\n", (int) st.st_mtime);
    TEST_ASSERT(t_before_open_ro > t_after_open
             && st.st_mtime >= t_before_open
             && st.st_mtime <= t_after_open);
    fclose(f);

    test_teardown();
}
#endif // CONFIG_SPIFFS_USE_MTIME
