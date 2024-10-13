/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file test_fatfs_common.h
 * @brief Common routines for FAT-on-SDMMC and FAT-on-WL tests
 */

#define HEAP_SIZE_CAPTURE(heap_size)  \
     heap_size = esp_get_free_heap_size();

#define HEAP_SIZE_CHECK(heap_size, tolerance) \
    do {\
        size_t final_heap_size = esp_get_free_heap_size(); \
        if (final_heap_size < heap_size - tolerance) { \
            printf("Initial heap size: %d, final: %d, diff=%d\n", heap_size, final_heap_size, heap_size - final_heap_size); \
        } \
    } while(0)

extern const char* fatfs_test_hello_str;
extern const char* fatfs_test_hello_str_utf;

void test_fatfs_create_file_with_text(const char* name, const char* text);

void test_fatfs_create_file_with_o_creat_flag(const char* filename);

void test_fatfs_open_file_with_o_creat_flag(const char* filename);

void test_fatfs_overwrite_append(const char* filename);

void test_fatfs_read_file(const char* filename);

void test_fatfs_read_file_utf_8(const char* filename);

void test_fatfs_pread_file(const char* filename);

void test_fatfs_pwrite_file(const char* filename);

void test_fatfs_open_max_files(const char* filename_prefix, size_t files_count);

void test_fatfs_lseek(const char* filename);

void test_fatfs_truncate_file(const char* path, bool allow_expanding_files);

void test_fatfs_ftruncate_file(const char* path, bool allow_expanding_files);

void test_fatfs_stat(const char* filename, const char* root_dir);

void test_fatfs_size(const char* filename, const char* content);

void test_fatfs_mtime_dst(const char* filename, const char* root_dir);

void test_fatfs_utime(const char* filename, const char* root_dir);

void test_fatfs_unlink(const char* filename);

void test_fatfs_link_rename(const char* filename_prefix);

void test_fatfs_concurrent(const char* filename_prefix);

void test_fatfs_mkdir_rmdir(const char* filename_prefix);

void test_fatfs_can_opendir(const char* path);

void test_fatfs_opendir_readdir_rewinddir(const char* dir_prefix);

void test_fatfs_opendir_readdir_rewinddir_utf_8(const char* dir_prefix);

void test_leading_spaces(void);

void test_fatfs_rw_speed(const char* filename, void* buf, size_t buf_size, size_t file_size, bool write);

void test_fatfs_info(const char* base_path, const char* filepath);

#if FF_USE_EXPAND
void test_fatfs_create_contiguous_file(const char* base_path, const char* full_path);
#endif

void test_fatfs_readdir_stat(const char* path);
