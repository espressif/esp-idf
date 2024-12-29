/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <filesystem>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("std::filesystem path, relative, proximate, absolute")
{
    // In IDF, CWD is always in the the root directory
    CHECK(std::filesystem::current_path() == "/");

    // Create absolute path from relative path
    std::filesystem::path rel_path("test/file.txt");
    std::filesystem::path abs_path = std::filesystem::absolute(rel_path);
    CHECK(abs_path == "/test/file.txt");

    // Create relative path from absolute path
    std::filesystem::path rel_path2 = std::filesystem::relative(abs_path);
    CHECK(rel_path2 == "test/file.txt");

    // Create relative path from absolute path with different base
    std::filesystem::path rel_path3 = std::filesystem::relative(abs_path, "/test");
    CHECK(rel_path3 == "file.txt");

    std::filesystem::path prox_path = std::filesystem::proximate("/root1/file", "/root2");
    CHECK(prox_path == "../root1/file");
}

TEST_CASE("std::filesystem weakly_canonical")
{
    CHECK(std::filesystem::weakly_canonical("/a/b/c/./d/../e/f/../g") == "/a/b/c/e/g");
}

TEST_CASE("std::filesystem current_path")
{
    // In IDF, CWD is always in the the root directory
    CHECK(std::filesystem::current_path() == "/");

    // Changing the current path in IDF is not supported
    CHECK_THROWS(std::filesystem::current_path("/test"));
}
