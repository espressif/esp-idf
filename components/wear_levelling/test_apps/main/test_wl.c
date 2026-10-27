/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
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
#include "spi_flash_mmap.h"


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
        fake_partition.size = SPI_FLASH_SEC_SIZE * (i);
        size_before = esp_get_free_heap_size();
        TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_mount(&fake_partition, &handle));
        // test that we didn't leak any memory
        size_after = esp_get_free_heap_size();
        TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    }

    // test minimum size partition: result should be OK
    fake_partition.size = SPI_FLASH_SEC_SIZE * 5;
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

    fake_partition.size = SPI_FLASH_SEC_SIZE * (4 + TEST_SECTORS_COUNT);

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

/*
 * Unaligned access across WL page boundaries (GH #19145).
 *
 * Logical WL pages are not physically contiguous: the dummy sector sits
 * between them, and after a full dummy sector pass logical page 0 maps to the
 * last data page. Each test runs on a small fake partition, so a full dummy
 * sector pass takes few moves. The host test also checks the dummy sector
 * position on flash; here the tests rely on flush() moving it exactly once.
 */

#define WL_TEST_PAGE                4096
#define WL_TEST_PART_PAGES          10      // 6 data pages, 4 usable in safe mode
// Layout of wl_state_t (private_include/WL_State.h) and wl_config_t
// (private_include/WL_Config.h), which are C++-only headers.
#define WL_TEST_STATE_SIZE          64
#define WL_TEST_CFG_RECORD_SIZE     20      // offsetof(wl_config_t, wl_pos_update_record_size)
#define WL_TEST_RECORD_DATA_SIZE    16      // WL_Flash::fillOkBuff() writes 4 x uint32_t per record

typedef struct {
    wl_handle_t handle;
} wl_test_dev_t;

static void wl_test_dev_open(wl_test_dev_t *dev, const esp_partition_t *partition)
{
    TEST_ESP_OK(wl_mount(partition, &dev->handle));
}

static void wl_test_dev_close(wl_test_dev_t *dev)
{
    TEST_ESP_OK(wl_unmount(dev->handle));
}

static esp_err_t wl_test_dev_erase(wl_test_dev_t *dev, size_t addr, size_t size)
{
    return wl_erase_range(dev->handle, addr, size);
}

// Move the dummy sector one position. wl_unmount() calls WL_Flash::flush(),
// which moves it exactly once.
static void wl_test_dev_move_dummy(wl_test_dev_t *dev, const esp_partition_t *partition)
{
    TEST_ESP_OK(wl_unmount(dev->handle));
    TEST_ESP_OK(wl_mount(partition, &dev->handle));
}

static esp_err_t wl_test_dev_write(wl_test_dev_t *dev, size_t addr, const void *src, size_t size)
{
    return wl_write(dev->handle, addr, src, size);
}

static esp_err_t wl_test_dev_read(wl_test_dev_t *dev, size_t addr, void *dst, size_t size)
{
    return wl_read(dev->handle, addr, dst, size);
}

// Position record size, read from the wl_config_t that WL stores in the
// last sector of the partition. Valid once the partition has been mounted.
static size_t wl_test_pos_record_size(const esp_partition_t *partition)
{
    uint32_t record_size;
    TEST_ESP_OK(esp_partition_read(partition, partition->size - partition->erase_size + WL_TEST_CFG_RECORD_SIZE,
                                   &record_size, sizeof(record_size)));
    TEST_ASSERT_GREATER_OR_EQUAL(WL_TEST_RECORD_DATA_SIZE, record_size);
    return record_size;
}

// Offset of the first WL state copy, mirroring WL_Flash::config() for a
// partition-relative start address of 0 and a one-sector wl_config_t.
static size_t wl_test_state1_addr(const esp_partition_t *partition)
{
    const size_t sector = partition->erase_size;
    size_t state_size = WL_TEST_STATE_SIZE + (partition->size / sector) * wl_test_pos_record_size(partition);
    state_size = (state_size + sector - 1) / sector * sector;
    return partition->size - 2 * state_size - sector;
}

static void wl_test_fake_partition(esp_partition_t *fake)
{
    memcpy(fake, get_test_data_partition(), sizeof(*fake));
    TEST_ASSERT_EQUAL(WL_TEST_PAGE, fake->erase_size);
    fake->size = WL_TEST_PART_PAGES * WL_TEST_PAGE;
}

static void wl_test_fill_pattern(uint8_t *buf, size_t size)
{
    // Unique byte per position, so data landing at a wrong offset is detected.
    for (size_t i = 0; i < size; i++) {
        buf[i] = (uint8_t)(i * 7 + 1);
    }
}

typedef struct {
    size_t offset;
    size_t size;
} wl_test_access_t;

static void test_unaligned_across_page_boundary(void)
{
    const size_t page = WL_TEST_PAGE;
    const size_t region = 4 * page;
    // Only the edge cases are kept here; the host test covers byte-granular
    // crossings and the full table.
    const wl_test_access_t cases[] = {
        { page / 2,     page / 2 },         // unaligned, ends on page boundary
        { page - 16,    32 },               // crosses boundary
        { page - 16,    page + 32 },        // crosses two boundaries
    };
    const size_t case_count = sizeof(cases) / sizeof(cases[0]);

    esp_partition_t partition;
    wl_test_fake_partition(&partition);
    uint8_t *write_buf = malloc(region);
    uint8_t *read_buf = malloc(region);
    uint8_t *expected = malloc(region);
    TEST_ASSERT_NOT_NULL(write_buf);
    TEST_ASSERT_NOT_NULL(read_buf);
    TEST_ASSERT_NOT_NULL(expected);
    wl_test_fill_pattern(write_buf, region);

    for (size_t c = 0; c < case_count; c++) {
        const size_t offset = cases[c].offset;
        const size_t size = cases[c].size;
        printf("offset=0x%zx size=0x%zx\n", offset, size);

        // Fresh WL state: dummy sector at physical page 0.
        TEST_ESP_OK(esp_partition_erase_range(&partition, 0, partition.size));
        wl_test_dev_t dev;
        wl_test_dev_open(&dev, &partition);
        TEST_ASSERT_GREATER_OR_EQUAL(region, wl_size(dev.handle));

        // Move the dummy sector to physical page 1, between logical pages 0 and 1.
        wl_test_dev_move_dummy(&dev, &partition);

        // Unaligned write, then aligned read of the whole region. Only
        // [offset, offset + size) may change; the rest stays erased.
        TEST_ESP_OK(wl_test_dev_write(&dev, offset, write_buf, size));
        TEST_ESP_OK(wl_test_dev_read(&dev, 0, read_buf, region));
        memset(expected, 0xFF, region);
        memcpy(expected + offset, write_buf, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, read_buf, region);

        // Unaligned read of the same range must return exactly what was
        // written and nothing past size.
        memset(read_buf, 0, region);
        TEST_ESP_OK(wl_test_dev_read(&dev, offset, read_buf, size));
        memset(expected, 0, region);
        memcpy(expected, write_buf, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, read_buf, region);

        wl_test_dev_close(&dev);
    }

    free(write_buf);
    free(read_buf);
    free(expected);
}

static void test_unaligned_write_after_dummy_wrap(void)
{
    const size_t page = WL_TEST_PAGE;
    const size_t region = 2 * page;
    const size_t offset = page / 2;
    const size_t size = page;

    esp_partition_t partition;
    wl_test_fake_partition(&partition);
    uint8_t *write_buf = malloc(size);
    uint8_t *read_buf = malloc(region);
    uint8_t *expected = malloc(region);
    TEST_ASSERT_NOT_NULL(write_buf);
    TEST_ASSERT_NOT_NULL(read_buf);
    TEST_ASSERT_NOT_NULL(expected);
    wl_test_fill_pattern(write_buf, size);

    TEST_ESP_OK(esp_partition_erase_range(&partition, 0, partition.size));
    wl_test_dev_t dev;
    wl_test_dev_open(&dev, &partition);

    // WL metadata (both state copies and config), known once mounted. It
    // starts right after the dummy sector positions.
    const size_t meta_addr = wl_test_state1_addr(&partition);
    const size_t meta_size = partition.size - meta_addr;
    uint8_t *meta_before = malloc(meta_size);
    uint8_t *meta_after = malloc(meta_size);
    TEST_ASSERT_NOT_NULL(meta_before);
    TEST_ASSERT_NOT_NULL(meta_after);

    // Complete one full dummy sector pass, one move per position. Logical
    // page 0 then maps to the last data page, directly below the WL state.
    for (size_t i = 0; i < meta_addr / page; i++) {
        wl_test_dev_move_dummy(&dev, &partition);
    }

    // WL metadata (both state copies and config) must not change.
    TEST_ESP_OK(esp_partition_read(&partition, meta_addr, meta_before, meta_size));
    TEST_ESP_OK(wl_test_dev_write(&dev, offset, write_buf, size));
    TEST_ESP_OK(esp_partition_read(&partition, meta_addr, meta_after, meta_size));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(meta_before, meta_after, meta_size);

    memset(expected, 0xFF, region);
    memcpy(expected + offset, write_buf, size);
    TEST_ESP_OK(wl_test_dev_read(&dev, 0, read_buf, region));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, read_buf, region);

    // Unaligned read across the same boundary must not read from the WL state.
    memset(read_buf, 0, size);
    TEST_ESP_OK(wl_test_dev_read(&dev, offset, read_buf, size));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(write_buf, read_buf, size);

    // The volume must still mount, keep the data and accept erases.
    wl_test_dev_close(&dev);
    wl_test_dev_open(&dev, &partition);
    TEST_ESP_OK(wl_test_dev_read(&dev, 0, read_buf, region));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, read_buf, region);
    TEST_ESP_OK(wl_test_dev_erase(&dev, 0, region));
    wl_test_dev_close(&dev);

    free(write_buf);
    free(read_buf);
    free(expected);
    free(meta_before);
    free(meta_after);
}

TEST(wear_levelling, unaligned_access_across_page_boundary)
{
    test_unaligned_across_page_boundary();
}

TEST(wear_levelling, unaligned_write_after_dummy_wrap)
{
    test_unaligned_write_after_dummy_wrap();
}

TEST_GROUP_RUNNER(wear_levelling)
{
    RUN_TEST_CASE(wear_levelling, wl_unmount_doesnt_leak_memory)
    RUN_TEST_CASE(wear_levelling, wl_mount_checks_partition_params)
    RUN_TEST_CASE(wear_levelling, multiple_tasks_single_handle)
    RUN_TEST_CASE(wear_levelling, write_doesnt_touch_other_sectors)
    RUN_TEST_CASE(wear_levelling, unaligned_access_across_page_boundary)
    RUN_TEST_CASE(wear_levelling, unaligned_write_after_dummy_wrap)

#if CONFIG_WL_SECTOR_SIZE_4096
    RUN_TEST_CASE(wear_levelling, version_update)
#endif
}

void app_main(void)
{
    UNITY_MAIN(wear_levelling);
}
