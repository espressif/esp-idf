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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>
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
    TEST_ESP_OK( esp_vfs_unregister("/foo") );
    TEST_ESP_OK( esp_vfs_unregister("/foo1") );
}
