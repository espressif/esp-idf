/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_blockdev.h"
#include "esp_blockdev/memory.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

#define RAM_DISK_SIZE (64 * 1024)

static esp_blockdev_handle_t make_ram_bdl(uint8_t **out_backing)
{
    uint8_t *backing = malloc(RAM_DISK_SIZE);
    TEST_ASSERT_NOT_NULL(backing);
    memset(backing, 0xFF, RAM_DISK_SIZE);
    const esp_blockdev_geometry_t geometry = {
        .disk_size = RAM_DISK_SIZE,
        .read_size = 1,
        .write_size = 1,
        .erase_size = 512,
    };
    esp_blockdev_handle_t bdl = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_blockdev_memory_get_from_buffer(backing, RAM_DISK_SIZE, &geometry, false, &bdl));
    *out_backing = backing;
    return bdl;
}

static void release_ram_bdl(esp_blockdev_handle_t bdl, uint8_t *backing)
{
    bdl->ops->release(bdl);
    free(backing);
}

static void ops_write(esp_vfs_fs_t fs, const char *path, const char *data)
{
    int fd = fs.ops->open_p(fs.ctx, path, O_WRONLY | O_CREAT | O_TRUNC, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(strlen(data), fs.ops->write_p(fs.ctx, fd, data, strlen(data)));
    TEST_ASSERT_EQUAL(0, fs.ops->close_p(fs.ctx, fd));
}

/* Returns false if the file can't be opened; otherwise `buf` holds its NUL-terminated contents */
static bool ops_read(esp_vfs_fs_t fs, const char *path, char *buf, size_t size)
{
    int fd = fs.ops->open_p(fs.ctx, path, O_RDONLY, 0);
    if (fd < 0) {
        return false;
    }
    ssize_t n = fs.ops->read_p(fs.ctx, fd, buf, size - 1);
    TEST_ASSERT_GREATER_OR_EQUAL(0, n);
    buf[n] = '\0';
    TEST_ASSERT_EQUAL(0, fs.ops->close_p(fs.ctx, fd));
    return true;
}

static void posix_expect(const char *path, const char *expected)
{
    char buf[64];
    int fd = open(path, O_RDONLY);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    TEST_ASSERT_GREATER_OR_EQUAL(0, n);
    buf[n] = '\0';
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ASSERT_EQUAL_STRING(expected, buf);
}

TEST_CASE("(instance) init in static memory, use unregistered, then register", "[fatfs][instance]")
{
    static esp_vfs_fat_ctx_t ctx;
    static esp_vfs_fat_file_t files[2];
    uint8_t *backing;
    esp_blockdev_handle_t bdl = make_ram_bdl(&backing);
    esp_vfs_fat_mount_config_t cfg = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    cfg.format_if_mount_failed = true;

    const esp_vfs_fat_resources_t no_slots = { .files = files, .max_files = 0 };
    const esp_vfs_fat_resources_t res = { .files = files, .max_files = 2 };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_vfs_fat_init(&ctx, &no_slots, bdl, &cfg));
    TEST_ESP_OK(esp_vfs_fat_init(&ctx, &res, bdl, &cfg));

    esp_vfs_fs_t fs = esp_vfs_fat_get_fs(&ctx);
    TEST_ASSERT_NOT_NULL(fs.ops);
    TEST_ASSERT_EQUAL(0, fs.flags & ESP_VFS_FLAG_READONLY_FS);

    char buf[64];
    ops_write(fs, "/hello.txt", "direct");
    TEST_ASSERT_TRUE(ops_read(fs, "/hello.txt", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("direct", buf);

    uint64_t total = 0, free_bytes = 0;
    TEST_ESP_OK(esp_vfs_fat_ctx_info(&ctx, &total, &free_bytes));
    TEST_ASSERT_GREATER_THAN(0, total);
    TEST_ASSERT_LESS_THAN(total, free_bytes);

    bool contiguous = false;
    TEST_ESP_OK(esp_vfs_fat_ctx_create_contiguous_file(&ctx, "/cont.bin", 8 * 1024, true));
    TEST_ESP_OK(esp_vfs_fat_ctx_test_contiguous_file(&ctx, "/cont.bin", &contiguous));
    TEST_ASSERT_TRUE(contiguous);

    TEST_ESP_OK(esp_vfs_register_fs_instance("/inst", fs));
    posix_expect("/inst/hello.txt", "direct");
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_fat_unmount("/inst"));
    TEST_ESP_OK(esp_vfs_unregister_fs("/inst"));

    TEST_ESP_OK(esp_vfs_fat_deinit(&ctx));
    release_ram_bdl(bdl, backing);
}

#if FF_USE_DYN_BUFFER
TEST_CASE("(instance) no heap use for file I/O with caller-provided resources", "[fatfs][instance][resources]")
{
    static esp_vfs_fat_ctx_t ctx;
    static esp_vfs_fat_file_t files[2];
    static uint8_t sector_buffers[ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(2, ESP_VFS_FAT_MAX_SECTOR_SIZE)] __attribute__((aligned(4)));
    static uint8_t lfn_buffer[ESP_VFS_FAT_LFN_BUFFER_SIZE + 2] __attribute__((aligned(4)));
    uint8_t *backing;
    esp_blockdev_handle_t bdl = make_ram_bdl(&backing);
    esp_vfs_fat_mount_config_t cfg = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    cfg.format_if_mount_failed = true;

    const size_t sector_size = esp_vfs_fat_get_sector_size(bdl);
    TEST_ASSERT_GREATER_THAN(0, sector_size);
    const esp_vfs_fat_resources_t res = {
        .files = files,
        .max_files = 2,
        .sector_buffers = sector_buffers,
        .sector_buffers_size = ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(2, sector_size),
        .lfn_buffer = lfn_buffer,
        .lfn_buffer_size = ESP_VFS_FAT_LFN_BUFFER_SIZE,
    };

    const size_t heap_before = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    TEST_ESP_OK(esp_vfs_fat_init(&ctx, &res, bdl, &cfg));
    esp_vfs_fs_t fs = esp_vfs_fat_get_fs(&ctx);
    /* Only the instance lock and the volume mutex come from the heap, nothing the size of a buffer */
    const size_t heap_initialized = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    TEST_ASSERT_LESS_THAN(sector_size, heap_before - heap_initialized);

    char buf[64];
    ops_write(fs, "/noheap.txt", "no heap");
    TEST_ASSERT_TRUE(ops_read(fs, "/noheap.txt", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("no heap", buf);
    struct stat st;
    TEST_ASSERT_EQUAL(0, fs.ops->dir->stat_p(fs.ctx, "/noheap.txt", &st));
    TEST_ASSERT_EQUAL(7, st.st_size);
    TEST_ASSERT_EQUAL(0, fs.ops->dir->unlink_p(fs.ctx, "/noheap.txt"));
    TEST_ASSERT_EQUAL(heap_initialized, heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

    TEST_ESP_OK(esp_vfs_fat_deinit(&ctx));
    TEST_ASSERT_EQUAL(heap_before, heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    release_ram_bdl(bdl, backing);
}
#endif

TEST_CASE("(instance) create/destroy and mount/unmount", "[fatfs][instance]")
{
    uint8_t *backing;
    esp_blockdev_handle_t bdl = make_ram_bdl(&backing);
    esp_vfs_fat_mount_config_t cfg = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    cfg.format_if_mount_failed = true;

    esp_vfs_fat_ctx_t *ctx = NULL;
    TEST_ESP_OK(esp_vfs_fat_create(bdl, &cfg, &ctx));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_fat_mount("/m", bdl, &cfg));
    ops_write(esp_vfs_fat_get_fs(ctx), "/a.txt", "heap");
    TEST_ESP_OK(esp_vfs_fat_destroy(ctx));

    TEST_ESP_OK(esp_vfs_fat_mount("/m", bdl, &cfg));
    posix_expect("/m/a.txt", "heap");
    uint64_t total = 0;
    TEST_ESP_OK(esp_vfs_fat_info("/m", &total, NULL));
    TEST_ASSERT_GREATER_THAN(0, total);
    TEST_ESP_OK(esp_vfs_fat_unmount("/m"));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_fat_unmount("/m"));

    release_ram_bdl(bdl, backing);
}
