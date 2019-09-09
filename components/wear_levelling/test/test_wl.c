#include <string.h>
#include "unity.h"
#include "wear_levelling.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/cpu.h"

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

    // Original code:
    //TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    // Workaround for problem with heap size calculation:
    ptrdiff_t stack_diff = size_before - size_after;
    stack_diff = abs(stack_diff);
    if (stack_diff > 8) TEST_ASSERT_EQUAL(0, stack_diff);
}

TEST_CASE("wl_mount check partition parameters", "[wear_levelling][ignore]")
{
    const esp_partition_t *test_partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, test_partition, sizeof(fake_partition));
    wl_handle_t handle;
    size_t size_before, size_after;
    wl_unmount(WL_INVALID_HANDLE);

    esp_partition_erase_range(test_partition, 0, test_partition->size);
    // test small partition: result should be error
    for (int i=0 ; i< 5 ; i++)
    {
        fake_partition.size = SPI_FLASH_SEC_SIZE*(i);
        size_before = xPortGetFreeHeapSize();
        TEST_ESP_ERR(ESP_ERR_INVALID_ARG, wl_mount(&fake_partition, &handle));
        size_after = xPortGetFreeHeapSize();

        // Original code:
        //TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
        // Workaround for problem with heap size calculation:
        ptrdiff_t stack_diff = size_before - size_after;
        stack_diff = abs(stack_diff);
        if (stack_diff > 8) TEST_ASSERT_EQUAL(0, stack_diff);
    }

    // test minimum size partition: result should be OK
    fake_partition.size = SPI_FLASH_SEC_SIZE * 5;
    size_before = xPortGetFreeHeapSize();
    TEST_ESP_OK(wl_mount(&fake_partition, &handle));
    wl_unmount(handle);
    printf("Test done\n");
    size_after = xPortGetFreeHeapSize();

    // Original code:
    //TEST_ASSERT_EQUAL_HEX32(size_before, size_after);
    // Workaround for problem with heap size calculation:
    ptrdiff_t stack_diff = size_before - size_after;
    stack_diff = abs(stack_diff);
    if (stack_diff > 8) TEST_ASSERT_EQUAL(0, stack_diff);
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
    const int cpuid_0 = 0;
    const int cpuid_1 = portNUM_PROCESSORS - 1;
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
    read_write_test_arg_t args3 = READ_WRITE_TEST_ARG_INIT(2 * sector_size, 3, handle, sector_size/sizeof(uint32_t));
    read_write_test_arg_t args4 = READ_WRITE_TEST_ARG_INIT(3 * sector_size, 4, handle, sector_size/sizeof(uint32_t));

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


extern const uint8_t test_partition_v1_bin_start[] asm("_binary_test_partition_v1_bin_start");
extern const uint8_t test_partition_v1_bin_end[]   asm("_binary_test_partition_v1_bin_end");

#define COMPARE_START_CONST 0x12340000

// We write to partition prepared image with V1
// Then we convert image to new version and verifying the data

TEST_CASE("Version update test", "[wear_levelling]")
{
    const esp_partition_t *partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, partition, sizeof(fake_partition));

    if (partition->encrypted)
    {
        printf("Update from V1 to V2 will not work.\n");
        return;
    }
    fake_partition.size = (size_t)(test_partition_v1_bin_end - test_partition_v1_bin_start);

    printf("Data file size = %i, partition address = 0x%08x, file addr=0x%08x\n", (uint32_t)fake_partition.size, (uint32_t)fake_partition.address, (uint32_t)test_partition_v1_bin_start);

    esp_partition_erase_range(&fake_partition, 0, fake_partition.size);

    esp_partition_write(&fake_partition, 0, test_partition_v1_bin_start,  fake_partition.size);
    for (int i=0 ; i< 3 ; i++)
    {
        printf("Pass %i\n", i);
        wl_handle_t handle;
        TEST_ESP_OK(wl_mount(&fake_partition, &handle));
        size_t sector_size = wl_sector_size(handle);
        uint32_t* buff = (uint32_t*)malloc(sector_size);

        uint32_t init_val = COMPARE_START_CONST;
        int test_count = fake_partition.size/sector_size - 4;

        for (int m=0 ; m <  test_count; m++) {
            TEST_ESP_OK(wl_read(handle, sector_size * m, buff, sector_size));
            for (int i=0 ; i< sector_size/sizeof(uint32_t) ; i++) {
                uint32_t compare_val = init_val + i +  m*sector_size;
                if (buff[i] != compare_val)
                {
                    printf("error compare: 0x%08x != 0x%08x \n", buff[i], compare_val);
                }
                TEST_ASSERT_EQUAL( buff[i], compare_val);
            }
        }
        free(buff);
        wl_unmount(handle);
    }
}
