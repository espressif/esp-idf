/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_err.h"
#include "esp_vfs_ops.h"
#include "unity.h"
#include "esp_vfs.h"

#include "unity_test_runner.h"

#define BUF_CAP 1024

typedef struct {
    uint8_t data[BUF_CAP];
    size_t head;
    size_t tail;
} cb_t;

void cb_reset(cb_t* buf) {
    buf->head = 0;
    buf->tail = 0;
}

size_t cb_count(const cb_t *buf) {
    return buf->head - buf->tail;
}

int cb_write(cb_t *buf, const uint8_t *data, size_t size) {
    size_t count = cb_count(buf);
    size_t space = BUF_CAP - count;

    size_t to_write = (size > space) ? space : size;

    size_t idx = buf->head % BUF_CAP;
    size_t first_chunk = BUF_CAP - idx;
    if (first_chunk > to_write)
        first_chunk = to_write;

    memcpy(&buf->data[idx], data, first_chunk);
    buf->head += first_chunk;

    if (first_chunk < to_write) {
        size_t second_chunk = to_write - first_chunk;
        memcpy(&buf->data[0], data + first_chunk, second_chunk);
        buf->head += second_chunk;
    }

    return (int)to_write;
}

int cb_read(cb_t *buf, uint8_t *dest, size_t count) {
    size_t available = cb_count(buf);
    size_t to_read = (count > available) ? available : count;

    size_t idx = buf->tail % BUF_CAP;
    size_t first_chunk = BUF_CAP - idx;
    if (first_chunk > to_read)
        first_chunk = to_read;

    memcpy(dest, &buf->data[idx], first_chunk);
    buf->tail += first_chunk;

    if (first_chunk < to_read) {
        size_t second_chunk = to_read - first_chunk;
        memcpy(dest + first_chunk, &buf->data[0], second_chunk);
        buf->tail += second_chunk;
    }

    return (int)to_read;
}


int buffer_open(void *ctx, const char *path, int flags, int mode) {
    return 0;
}

int buffer_write(void *ctx, int fd, const void *data, size_t size) {
    cb_t* buf = (cb_t*) ctx;
    return cb_write(buf, data, size);
}

int buffer_read(void *ctx, int fd, void *data, size_t size) {
    cb_t* buf = (cb_t*) ctx;
    return cb_read(buf, data, size);
}

int buffer_close(void *ctx, int fd) {
    cb_reset((cb_t*) ctx);
    return 0;
}

static esp_vfs_fs_ops_t s_buffer_fs = {
    .write_p = buffer_write,
    .read_p = buffer_read,
    .open_p = buffer_open,
    .close_p = buffer_close,
};

TEST_CASE("VFS won't create a copy when ESP_FLAG_VFS_STATIC is specified", "[esp_vfs_fs_ops_t]")
{
    TEST_MESSAGE("test");
    static esp_vfs_dir_ops_t dir = {};
    static esp_vfs_fs_ops_t vfs = {
        .dir = &dir,
    };

    cb_t *buffer = calloc(1, sizeof(cb_t));

    esp_err_t err = ESP_OK;
    err = esp_vfs_register_fs("/buffer", &s_buffer_fs, ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC, buffer);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = esp_vfs_register_fs("/static", &vfs, ESP_VFS_FLAG_STATIC, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = esp_vfs_register_fs("/dynamic", &vfs, ESP_VFS_FLAG_DEFAULT, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    FILE *buf_f = fopen("/buffer/a", "r+");

    esp_vfs_dump_registered_paths(buf_f);

    char read_buffer[512];
    size_t bytes_read = fread(read_buffer, 1, sizeof(read_buffer) - 1, buf_f);
    read_buffer[bytes_read] = '\0'; // Null-terminate the string

    // Parse the buffer to extract VFS pointers
    char *line = strtok(read_buffer, "\n");
    void *static_vfs_ptr = NULL;
    void *dynamic_vfs_ptr = NULL;

    while (line != NULL) {
        int index;
        char path_prefix[64];
        char ptr_str[32];
        TEST_MESSAGE(line);
        if (sscanf(line, "%d:%63s -> %31s", &index, path_prefix, ptr_str) == 3) {
            void *vfs_ptr = (void *)strtoul(ptr_str, NULL, 0);
            if (strcmp(path_prefix, "/static") == 0) {
                static_vfs_ptr = vfs_ptr;
            } else if (strcmp(path_prefix, "/dynamic") == 0) {
                dynamic_vfs_ptr = vfs_ptr;
            }
        }
        line = strtok(NULL, "\n");
    }

    // Check that the pointer for "/static" is the same as 'vfs' and "/dynamic" is different
    TEST_ASSERT_EQUAL_PTR(&vfs, static_vfs_ptr);
    TEST_ASSERT_NOT_EQUAL(&vfs, dynamic_vfs_ptr);

    free(buffer);
    fclose(buf_f);

    esp_vfs_unregister("/buffer");
    esp_vfs_unregister("/static");
    esp_vfs_unregister("/dynamic");
}
