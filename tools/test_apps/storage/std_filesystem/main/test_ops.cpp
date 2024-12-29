/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstddef>
#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include "esp_vfs_fat.h"
#include "wear_levelling.h"

class OpsTest {
private:
    wl_handle_t m_wl_handle;
public:
    OpsTest()
    {
        esp_vfs_fat_mount_config_t mount_config = VFS_FAT_MOUNT_DEFAULT_CONFIG();

        esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl("/test", "storage", &mount_config, &m_wl_handle);
        if (err != ESP_OK) {
            throw std::runtime_error("Failed to mount FAT filesystem");
        }
    }
    ~OpsTest()
    {
        esp_vfs_fat_spiflash_unmount_rw_wl("/test", m_wl_handle);
    }

    void test_create_remove()
    {
        std::filesystem::create_directory("/test/dir");
        CHECK(std::filesystem::exists("/test/dir"));

        CHECK(std::filesystem::remove("/test/dir"));
        CHECK(!std::filesystem::exists("/test/dir"));
    }

    /*
        The following two tests rely on the following directory structure
        in the generated FAT filesystem:

        /test
        └── test_dir_iter
            ├── dir1
            │   └── f1
            └── dir2
                └── dir3
                    └── f3
    */
    void test_directory_iterator()
    {
        std::filesystem::directory_iterator it("/test/test_dir_iter");
        CHECK(it != std::filesystem::directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir1");
        CHECK(it->is_directory());
        ++it;
        CHECK(it != std::filesystem::directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir2");
        CHECK(it->is_directory());
        ++it;
        CHECK(it == std::filesystem::directory_iterator());
    }

    void test_recursive_directory_iterator()
    {
        std::filesystem::recursive_directory_iterator it("/test/test_dir_iter");
        CHECK(it != std::filesystem::recursive_directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir1");
        CHECK(it->is_directory());
        ++it;
        CHECK(it != std::filesystem::recursive_directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir1/f1");
        CHECK(it->is_regular_file());
        ++it;
        CHECK(it != std::filesystem::recursive_directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir2");
        CHECK(it->is_directory());
        ++it;
        CHECK(it != std::filesystem::recursive_directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir2/dir3");
        CHECK(it->is_directory());
        ++it;
        CHECK(it != std::filesystem::recursive_directory_iterator());
        CHECK(it->path() == "/test/test_dir_iter/dir2/dir3/f3");
        CHECK(it->is_regular_file());
        ++it;
        CHECK(it == std::filesystem::recursive_directory_iterator());
    }

    void test_copy_remove_recursive_copy()
    {
        if (std::filesystem::exists("/test/copy_dir")) {
            CHECK(std::filesystem::remove_all("/test/copy_dir"));
        }

        CHECK(std::filesystem::create_directory("/test/copy_dir"));
        REQUIRE_NOTHROW(std::filesystem::copy("/test/test_dir_iter/dir1/f1", "/test/copy_dir/f1"));
        CHECK(std::filesystem::exists("/test/copy_dir/f1"));
        CHECK(std::filesystem::remove("/test/copy_dir/f1"));
        CHECK(std::filesystem::remove("/test/copy_dir"));

        REQUIRE_NOTHROW(std::filesystem::copy("/test/test_dir_iter", "/test/copy_dir", std::filesystem::copy_options::recursive));
        CHECK(std::filesystem::exists("/test/copy_dir/dir1/f1"));
        CHECK(std::filesystem::exists("/test/copy_dir/dir2/dir3/f3"));
        CHECK(std::filesystem::remove_all("/test/copy_dir"));
    }

    void test_create_directories()
    {
        if (std::filesystem::exists("/test/create_dir")) {
            CHECK(std::filesystem::remove_all("/test/create_dir"));
        }
        CHECK(std::filesystem::create_directories("/test/create_dir/dir1/dir2"));
        CHECK(std::filesystem::exists("/test/create_dir/dir1/dir2"));
        CHECK(std::filesystem::remove_all("/test/create_dir"));
    }

    void test_rename_file()
    {
        if (std::filesystem::exists("/test/rename_file")) {
            CHECK(std::filesystem::remove("/test/rename_file"));
        }
        std::filesystem::create_directory("/test/rename_file");
        std::filesystem::copy_file("/test/file", "/test/rename_file/file");
        CHECK(std::filesystem::exists("/test/rename_file/file"));
        std::filesystem::rename("/test/rename_file/file", "/test/rename_file/file2");
        CHECK(std::filesystem::exists("/test/rename_file/file2"));
        CHECK(std::filesystem::remove_all("/test/rename_file"));
    }

    void test_file_size_resize()
    {
        if (std::filesystem::exists("/test/file_size")) {
            CHECK(std::filesystem::remove("/test/file_size"));
        }
        std::filesystem::copy_file("/test/file", "/test/file_size");
        CHECK(std::filesystem::file_size("/test/file_size") == 11);
        // Not supported: libstdc++ has to be built with _GLIBCXX_HAVE_TRUNCATE
        CHECK_THROWS(std::filesystem::resize_file("/test/file_size", 20));
        CHECK(std::filesystem::remove("/test/file_size"));
    }

    void test_file_last_write_time()
    {
        if (std::filesystem::exists("/test/file_time")) {
            CHECK(std::filesystem::remove("/test/file_time"));
        }
        std::filesystem::copy_file("/test/file", "/test/file_time");
        auto time = std::filesystem::last_write_time("/test/file_time");
        struct stat st = {};
        stat("/test/file_time", &st);
        struct utimbuf times = {st.st_atime, st.st_mtime + 1000000000};
        utime("/test/file_time", &times);

        auto time2 = std::filesystem::last_write_time("/test/file_time");
        CHECK(time2 > time);
    }

    void test_space()
    {
        // Not supported: libstdc++ has to be built with _GLIBCXX_HAVE_SYS_STATVFS_H and statvfs function
        // has to be defined
        CHECK_THROWS(std::filesystem::space("/test"));
    }

    void test_permissions()
    {
        auto perm = std::filesystem::status("/test/file").permissions();
        CHECK(perm == std::filesystem::perms::all);

        std::filesystem::permissions("/test/file", std::filesystem::perms::owner_read, std::filesystem::perm_options::replace);

        // setting permissions is not supported and has no effect
        perm = std::filesystem::status("/test/file").permissions();
        CHECK(perm == std::filesystem::perms::all);
    }


    // when adding a test method, don't forget to add it to the list below
};

METHOD_AS_TEST_CASE(OpsTest::test_create_remove, "Test create and remove directories");
METHOD_AS_TEST_CASE(OpsTest::test_directory_iterator, "Test directory iterator");
METHOD_AS_TEST_CASE(OpsTest::test_recursive_directory_iterator, "Test recursive directory iterator");
METHOD_AS_TEST_CASE(OpsTest::test_copy_remove_recursive_copy, "Test copy, remove and recursive copy");
METHOD_AS_TEST_CASE(OpsTest::test_create_directories, "Test create directories");
METHOD_AS_TEST_CASE(OpsTest::test_rename_file, "Test rename file");
METHOD_AS_TEST_CASE(OpsTest::test_file_size_resize, "Test file size and resize");
METHOD_AS_TEST_CASE(OpsTest::test_file_last_write_time, "Test file last write time");
METHOD_AS_TEST_CASE(OpsTest::test_space, "Test space");
METHOD_AS_TEST_CASE(OpsTest::test_permissions, "Test permissions");
