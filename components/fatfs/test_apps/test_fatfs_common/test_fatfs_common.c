/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>
#include "unity.h"
#include "esp_vfs.h"
#include "esp_timer.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_fatfs_common.h"
#include "ff.h"

const char* fatfs_test_hello_str = "Hello, World!\n";
const char* fatfs_test_hello_str_utf = "世界，你好！\n";

void test_fatfs_create_file_with_text(const char* name, const char* text)
{
    FILE* f = fopen(name, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs(text, f) != EOF);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_fatfs_create_file_with_o_creat_flag(const char* filename)
{
    const int fd = open(filename, O_CREAT|O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    const int r = pwrite(fd, fatfs_test_hello_str, strlen(fatfs_test_hello_str), 0); //offset=0
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), r);

    TEST_ASSERT_EQUAL(0, close(fd));
}

void test_fatfs_open_file_with_o_creat_flag(const char* filename)
{
    char buf[32] = { 0 };
    const int fd = open(filename, O_CREAT|O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int r = pread(fd, buf, sizeof(buf), 0); // it is a regular read() with offset==0
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), r);

    TEST_ASSERT_EQUAL(0, close(fd));
}

void test_fatfs_overwrite_append(const char* filename)
{
    /* Create new file with 'aaaa' */
    test_fatfs_create_file_with_text(filename, "aaaa");

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
    test_fatfs_create_file_with_text(filename, "cccc");

    /* Verify file now only contains 'cccc' */
    f_r = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f_r);
    bzero(buf, sizeof(buf));
    TEST_ASSERT_EQUAL(4, fread(buf, 1, 8, f_r)); // trying to read 8 bytes, only expecting 4
    TEST_ASSERT_EQUAL_STRING_LEN("cccc", buf, 4);
    TEST_ASSERT_EQUAL(0, fclose(f_r));
}

void test_fatfs_read_file(const char* filename)
{
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[32] = { 0 };
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), cb);
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_fatfs_read_file_utf_8(const char* filename)
{
    FILE* f = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[64] = { 0 };   //Doubled buffer size to allow for longer UTF-8 strings
    int cb = fread(buf, 1, sizeof(buf), f);
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str_utf), cb);
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str_utf, buf));
    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_fatfs_pread_file(const char* filename)
{
    char buf[32] = { 0 };
    const int fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int r = pread(fd, buf, sizeof(buf), 0); // it is a regular read() with offset==0
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), 1); // offset==1
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str + 1, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str) - 1, r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), 5); // offset==5
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str + 5, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str) - 5, r);

    // regular read() should work now because pread() should not affect the current position in file

    memset(buf, 0, sizeof(buf));
    r = read(fd, buf, sizeof(buf)); // note that this is read() and not pread()
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str), r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), 10); // offset==10
    TEST_ASSERT_EQUAL(0, strcmp(fatfs_test_hello_str + 10, buf));
    TEST_ASSERT_EQUAL(strlen(fatfs_test_hello_str) - 10, r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), strlen(fatfs_test_hello_str) + 1); // offset to EOF
    TEST_ASSERT_EQUAL(0, r);

    TEST_ASSERT_EQUAL(0, close(fd));
}

static void test_pwrite(const char *filename, off_t offset, const char *msg)
{
    const int fd = open(filename, O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    const off_t current_pos = lseek(fd, 0, SEEK_END); // O_APPEND is not the same - jumps to the end only before write()

    const int r = pwrite(fd, msg, strlen(msg), offset);
    TEST_ASSERT_EQUAL(strlen(msg), r);

    TEST_ASSERT_EQUAL(current_pos, lseek(fd, 0, SEEK_CUR)); // pwrite should not move the pointer

    TEST_ASSERT_EQUAL(0, close(fd));
}

static void test_file_content(const char *filename, const char *msg)
{
    char buf[32] = { 0 };
    const int fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int r = read(fd, buf, sizeof(buf));
    TEST_ASSERT_NOT_EQUAL(-1, r);
    TEST_ASSERT_EQUAL(0, strcmp(msg, buf));

    TEST_ASSERT_EQUAL(0, close(fd));
}

void test_fatfs_pwrite_file(const char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(0, close(fd));

    test_pwrite(filename, 0, "Hello");
    test_file_content(filename, "Hello");

    test_pwrite(filename, strlen("Hello"), ", world!");
    test_file_content(filename, "Hello, world!");
    test_pwrite(filename, strlen("Hello, "), "Dolly");
    test_file_content(filename, "Hello, Dolly!");
}

void test_fatfs_open_max_files(const char* filename_prefix, size_t files_count)
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

void test_fatfs_lseek(const char* filename)
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

#ifdef CONFIG_FATFS_USE_FASTSEEK
    f = fopen(filename, "rb+");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    TEST_ASSERT_EQUAL(18, ftell(f));
    TEST_ASSERT_EQUAL(0, fseek(f, -4, SEEK_CUR));
    TEST_ASSERT_EQUAL(14, ftell(f));
    TEST_ASSERT_EQUAL(0, fseek(f, -14, SEEK_CUR));
    TEST_ASSERT_EQUAL(0, ftell(f));

    TEST_ASSERT_EQUAL(18, fread(buf, 1, sizeof(buf), f));
    TEST_ASSERT_EQUAL_INT8_ARRAY(ref_buf, buf, sizeof(ref_buf) - 1);
    TEST_ASSERT_EQUAL(0, fclose(f));
#endif

}

void test_fatfs_truncate_file(const char* filename, bool allow_expanding_files)
{
    int read = 0;
    int truncated_len = 0;

    const char input[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char output[sizeof(input)];

    FILE* f = fopen(filename, "wb");

    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(strlen(input), fprintf(f, input));

    TEST_ASSERT_EQUAL(0, fclose(f));

    struct stat st;
    size_t size;

    stat(filename, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(strlen(input), size);

    if (allow_expanding_files) {
        size_t trunc_add = 2;
        size_t new_size = strlen(input) + trunc_add;
        TEST_ASSERT_EQUAL(0, truncate(filename, new_size));

        stat(filename, &st);
        size = st.st_size;
        TEST_ASSERT_EQUAL(new_size, size);

        f = fopen(filename, "rb");
        TEST_ASSERT_NOT_NULL(f);

        char expanded_output[sizeof(input) + trunc_add];
        memset(expanded_output, 42, sizeof(expanded_output)); // set to something else than 0 (42)

        read = fread(expanded_output, 1, sizeof(input) + trunc_add, f);
        TEST_ASSERT_EQUAL(new_size, read);

        TEST_ASSERT_EQUAL('Z',  expanded_output[strlen(input) - 1]); // 'Z' character
        TEST_ASSERT_EQUAL('\0', expanded_output[sizeof(input) + trunc_add - 3]); // zeroed expanded space
        TEST_ASSERT_EQUAL('\0', expanded_output[sizeof(input) + trunc_add - 2]); // zeroed expanded space
        TEST_ASSERT_EQUAL(42,   expanded_output[sizeof(input) + trunc_add - 1]); // 42 set with memset, end of the array

        TEST_ASSERT_EQUAL(0, fclose(f));
    } else {
        TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input) + 1));
        TEST_ASSERT_EQUAL(errno, EPERM);

        TEST_ASSERT_EQUAL(-1, truncate(filename, -1));
        TEST_ASSERT_EQUAL(errno, EINVAL);
    }

    // Truncating should succeed
    const char truncated_1[] = "ABCDEFGHIJ";
    truncated_len = strlen(truncated_1);

    TEST_ASSERT_EQUAL(0, truncate(filename, truncated_len));

    stat(filename, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(strlen(truncated_1), size);

    f = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(f);

    memset(output, 0, sizeof(output));
    read = fread(output, 1, sizeof(output), f);

    TEST_ASSERT_EQUAL(truncated_len, read);
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_1, output, truncated_len);

    TEST_ASSERT_EQUAL(0, fclose(f));

    if (allow_expanding_files) {
        TEST_ASSERT_EQUAL(0, truncate(filename, truncated_len + 1));
    } else {
        // Once truncated, the new file size should be the basis
        // whether truncation should succeed or not when `allow_expanding_files == false`
        TEST_ASSERT_EQUAL(-1, truncate(filename, truncated_len + 1));
        TEST_ASSERT_EQUAL(EPERM, errno);

        TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input)));
        TEST_ASSERT_EQUAL(EPERM, errno);

        TEST_ASSERT_EQUAL(-1, truncate(filename, strlen(input) + 1));
        TEST_ASSERT_EQUAL(EPERM, errno);
    }

    TEST_ASSERT_EQUAL(-1, truncate(filename, -1));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    // Truncating a truncated file should succeed
    const char truncated_2[] = "ABCDE";
    truncated_len = strlen(truncated_2);

    TEST_ASSERT_EQUAL(0, truncate(filename, truncated_len));

    stat(filename, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(strlen(truncated_2), size);

    f = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(f);

    memset(output, 0, sizeof(output));
    read = fread(output, 1, sizeof(output), f);

    TEST_ASSERT_EQUAL(truncated_len, read);
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_2, output, truncated_len);

    TEST_ASSERT_EQUAL(0, fclose(f));
}

void test_fatfs_ftruncate_file(const char* filename, bool allow_expanding_files)
{
    int truncated_len = 0;

    const char input[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char output[sizeof(input)];

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    TEST_ASSERT_EQUAL(strlen(input), write(fd, input, strlen(input)));

    struct stat st;
    size_t size;

    fstat(fd, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(strlen(input), size);

    if (allow_expanding_files) {
        size_t trunc_add = 2;
        size_t new_size = strlen(input) + trunc_add;
        TEST_ASSERT_EQUAL(0, ftruncate(fd, new_size));

        fstat(fd, &st);
        size = st.st_size;
        TEST_ASSERT_EQUAL(new_size, size);

        char expanded_output[sizeof(input) + trunc_add];
        memset(expanded_output, 42, sizeof(expanded_output)); // set to something else than 0 (42)

        lseek(fd, 0, SEEK_SET);
        int r = read(fd, expanded_output, sizeof(input) + trunc_add);
        TEST_ASSERT_NOT_EQUAL(-1, r);
        TEST_ASSERT_EQUAL(new_size, r);

        TEST_ASSERT_EQUAL('Z',  expanded_output[strlen(input) - 1]); // 'Z' character
        TEST_ASSERT_EQUAL('\0', expanded_output[sizeof(input) + trunc_add - 3]);   // zeroed expanded space
        TEST_ASSERT_EQUAL('\0', expanded_output[sizeof(input) + trunc_add - 2]); // zeroed expanded space
        TEST_ASSERT_EQUAL(42,   expanded_output[sizeof(input) + trunc_add - 1]); // 42 set with memset, end of the array
    } else {
        TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input) + 1));
        TEST_ASSERT_EQUAL(errno, EPERM);

        TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));
        TEST_ASSERT_EQUAL(errno, EINVAL);
    }

    // Truncating should succeed
    const char truncated_1[] = "ABCDEFGHIJ";
    truncated_len = strlen(truncated_1);
    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));

    fstat(fd, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(truncated_len, size);

    TEST_ASSERT_EQUAL(0, close(fd));

    // open file for reading and validate the content
    fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    memset(output, 0, sizeof(output));

    TEST_ASSERT_EQUAL(truncated_len, read(fd, output, sizeof(output)));

    TEST_ASSERT_EQUAL_STRING_LEN(truncated_1, output, truncated_len);

    TEST_ASSERT_EQUAL(0, close(fd));

    // further truncate the file
    fd = open(filename, O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    if (allow_expanding_files) {
        TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len + 1));
    } else {
        // Once truncated, the new file size should be the basis
        // whether truncation should succeed or not when `allow_expanding_files == false`
        TEST_ASSERT_EQUAL(-1, ftruncate(fd, truncated_len + 1));
        TEST_ASSERT_EQUAL(EPERM, errno);

        TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input)));
        TEST_ASSERT_EQUAL(EPERM, errno);

        TEST_ASSERT_EQUAL(-1, ftruncate(fd, strlen(input) + 1));
        TEST_ASSERT_EQUAL(EPERM, errno);

        TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));
        TEST_ASSERT_EQUAL(EINVAL, errno);
    }

    // Truncating a truncated file should succeed
    const char truncated_2[] = "ABCDE";
    truncated_len = strlen(truncated_2);

    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));

    fstat(fd, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(truncated_len, size);

    TEST_ASSERT_EQUAL(0, close(fd));

    // open file for reading and validate the content
    fd = open(filename, O_RDONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    memset(output, 0, sizeof(output));

    TEST_ASSERT_EQUAL(truncated_len, read(fd, output, sizeof(output)));
    TEST_ASSERT_EQUAL_STRING_LEN(truncated_2, output, truncated_len);

    TEST_ASSERT_EQUAL(0, close(fd));
}

void test_fatfs_stat(const char* filename, const char* root_dir)
{
    struct tm tm = {
        .tm_year = 2017 - 1900,
        .tm_mon = 11,
        .tm_mday = 8,
        .tm_hour = 19,
        .tm_min = 51,
        .tm_sec = 10
    };
    time_t t = mktime(&tm);
    printf("Setting time: %s", asctime(&tm));
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    test_fatfs_create_file_with_text(filename, "foo\n");

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    time_t mtime = st.st_mtime;
    struct tm mtm;
    localtime_r(&mtime, &mtm);
    printf("File time: %s", asctime(&mtm));
    TEST_ASSERT(llabs(mtime - t) < 2);    // fatfs library stores time with 2 second precision

    TEST_ASSERT(st.st_mode & S_IFREG);
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);

    memset(&st, 0, sizeof(st));
    TEST_ASSERT_EQUAL(0, stat(root_dir, &st));
    TEST_ASSERT(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);
}

void test_fatfs_size(const char* filename, const char* content) {
    size_t expected_size = strlen(content);

    int fd = open(filename, O_CREAT | O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    ssize_t wr = write(fd, content, expected_size);
    TEST_ASSERT_NOT_EQUAL(-1, wr);

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr, st.st_size);

    ssize_t wr2 = pwrite(fd, content, expected_size, expected_size);
    TEST_ASSERT_NOT_EQUAL(-1, wr2);

    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr + wr2, st.st_size);

    TEST_ASSERT_EQUAL(0, ftruncate(fd, wr));

    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr, st.st_size);

    TEST_ASSERT_EQUAL(0, close(fd));

    wr /= 2;

    TEST_ASSERT_EQUAL(0, truncate(filename, wr));

    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(wr, st.st_size);
}

void test_fatfs_mtime_dst(const char* filename, const char* root_dir)
{
    struct timeval tv = { 1653638041, 0 };
    settimeofday(&tv, NULL);
    setenv("TZ", "MST7MDT,M3.2.0,M11.1.0", 1);
    tzset();

    struct tm tm;
    time_t sys_time = tv.tv_sec;
    localtime_r(&sys_time, &tm);
    printf("Setting time: %s", asctime(&tm));

    test_fatfs_create_file_with_text(filename, "foo\n");

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));

    time_t mtime = st.st_mtime;
    struct tm mtm;
    localtime_r(&mtime, &mtm);
    printf("File time: %s", asctime(&mtm));

    TEST_ASSERT(llabs(mtime - sys_time) < 2);    // fatfs library stores time with 2 second precision

    unsetenv("TZ");
    tzset();
}

void test_fatfs_utime(const char* filename, const char* root_dir)
{
    struct stat achieved_stat;
    struct tm desired_tm;
    struct utimbuf desired_time = {
        .actime = 0, // access time is not supported
        .modtime = 0,
    };
    time_t false_now = 0;
    memset(&desired_tm, 0, sizeof(struct tm));

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
    test_fatfs_create_file_with_text(filename, "");

    // 00:00:00. January 1st, 1980 - FATFS cannot handle earlier dates
    desired_tm.tm_mon = 1 - 1;
    desired_tm.tm_mday = 1;
    desired_tm.tm_year = 1980 - 1900;
    desired_tm.tm_hour = 0;
    desired_tm.tm_min = 0;
    desired_tm.tm_sec = 0;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    TEST_ASSERT_EQUAL(0, utime(filename, &desired_time));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    TEST_ASSERT_EQUAL_UINT32(desired_time.modtime, achieved_stat.st_mtime);

    // current time
    TEST_ASSERT_EQUAL(0, utime(filename, NULL));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    printf("Mod. time changed to (false actual time): %s", ctime(&achieved_stat.st_mtime));
    TEST_ASSERT_NOT_EQUAL(desired_time.modtime, achieved_stat.st_mtime);
    TEST_ASSERT(false_now - achieved_stat.st_mtime <= 2); // two seconds of tolerance are given

    // 23:59:08. December 31st, 2037
    desired_tm.tm_mon = 12 - 1;
    desired_tm.tm_mday = 31;
    desired_tm.tm_year = 2037 - 1900;
    desired_tm.tm_hour = 23;
    desired_tm.tm_min = 59;
    desired_tm.tm_sec = 8;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    TEST_ASSERT_EQUAL(0, utime(filename, &desired_time));
    TEST_ASSERT_EQUAL(0, stat(filename, &achieved_stat));
    TEST_ASSERT_EQUAL_UINT32(desired_time.modtime, achieved_stat.st_mtime);

    //WARNING: it has the Unix Millennium bug (Y2K38)

    // 00:00:00. January 1st, 1970 - FATFS cannot handle years before 1980
    desired_tm.tm_mon = 1 - 1;
    desired_tm.tm_mday = 1;
    desired_tm.tm_year = 1970 - 1900;
    desired_tm.tm_hour = 0;
    desired_tm.tm_min = 0;
    desired_tm.tm_sec = 0;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    TEST_ASSERT_EQUAL(-1, utime(filename, &desired_time));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

void test_fatfs_unlink(const char* filename)
{
    test_fatfs_create_file_with_text(filename, "unlink\n");

    TEST_ASSERT_EQUAL(0, unlink(filename));

    TEST_ASSERT_NULL(fopen(filename, "r"));
}

void test_fatfs_link_rename(const char* filename_prefix)
{
    char name_copy[64];
    char name_dst[64];
    char name_src[64];
    snprintf(name_copy, sizeof(name_copy), "%s_cpy.txt", filename_prefix);
    snprintf(name_dst, sizeof(name_dst), "%s_dst.txt", filename_prefix);
    snprintf(name_src, sizeof(name_src), "%s_src.txt", filename_prefix);

    unlink(name_copy);
    unlink(name_dst);
    unlink(name_src);

    FILE* f = fopen(name_src, "w+");
    TEST_ASSERT_NOT_NULL(f);
    const char* str = "0123456789";
    for (int i = 0; i < 4000; ++i) {
        TEST_ASSERT_NOT_EQUAL(EOF, fputs(str, f));
    }
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL(0, link(name_src, name_copy));
    FILE* fcopy = fopen(name_copy, "r");
    TEST_ASSERT_NOT_NULL(fcopy);
    TEST_ASSERT_EQUAL(0, fseek(fcopy, 0, SEEK_END));
    TEST_ASSERT_EQUAL(40000, ftell(fcopy));
    TEST_ASSERT_EQUAL(0, fclose(fcopy));
    TEST_ASSERT_EQUAL(0, rename(name_copy, name_dst));
    TEST_ASSERT_NULL(fopen(name_copy, "r"));
    FILE* fdst = fopen(name_dst, "r");
    TEST_ASSERT_NOT_NULL(fdst);
    TEST_ASSERT_EQUAL(0, fseek(fdst, 0, SEEK_END));
    TEST_ASSERT_EQUAL(40000, ftell(fdst));
    TEST_ASSERT_EQUAL(0, fclose(fdst));
}

void test_fatfs_mkdir_rmdir(const char* filename_prefix)
{
    char name_dir1[64];
    char name_dir2[64];
    char name_dir2_file[64];
    snprintf(name_dir1, sizeof(name_dir1), "%s1", filename_prefix);
    snprintf(name_dir2, sizeof(name_dir2), "%s2", filename_prefix);
    snprintf(name_dir2_file, sizeof(name_dir2_file), "%s2/1.txt", filename_prefix);

    TEST_ASSERT_EQUAL(0, mkdir(name_dir1, 0755));
    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(name_dir1, &st));
    TEST_ASSERT_TRUE(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(0, rmdir(name_dir1));
    TEST_ASSERT_EQUAL(-1, stat(name_dir1, &st));

    TEST_ASSERT_EQUAL(0, mkdir(name_dir2, 0755));
    test_fatfs_create_file_with_text(name_dir2_file, "foo\n");
    TEST_ASSERT_EQUAL(0, stat(name_dir2, &st));
    TEST_ASSERT_TRUE(st.st_mode & S_IFDIR);
    TEST_ASSERT_FALSE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(0, stat(name_dir2_file, &st));
    TEST_ASSERT_FALSE(st.st_mode & S_IFDIR);
    TEST_ASSERT_TRUE(st.st_mode & S_IFREG);
    TEST_ASSERT_EQUAL(-1, rmdir(name_dir2));
    TEST_ASSERT_EQUAL(0, unlink(name_dir2_file));
    TEST_ASSERT_EQUAL(0, rmdir(name_dir2));
}

void test_fatfs_can_opendir(const char* path)
{
    char name_dir_file[64];
    const char * file_name = "test_opd.txt";
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/%s", path, file_name);
    unlink(name_dir_file);
    test_fatfs_create_file_with_text(name_dir_file, "test_opendir\n");
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

void test_fatfs_readdir_stat(const char* dir_prefix)
{
    char name_dir_file[64];
    char name_dir_stat[64];
    int file_num = 25;

    rmdir(dir_prefix);
    TEST_ASSERT_EQUAL(0, mkdir(dir_prefix, 0755));

    for(int i=0;i<file_num;i++) {
        snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_%d.bin", dir_prefix,i);
        test_fatfs_create_file_with_text(name_dir_file, fatfs_test_hello_str);
    }

    printf("Start counting\n");
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
        snprintf(name_dir_stat, sizeof(dir_prefix)+sizeof(de->d_name), "%s/%s", dir_prefix, de->d_name);
        TEST_ASSERT_EQUAL(0, stat(name_dir_stat, &st));
        dir_size += st.st_size;
    }
    TEST_ASSERT_EQUAL(0, closedir(dir));
    int64_t end = esp_timer_get_time();
    int64_t total_time_readdir = end-start;
    printf("Time in us for calculating directory size by calling readdir first and then stat func:  %lld \n",total_time_readdir);
    printf("Size of the directory %s is %"PRIu32"bytes\n", dir_prefix, dir_size);
    TEST_ASSERT_EQUAL(file_num*14, dir_size); //each file size is 14 bytes

    // Call stat function directly and record the time needed to calculate the directory size
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
    printf("Size of the directory %s is %"PRIu32"bytes\n", dir_prefix, dir_size);
    TEST_ASSERT_EQUAL(file_num*14, dir_size); //each file size is 14 bytes

    for(int i=0;i<file_num;i++) {
        snprintf(name_dir_file, sizeof(name_dir_file), "%s/boo_%d.bin", dir_prefix,i);
        unlink(name_dir_file);
    }
}

void test_fatfs_opendir_readdir_rewinddir(const char* dir_prefix)
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

    TEST_ASSERT_EQUAL(0, mkdir(dir_prefix, 0755));
    test_fatfs_create_file_with_text(name_dir_file1, "1\n");
    test_fatfs_create_file_with_text(name_dir_file2, "2\n");
    test_fatfs_create_file_with_text(name_dir_file3, "\01\02\03");
    TEST_ASSERT_EQUAL(0, mkdir(name_dir_inner, 0755));
    test_fatfs_create_file_with_text(name_dir_inner_file, "3\n");

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
}

void test_fatfs_opendir_readdir_rewinddir_utf_8(const char* dir_prefix)
{
    char name_dir_inner_file[64];
    char name_dir_inner[64];
    char name_dir_file3[64];
    char name_dir_file2[64];
    char name_dir_file1[64];

    snprintf(name_dir_inner_file, sizeof(name_dir_inner_file), "%s/内部目录/内部文件.txt", dir_prefix);
    snprintf(name_dir_inner, sizeof(name_dir_inner), "%s/内部目录", dir_prefix);
    snprintf(name_dir_file3, sizeof(name_dir_file3), "%s/文件三.bin", dir_prefix);
    snprintf(name_dir_file2, sizeof(name_dir_file2), "%s/文件二.txt", dir_prefix);
    snprintf(name_dir_file1, sizeof(name_dir_file1), "%s/文件一.txt", dir_prefix);

    unlink(name_dir_inner_file);
    rmdir(name_dir_inner);
    unlink(name_dir_file1);
    unlink(name_dir_file2);
    unlink(name_dir_file3);
    rmdir(dir_prefix);

    TEST_ASSERT_EQUAL(0, mkdir(dir_prefix, 0755));
    test_fatfs_create_file_with_text(name_dir_file1, "一号\n");
    test_fatfs_create_file_with_text(name_dir_file2, "二号\n");
    test_fatfs_create_file_with_text(name_dir_file3, "\0一\0二\0三");
    TEST_ASSERT_EQUAL(0, mkdir(name_dir_inner, 0755));
    test_fatfs_create_file_with_text(name_dir_inner_file, "三号\n");

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
        if (strcasecmp(de->d_name, "文件一.txt") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "文件一.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "文件二.txt") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "文件二.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "内部目录") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_DIR);
            names[count] = "内部目录";
            ++count;
        } else if (strcasecmp(de->d_name, "文件三.bin") == 0) {
            TEST_ASSERT_TRUE(de->d_type == DT_REG);
            names[count] = "文件三.bin";
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

typedef struct {
    const char* filename;
    bool write;
    size_t word_count;
    unsigned seed;
    SemaphoreHandle_t done;
    esp_err_t result;
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
        unsigned val = rand();
        if (args->write) {
            int cnt = fwrite(&val, sizeof(val), 1, f);
            if (cnt != 1) {
                printf("E(w): i=%d, cnt=%d val=0x08%x\n", i, cnt, val);
                args->result = ESP_FAIL;
                goto close;
            }
        } else {
            unsigned rval;
            int cnt = fread(&rval, sizeof(rval), 1, f);
            if (cnt != 1 || rval != val) {
                printf("E(r): i=%d, cnt=%d rval=0x08%x val=0x08%x\n", i, cnt, rval, val);
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

void test_fatfs_concurrent(const char* filename_prefix)
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
    const int cpuid_1 = CONFIG_FREERTOS_NUMBER_OF_CORES - 1;
    const int stack_size = 4096;
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

void test_leading_spaces(void){
    // fatfs should ignore leading and trailing whitespaces
    // and files "/spiflash/        thelongfile.txt    " and "/spiflash/thelongfile.txt" should be equivalent
    // this feature is currently not implemented
    FILE* f = fopen( "/spiflash/        thelongfile.txt    ", "wb");
    fclose(f);
    TEST_ASSERT_NULL(fopen("/spiflash/thelongfile.txt", "r"));
}

void test_fatfs_rw_speed(const char* filename, void* buf, size_t buf_size, size_t file_size, bool is_write)
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

void test_fatfs_info(const char* base_path, const char* filepath)
{
    // Empty FS
    uint64_t total_bytes = 0;
    uint64_t free_bytes = 0;
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_fat_info(base_path, &total_bytes, &free_bytes));
    ESP_LOGD("fatfs info", "total_bytes=%llu, free_bytes=%llu", total_bytes, free_bytes);
    TEST_ASSERT_NOT_EQUAL(0, total_bytes);

    // FS with a file
    FILE* f = fopen(filepath, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs(fatfs_test_hello_str, f) != EOF);
    TEST_ASSERT_EQUAL(0, fclose(f));

    uint64_t free_bytes_new = 0;
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_fat_info(base_path, &total_bytes, &free_bytes_new));
    ESP_LOGD("fatfs info", "total_bytes=%llu, free_bytes_new=%llu", total_bytes, free_bytes_new);
    TEST_ASSERT_NOT_EQUAL(free_bytes, free_bytes_new);

    // File removed
    TEST_ASSERT_EQUAL(0, remove(filepath));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_fat_info(base_path, &total_bytes, &free_bytes_new));
    ESP_LOGD("fatfs info", "total_bytes=%llu, free_bytes_after_delete=%llu", total_bytes, free_bytes_new);
    TEST_ASSERT_EQUAL(free_bytes, free_bytes_new);
}

#if FF_USE_EXPAND
void test_fatfs_create_contiguous_file(const char* base_path, const char* full_path)
{
    size_t desired_file_size = 64;

    // Don't check for errors, file may not exist at first
    remove(full_path); // esp_vfs_fat_create_contiguous_file will fail if the file already exists

    esp_err_t err = esp_vfs_fat_create_contiguous_file(base_path, full_path, desired_file_size, true);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    struct stat st;
    size_t size;

    stat(full_path, &st);
    size = st.st_size;
    TEST_ASSERT_EQUAL(desired_file_size, size);

    bool is_contiguous = false;
    err = esp_vfs_fat_test_contiguous_file(base_path, full_path, &is_contiguous);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_TRUE(is_contiguous);
}
#endif
