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


const char* fatfs_test_hello_str;

void test_fatfs_create_file_with_text(const char* name, const char* text);

void test_fatfs_overwrite_append(const char* filename);

void test_fatfs_read_file(const char* filename);

void test_fatfs_open_max_files(const char* filename_prefix, size_t files_count);

void test_fatfs_lseek(const char* filename);

void test_fatfs_stat(const char* filename);

void test_fatfs_unlink(const char* filename);

void test_fatfs_link_rename(const char* filename_prefix);

void test_fatfs_concurrent(const char* filename_prefix);

void test_fatfs_mkdir_rmdir(const char* filename_prefix);

void test_fatfs_can_opendir(const char* path);

void test_fatfs_opendir_readdir_rewinddir(const char* dir_prefix);

void test_fatfs_rw_speed(const char* filename, void* buf, size_t buf_size, size_t file_size, bool write);
