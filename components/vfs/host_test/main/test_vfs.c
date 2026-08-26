/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "esp_vfs.h"
#include "esp_vfs_ops.h"
#include "test_vfs_linux_dev.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(vfs_linux);

/* Forward declaration; defined together with the VFS registration regression
 * tests below. Used by TEST_TEAR_DOWN to clean up leaked registrations. */
static void unregister_all_tracked(void);

TEST_SETUP(vfs_linux)
{
}

TEST_TEAR_DOWN(vfs_linux)
{
    /* Unregister any VFS entries that a test registered but did not clean up
     * itself (e.g. because a TEST_ASSERT_* aborted the test early). This keeps
     * the VFS table clean for subsequent tests. */
    unregister_all_tracked();
}

static void test_create_file_with_text(const char* name, const char* text)
{
    int fd = -1;
    fd = open(name, O_CREAT|O_RDWR, 0777);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    ssize_t sz = write(fd, text, strlen(text));
    TEST_ASSERT_EQUAL(strlen(text), sz);

    TEST_ASSERT_EQUAL(0, close(fd));
}

TEST(vfs_linux,test_linux_vfs_open)
{
    linux_vfs_dev_register();

    const char *test_str = "Espressif";
    const char *filename = "/linux/test.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int len = strlen(test_str);
    char buf[10];
    TEST_ASSERT_EQUAL(len, read(fd, buf, len));
    TEST_ASSERT_EQUAL(0, strncmp(test_str, buf, len));
    close(fd);
    unlink(filename);
    printf("Inside %s\n",__func__);

    linux_vfs_dev_unregister();
}

static void test_lseek(void)
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/lseek.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    off_t off = lseek(fd, 6, SEEK_CUR);
    TEST_ASSERT_EQUAL(6, off);
    off = lseek(fd, 3, SEEK_SET);
    TEST_ASSERT_EQUAL(3, off);
    off = lseek(fd, -9, SEEK_END);
    TEST_ASSERT_EQUAL(2, off);

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_lseek_via_vfs)
{
    printf("Inside %s\n",__func__);
    linux_vfs_dev_register();
    printf("Inside %s\n",__func__);
    test_lseek();
    linux_vfs_dev_unregister();
}

static void test_pread_pwrite(void)
{
    const char *test_str = "0123456789";
    const char *filename = "/linux/pread_pwrite.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test pwrite
    const char *write_str = "ABCD";
    ssize_t written = pwrite(fd, write_str, strlen(write_str), 2);
    TEST_ASSERT_EQUAL(strlen(write_str), written);

    // Test pread
    char read_buf[10] = {0};
    ssize_t bytes_read = pread(fd, read_buf, 4, 2);
    TEST_ASSERT_EQUAL(4, bytes_read);
    TEST_ASSERT_EQUAL(0, strncmp(write_str, read_buf, 4));

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_pread_pwrite_via_vfs)
{
    linux_vfs_dev_register();
    test_pread_pwrite();
    linux_vfs_dev_unregister();
}

static void test_unlink(void)
{
    const char *filename = "/linux/unlink.txt";
    test_create_file_with_text(filename, "test");

    // Test that file exists
    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    close(fd);

    // Test unlink
    TEST_ASSERT_EQUAL(0, unlink(filename));

    // Test that file no longer exists
    fd = open(filename, O_RDWR);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(ENOENT, errno);
}

TEST(vfs_linux, test_unlink_via_vfs)
{
    linux_vfs_dev_register();
    test_unlink();
    linux_vfs_dev_unregister();
}

static void test_fstat(void)
{
    const char *filename = "/linux/fstat.txt";
    test_create_file_with_text(filename, "test");

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    struct stat st;
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_TRUE(S_ISREG(st.st_mode));  // Check if it's a regular file
    TEST_ASSERT_EQUAL(4, st.st_size);       // Check file size

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_fstat_via_vfs)
{
    linux_vfs_dev_register();
    test_fstat();
    linux_vfs_dev_unregister();
}

static void test_fcntl(void)
{
    const char *filename = "/linux/fcntl.txt";
    test_create_file_with_text(filename, "test");

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test F_GETFL
    int flags = fcntl(fd, F_GETFL);
    TEST_ASSERT_NOT_EQUAL(-1, flags);
    TEST_ASSERT_EQUAL(O_RDWR, (flags & O_RDWR));  // Check if file is opened in read-write mode
    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_fcntl_via_vfs)
{
    linux_vfs_dev_register();
    test_fcntl();
    linux_vfs_dev_unregister();
}

static void test_ftruncate(void)
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/ftruncate.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test truncating to a smaller size
    const char truncated_1[] = "01234";
    off_t truncated_len = strlen(truncated_1);
    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));

    // Verify the file size after truncation
    struct stat st;
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(truncated_len, st.st_size);

    // Read back the truncated content
    char buf[32] = {0};
    TEST_ASSERT_EQUAL(truncated_len, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(0, strncmp(truncated_1, buf, truncated_len));

    // Test truncating to a larger size
    off_t new_size = truncated_len + 5;
    TEST_ASSERT_EQUAL(0, ftruncate(fd, new_size));
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(new_size, st.st_size);

    // Test truncating to zero
    TEST_ASSERT_EQUAL(0, ftruncate(fd, 0));
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(0, st.st_size);

    // Test invalid length
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_ftruncate_via_vfs)
{
    linux_vfs_dev_register();
    test_ftruncate();
    linux_vfs_dev_unregister();
}

/* Regression tests for the slot-accounting bug in esp_vfs_register_fs_common().
 *
 * The registration code used to track an ever-increasing counter (s_vfs_count)
 * which was incremented when a VFS was registered into a new top slot but was
 * never decremented on unregister. The "is there room for another VFS?" check
 * compared that counter against VFS_MAX_COUNT. Once the table had been filled
 * once, unregistering and registering again could permanently fail with
 * ESP_ERR_NO_MEM even though free slots were available.
 *
 * The registration code path (esp_vfs_register_fs -> esp_vfs_register_fs_common)
 * is compiled and exercised on the Linux host target, so these tests reproduce
 * the bug here. They are expected to fail on the pre-fix code and pass on the
 * fixed code.
 */

/* Maximum number of VFS slots in this build (CONFIG_VFS_MAX_COUNT, default 8).
 * The upper limit of the Kconfig range is 20, so a loop bound of 20 is always
 * enough to fill the table regardless of configuration. */
#define TEST_VFS_REGISTER_LIMIT 20

/* A minimal VFS used only for registration in the tests below. We never open
 * files through it, only register/unregister it to exercise the VFS slot
 * bookkeeping, so the (context-pointer) open/close operations are never actually
 * invoked. The modern ops API is used to avoid the deprecation warnings that the
 * legacy esp_vfs_t fields carry. */
static int dummy_open(void *ctx, const char *path, int flags, int mode)
{
    (void) ctx; (void) path; (void) flags; (void) mode;
    errno = ENOENT;
    return -1;
}

static int dummy_close(void *ctx, int fd)
{
    (void) ctx; (void) fd;
    return 0;
}

static const esp_vfs_fs_ops_t s_dummy_vfs = {
    .open_p = &dummy_open,
    .close_p = &dummy_close,
};

/* Track every mount point this test file registers, so that TEST_TEAR_DOWN can
 * unregister all of them even if a TEST_ASSERT_* aborts the test early (Unity
 * Fixture uses longjmp on failure, which would otherwise skip in-test cleanup
 * and leave the VFS table in a corrupted state for the next test). */
static char s_tracked_paths[TEST_VFS_REGISTER_LIMIT][8];
static int s_tracked_count;

/* Register the dummy VFS at the given path and record it for cleanup. Returns
 * the esp_vfs_register_fs() result; only successful registrations are tracked. */
static esp_err_t register_tracked(const char *path)
{
    esp_err_t err = esp_vfs_register_fs(path, &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL);
    if (err == ESP_OK) {
        snprintf(s_tracked_paths[s_tracked_count], sizeof(s_tracked_paths[0]), "%s", path);
        s_tracked_count++;
    }
    return err;
}

/* Unregister all still-registered tracked paths. Safe to call repeatedly. */
static void unregister_all_tracked(void)
{
    for (int i = 0; i < s_tracked_count; ++i) {
        esp_vfs_unregister(s_tracked_paths[i]);
    }
    s_tracked_count = 0;
}

/* Build a short, unique mount point ("/t<idx>") for the test VFS entries. */
static void make_test_path(char *buf, size_t buf_len, int idx)
{
    snprintf(buf, buf_len, "/t%d", idx);
}

/* Fill every free VFS slot with the dummy VFS. Other VFSes may already be
 * registered, so we keep going until registration reports the table is full.
 * Returns the number of entries this function registered; paths[] is filled
 * with the corresponding mount points. */
static int fill_vfs_table(char paths[][8])
{
    int registered = 0;
    for (int i = 0; i < TEST_VFS_REGISTER_LIMIT; ++i) {
        make_test_path(paths[i], 8, i);
        esp_err_t err = register_tracked(paths[i]);
        if (err == ESP_ERR_NO_MEM) {
            break;  // table is full
        }
        TEST_ASSERT_EQUAL(ESP_OK, err);
        registered++;
    }
    return registered;
}

TEST(vfs_linux, test_register_after_table_full)
{
    char paths[TEST_VFS_REGISTER_LIMIT][8];

    int registered = fill_vfs_table(paths);

    /* We must have registered at least one entry, and the table must be full
     * now (the next registration must fail with ESP_ERR_NO_MEM). */
    TEST_ASSERT_GREATER_THAN(0, registered);
    TEST_ASSERT_EQUAL(ESP_ERR_NO_MEM,
                      esp_vfs_register_fs("/overflow", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL));

    /* Free the topmost entry, then register again. On the buggy code the stale
     * counter would still equal VFS_MAX_COUNT and this would fail with
     * ESP_ERR_NO_MEM. */
    int top = registered - 1;
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_unregister(paths[top]));
    /* paths[top] is still tracked from fill_vfs_table(), so re-register with the
     * raw API instead of register_tracked() to avoid a duplicate tracker entry. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_register_fs(paths[top], &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL));

    /* Cleanup of registered entries happens in TEST_TEAR_DOWN. */
}

TEST(vfs_linux, test_register_into_middle_hole)
{
    char paths[TEST_VFS_REGISTER_LIMIT][8];

    int registered = fill_vfs_table(paths);

    /* Need at least 3 entries so there is a genuine middle entry that is neither
     * the first nor the topmost slot. */
    TEST_ASSERT_GREATER_OR_EQUAL_INT(3, registered);

    /* The table must be full now. */
    TEST_ASSERT_EQUAL(ESP_ERR_NO_MEM,
                      esp_vfs_register_fs("/overflow", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL));

    /* Unregister an entry in the middle (not the first, not the topmost). This
     * leaves a NULL hole below the upper bound. The path stays tracked, so it is
     * cleaned up in TEST_TEAR_DOWN regardless of what happens below. */
    int middle = registered / 2;
    TEST_ASSERT_NOT_EQUAL(0, middle);
    TEST_ASSERT_NOT_EQUAL(registered - 1, middle);
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_unregister(paths[middle]));

    /* Register again: with only the middle slot free, the registration must
     * reuse that hole and succeed. On the buggy code it failed with
     * ESP_ERR_NO_MEM. (The path is already tracked from fill_vfs_table().) */
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_register_fs(paths[middle], &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL));

    /* The table must be full again - the hole was reused, not appended. */
    TEST_ASSERT_EQUAL(ESP_ERR_NO_MEM,
                      esp_vfs_register_fs("/overflow", &s_dummy_vfs, ESP_VFS_FLAG_CONTEXT_PTR, NULL));

    /* Cleanup of registered entries happens in TEST_TEAR_DOWN. */
}

TEST(vfs_linux, test_register_unregister_cycles)
{
    /* Fill the whole VFS table and then drain it again, several times over.
     *
     * On the fixed code every round must be able to register exactly the same
     * number of entries, because unregistering lowers the upper bound again.
     * On the buggy code the stale counter never came back down, so the second
     * and later rounds would be able to register fewer entries (and eventually
     * none at all), which this test detects. */
    char paths[TEST_VFS_REGISTER_LIMIT][8];

    const int rounds = 5;
    int first_round_count = -1;
    for (int r = 0; r < rounds; ++r) {
        /* Register until the table is full. */
        int count = fill_vfs_table(paths);

        if (first_round_count < 0) {
            first_round_count = count;
            TEST_ASSERT_GREATER_THAN(0, first_round_count);
        } else {
            /* The capacity must not shrink between rounds. */
            TEST_ASSERT_EQUAL_INT(first_round_count, count);
        }

        /* Drain the table again. The paths stay tracked until they are actually
         * unregistered here; TEST_TEAR_DOWN is only the safety net for an early
         * abort. */
        unregister_all_tracked();
    }
}

TEST_GROUP_RUNNER(vfs_linux)
{
    RUN_TEST_CASE(vfs_linux, test_linux_vfs_open);
    RUN_TEST_CASE(vfs_linux, test_lseek_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_pread_pwrite_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_unlink_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_fstat_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_fcntl_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_ftruncate_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_register_after_table_full);
    RUN_TEST_CASE(vfs_linux, test_register_into_middle_hole);
    RUN_TEST_CASE(vfs_linux, test_register_unregister_cycles);
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(vfs_linux);
}

int main(int argc, char **argv)
{
    UNITY_MAIN_FUNC(run_all_tests);
    return 0;
}
