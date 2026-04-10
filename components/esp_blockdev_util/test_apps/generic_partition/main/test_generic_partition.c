/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"
#include "unity_test_utils.h"

#include "esp_blockdev.h"
#include "esp_blockdev/generic_partition.h"
#include "esp_blockdev/memory.h"

static esp_blockdev_handle_t create_memory_parent(uint8_t *backing, size_t backing_size)
{
    const esp_blockdev_geometry_t geometry = {
        .disk_size = backing_size,
        .read_size = 1,
        .write_size = 1,
        .erase_size = 1,
        .recommended_write_size = 8,
        .recommended_read_size = 8,
        .recommended_erase_size = 16,
    };

    esp_blockdev_handle_t parent = NULL;
    TEST_ESP_OK(esp_blockdev_memory_get_from_buffer(backing, backing_size, &geometry, false, &parent));
    TEST_ASSERT_NOT_NULL(parent);

    return parent;
}

TEST_CASE("generic partition basic read/write/erase", "[generic_partition]")
{
    uint8_t backing[256];
    memset(backing, 0xEE, sizeof(backing));

    esp_blockdev_handle_t parent = create_memory_parent(backing, sizeof(backing));

    const size_t partition_offset = 64;
    const size_t partition_size = 128;

    esp_blockdev_handle_t part = NULL;
    TEST_ESP_OK(esp_blockdev_generic_partition_get(parent, partition_offset, partition_size, &part));
    TEST_ASSERT_NOT_NULL(part);

    TEST_ASSERT_EQUAL_UINT64(partition_size, part->geometry.disk_size);
    TEST_ASSERT_EQUAL_UINT32(parent->geometry.read_size, part->geometry.read_size);
    TEST_ASSERT_EQUAL_UINT32(parent->geometry.write_size, part->geometry.write_size);
    TEST_ASSERT_EQUAL_UINT32(parent->geometry.erase_size, part->geometry.erase_size);

    uint8_t pattern[32];
    for (size_t i = 0; i < sizeof(pattern); ++i) {
        pattern[i] = (uint8_t)(i ^ 0xA5);
    }

    const uint64_t write_addr = 48;
    TEST_ESP_OK(part->ops->write(part, pattern, write_addr, sizeof(pattern)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(pattern, &backing[partition_offset + write_addr], sizeof(pattern));

    uint8_t read_buf[sizeof(pattern)];
    memset(read_buf, 0, sizeof(read_buf));
    TEST_ESP_OK(part->ops->read(part, read_buf, sizeof(read_buf), write_addr, sizeof(read_buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(pattern, read_buf, sizeof(pattern));

    TEST_ESP_OK(part->ops->erase(part, write_addr, sizeof(pattern)));
    TEST_ASSERT_EACH_EQUAL_UINT8(0x00, &backing[partition_offset + write_addr], sizeof(pattern));

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(parent->ops->release(parent));
}

TEST_CASE("generic partition translate helpers", "[generic_partition]")
{
    uint8_t backing[32];
    memset(backing, 0x00, sizeof(backing));

    esp_blockdev_handle_t parent = create_memory_parent(backing, sizeof(backing));

    esp_blockdev_handle_t part = NULL;
    TEST_ESP_OK(esp_blockdev_generic_partition_get(parent, 4, 16, &part));

    ssize_t parent_addr = esp_blockdev_generic_partition_translate_address_to_parent(part, 3);
    TEST_ASSERT_GREATER_OR_EQUAL(0, parent_addr);
    TEST_ASSERT_EQUAL_INT(7, parent_addr);

    ssize_t child_addr = esp_blockdev_generic_partition_translate_address_to_child(part, 10);
    TEST_ASSERT_GREATER_OR_EQUAL(0, child_addr);
    TEST_ASSERT_EQUAL_INT(6, child_addr);

    TEST_ASSERT_EQUAL_INT(-1, esp_blockdev_generic_partition_translate_address_to_parent(part, 20));
    TEST_ASSERT_EQUAL_INT(-1, esp_blockdev_generic_partition_translate_address_to_child(part, 2));

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(parent->ops->release(parent));
}

typedef struct {
    esp_blockdev_t dev;
    struct {
        uint8_t *dst_buf;
        size_t dst_buf_size;
        uint64_t addr;
        size_t len;
        bool called;
    } read;
    struct {
        const uint8_t *src_buf;
        uint64_t addr;
        size_t len;
        bool called;
    } write;
    struct {
        uint64_t addr;
        size_t len;
        bool called;
    } erase;
    bool sync_called;
    struct {
        uint8_t cmd;
        void *args;
        esp_blockdev_cmd_arg_erase_t erase_arg;
        bool erase_arg_copied;
        bool called;
    } ioctl;
} mock_blockdev_t;

static esp_err_t mock_read(esp_blockdev_handle_t dev, uint8_t *dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_len)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    mock->read.called = true;
    mock->read.dst_buf = dst_buf;
    mock->read.dst_buf_size = dst_buf_size;
    mock->read.addr = src_addr;
    mock->read.len = data_len;
    return ESP_OK;
}

static esp_err_t mock_write(esp_blockdev_handle_t dev, const uint8_t *src_buf, uint64_t dst_addr, size_t data_len)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    mock->write.called = true;
    mock->write.src_buf = src_buf;
    mock->write.addr = dst_addr;
    mock->write.len = data_len;
    return ESP_OK;
}

static esp_err_t mock_erase(esp_blockdev_handle_t dev, uint64_t start_addr, size_t erase_len)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    mock->erase.called = true;
    mock->erase.addr = start_addr;
    mock->erase.len = erase_len;
    return ESP_OK;
}

static esp_err_t mock_sync(esp_blockdev_handle_t dev)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    mock->sync_called = true;
    return ESP_OK;
}

static esp_err_t mock_ioctl(esp_blockdev_handle_t dev, const uint8_t cmd, void *args)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    mock->ioctl.called = true;
    mock->ioctl.cmd = cmd;
    mock->ioctl.args = args;
    if ((cmd == ESP_BLOCKDEV_CMD_MARK_DELETED || cmd == ESP_BLOCKDEV_CMD_ERASE_CONTENTS) && args != NULL) {
        mock->ioctl.erase_arg = *(esp_blockdev_cmd_arg_erase_t *)args;
        mock->ioctl.erase_arg_copied = true;
    }
    return ESP_OK;
}

static esp_err_t mock_release(esp_blockdev_handle_t dev)
{
    mock_blockdev_t *mock = (mock_blockdev_t *)dev->ctx;
    memset(mock, 0, sizeof(*mock));
    return ESP_OK;
}

static const esp_blockdev_ops_t MOCK_OPS = {
    .read = mock_read,
    .write = mock_write,
    .erase = mock_erase,
    .sync = mock_sync,
    .ioctl = mock_ioctl,
    .release = mock_release,
};

static const esp_blockdev_ops_t MOCK_OPS_NO_SYNC_IOCTL = {
    .read = mock_read,
    .write = mock_write,
    .erase = mock_erase,
    .sync = NULL,
    .ioctl = NULL,
    .release = mock_release,
};

static void init_mock_with_ops(mock_blockdev_t *mock, uint64_t disk_size, const esp_blockdev_ops_t *ops);

static void init_mock(mock_blockdev_t *mock, uint64_t disk_size)
{
    init_mock_with_ops(mock, disk_size, &MOCK_OPS);
}

static void init_mock_with_ops(mock_blockdev_t *mock, uint64_t disk_size, const esp_blockdev_ops_t *ops)
{
    memset(mock, 0, sizeof(*mock));
    mock->dev.ctx = mock;
    mock->dev.device_flags.val = 0;
    mock->dev.geometry = (esp_blockdev_geometry_t) {
        .disk_size = disk_size,
        .read_size = 4,
        .write_size = 4,
        .erase_size = 16,
        .recommended_read_size = 8,
        .recommended_write_size = 8,
        .recommended_erase_size = 64,
    };
    mock->dev.ops = ops;
}

TEST_CASE("generic partition forwards operations to parent", "[generic_partition]")
{
    mock_blockdev_t mock_parent;
    init_mock(&mock_parent, 1024);

    esp_blockdev_handle_t part = NULL;
    const size_t start = 128;
    const size_t size = 512;

    TEST_ESP_OK(esp_blockdev_generic_partition_get((esp_blockdev_handle_t)&mock_parent, start, size, &part));

    uint8_t buffer[64];
    uint8_t src[32];
    memset(src, 0xCC, sizeof(src));

    TEST_ESP_OK(part->ops->read(part, buffer, sizeof(buffer), 12, sizeof(buffer)));
    TEST_ASSERT_TRUE(mock_parent.read.called);
    TEST_ASSERT_EQUAL_PTR(buffer, mock_parent.read.dst_buf);
    TEST_ASSERT_EQUAL_UINT32(sizeof(buffer), mock_parent.read.dst_buf_size);
    TEST_ASSERT_EQUAL_UINT64(start + 12, mock_parent.read.addr);
    TEST_ASSERT_EQUAL_UINT32(sizeof(buffer), mock_parent.read.len);

    TEST_ESP_OK(part->ops->write(part, src, 24, sizeof(src)));
    TEST_ASSERT_TRUE(mock_parent.write.called);
    TEST_ASSERT_EQUAL_PTR(src, mock_parent.write.src_buf);
    TEST_ASSERT_EQUAL_UINT64(start + 24, mock_parent.write.addr);
    TEST_ASSERT_EQUAL_UINT32(sizeof(src), mock_parent.write.len);

    TEST_ESP_OK(part->ops->erase(part, 48, 96));
    TEST_ASSERT_TRUE(mock_parent.erase.called);
    TEST_ASSERT_EQUAL_UINT64(start + 48, mock_parent.erase.addr);
    TEST_ASSERT_EQUAL_UINT32(96, mock_parent.erase.len);

    TEST_ESP_OK(part->ops->sync(part));
    TEST_ASSERT_TRUE(mock_parent.sync_called);

    uint32_t ioctl_arg = 0xDEADBEEF;
    TEST_ESP_OK(part->ops->ioctl(part, 0x42, &ioctl_arg));
    TEST_ASSERT_TRUE(mock_parent.ioctl.called);
    TEST_ASSERT_EQUAL_UINT8(0x42, mock_parent.ioctl.cmd);
    TEST_ASSERT_EQUAL_PTR(&ioctl_arg, mock_parent.ioctl.args);

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(mock_parent.dev.ops->release((esp_blockdev_handle_t)&mock_parent));
}

TEST_CASE("generic partition remaps mark deleted ioctl addresses", "[generic_partition]")
{
    mock_blockdev_t mock_parent;
    init_mock(&mock_parent, 1024);

    esp_blockdev_handle_t part = NULL;
    const size_t start = 128;
    const size_t size = 512;

    TEST_ESP_OK(esp_blockdev_generic_partition_get((esp_blockdev_handle_t)&mock_parent, start, size, &part));

    esp_blockdev_cmd_arg_erase_t mark_deleted = {
        .start_addr = 48,
        .erase_len = 96,
    };

    TEST_ESP_OK(part->ops->ioctl(part, ESP_BLOCKDEV_CMD_MARK_DELETED, &mark_deleted));
    TEST_ASSERT_TRUE(mock_parent.ioctl.called);
    TEST_ASSERT_EQUAL_UINT8(ESP_BLOCKDEV_CMD_MARK_DELETED, mock_parent.ioctl.cmd);
    TEST_ASSERT_TRUE(mock_parent.ioctl.erase_arg_copied);
    TEST_ASSERT_EQUAL_UINT64(start + 48, mock_parent.ioctl.erase_arg.start_addr);
    TEST_ASSERT_EQUAL_UINT32(96, mock_parent.ioctl.erase_arg.erase_len);
    TEST_ASSERT_EQUAL_UINT64(48, mark_deleted.start_addr);
    TEST_ASSERT_EQUAL_UINT32(96, mark_deleted.erase_len);

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(mock_parent.dev.ops->release((esp_blockdev_handle_t)&mock_parent));
}

TEST_CASE("generic partition remaps erase contents ioctl addresses", "[generic_partition]")
{
    mock_blockdev_t mock_parent;
    init_mock(&mock_parent, 1024);

    esp_blockdev_handle_t part = NULL;
    const size_t start = 128;
    const size_t size = 512;

    TEST_ESP_OK(esp_blockdev_generic_partition_get((esp_blockdev_handle_t)&mock_parent, start, size, &part));

    esp_blockdev_cmd_arg_erase_t erase_contents = {
        .start_addr = 64,
        .erase_len = 96,
    };

    TEST_ESP_OK(part->ops->ioctl(part, ESP_BLOCKDEV_CMD_ERASE_CONTENTS, &erase_contents));
    TEST_ASSERT_TRUE(mock_parent.ioctl.called);
    TEST_ASSERT_EQUAL_UINT8(ESP_BLOCKDEV_CMD_ERASE_CONTENTS, mock_parent.ioctl.cmd);
    TEST_ASSERT_TRUE(mock_parent.ioctl.erase_arg_copied);
    TEST_ASSERT_EQUAL_UINT64(start + 64, mock_parent.ioctl.erase_arg.start_addr);
    TEST_ASSERT_EQUAL_UINT32(96, mock_parent.ioctl.erase_arg.erase_len);
    TEST_ASSERT_EQUAL_UINT64(64, erase_contents.start_addr);
    TEST_ASSERT_EQUAL_UINT32(96, erase_contents.erase_len);

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(mock_parent.dev.ops->release((esp_blockdev_handle_t)&mock_parent));
}

TEST_CASE("generic partition tolerates missing optional parent ops", "[generic_partition]")
{
    mock_blockdev_t mock_parent;
    init_mock_with_ops(&mock_parent, 1024, &MOCK_OPS_NO_SYNC_IOCTL);

    esp_blockdev_handle_t part = NULL;
    TEST_ESP_OK(esp_blockdev_generic_partition_get((esp_blockdev_handle_t)&mock_parent, 128, 512, &part));

    TEST_ESP_OK(part->ops->sync(part));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, part->ops->ioctl(part, 0x44, NULL));

    TEST_ESP_OK(part->ops->release(part));
    TEST_ESP_OK(mock_parent.dev.ops->release((esp_blockdev_handle_t)&mock_parent));
}

TEST_CASE("generic partition creation validation", "[generic_partition]")
{
    uint8_t backing[128];
    memset(backing, 0, sizeof(backing));
    esp_blockdev_handle_t parent = create_memory_parent(backing, sizeof(backing));

    esp_blockdev_handle_t part = (esp_blockdev_handle_t)0xDEADBEEF;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_blockdev_generic_partition_get(parent, 64, 256, &part));
    TEST_ASSERT_EQUAL_PTR(NULL, part);

    part = (esp_blockdev_handle_t)0xDEADBEEF;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_blockdev_generic_partition_get(parent, SIZE_MAX, 64, &part));
    TEST_ASSERT_EQUAL_PTR(NULL, part);

    TEST_ESP_OK(parent->ops->release(parent));
}

void app_main(void)
{
    unity_run_menu();
}
