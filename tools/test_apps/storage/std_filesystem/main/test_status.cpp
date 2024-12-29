/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <filesystem>
#include <cstring>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/dirent.h>
#include "esp_vfs.h"
#include "esp_err.h"
#include <catch2/catch_test_macros.hpp>

/* Helper VFS driver to test std::filesystem */

typedef struct {
    const char* cmp_path;
    int ret_errno;
    struct stat ret_stat;
    DIR out_dir;
    int n_dir_entries;
    struct dirent* ret_dirent_array;
} test_vfs_ctx_t;

static int test_vfs_open(void* ctx, const char* path, int flags, int mode)
{
    test_vfs_ctx_t* vfs_ctx = (test_vfs_ctx_t*)ctx;
    if (strcmp(path, vfs_ctx->cmp_path) != 0) {
        errno = vfs_ctx->ret_errno;
        return -1;
    }
    return 0;
}

static int test_vfs_stat(void* ctx, const char* path, struct stat* st)
{
    test_vfs_ctx_t* vfs_ctx = (test_vfs_ctx_t*)ctx;
    if (strcmp(path, vfs_ctx->cmp_path) != 0) {
        errno = vfs_ctx->ret_errno;
        return -1;
    }
    *st = vfs_ctx->ret_stat;
    return 0;
}

static DIR* test_vfs_opendir(void* ctx, const char* name)
{
    test_vfs_ctx_t* vfs_ctx = (test_vfs_ctx_t*)ctx;
    if (strcmp(name, vfs_ctx->cmp_path) != 0) {
        errno = vfs_ctx->ret_errno;
        return nullptr;
    }
    return &vfs_ctx->out_dir;
}

static struct dirent* test_vfs_readdir(void* ctx, DIR* pdir)
{
    test_vfs_ctx_t* vfs_ctx = (test_vfs_ctx_t*)ctx;
    if (vfs_ctx->ret_errno) {
        errno = vfs_ctx->ret_errno;
        return nullptr;
    }
    if (vfs_ctx->n_dir_entries == 0) {
        return nullptr;
    }
    vfs_ctx->n_dir_entries--;
    struct dirent* ret = &vfs_ctx->ret_dirent_array[0];
    vfs_ctx->ret_dirent_array++;
    return ret;
}

static int test_vfs_closedir(void* ctx, DIR* pdir)
{
    return 0;
}

/* Actual test case starts here */

TEST_CASE("std::filesystem status functions")
{
    test_vfs_ctx_t test_ctx = {};
    esp_vfs_t desc = {};
    desc.flags = ESP_VFS_FLAG_CONTEXT_PTR;
    desc.open_p = test_vfs_open;
    desc.stat_p = test_vfs_stat;
    desc.opendir_p = test_vfs_opendir;
    desc.readdir_p = test_vfs_readdir;
    desc.closedir_p = test_vfs_closedir;

    REQUIRE(esp_vfs_register("/test", &desc, &test_ctx) == ESP_OK);

    SECTION("Test file exists") {
        test_ctx.cmp_path = "/file.txt";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFREG;

        CHECK(std::filesystem::exists("/test/file.txt"));
        CHECK(std::filesystem::is_regular_file("/test/file.txt"));
    }

    SECTION("Test directory exists") {
        test_ctx.cmp_path = "/dir";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFDIR;

        CHECK(std::filesystem::exists("/test/dir"));
        CHECK(std::filesystem::is_directory("/test/dir"));
    }

    SECTION("Test non-existent file") {
        test_ctx.cmp_path = "";
        test_ctx.ret_errno = ENOENT;

        CHECK(!std::filesystem::exists("/test/nonexistent"));
    }

    SECTION("Test is_character_file") {
        test_ctx.cmp_path = "/chardev";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFCHR;

        CHECK(std::filesystem::exists("/test/chardev"));
        CHECK(std::filesystem::is_character_file("/test/chardev"));
    }

    SECTION("Test is_block_file") {
        test_ctx.cmp_path = "/blockdev";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFBLK;

        CHECK(std::filesystem::exists("/test/blockdev"));
        CHECK(std::filesystem::is_block_file("/test/blockdev"));
    }

    SECTION("Test is_fifo") {
        test_ctx.cmp_path = "/fifo";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFIFO;

        CHECK(std::filesystem::exists("/test/fifo"));
        CHECK(std::filesystem::is_fifo("/test/fifo"));
    }

    SECTION("Test is_socket") {
        test_ctx.cmp_path = "/socket";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFSOCK;

        CHECK(std::filesystem::exists("/test/socket"));
        CHECK(std::filesystem::is_socket("/test/socket"));
    }

    SECTION("Test is_symlink") {
        test_ctx.cmp_path = "/symlink";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFLNK;

        CHECK(std::filesystem::exists("/test/symlink"));
        CHECK(std::filesystem::is_symlink("/test/symlink"));
    }

    SECTION("Test is_empty with file") {
        test_ctx.cmp_path = "/file.txt";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFREG;
        test_ctx.ret_stat.st_size = 10;

        CHECK(!std::filesystem::is_empty("/test/file.txt"));

        test_ctx.ret_stat.st_size = 0;
        CHECK(std::filesystem::is_empty("/test/file.txt"));
    }

    SECTION("Test is_empty with directory") {
        test_ctx.cmp_path = "/dir";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFDIR;

        CHECK(std::filesystem::is_empty("/test/dir"));
    }

    SECTION("Test is_empty with non-empty directory") {
        test_ctx.cmp_path = "/dir";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFDIR;
        test_ctx.n_dir_entries = 2;
        struct dirent entries[2] = {
            { .d_ino = 0, .d_type = DT_REG, .d_name = "foo" },
            { .d_ino = 0, .d_type = DT_REG, .d_name = "bar" },
        };
        test_ctx.ret_dirent_array = entries;

        CHECK(!std::filesystem::is_empty("/test/dir"));
    }

    SECTION("directory_iterator, empty directory") {
        test_ctx.cmp_path = "/dir";
        test_ctx.ret_stat = {};
        test_ctx.ret_stat.st_mode = S_IFDIR;
        test_ctx.n_dir_entries = 0;
        struct dirent entries[2] = {
            { .d_ino = 0, .d_type = DT_REG, .d_name = "." },
            { .d_ino = 0, .d_type = DT_REG, .d_name = ".." },
        };
        test_ctx.ret_dirent_array = entries;

        CHECK(std::filesystem::directory_iterator("/test/dir") == std::filesystem::directory_iterator{});
    }

    CHECK(esp_vfs_unregister("/test") == ESP_OK);
}
