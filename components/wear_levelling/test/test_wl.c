#include <string.h>
#include "unity.h"
#include "wear_levelling.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

TEST_CASE("wl_unmount doesn't leak memory", "[wear_levelling]")
{
    const esp_partition_t *partition = get_test_data_partition();
    wl_handle_t handle;
    // dummy unmount is needed to initialize static lock in WL
    wl_unmount(WL_INVALID_HANDLE);
    size_t size_before = xPortGetFreeHeapSize();
    TEST_ESP_OK(wl_mount(partition, &handle));
    wl_unmount(handle);
    size_t size_after = xPortGetFreeHeapSize();
    TEST_ASSERT_EQUAL_UINT32(size_before, size_after);
}

TEST_CASE("wl_mount check partition parameters", "[wear_levelling][ignore]")
{
    const esp_partition_t *test_partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, test_partition, sizeof(fake_partition));
    wl_handle_t handle;
    size_t size_before, size_after;

    // test small partition
    fake_partition.size = SPI_FLASH_SEC_SIZE;
    size_before = xPortGetFreeHeapSize();
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_mount(&fake_partition, &handle));
    size_after = xPortGetFreeHeapSize();
    TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    // currently this test leaks memory

    // test slightly bigger partition
    fake_partition.size = SPI_FLASH_SEC_SIZE * 3;
    size_before = xPortGetFreeHeapSize();
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_mount(&fake_partition, &handle));
    size_after = xPortGetFreeHeapSize();
    TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    // currently this test hangs
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

static void read_write_task(void* param)
{
    read_write_test_arg_t* args = (read_write_test_arg_t*) param;
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
                ets_printf("E: i=%d, cnt=%d rval=%d val=%d\n\n", i, args->word_count, rval, val);
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

TEST_CASE("multiple tasks can access wl handle simultaneously", "[wear_levelling]")
{
    const esp_partition_t *partition = get_test_data_partition();
    wl_handle_t handle;
    TEST_ESP_OK(wl_mount(partition, &handle));

    size_t sector_size = wl_sector_size(handle);
    TEST_ESP_OK(wl_erase_range(handle, 0, sector_size * 8));
    read_write_test_arg_t args1 = READ_WRITE_TEST_ARG_INIT(0, 1, handle, sector_size/sizeof(uint32_t));
    read_write_test_arg_t args2 = READ_WRITE_TEST_ARG_INIT(sector_size, 2, handle, sector_size/sizeof(uint32_t));
    const size_t stack_size = 4096;

    printf("writing 1 and 2\n");
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, 1);

    xSemaphoreTake(args1.done, portMAX_DELAY);
    printf("f1 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args1.result);
    xSemaphoreTake(args2.done, portMAX_DELAY);
    printf("f2 done\n");
    TEST_ASSERT_EQUAL(ESP_OK, args2.result);

    args1.write = false;
    args2.write = false;
    read_write_test_arg_t args3 = READ_WRITE_TEST_ARG_INIT(2 * sector_size, 3, handle, sector_size/sizeof(uint32_t));
    read_write_test_arg_t args4 = READ_WRITE_TEST_ARG_INIT(3 * sector_size, 4, handle, sector_size/sizeof(uint32_t));

    printf("reading 1 and 2, writing 3 and 4\n");
    xTaskCreatePinnedToCore(&read_write_task, "rw3", stack_size, &args3, 3, NULL, 1);
    xTaskCreatePinnedToCore(&read_write_task, "rw4", stack_size, &args4, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw1", stack_size, &args1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&read_write_task, "rw2", stack_size, &args2, 3, NULL, 1);

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
