/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/fcntl.h>
#ifdef __clang__ // TODO LLVM-330
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif
#include "esp_vfs.h"
#include "unity.h"
#include "esp_log.h"

/* Dummy VFS implementation to check if VFS is called or not with expected path
 */
typedef struct {
    const char* match_path;
    bool called;
} dummy_vfs_t;

static int dummy_open(void* ctx, const char * path, int flags, int mode)
{
    dummy_vfs_t* dummy = (dummy_vfs_t*) ctx;
    dummy->called = true;
    if (strcmp(dummy->match_path, path) == 0) {
        return 1;
    }
    errno = ENOENT;
    return -1;
}

static int dummy_close(void* ctx, int fd)
{
    dummy_vfs_t* dummy = (dummy_vfs_t*) ctx;
    dummy->called = true;
    if (fd == 1) {
        return 0;
    }
    errno = EBADF;
    return -1;
}

static DIR* dummy_opendir(void* ctx, const char* path)
{
    dummy_vfs_t* dummy = (dummy_vfs_t*) ctx;
    dummy->called = true;
    if (strcmp(dummy->match_path, path) == 0) {
        DIR* result = calloc(1, sizeof(DIR));
        TEST_ASSERT_NOT_NULL(result);
        return result;
    }
    errno = ENOENT;
    return NULL;
}

static int dummy_closedir(void* ctx, DIR* pdir)
{
    dummy_vfs_t* dummy = (dummy_vfs_t*) ctx;
    dummy->called = true;
    free(pdir);
    return 0;
}

/* Initializer for this dummy VFS implementation
 */

#define DUMMY_VFS() { \
        .flags = ESP_VFS_FLAG_CONTEXT_PTR, \
        .open_p = dummy_open, \
        .close_p = dummy_close, \
        .opendir_p = dummy_opendir, \
        .closedir_p = dummy_closedir \
    }

/* Helper functions to test VFS behavior
 */

static void test_open(dummy_vfs_t* instance, const char* path,
        bool should_be_called, bool should_be_opened, int line)
{
    const int flags = O_CREAT | O_TRUNC | O_RDWR;
    instance->called = false;
    int fd = esp_vfs_open(__getreent(), path, flags, 0);
    UNITY_TEST_ASSERT_EQUAL_INT(should_be_called, instance->called, line,
            "should_be_called check failed");
    if (should_be_called) {
        if (should_be_opened) {
            UNITY_TEST_ASSERT(fd >= 0, line, "should be opened");
        } else {
            UNITY_TEST_ASSERT(fd < 0, line, "should not be opened");
        }
    }
    esp_vfs_close(__getreent(), fd);
}

static void test_opendir(dummy_vfs_t* instance, const char* path,
        bool should_be_called, bool should_be_opened, int line)
{
    instance->called = false;
    DIR* dir = opendir(path);
    UNITY_TEST_ASSERT_EQUAL_INT(should_be_called, instance->called, line,
            "should_be_called check failed");
    if (should_be_called) {
        if (should_be_opened) {
            UNITY_TEST_ASSERT(dir != NULL, line, "should be opened");
        } else {
            UNITY_TEST_ASSERT(dir == 0, line, "should not be opened");
        }
    }
    if (dir) {
        closedir(dir);
    }
}

/* Helper macros which forward line number to assertion macros inside test_open
 * and test_opendir
 */

#define test_opened(instance, path) test_open(instance, path, true, true, __LINE__)
#define test_not_opened(instance, path) test_open(instance, path, true, false, __LINE__)
#define test_not_called(instance, path) test_open(instance, path, false, false, __LINE__)

#define test_dir_opened(instance, path) test_opendir(instance, path, true, true, __LINE__)
#define test_dir_not_opened(instance, path) test_opendir(instance, path, true, false, __LINE__)
#define test_dir_not_called(instance, path) test_opendir(instance, path, false, false, __LINE__)



TEST_CASE("vfs parses paths correctly", "[vfs]")
{
    dummy_vfs_t inst_foo = {
            .match_path = "",
            .called = false
    };
    esp_vfs_t desc_foo = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("/foo", &desc_foo, &inst_foo) );

    dummy_vfs_t inst_foo1 = {
        .match_path = "",
        .called = false
    };
    esp_vfs_t desc_foo1 = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("/foo1", &desc_foo1, &inst_foo1) );

    inst_foo.match_path = "/file";
    test_opened(&inst_foo, "/foo/file");
    test_not_opened(&inst_foo, "/foo/file1");
    test_not_called(&inst_foo, "/foo1/file");
    test_not_called(&inst_foo, "/foo1");
    test_not_opened(&inst_foo, "/foo");
    inst_foo.match_path = "/junk";
    test_dir_opened(&inst_foo, "/foo/junk");
    inst_foo.match_path = "/";
    test_dir_opened(&inst_foo, "/foo/");
    test_dir_opened(&inst_foo, "/foo");
    test_dir_not_called(&inst_foo1, "/foo");
    test_dir_not_opened(&inst_foo, "/foo/1");
    test_dir_not_called(&inst_foo, "/foo1");

    inst_foo1.match_path = "/file1";
    test_not_called(&inst_foo1, "/foo/file1");
    test_opened(&inst_foo1, "/foo1/file1");
    test_not_opened(&inst_foo1, "/foo1/file");

    // Test nested VFS entries
    dummy_vfs_t inst_foobar = {
        .match_path = "",
        .called = false
    };
    esp_vfs_t desc_foobar = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("/foo/bar", &desc_foobar, &inst_foobar) );

    dummy_vfs_t inst_toplevel = {
        .match_path = "",
        .called = false
    };
    esp_vfs_t desc_toplevel = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("", &desc_toplevel, &inst_toplevel) );

    inst_foo.match_path = "/bar/file";
    inst_foobar.match_path = "/file";
    test_not_called(&inst_foo, "/foo/bar/file");
    test_opened(&inst_foobar, "/foo/bar/file");
    test_dir_not_called(&inst_foo, "/foo/bar/file");
    test_dir_opened(&inst_foobar, "/foo/bar/file");
    inst_toplevel.match_path = "/tmp/foo";
    test_opened(&inst_toplevel, "/tmp/foo");
    inst_toplevel.match_path = "foo";
    test_opened(&inst_toplevel, "foo");

    TEST_ESP_OK( esp_vfs_unregister("/foo") );
    TEST_ESP_OK( esp_vfs_unregister("/foo1") );
    TEST_ESP_OK( esp_vfs_unregister("/foo/bar") );
    TEST_ESP_OK( esp_vfs_unregister("") );
}

TEST_CASE("vfs unregisters correct nested mount point", "[vfs]")
{
    dummy_vfs_t inst_foobar = {
        .match_path = "/file",
        .called = false
    };
    esp_vfs_t desc_foobar = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("/foo/bar", &desc_foobar, &inst_foobar) );

    dummy_vfs_t inst_foo = {
        .match_path = "/bar/file",
        .called = false
    };
    esp_vfs_t desc_foo = DUMMY_VFS();
    TEST_ESP_OK( esp_vfs_register("/foo", &desc_foo, &inst_foo) );

    /* basic operation */
    test_opened(&inst_foobar, "/foo/bar/file");
    test_not_called(&inst_foo, "/foo/bar/file");

    /* this should not match anything */
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_vfs_unregister("/foo/b"));

    /* unregister "/foo" and check that we haven't unregistered "/foo/bar" */
    TEST_ESP_OK( esp_vfs_unregister("/foo") );
    test_not_called(&inst_foo, "/foo/bar/file");
    test_opened(&inst_foobar, "/foo/bar/file");

    /* repeat the above, with the reverse order of registration */
    TEST_ESP_OK( esp_vfs_unregister("/foo/bar") );
    TEST_ESP_OK( esp_vfs_register("/foo", &desc_foo, &inst_foo) );
    TEST_ESP_OK( esp_vfs_register("/foo/bar", &desc_foobar, &inst_foobar) );
    test_opened(&inst_foobar, "/foo/bar/file");
    test_not_called(&inst_foo, "/foo/bar/file");
    TEST_ESP_OK( esp_vfs_unregister("/foo") );
    test_not_called(&inst_foo, "/foo/bar/file");
    test_opened(&inst_foobar, "/foo/bar/file");
    TEST_ESP_OK( esp_vfs_unregister("/foo/bar") );
}


void test_vfs_register(const char* prefix, bool expect_success, int line)
{
    dummy_vfs_t inst;
    esp_vfs_t desc = DUMMY_VFS();
    esp_err_t err = esp_vfs_register(prefix, &desc, &inst);
    if (expect_success) {
        UNITY_TEST_ASSERT_EQUAL_INT(ESP_OK, err, line, "esp_vfs_register should succeed");
    } else {
        UNITY_TEST_ASSERT_EQUAL_INT(ESP_ERR_INVALID_ARG,
                err, line, "esp_vfs_register should fail");
    }
    if (err == ESP_OK) {
        TEST_ESP_OK( esp_vfs_unregister(prefix) );
    }
}

#define test_register_ok(prefix) test_vfs_register(prefix, true, __LINE__)
#define test_register_fail(prefix) test_vfs_register(prefix, false, __LINE__)

TEST_CASE("vfs checks mount point path", "[vfs]")
{
    test_register_ok("");
    test_register_fail("/");
    test_register_fail("a");
    test_register_fail("aa");
    test_register_fail("aaa");
    test_register_ok("/a");
    test_register_ok("/aa");
    test_register_ok("/aaa/bbb");
    test_register_fail("/aaa/");
    test_register_fail("/aaa/bbb/");
    test_register_ok("/23456789012345");
    test_register_fail("/234567890123456");
}
