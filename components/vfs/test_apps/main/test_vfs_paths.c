/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include "esp_vfs.h"
#include "esp_vfs_ops.h"
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

static const esp_vfs_dir_ops_t s_dummy_vfs_dir = {
    .opendir_p = dummy_opendir,
    .closedir_p = dummy_closedir,
};

static const esp_vfs_fs_ops_t s_dummy_vfs = {
    .open_p = dummy_open,
    .close_p = dummy_close,
    .dir = &s_dummy_vfs_dir,
};

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
    TEST_ESP_OK( esp_vfs_register_fs("/foo", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foo) );

    dummy_vfs_t inst_foo1 = {
        .match_path = "",
        .called = false
    };
    TEST_ESP_OK( esp_vfs_register_fs("/foo1", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foo1) );

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
    TEST_ESP_OK( esp_vfs_register_fs("/foo/bar", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foobar) );

    dummy_vfs_t inst_toplevel = {
        .match_path = "",
        .called = false
    };
    TEST_ESP_OK( esp_vfs_register_fs("", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_toplevel) );

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
    TEST_ESP_OK( esp_vfs_register_fs("/foo/bar", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foobar) );

    dummy_vfs_t inst_foo = {
        .match_path = "/bar/file",
        .called = false
    };
    TEST_ESP_OK( esp_vfs_register_fs("/foo", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foo) );

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
    TEST_ESP_OK( esp_vfs_register_fs("/foo", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foo) );
    TEST_ESP_OK( esp_vfs_register_fs("/foo/bar", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst_foobar) );
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
    esp_err_t err = esp_vfs_register_fs(prefix, &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, &inst);
    if (expect_success) {
        UNITY_TEST_ASSERT_EQUAL_INT(ESP_OK, err, line, "esp_vfs_register_fs should succeed");
    } else {
        UNITY_TEST_ASSERT_EQUAL_INT(ESP_ERR_INVALID_ARG,
                err, line, "esp_vfs_register_fs should fail");
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

/* Regression test for a slot-accounting bug in esp_vfs_register_fs_common().
 *
 * The registration code used to keep an ever-increasing counter (s_vfs_count/s_vfs_upper_bound)
 * which was incremented when a VFS was registered into a new top slot, but was
 * never decremented on unregister. The "is there room for another VFS?" check
 * compared that counter against VFS_MAX_COUNT. As a result, after the VFS table
 * had been filled once, repeatedly unregistering and re-registering a VFS would
 * eventually (and permanently) fail with ESP_ERR_NO_MEM even though free slots
 * were available.
 *
 * The bug was fixed by checking for an actually-free slot (esp_get_free_index())
 * instead of relying on the counter, and by lowering the upper bound when the
 * topmost entry is removed.
 *
 * These tests register/unregister the dummy VFS many times to ensure the slot
 * accounting stays correct over time. They are expected to fail on the
 * pre-fix code and pass on the fixed code.
 */

/* Number of VFS slots available for this test app (see sdkconfig.defaults). */
#define TEST_VFS_MAX_COUNT  CONFIG_VFS_MAX_COUNT

/* Build a short, unique mount point ("/t<idx>") for the test VFS entries. */
static void make_test_path(char *buf, size_t buf_len, int idx)
{
    snprintf(buf, buf_len, "/t%d", idx);
}

TEST_CASE("vfs can re-register after the table has been filled", "[vfs]")
{
    /* Separate context per registered VFS, because ESP_VFS_FLAG_CONTEXT_PTR
     * stores the pointer for the lifetime of the registration. */
    static dummy_vfs_t insts[TEST_VFS_MAX_COUNT];
    char paths[TEST_VFS_MAX_COUNT][8];

    /* Fill every free VFS slot. Some slots may already be taken by VFSes that
     * the system registered at startup (e.g. /dev/null), so we keep going until
     * registration reports the table is full instead of assuming a fixed count. */
    int registered = 0;
    for (int i = 0; i < TEST_VFS_MAX_COUNT; ++i) {
        make_test_path(paths[i], sizeof(paths[i]), i);
        insts[i] = (dummy_vfs_t) { .match_path = "", .called = false };
        esp_err_t err = esp_vfs_register_fs(paths[i], &s_dummy_vfs,
                                            ESP_VFS_FLAG_CONTEXT_PTR, &insts[i]);
        if (err == ESP_ERR_NO_MEM) {
            break;  // table is full
        }
        TEST_ESP_OK(err);
        registered++;
    }

    /* We must have been able to register at least one entry, and the table must
     * actually be full now (the next registration must fail with NO_MEM). */
    TEST_ASSERT_GREATER_THAN(0, registered);
    dummy_vfs_t overflow_inst = { .match_path = "", .called = false };
    TEST_ESP_ERR(ESP_ERR_NO_MEM,
                 esp_vfs_register_fs("/overflow", &s_dummy_vfs,
                                     ESP_VFS_FLAG_CONTEXT_PTR, &overflow_inst));

    /* Free the topmost entry we registered, then try to register again.
     * On the buggy code the stale counter would still equal VFS_MAX_COUNT and
     * this registration would incorrectly fail with ESP_ERR_NO_MEM. */
    int top = registered - 1;
    TEST_ESP_OK(esp_vfs_unregister(paths[top]));

    insts[top] = (dummy_vfs_t) { .match_path = "", .called = false };
    TEST_ESP_OK(esp_vfs_register_fs(paths[top], &s_dummy_vfs,
                                    ESP_VFS_FLAG_CONTEXT_PTR, &insts[top]));

    /* Clean up everything we registered so the leak check in tearDown passes. */
    for (int i = 0; i < registered; ++i) {
        TEST_ESP_OK(esp_vfs_unregister(paths[i]));
    }
}

TEST_CASE("vfs can re-register into a hole in the middle of a full table", "[vfs]")
{
    /* This targets the case where an entry that is NOT the topmost one is
     * unregistered while the table is full. That leaves a NULL "hole" in the
     * middle of the s_vfs table and, crucially, does NOT lower s_vfs_upper_bound
     * (the hole is below the upper bound). Registering again must reuse that
     * hole.
     *
     * On the buggy code the stale counter stayed at VFS_MAX_COUNT, so this
     * re-registration failed with ESP_ERR_NO_MEM even though the freed middle
     * slot was available. */
    static dummy_vfs_t insts[TEST_VFS_MAX_COUNT];
    char paths[TEST_VFS_MAX_COUNT][8];

    /* Fill the table completely. */
    int registered = 0;
    for (int i = 0; i < TEST_VFS_MAX_COUNT; ++i) {
        make_test_path(paths[i], sizeof(paths[i]), i);
        insts[i] = (dummy_vfs_t) { .match_path = "", .called = false };
        esp_err_t err = esp_vfs_register_fs(paths[i], &s_dummy_vfs,
                                            ESP_VFS_FLAG_CONTEXT_PTR, &insts[i]);
        if (err == ESP_ERR_NO_MEM) {
            break;  // table is full
        }
        TEST_ESP_OK(err);
        registered++;
    }

    /* We need at least 3 entries so that there is a genuine middle entry that is
     * neither the first nor the topmost slot. */
    TEST_ASSERT_GREATER_OR_EQUAL_INT(3, registered);

    /* The table must be full now. */
    dummy_vfs_t overflow_inst = { .match_path = "", .called = false };
    TEST_ESP_ERR(ESP_ERR_NO_MEM,
                 esp_vfs_register_fs("/overflow", &s_dummy_vfs,
                                     ESP_VFS_FLAG_CONTEXT_PTR, &overflow_inst));

    /* Unregister an entry in the middle (not the first, not the topmost). This
     * creates a NULL hole below the upper bound. */
    int middle = registered / 2;
    TEST_ASSERT_NOT_EQUAL(0, middle);
    TEST_ASSERT_NOT_EQUAL(registered - 1, middle);
    TEST_ESP_OK(esp_vfs_unregister(paths[middle]));

    /* Register again: with only the middle slot free, esp_get_free_index() must
     * return exactly that slot and the registration must succeed. */
    insts[middle] = (dummy_vfs_t) { .match_path = "", .called = false };
    TEST_ESP_OK(esp_vfs_register_fs(paths[middle], &s_dummy_vfs,
                                    ESP_VFS_FLAG_CONTEXT_PTR, &insts[middle]));

    /* The table must be full again - the hole was reused, not appended. */
    TEST_ESP_ERR(ESP_ERR_NO_MEM,
                 esp_vfs_register_fs("/overflow", &s_dummy_vfs,
                                     ESP_VFS_FLAG_CONTEXT_PTR, &overflow_inst));

    /* Clean up everything. */
    for (int i = 0; i < registered; ++i) {
        TEST_ESP_OK(esp_vfs_unregister(paths[i]));
    }
}

TEST_CASE("vfs survives repeated register/unregister cycles", "[vfs]")
{
    /* Fill the whole VFS table and then drain it again, several times over.
     *
     * On the fixed code every round must be able to register exactly the same
     * number of entries, because unregistering lowers the upper bound again.
     * On the buggy code the stale counter never came back down, so the second
     * and later rounds would be able to register fewer entries (and eventually
     * none at all), which this test detects. */
    static dummy_vfs_t insts[TEST_VFS_MAX_COUNT];
    char paths[TEST_VFS_MAX_COUNT][8];
    for (int i = 0; i < TEST_VFS_MAX_COUNT; ++i) {
        make_test_path(paths[i], sizeof(paths[i]), i);
    }

    const int rounds = 5;
    int first_round_count = -1;
    for (int r = 0; r < rounds; ++r) {
        /* Register until the table is full. */
        int count = 0;
        for (int i = 0; i < TEST_VFS_MAX_COUNT; ++i) {
            insts[i] = (dummy_vfs_t) { .match_path = "", .called = false };
            esp_err_t err = esp_vfs_register_fs(paths[i], &s_dummy_vfs,
                                                ESP_VFS_FLAG_CONTEXT_PTR, &insts[i]);
            if (err == ESP_ERR_NO_MEM) {
                break;
            }
            TEST_ESP_OK(err);
            count++;
        }

        if (first_round_count < 0) {
            first_round_count = count;
            TEST_ASSERT_GREATER_THAN(0, first_round_count);
        } else {
            /* The capacity must not shrink between rounds. */
            TEST_ASSERT_EQUAL_INT(first_round_count, count);
        }

        /* Drain the table again. */
        for (int i = 0; i < count; ++i) {
            TEST_ESP_OK(esp_vfs_unregister(paths[i]));
        }
    }
}
