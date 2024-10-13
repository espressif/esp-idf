/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <stdlib.h>
#ifdef __clang__ // TODO LLVM-330
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_vfs.h"

TEST_CASE("misc - posix_memalign", "[newlib_misc]")
{
    void* outptr = NULL;
    int ret;

    ret = posix_memalign(&outptr, 4, 0);
    TEST_ASSERT_EQUAL_PTR(NULL, outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    void* magic = (void*) 0xEFEFEFEF;
    outptr = magic;
    ret = posix_memalign(&outptr, 0x10000000, 64);  // too big alignment - should fail on all targets
    TEST_ASSERT_EQUAL_INT(ret, ENOMEM);
    TEST_ASSERT_EQUAL_PTR(magic, outptr);  // was not modified

    outptr = magic;
    ret = posix_memalign(&outptr, 16, 0x10000000);  // too big size - should fail on all targets
    TEST_ASSERT_EQUAL_INT(ret, ENOMEM);
    TEST_ASSERT_EQUAL_PTR(magic, outptr);  // was not modified

    outptr = magic;
    ret = posix_memalign(&outptr, 16, 64);
    TEST_ASSERT_TRUE(outptr != magic);
    TEST_ASSERT_NOT_NULL(outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);
    free(outptr);

    outptr = magic;
    heap_caps_malloc_extmem_enable(128);
    ret = posix_memalign(&outptr, 16, 64);
    TEST_ASSERT_TRUE(outptr != magic);
    TEST_ASSERT_NOT_NULL(outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);
    free(outptr);

    outptr = magic;
    heap_caps_malloc_extmem_enable(64);
    ret = posix_memalign(&outptr, 16, 128);
    TEST_ASSERT_TRUE(outptr != magic);
    TEST_ASSERT_NOT_NULL(outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);
    free(outptr);

#if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
}

TEST_CASE("misc - sysconf", "[newlib_misc]")
{
    TEST_ASSERT_NOT_EQUAL(-1, sysconf(_SC_NPROCESSORS_ONLN));
}

TEST_CASE("misc - realpath", "[newlib_misc]")
{
    char out[PATH_MAX];

    TEST_ASSERT_EQUAL_STRING("/", realpath("/", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("//", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath(".", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("./", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/.", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("./.", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("..", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("../..", out));
    TEST_ASSERT_EQUAL_STRING("/a", realpath("/a/", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/a/..", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/a/../..", out));
    TEST_ASSERT_EQUAL_STRING("/c", realpath("/a/../b/../c", out));
    TEST_ASSERT_EQUAL_STRING("/abc/def", realpath("/abc/./def/ghi/..", out));
    char* out_new = realpath("/abc/./def/ghi/..", NULL);
    TEST_ASSERT_NOT_NULL(out_new);
    TEST_ASSERT_EQUAL_STRING("/abc/def", out_new);
    free(out_new);
}

static int s_select_calls = 0;

typedef struct {
    const char **filenames;
    size_t num_files;
    size_t current_index;
} scandir_test_dir_context_t;

static DIR *scandir_test_opendir(void* ctx, const char* name)
{
    scandir_test_dir_context_t *dir_ctx = (scandir_test_dir_context_t *)ctx;
    dir_ctx->current_index = 0;
    static DIR dir = {};
    return &dir;
}

static struct dirent *scandir_test_readdir(void* ctx, DIR* pdir)
{
    scandir_test_dir_context_t *dir_ctx = (scandir_test_dir_context_t *)ctx;
    if (dir_ctx->current_index >= dir_ctx->num_files) {
        return NULL;
    }

    static struct dirent entry;
    snprintf(entry.d_name, sizeof(entry.d_name), "%s", dir_ctx->filenames[dir_ctx->current_index]);
    dir_ctx->current_index++;
    return &entry;
}

static int scandir_test_closedir(void* ctx, DIR* pdir)
{
    return 0;
}

static int scandir_test_select(const struct dirent *entry)
{
    s_select_calls++;
    return strstr(entry->d_name, "test") != NULL;
}

TEST_CASE("file - scandir", "[newlib_misc]")
{
    const char *test_filenames[] = {
        ".",
        "..",
        "test_file1.txt",
        "file2.txt",
        "test_file3.txt",
        "test_file4.txt",
        "test_file5.txt",
        "test_file6.txt",
        "test_file7.txt",
        "test_file8.txt",
        "test_file9.txt",
        "test_file10.txt",
    };
    size_t num_test_files = sizeof(test_filenames) / sizeof(test_filenames[0]);

    scandir_test_dir_context_t scandir_test_dir_ctx = { .filenames = test_filenames, .num_files = num_test_files };

    const esp_vfs_t scandir_test_vfs = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .opendir_p = scandir_test_opendir,
        .readdir_p = scandir_test_readdir,
        .closedir_p = scandir_test_closedir
    };

    TEST_ESP_OK(esp_vfs_register("/data", &scandir_test_vfs, &scandir_test_dir_ctx));

    struct dirent **namelist;
    s_select_calls = 0;
    int n = scandir("/data", &namelist, scandir_test_select, alphasort);
    TEST_ASSERT_NOT_NULL(namelist);
    TEST_ASSERT_GREATER_THAN(0, n);

    TEST_ASSERT_EQUAL(num_test_files, s_select_calls);
    TEST_ASSERT_EQUAL(9, n);

    for (int i = 0; i < n; ++i) {
        TEST_ASSERT_NOT_NULL(strstr(namelist[i]->d_name, "test"));
        free(namelist[i]);
    }
    free(namelist);

    esp_vfs_unregister("/data");
}
