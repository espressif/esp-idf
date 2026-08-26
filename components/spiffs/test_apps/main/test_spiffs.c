/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_partition.h"
#include "esp_random.h"
#include "esp_rom_sys.h"

const char* spiffs_test_hello_str = "Hello, World!\n";
const char* spiffs_test_partition_label = "storage";

void app_main(void)
{
    unity_run_menu();
}

static const esp_partition_t *get_partition(void)
{
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_SPIFFS, spiffs_test_partition_label);
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "partition table not set correctly");
    return result;
}

static void test_spiffs_create_file_with_text(const char* name, const char* text)
{
    FILE* f = fopen(name, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs(text, f) != EOF);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

static void test_spiffs_overwrite_append(const char* filename)
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

static void test_spiffs_read_file(const char* filename)
{
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[32] = { 0 };
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(spiffs_test_hello_str), cb);
    TEST_ASSERT_EQUAL(0, strcmp(spiffs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
}

static void test_spiffs_open_max_files(const char* filename_prefix, size_t files_count)
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

static void test_spiffs_lseek(const char* filename)
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

static void test_spiffs_stat(const char* filename)
{
    test_spiffs_create_file_with_text(filename, "foo\n");
    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT(st.st_mode & S_IFREG);
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);
}

static void test_spiffs_unlink(const char* filename)
{
    test_spiffs_create_file_with_text(filename, "unlink\n");

    TEST_ASSERT_EQUAL(0, unlink(filename));

    TEST_ASSERT_NULL(fopen(filename, "r"));
}

static void test_spiffs_rename(const char* filename_prefix)
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

static void test_spiffs_truncate(const char *filename)
{
    int read = 0;
    int truncated_len = 0;

    const char input[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char output[sizeof(input)];

    test_spiffs_create_file_with_text(filename, input);

    // Extending file beyond size is not supported
    TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input) + 1));
    TEST_ASSERT_EQUAL(-1, truncate(filename, -1));

    // Truncating should succeed
    const char truncated_1[] = "ABCDEFGHIJ";
    truncated_len = strlen(truncated_1);
    TEST_ASSERT_EQUAL(0, truncate(filename, truncated_len));


    FILE* f = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(f);
    memset(output, 0, sizeof(output));
    read = fread(output, 1, sizeof(output), f);
    TEST_ASSERT_EQUAL(truncated_len, read);
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_1, output, truncated_len);
    TEST_ASSERT_EQUAL(0, fclose(f));

    // Once truncated, the new file size should be the basis
    // whether truncation should succeed or not
    TEST_ASSERT_EQUAL(-1, truncate(filename, truncated_len + 1));
    TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input)));
    TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input) + 1));
    TEST_ASSERT_EQUAL(-1, truncate(filename, -1));


    // Truncating a truncated file should succeed
    const char truncated_2[] = "ABCDE";
    truncated_len = strlen(truncated_2);
    TEST_ASSERT_EQUAL(0, truncate(filename, truncated_len));

    f = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(f);
    memset(output, 0, sizeof(output));
    read = fread(output, 1, sizeof(output), f);
    TEST_ASSERT_EQUAL(truncated_len, read);
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_2, output, truncated_len);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

static void test_spiffs_ftruncate(const char *filename)
{
    int truncated_len = 0;

    const char input[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char output[sizeof(input)];

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    TEST_ASSERT_EQUAL(strlen(input), write(fd, input, strlen(input)));

    // Extending file beyond size is not supported
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input) + 1));
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));

    // Truncating should succeed
    const char truncated_1[] = "ABCDEFGHIJ";
    truncated_len = strlen(truncated_1);
    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));
    TEST_ASSERT_EQUAL(0, close(fd));

    fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    memset(output, 0, sizeof(output));
    TEST_ASSERT_EQUAL(truncated_len, read(fd, output, sizeof(output)));
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_1, output, truncated_len);
    TEST_ASSERT_EQUAL(0, close(fd));

    // further truncate the file
    fd = open(filename, O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    // Once truncated, the new file size should be the basis
    // whether truncation should succeed or not
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, truncated_len + 1));
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input)));
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input) + 1));
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));

    // Truncating a truncated file should succeed
    const char truncated_2[] = "ABCDE";
    truncated_len = strlen(truncated_2);

    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));
    TEST_ASSERT_EQUAL(0, close(fd));

    // open file for reading and validate the content
    fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    memset(output, 0, sizeof(output));
    TEST_ASSERT_EQUAL(truncated_len, read(fd, output, sizeof(output)));
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_2, output, truncated_len);
    TEST_ASSERT_EQUAL(0, close(fd));
}

static void test_spiffs_fsync(const char *filename)
{
    const char input[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t expected_size = strlen(input);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    ssize_t wr = write(fd, input, expected_size);
    TEST_ASSERT_NOT_EQUAL(-1, wr);

    TEST_ASSERT_EQUAL(0, fsync(fd));
    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr, st.st_size);

    ssize_t wr2 = write(fd, input, expected_size);
    TEST_ASSERT_NOT_EQUAL(-1, wr2);

    TEST_ASSERT_EQUAL(0, fsync(fd));
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr + wr2, st.st_size);

    TEST_ASSERT_EQUAL(0, ftruncate(fd, wr));
    TEST_ASSERT_EQUAL(0, fsync(fd));
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr, st.st_size);

    TEST_ASSERT_EQUAL(0, close(fd));
}

static void test_spiffs_can_opendir(const char* path)
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

static void test_spiffs_opendir_readdir_rewinddir(const char* dir_prefix)
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

static void test_spiffs_readdir_many_files(const char* dir_prefix)
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
                printf("E(w): i=%d, cnt=%d val=0x%" PRIx32 "\n\n", i, cnt, val);
                args->result = ESP_FAIL;
                goto close;
            }
        } else {
            uint32_t rval;
            int cnt = fread(&rval, sizeof(rval), 1, f);
            if (cnt != 1 || rval != val) {
                esp_rom_printf("E(r): i=%d, cnt=%d val=0x%" PRIx32 " rval=0x%" PRIx32 "\n\n", i, cnt, rval);
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

static void test_spiffs_concurrent(const char* filename_prefix)
{
    char names[4][64];
    for (size_t i = 0; i < 4; ++i) {
        snprintf(names[i], sizeof(names[i]), "%s%d", filename_prefix, i + 1);
        unlink(names[i]);
    }

    read_write_test_arg_t args1 = READ_WRITE_TEST_ARG_INIT(names[0], 1);
    read_write_test_arg_t args2 = READ_WRITE_TEST_ARG_INIT(names[1], 2);

    const uint32_t stack_size = 3072;

    printf("writing f1 and f2\n");
    const int cpuid_0 = 0;
    const int cpuid_1 = CONFIG_FREERTOS_NUMBER_OF_CORES - 1;
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, cpuid_1);

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

    xTaskCreatePinnedToCore(&read_write_task, "rw3", stack_size, &args3, 3, NULL, cpuid_1);
    xTaskCreatePinnedToCore(&read_write_task, "rw4", stack_size, &args4, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, cpuid_1);

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


static void test_setup(void)
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = spiffs_test_partition_label,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    TEST_ESP_OK(esp_vfs_spiffs_register(&conf));
}

static void test_teardown(void)
{
    TEST_ESP_OK(esp_vfs_spiffs_unregister(spiffs_test_partition_label));
}

TEST_CASE("can initialize SPIFFS in erased partition", "[spiffs]")
{
    const esp_partition_t* part = get_partition();
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
    const esp_partition_t* part = get_partition();
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
    const esp_partition_t* part = get_partition();
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

TEST_CASE("truncate a file", "[spiffs]")
{
    test_setup();
    test_spiffs_truncate("/spiffs/truncate.txt");
    test_teardown();
}

TEST_CASE("ftruncate a file", "[spiffs]")
{
    test_setup();
    test_spiffs_ftruncate("/spiffs/ftrunc.txt");
    test_teardown();
}

TEST_CASE("fsync works correctly", "[spiffs]")
{
    test_setup();
    test_spiffs_fsync("/spiffs/fsync.txt");
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

TEST_CASE("utime() works well", "[spiffs]")
{
    const char filename[] = "/spiffs/utime.txt";
    struct stat achieved_stat;
    struct tm desired_tm;
    struct utimbuf desired_time = {
        .actime = 0, // access time is not supported
        .modtime = 0,
    };
    time_t false_now = 0;
    memset(&desired_tm, 0, sizeof(struct tm));

    test_setup();
    {
        // Setting up a false actual time - used when the file is created and for modification with the current time
        desired_tm.tm_mon = 10 - 1;
        desired_tm.tm_mday = 31;
        desired_tm.tm_year = 2018 - 1900;
        desired_tm.tm_hour = 10;
        desired_tm.tm_min = 35;
        desired_tm.tm_sec = 23;

        false_now = mktime(&desired_tm);

        struct timeval now = { .tv_sec = false_now };
        settimeofday(&now, NULL);
    }
    test_spiffs_create_file_with_text(filename, "");

    // 00:00:00. January 1st, 1900
    desired_tm.tm_mon = 1 - 1;
    desired_tm.tm_mday = 1;
    desired_tm.tm_year = 0;
    desired_tm.tm_hour = 0;
    desired_tm.tm_min = 0;
    desired_tm.tm_sec = 0;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    TEST_ASSERT_EQUAL(0, utime(filename, &desired_time));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    TEST_ASSERT_EQUAL_UINT32(desired_time.modtime, achieved_stat.st_mtime);

    // 23:59:08. December 31st, 2145
    desired_tm.tm_mon = 12 - 1;
    desired_tm.tm_mday = 31;
    desired_tm.tm_year = 2145 - 1900;
    desired_tm.tm_hour = 23;
    desired_tm.tm_min = 59;
    desired_tm.tm_sec = 8;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    TEST_ASSERT_EQUAL(0, utime(filename, &desired_time));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    TEST_ASSERT_EQUAL_UINT32(desired_time.modtime, achieved_stat.st_mtime);

    // Current time
    TEST_ASSERT_EQUAL(0, utime(filename, NULL));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    printf("Mod. time changed to (false actual time): %s", ctime(&achieved_stat.st_mtime));
    TEST_ASSERT_NOT_EQUAL(desired_time.modtime, achieved_stat.st_mtime);
    TEST_ASSERT(false_now - achieved_stat.st_mtime <= 2); // two seconds of tolerance are given

    test_teardown();
}
#endif // CONFIG_SPIFFS_USE_MTIME

static void test_spiffs_rw_speed(const char* filename, void* buf, size_t buf_size, size_t file_size, bool is_write)
{
    const size_t buf_count = file_size / buf_size;

    FILE* f = fopen(filename, (is_write) ? "wb" : "rb");
    TEST_ASSERT_NOT_NULL(f);

    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    for (size_t n = 0; n < buf_count; ++n) {
        if (is_write) {
            TEST_ASSERT_EQUAL(buf_size, write(fileno(f), buf, buf_size));
        } else {
            if (read(fileno(f), buf, buf_size) != buf_size) {
                printf("reading at n=%d, eof=%d", n, feof(f));
                TEST_FAIL();
            }
        }
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);

    TEST_ASSERT_EQUAL(0, fclose(f));

    float t_s = tv_end.tv_sec - tv_start.tv_sec + 1e-6f * (tv_end.tv_usec - tv_start.tv_usec);
    printf("%s %d bytes (block size %d) in %.3fms (%.3f MB/s)\n",
            (is_write)?"Wrote":"Read", file_size, buf_size, t_s * 1e3,
                    file_size / (1024.0f * 1024.0f * t_s));
}

TEST_CASE("write/read speed test", "[spiffs][timeout=60]")
{
    /* Erase partition before running the test to get consistent results */
    const esp_partition_t* part = get_partition();
    esp_partition_erase_range(part, 0, part->size);

    test_setup();

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
    const size_t file_size = part->size / 2;
    const char* file = "/spiffs/speedtest.bin";

    test_spiffs_rw_speed(file, buf, 4 * 1024, file_size, true);
    TEST_ASSERT_EQUAL(0, unlink(file));
    TEST_ESP_OK(esp_spiffs_gc(spiffs_test_partition_label, file_size));

    test_spiffs_rw_speed(file, buf, 8 * 1024, file_size, true);
    TEST_ASSERT_EQUAL(0, unlink(file));
    TEST_ESP_OK(esp_spiffs_gc(spiffs_test_partition_label, file_size));

    test_spiffs_rw_speed(file, buf, 16 * 1024, file_size, true);

    test_spiffs_rw_speed(file, buf, 4 * 1024, file_size, false);
    test_spiffs_rw_speed(file, buf, 8 * 1024, file_size, false);
    test_spiffs_rw_speed(file, buf, 16 * 1024, file_size, false);
    TEST_ASSERT_EQUAL(0, unlink(file));
    TEST_ESP_OK(esp_spiffs_gc(spiffs_test_partition_label, file_size));

    free(buf);
    test_teardown();
}

TEST_CASE("SPIFFS garbage-collect", "[spiffs][timeout=60]")
{
    // should fail until the partition is initialized
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_spiffs_gc(spiffs_test_partition_label, 4096));

    test_setup();

    // reclaiming one block should be possible
    TEST_ESP_OK(esp_spiffs_gc(spiffs_test_partition_label, 4096));

    // shouldn't be possible to reclaim more than the partition size
    const esp_partition_t* part = get_partition();
    TEST_ESP_ERR(ESP_ERR_NOT_FINISHED, esp_spiffs_gc(spiffs_test_partition_label, part->size * 2));

    test_teardown();
}
