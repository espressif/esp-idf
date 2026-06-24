/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_fixture.h"
#include "wear_levelling.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/esp_clk.h"
#include "sdkconfig.h"
#include "esp_cpu.h"
#include "esp_system.h"


TEST_GROUP(wear_levelling);

TEST_SETUP(wear_levelling)
{
}

TEST_TEAR_DOWN(wear_levelling)
{
}

static const esp_partition_t *get_test_data_partition(void)
{
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                    ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}

TEST(wear_levelling, wl_unmount_doesnt_leak_memory)
{
    const esp_partition_t *partition = get_test_data_partition();
    wl_handle_t handle;
    // mount and unmount once to initialize static locks
    TEST_ESP_OK(wl_mount(partition, &handle));
    wl_unmount(handle);

    // test that we didn't leak any memory on the next init/deinit
    size_t size_before = esp_get_free_heap_size();
    TEST_ESP_OK(wl_mount(partition, &handle));
    wl_unmount(handle);
    size_t size_after = esp_get_free_heap_size();

    TEST_ASSERT_EQUAL(size_before, size_after);
}

TEST(wear_levelling, wl_mount_checks_partition_params)
{
    const esp_partition_t *test_partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, test_partition, sizeof(fake_partition));
    wl_handle_t handle;
    size_t size_before, size_after;
    wl_unmount(WL_INVALID_HANDLE);

    esp_partition_erase_range(test_partition, 0, test_partition->size);
    // test small partition: result should be error
    for (int i = 0; i < 5; i++) {
        fake_partition.size = test_partition->erase_size * (i);
        size_before = esp_get_free_heap_size();
        TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_mount(&fake_partition, &handle));
        // test that we didn't leak any memory
        size_after = esp_get_free_heap_size();
        TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    }

    // test minimum size partition: result should be OK
    fake_partition.size = test_partition->erase_size * 5;
    size_before = esp_get_free_heap_size();
    TEST_ESP_OK(wl_mount(&fake_partition, &handle));
    wl_unmount(handle);

    // test that we didn't leak any memory
    size_after = esp_get_free_heap_size();
    TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
}

typedef struct {
    size_t offset;
    bool write;
    size_t word_count;
    int seed;
    SemaphoreHandle_t done;
    int result;
    wl_handle_t handle;
} read_write_test_arg_t;

#define READ_WRITE_TEST_ARG_INIT(offset_, seed_, handle_, count_) \
        { \
            .offset = offset_, \
            .seed = seed_, \
            .word_count = count_, \
            .write = true, \
            .done = xSemaphoreCreateBinary(), \
            .handle = handle_ \
        }

static void read_write_task(void *param)
{
    read_write_test_arg_t *args = (read_write_test_arg_t *) param;
    esp_err_t err;
    srand(args->seed);
    for (size_t i = 0; i < args->word_count; ++i) {
        uint32_t val = rand();
        if (args->write) {
            err = wl_write(args->handle, args->offset + i * sizeof(val), &val, sizeof(val));
            if (err != ESP_OK) {
                args->result = err;
                goto done;
            }
        } else {
            uint32_t rval;
            err = wl_read(args->handle, args->offset + i * sizeof(rval), &rval, sizeof(rval));
            if (err != ESP_OK || rval != val) {
                printf("E: i=%" PRIu32 ", cnt=%" PRIu32 " rval=%" PRIu32 " val=%" PRIu32 "\n\n", (uint32_t) i, (uint32_t) args->word_count, rval, val);
                args->result = ESP_FAIL;
                goto done;
            }
        }
    }
    args->result = ESP_OK;

done:
    xSemaphoreGive(args->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST(wear_levelling, multiple_tasks_single_handle)
{
    const esp_partition_t *partition = get_test_data_partition();
    wl_handle_t handle;
    TEST_ESP_OK(wl_mount(partition, &handle));

    size_t sector_size = wl_sector_size(handle);
    TEST_ESP_OK(wl_erase_range(handle, 0, sector_size * 8));
    read_write_test_arg_t args1 = READ_WRITE_TEST_ARG_INIT(0, 1, handle, sector_size / sizeof(uint32_t));
    read_write_test_arg_t args2 = READ_WRITE_TEST_ARG_INIT(sector_size, 2, handle, sector_size / sizeof(uint32_t));
    const size_t stack_size = 8192;

    printf("writing 1 and 2\n");
    const int cpuid_0 = 0;
    const int cpuid_1 = CONFIG_FREERTOS_NUMBER_OF_CORES - 1;
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, cpuid_1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);

    args1.write = false;
    args2.write = false;
    read_write_test_arg_t args3 = READ_WRITE_TEST_ARG_INIT(2 * sector_size, 3, handle, sector_size / sizeof(uint32_t));
    read_write_test_arg_t args4 = READ_WRITE_TEST_ARG_INIT(3 * sector_size, 4, handle, sector_size / sizeof(uint32_t));

    printf("reading 1 and 2, writing 3 and 4\n");
    xTaskCreatePinnedToCore(&read_write_task, "rw3", stack_size, &args3, 3, NULL, cpuid_1);
    xTaskCreatePinnedToCore(&read_write_task, "rw4", stack_size, &args4, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, cpuid_0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, cpuid_1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);
    xSemaphoreTake(args3.done, portMAX_DELAY);
    printf("f3 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args3.result);
    xSemaphoreTake(args4.done, portMAX_DELAY);
    printf("f4 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args4.result);

    vSemaphoreDelete(args1.done);
    vSemaphoreDelete(args2.done);
    vSemaphoreDelete(args3.done);
    vSemaphoreDelete(args4.done);
    wl_unmount(handle);
}

#define TEST_SECTORS_COUNT 8

static void check_mem_data(wl_handle_t handle, uint32_t init_val, uint32_t *buff)
{
    size_t sector_size = wl_sector_size(handle);

    for (int m = 0; m < TEST_SECTORS_COUNT; m++) {
        TEST_ESP_OK(wl_read(handle, sector_size * m, buff, sector_size));
        for (int i = 0; i < sector_size / sizeof(uint32_t); i++) {
            uint32_t compare_val = init_val + i +  m * sector_size;
            TEST_ASSERT_EQUAL( buff[i], compare_val);
        }
    }
}


// We write complete memory with defined data
// And then write one sector many times.
// A data in other secors should be the same.
// We do this also with unmount
TEST(wear_levelling, write_doesnt_touch_other_sectors)
{
    const esp_partition_t *partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, partition, sizeof(fake_partition));

    fake_partition.size = partition->erase_size * (4 + TEST_SECTORS_COUNT);

    wl_handle_t handle;
    TEST_ESP_OK(wl_mount(&fake_partition, &handle));

    size_t sector_size = wl_sector_size(handle);
    // Erase 8 sectors
    TEST_ESP_OK(wl_erase_range(handle, 0, sector_size * TEST_SECTORS_COUNT));
    // Write data to all sectors
    printf("Check 1 sector_size=0x%08" PRIx32 "\n", (uint32_t) sector_size);
    // Set initial random value
    uint32_t init_val = rand();

    uint32_t *buff = (uint32_t *)malloc(sector_size);
    for (int m = 0; m < TEST_SECTORS_COUNT; m++) {
        for (int i = 0; i < sector_size / sizeof(uint32_t); i++) {
            buff[i] = init_val + i +  m * sector_size;
        }
        TEST_ESP_OK(wl_erase_range(handle, sector_size * m, sector_size));
        TEST_ESP_OK(wl_write(handle, sector_size * m, buff, sector_size));
    }

    check_mem_data(handle, init_val, buff);

    uint32_t start;
    start = esp_cpu_get_cycle_count();


    for (int m = 0; m < 100000; m++) {
        uint32_t sector = m % TEST_SECTORS_COUNT;
        for (int i = 0; i < sector_size / sizeof(uint32_t); i++) {
            buff[i] = init_val + i +  sector * sector_size;
        }
        TEST_ESP_OK(wl_erase_range(handle, sector_size * sector, sector_size));
        TEST_ESP_OK(wl_write(handle, sector_size * sector, buff, sector_size));
        check_mem_data(handle, init_val, buff);

        uint32_t end;
        end = esp_cpu_get_cycle_count();
        uint32_t ms = (end - start) / (esp_clk_cpu_freq() / 1000);
        printf("loop %4d pass, time= %" PRIu32 "ms\n", m, ms);
        if (ms > 10000) {
            break;
        }
    }

    free(buff);
    wl_unmount(handle);
}


#if CONFIG_WL_SECTOR_SIZE_4096
// This test runs for 4k sector size only, since the original (version 1) partition binary is generated this way
extern const uint8_t test_partition_v1_bin_start[] asm("_binary_test_partition_v1_bin_start");
extern const uint8_t test_partition_v1_bin_end[]   asm("_binary_test_partition_v1_bin_end");

#define COMPARE_START_CONST 0x12340000

// We write to partition prepared image with V1
// Then we convert image to new version and verifying the data
TEST(wear_levelling, version_update)
{
    const esp_partition_t *partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, partition, sizeof(fake_partition));

    if (partition->encrypted) {
        printf("Update from V1 to V2 will not work.\n");
        return;
    }
    fake_partition.size = (size_t)(test_partition_v1_bin_end - test_partition_v1_bin_start);

    printf("Data file size = %" PRIu32 ", partition address = 0x%08" PRIx32 ", file addr=0x%08" PRIx32 "\n", (uint32_t) fake_partition.size, (uint32_t) fake_partition.address, (uint32_t) test_partition_v1_bin_start);

    esp_partition_erase_range(&fake_partition, 0, fake_partition.size);

    esp_partition_write(&fake_partition, 0, test_partition_v1_bin_start,  fake_partition.size);
    for (int i = 0; i < 3; i++) {
        printf("Pass %d\n", i);
        wl_handle_t handle;
        TEST_ESP_OK(wl_mount(&fake_partition, &handle));
        size_t sector_size = wl_sector_size(handle);
        uint32_t *buff = (uint32_t *)malloc(sector_size);

        uint32_t init_val = COMPARE_START_CONST;
        int test_count = fake_partition.size / sector_size - 4;

        for (int m = 0; m <  test_count; m++) {
            TEST_ESP_OK(wl_read(handle, sector_size * m, buff, sector_size));
            for (int i = 0; i < sector_size / sizeof(uint32_t); i++) {
                uint32_t compare_val = init_val + i +  m * sector_size;
                if (buff[i] != compare_val) {
                    printf("error compare: 0x%08" PRIx32 " != 0x%08" PRIx32 " \n", buff[i], compare_val);
                }
                TEST_ASSERT_EQUAL( buff[i], compare_val);
            }
        }
        free(buff);
        wl_unmount(handle);
    }
}
#endif // CONFIG_WL_SECTOR_SIZE_4096

TEST(wear_levelling, test_bdl_read_write_erase)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    TEST_ASSERT(wl_blockdev->geometry.disk_size > 0);
    TEST_ASSERT(wl_blockdev->geometry.erase_size > 0);

    const size_t data_size = 256;
    uint8_t test_data[data_size];
    uint8_t data_buffer[data_size];
    const off_t target_addr = 3 * 4 * 1024;

    TEST_ESP_OK(wl_blockdev->ops->erase(wl_blockdev, target_addr, wl_blockdev->geometry.erase_size));
    memset((void*)test_data, 0xFF, data_size);
    TEST_ESP_OK(wl_blockdev->ops->read(wl_blockdev, data_buffer, data_size, target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    memset((void*)test_data, 'P', data_size);
    TEST_ESP_OK(wl_blockdev->ops->write(wl_blockdev, test_data, target_addr, data_size));
    TEST_ESP_OK(wl_blockdev->ops->read(wl_blockdev, data_buffer, data_size, target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    TEST_ESP_OK(wl_blockdev->ops->sync(wl_blockdev));

    // Release WL BDL (internally unmounts WL)
    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));

    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_error_paths)
{
    esp_blockdev_handle_t out = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_get_blockdev(ESP_BLOCKDEV_HANDLE_INVALID, &out));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_get_blockdev(ESP_BLOCKDEV_HANDLE_INVALID, NULL));

    const esp_partition_t *partition = get_test_data_partition();
    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_get_blockdev(part_blockdev, NULL));

    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_geometry_and_flags)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    TEST_ASSERT(wl_blockdev->geometry.disk_size > 0);
    TEST_ASSERT(wl_blockdev->geometry.disk_size < part_blockdev->geometry.disk_size);
    TEST_ASSERT(wl_blockdev->geometry.erase_size > 0);
    TEST_ASSERT_EQUAL(part_blockdev->geometry.read_size, wl_blockdev->geometry.read_size);
    TEST_ASSERT_EQUAL(part_blockdev->geometry.write_size, wl_blockdev->geometry.write_size);
    TEST_ASSERT_EQUAL(part_blockdev->geometry.recommended_read_size, wl_blockdev->geometry.recommended_read_size);
    TEST_ASSERT_EQUAL(part_blockdev->geometry.recommended_write_size, wl_blockdev->geometry.recommended_write_size);

    TEST_ASSERT_EQUAL(part_blockdev->device_flags.val, wl_blockdev->device_flags.val);

    TEST_ASSERT_NOT_NULL(wl_blockdev->ops);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->read);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->write);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->erase);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->sync);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->ioctl);
    TEST_ASSERT_NOT_NULL(wl_blockdev->ops->release);

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_sync)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    size_t erase_size = wl_blockdev->geometry.erase_size;
    TEST_ESP_OK(wl_blockdev->ops->erase(wl_blockdev, 0, erase_size));

    uint8_t test_data[64];
    memset(test_data, 'S', sizeof(test_data));
    TEST_ESP_OK(wl_blockdev->ops->write(wl_blockdev, test_data, 0, sizeof(test_data)));
    TEST_ESP_OK(wl_blockdev->ops->sync(wl_blockdev));

    uint8_t read_data[64];
    TEST_ESP_OK(wl_blockdev->ops->read(wl_blockdev, read_data, sizeof(read_data), 0, sizeof(read_data)));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, read_data, sizeof(test_data)));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_ioctl)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED,
                      wl_blockdev->ops->ioctl(wl_blockdev, ESP_BLOCKDEV_CMD_MARK_DELETED, NULL));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_erase_alignment)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    size_t erase_size = wl_blockdev->geometry.erase_size;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, wl_blockdev->ops->erase(wl_blockdev, 1, erase_size));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, wl_blockdev->ops->erase(wl_blockdev, 0, erase_size - 1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, wl_blockdev->ops->erase(wl_blockdev, 1, erase_size + 1));
    TEST_ESP_OK(wl_blockdev->ops->erase(wl_blockdev, 0, erase_size));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_read_buffer_check)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    uint8_t buf[64];
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, wl_blockdev->ops->read(wl_blockdev, buf, 32, 0, 64));
    TEST_ESP_OK(wl_blockdev->ops->read(wl_blockdev, buf, sizeof(buf), 0, sizeof(buf)));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(wear_levelling, test_bdl_release_and_reconnect)
{
    const esp_partition_t *partition = get_test_data_partition();

    esp_blockdev_handle_t part_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(partition, &part_blockdev));

    esp_blockdev_handle_t wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    size_t erase_size = wl_blockdev->geometry.erase_size;
    TEST_ESP_OK(wl_blockdev->ops->erase(wl_blockdev, 0, erase_size));
    uint8_t test_data[64];
    memset(test_data, 'R', sizeof(test_data));
    TEST_ESP_OK(wl_blockdev->ops->write(wl_blockdev, test_data, 0, sizeof(test_data)));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));

    wl_blockdev = ESP_BLOCKDEV_HANDLE_INVALID;
    TEST_ESP_OK(wl_get_blockdev(part_blockdev, &wl_blockdev));

    uint8_t read_data[64];
    TEST_ESP_OK(wl_blockdev->ops->read(wl_blockdev, read_data, sizeof(read_data), 0, sizeof(read_data)));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, read_data, sizeof(test_data)));

    TEST_ESP_OK(wl_blockdev->ops->release(wl_blockdev));
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST_GROUP_RUNNER(wear_levelling)
{
    RUN_TEST_CASE(wear_levelling, wl_unmount_doesnt_leak_memory)
    RUN_TEST_CASE(wear_levelling, wl_mount_checks_partition_params)
    RUN_TEST_CASE(wear_levelling, multiple_tasks_single_handle)
    RUN_TEST_CASE(wear_levelling, write_doesnt_touch_other_sectors)
    RUN_TEST_CASE(wear_levelling, test_bdl_read_write_erase)
    RUN_TEST_CASE(wear_levelling, test_bdl_error_paths)
    RUN_TEST_CASE(wear_levelling, test_bdl_geometry_and_flags)
    RUN_TEST_CASE(wear_levelling, test_bdl_sync)
    RUN_TEST_CASE(wear_levelling, test_bdl_ioctl)
    RUN_TEST_CASE(wear_levelling, test_bdl_erase_alignment)
    RUN_TEST_CASE(wear_levelling, test_bdl_read_buffer_check)
    RUN_TEST_CASE(wear_levelling, test_bdl_release_and_reconnect)

#if CONFIG_WL_SECTOR_SIZE_4096
    RUN_TEST_CASE(wear_levelling, version_update)
#endif
}

void app_main(void)
{
    UNITY_MAIN(wear_levelling);
}
