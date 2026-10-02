/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cerrno>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#include "esp_blockdev.h"
#include "esp_blockdev/memory.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

#include <catch2/catch_test_macros.hpp>

static esp_blockdev_handle_t make_ram_bdl(uint8_t *backing, size_t size)
{
    memset(backing, 0xFF, size);
    const esp_blockdev_geometry_t geometry = {
        .disk_size = size,
        .read_size = 1,
        .write_size = 1,
        .erase_size = 4096,
        .recommended_write_size = 0,
        .recommended_read_size = 0,
        .recommended_erase_size = 0,
    };
    esp_blockdev_handle_t bdl = ESP_BLOCKDEV_HANDLE_INVALID;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, size, &geometry, false, &bdl) == ESP_OK);
    return bdl;
}

static esp_vfs_fat_mount_config_t format_config(void)
{
    esp_vfs_fat_mount_config_t cfg = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    cfg.format_if_mount_failed = true;
    return cfg;
}

/* Direct calls into an instance, bypassing VFS entirely */
static void ops_write(const esp_vfs_fs_t &fs, const char *path, const std::string &data)
{
    int fd = fs.ops->open_p(fs.ctx, path, O_WRONLY | O_CREAT | O_TRUNC, 0);
    REQUIRE(fd >= 0);
    REQUIRE(fs.ops->write_p(fs.ctx, fd, data.data(), data.size()) == (ssize_t)data.size());
    REQUIRE(fs.ops->close_p(fs.ctx, fd) == 0);
}

static bool ops_read(const esp_vfs_fs_t &fs, const char *path, std::string *out)
{
    int fd = fs.ops->open_p(fs.ctx, path, O_RDONLY, 0);
    if (fd < 0) {
        return false;
    }
    char buf[128] = {};
    ssize_t n = fs.ops->read_p(fs.ctx, fd, buf, sizeof(buf));
    REQUIRE(n >= 0);
    REQUIRE(fs.ops->close_p(fs.ctx, fd) == 0);
    out->assign(buf, n);
    return true;
}

static void posix_write(const char *path, const std::string &data)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    REQUIRE(fd >= 0);
    REQUIRE(write(fd, data.data(), data.size()) == (ssize_t)data.size());
    REQUIRE(close(fd) == 0);
}

static std::string posix_read(const char *path)
{
    int fd = open(path, O_RDONLY);
    REQUIRE(fd >= 0);
    char buf[128] = {};
    ssize_t n = read(fd, buf, sizeof(buf));
    REQUIRE(n >= 0);
    REQUIRE(close(fd) == 0);
    return std::string(buf, n);
}

static esp_vfs_fat_resources_t slots(esp_vfs_fat_file_t *files, size_t max_files)
{
    esp_vfs_fat_resources_t res = {};
    res.files = files;
    res.max_files = max_files;
    return res;
}

static bool contains(const uint8_t *hay, size_t hay_len, const void *needle, size_t needle_len)
{
    return memmem(hay, hay_len, needle, needle_len) != nullptr;
}

#if FF_USE_DYN_BUFFER
TEST_CASE("instance: caller-provided sector and LFN buffers", "[fatfs][instance][resources]")
{
    static uint8_t backing[256 * 1024];
    static esp_vfs_fat_ctx_t ctx;
    static esp_vfs_fat_file_t files[2];
    /* Static on purpose: FATFS freeing any of these would abort the test */
    alignas(4) static uint8_t sector_buffers[ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(2, ESP_VFS_FAT_MAX_SECTOR_SIZE)];
    alignas(4) static uint8_t lfn_buffer[ESP_VFS_FAT_LFN_BUFFER_SIZE + 2];
    esp_blockdev_handle_t bdl = make_ram_bdl(backing, sizeof(backing));
    const esp_vfs_fat_mount_config_t cfg = format_config();

    const size_t sector_size = esp_vfs_fat_get_sector_size(bdl);
    REQUIRE(sector_size > 0);
    const size_t needed = ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(2, sector_size);
    REQUIRE(needed <= sizeof(sector_buffers));

    esp_vfs_fat_resources_t res = slots(files, 2);
    res.sector_buffers = sector_buffers;
    res.sector_buffers_size = needed - 1;
    if (ESP_VFS_FAT_LFN_BUFFER_SIZE > 0) {
        res.lfn_buffer = lfn_buffer;
        res.lfn_buffer_size = ESP_VFS_FAT_LFN_BUFFER_SIZE;
    }
    REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &cfg) == ESP_ERR_INVALID_SIZE);
    res.sector_buffers_size = needed;
    if (ESP_VFS_FAT_LFN_BUFFER_SIZE > 0) {
        esp_vfs_fat_resources_t partial = slots(files, 2);
        partial.sector_buffers = sector_buffers;
        partial.sector_buffers_size = needed;
        REQUIRE(esp_vfs_fat_init(&ctx, &partial, bdl, &cfg) == ESP_ERR_INVALID_ARG);
        res.lfn_buffer = lfn_buffer;
        res.lfn_buffer_size = ESP_VFS_FAT_LFN_BUFFER_SIZE - 2;
        REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &cfg) == ESP_ERR_INVALID_SIZE);
        res.lfn_buffer = lfn_buffer + 1;
        res.lfn_buffer_size = ESP_VFS_FAT_LFN_BUFFER_SIZE;
        REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &cfg) == ESP_ERR_INVALID_ARG);
        res.lfn_buffer = lfn_buffer;
    }

    /* Formatting uses the volume window as its work area */
    memset(sector_buffers, 0, sizeof(sector_buffers));
    REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &cfg) == ESP_OK);
    const esp_vfs_fs_t fs = esp_vfs_fat_get_fs(&ctx);

    /* Unsynced file data sits in the slot's sector buffer */
    const char *marker = "caller-owned-sector-buffer";
    const char *file_name = FF_USE_LFN ? "/a_rather_long_file_name_for_lfn.txt" : "/sfn.txt";
    int fd = fs.ops->open_p(fs.ctx, file_name, O_WRONLY | O_CREAT | O_TRUNC, 0);
    REQUIRE(fd >= 0);
    REQUIRE(fs.ops->write_p(fs.ctx, fd, marker, strlen(marker)) == (ssize_t)strlen(marker));
    const uint8_t *file_bufs = sector_buffers + sector_size;
    REQUIRE(contains(file_bufs, 2 * sector_size, marker, strlen(marker)));
    REQUIRE(fs.ops->close_p(fs.ctx, fd) == 0);

    if (ESP_VFS_FAT_LFN_BUFFER_SIZE > 0) {
        const char16_t utf16[] = u"rather_long";
        REQUIRE(contains(lfn_buffer, ESP_VFS_FAT_LFN_BUFFER_SIZE, utf16, sizeof(utf16) - sizeof(char16_t)));
    }

    /* Buffers stay attached across close/open */
    std::string content;
    REQUIRE(ops_read(fs, file_name, &content));
    REQUIRE(content == marker);
    for (int i = 0; i < 3; i++) {
        ops_write(fs, "/again.txt", "x");
    }

    REQUIRE(esp_vfs_fat_deinit(&ctx) == ESP_OK);

    /* Remount the formatted volume on the same memory */
    esp_vfs_fat_mount_config_t mount_cfg = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &mount_cfg) == ESP_OK);
    REQUIRE(ops_read(esp_vfs_fat_get_fs(&ctx), file_name, &content));
    REQUIRE(content == marker);
    REQUIRE(esp_vfs_fat_deinit(&ctx) == ESP_OK);
    bdl->ops->release(bdl);
}
#endif

TEST_CASE("instance: init in static memory and use without VFS registration", "[fatfs][instance]")
{
    static uint8_t backing[256 * 1024];
    static esp_vfs_fat_ctx_t ctx;
    static esp_vfs_fat_file_t files[2];
    esp_blockdev_handle_t bdl = make_ram_bdl(backing, sizeof(backing));
    const esp_vfs_fat_mount_config_t cfg = format_config();

    const esp_vfs_fat_resources_t no_slots = slots(files, 0);
    const esp_vfs_fat_resources_t res = slots(files, 2);
    REQUIRE(esp_vfs_fat_init(&ctx, &no_slots, bdl, &cfg) == ESP_ERR_INVALID_ARG);
    REQUIRE(esp_vfs_fat_init(&ctx, &res, bdl, &cfg) == ESP_OK);

    const esp_vfs_fs_t fs = esp_vfs_fat_get_fs(&ctx);
    REQUIRE(fs.ops != nullptr);
    REQUIRE((fs.flags & ESP_VFS_FLAG_READONLY_FS) == 0);

    ops_write(fs, "/hello.txt", "direct");
    std::string content;
    REQUIRE(ops_read(fs, "/hello.txt", &content));
    REQUIRE(content == "direct");

    uint64_t total = 0, free_bytes = 0;
    REQUIRE(esp_vfs_fat_ctx_info(&ctx, &total, &free_bytes) == ESP_OK);
    REQUIRE(total > 0);
    REQUIRE(free_bytes < total);

    bool contiguous = false;
    REQUIRE(esp_vfs_fat_ctx_create_contiguous_file(&ctx, "/cont.bin", 16 * 1024, true) == ESP_OK);
    REQUIRE(esp_vfs_fat_ctx_test_contiguous_file(&ctx, "/cont.bin", &contiguous) == ESP_OK);
    REQUIRE(contiguous);

    /* The same instance can then be registered with VFS by the user */
    REQUIRE(esp_vfs_register_fs_instance("/inst", fs) == ESP_OK);
    REQUIRE(posix_read("/inst/hello.txt") == "direct");
    REQUIRE(esp_vfs_unregister_fs("/inst") == ESP_OK);

    REQUIRE(esp_vfs_fat_deinit(&ctx) == ESP_OK);
    bdl->ops->release(bdl);
}

TEST_CASE("instance: create and destroy on the heap", "[fatfs][instance]")
{
    static uint8_t backing[256 * 1024];
    esp_blockdev_handle_t bdl = make_ram_bdl(backing, sizeof(backing));
    const esp_vfs_fat_mount_config_t cfg = format_config();

    esp_vfs_fat_ctx_t *ctx = nullptr;
    REQUIRE(esp_vfs_fat_create(bdl, &cfg, &ctx) == ESP_OK);
    REQUIRE(ctx != nullptr);

    const esp_vfs_fs_t fs = esp_vfs_fat_get_fs(ctx);
    ops_write(fs, "/a.txt", "heap");
    std::string content;
    REQUIRE(ops_read(fs, "/a.txt", &content));
    REQUIRE(content == "heap");

    REQUIRE(esp_vfs_fat_destroy(ctx) == ESP_OK);
    bdl->ops->release(bdl);
}

TEST_CASE("instance: bdl_mount and path-based API use the new layers", "[fatfs][instance]")
{
    static uint8_t backing[256 * 1024];
    esp_blockdev_handle_t bdl = make_ram_bdl(backing, sizeof(backing));
    const esp_vfs_fat_mount_config_t cfg = format_config();

    REQUIRE(esp_vfs_fat_bdl_mount("/bdlm", bdl, &cfg) == ESP_OK);
    REQUIRE(esp_vfs_fat_bdl_mount("/bdlm", bdl, &cfg) == ESP_ERR_INVALID_STATE);
    REQUIRE(esp_vfs_fat_bdl_mount("/bdlm2", bdl, &cfg) == ESP_ERR_INVALID_STATE);
    posix_write("/bdlm/x.txt", "mounted");

    uint64_t total = 0;
    REQUIRE(esp_vfs_fat_info("/bdlm", &total, nullptr) == ESP_OK);
    REQUIRE(total > 0);

    /* Only esp_vfs_fat_register() instances may be torn down with unregister_path */
    REQUIRE(esp_vfs_fat_unregister_path("/bdlm") == ESP_ERR_INVALID_STATE);
    REQUIRE(esp_vfs_fat_bdl_unmount("/bdlm", bdl) == ESP_OK);
    REQUIRE(esp_vfs_fat_bdl_unmount("/bdlm", bdl) == ESP_ERR_INVALID_STATE);

    /* The data survives remounting through the generic mount layer */
    REQUIRE(esp_vfs_fat_mount("/m", bdl, &cfg) == ESP_OK);
    REQUIRE(posix_read("/m/x.txt") == "mounted");
    REQUIRE(esp_vfs_fat_unmount("/m") == ESP_OK);
    REQUIRE(esp_vfs_fat_unmount("/m") == ESP_ERR_INVALID_STATE);
    bdl->ops->release(bdl);
}
