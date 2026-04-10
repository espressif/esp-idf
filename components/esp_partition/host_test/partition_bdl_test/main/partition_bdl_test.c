/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Linux host partition API test
 */

#include <string.h>
#include <unistd.h>
#include "esp_partition.h"
#include "esp_private/partition_linux.h"
#include "unity.h"
#include "unity_fixture.h"

const char *TAG = "partition_bdl_test";

TEST_GROUP(partition_bdl);

TEST_SETUP(partition_bdl)
{
}

TEST_TEAR_DOWN(partition_bdl)
{
}

/* Test all the basic Block Device Layer APIs working correctly over emulated esp_partition instance.
 * NOR flash behavior expected.
 * */
TEST(partition_bdl, test_partition_bdl_ops)
{
    //get the block-device interface instance
    esp_blockdev_handle_t part_blockdev = NULL;
    TEST_ESP_OK(esp_partition_get_blockdev(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1", &part_blockdev));

    const size_t data_size = 256;
    uint8_t test_data[data_size];
    const off_t target_addr = 3*4*1024;
    uint8_t data_buffer[data_size];
    memset((void*)data_buffer, 0, data_size);

    //erase the first sector data from the blockdev and check it's really wiped
    TEST_ESP_OK(part_blockdev->ops->erase(part_blockdev, target_addr, part_blockdev->geometry.erase_size));
    memset((void*)test_data, 0xFF, data_size); //erased NOR flash sector contains only 1s

    TEST_ESP_OK(part_blockdev->ops->read(part_blockdev, data_buffer, sizeof(data_buffer), target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    //write to the blockdev
    memset((void*)test_data, 'A', data_size);
    TEST_ESP_OK(part_blockdev->ops->write(part_blockdev, test_data, target_addr, data_size));

    //read from the blockdev the data written before
    TEST_ESP_OK(part_blockdev->ops->read(part_blockdev, data_buffer, sizeof(data_buffer), target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    //release the BDL object
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

/* Test parallel access to two independent partitions through BDL interface.
 * Use both 'esp_partition_t' and type-subtype-label BDL getters.
 * NOR flash behavior expected.
 * */
TEST(partition_bdl, test_two_partitions_bdl_ops)
{
    //get the block-device interface instance for partition 'storage1'
    esp_blockdev_handle_t part_blockdev_1 = NULL;
    TEST_ESP_OK(esp_partition_get_blockdev(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1", &part_blockdev_1));

    //get pointer to esp_partition_t object for partition 'storage2'
    esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage2");
    TEST_ASSERT_NOT_NULL(iter);
    esp_partition_t *part = (esp_partition_t*)esp_partition_get(iter);
    TEST_ASSERT_NOT_NULL(part);
    esp_partition_iterator_release(iter);

    esp_blockdev_handle_t part_blockdev_2 = NULL;
    TEST_ESP_OK(esp_partition_ptr_get_blockdev(part, &part_blockdev_2));

    //erase & write & read data on both partitions in parallel
    const size_t data_size = 256;
    uint8_t test_data[data_size];
    const off_t target_addr = 3*4*1024;
    uint8_t data_buffer_1[data_size];
    uint8_t data_buffer_2[data_size];
    memset((void*)data_buffer_1, 0, data_size);
    memset((void*)data_buffer_2, 0, data_size);

    //erase the first sector data from the blockdev and check it's really wiped
    TEST_ESP_OK(part_blockdev_1->ops->erase(part_blockdev_1, target_addr, part_blockdev_1->geometry.erase_size));
    TEST_ESP_OK(part_blockdev_2->ops->erase(part_blockdev_2, target_addr, part_blockdev_2->geometry.erase_size));
    memset((void*)test_data, 0xFF, data_size); //erased NOR flash sector contains only 1s

    TEST_ESP_OK(part_blockdev_1->ops->read(part_blockdev_1, data_buffer_1, sizeof(data_buffer_1), target_addr, data_size));
    TEST_ESP_OK(part_blockdev_2->ops->read(part_blockdev_2, data_buffer_2, sizeof(data_buffer_2), target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer_1, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer_2, data_size));

    //write to the blockdev 1
    memset((void*)test_data, 'A', data_size);
    TEST_ESP_OK(part_blockdev_1->ops->write(part_blockdev_1, test_data, target_addr, data_size));

    //read the data written before from the blockdev 1
    TEST_ESP_OK(part_blockdev_1->ops->read(part_blockdev_1, data_buffer_1, sizeof(data_buffer_1), target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer_1, data_size));

    //write to the blockdev 2
    memset((void*)test_data, 'B', data_size);
    TEST_ESP_OK(part_blockdev_2->ops->write(part_blockdev_2, test_data, target_addr, data_size));

    //read the data written before from the blockdev 2
    TEST_ESP_OK(part_blockdev_2->ops->read(part_blockdev_2, data_buffer_2, sizeof(data_buffer_2), target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer_2, data_size));

    //release the BDL objects
    TEST_ESP_OK(part_blockdev_1->ops->release(part_blockdev_1));
    TEST_ESP_OK(part_blockdev_2->ops->release(part_blockdev_2));
}

TEST(partition_bdl, test_partition_bdl_limits)
{
    // Limits tested:
    //  - geometry alignment with parameter size for all ops
    //  - dst buffer size check when reading
    // Partition boundaries and other checks are provided by underlying partition APIs and are not verified here

    esp_blockdev_handle_t part_blockdev = NULL;
    TEST_ESP_OK(esp_partition_get_blockdev(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1", &part_blockdev));

    //artifitial setup setup for this test only
    part_blockdev->geometry.erase_size = 512;
    part_blockdev->geometry.read_size = 4;
    part_blockdev->geometry.write_size = 16;

    const size_t data_size = part_blockdev->geometry.erase_size;
    uint8_t test_data[data_size];
    uint8_t test_data_err[data_size-3];
    const off_t target_addr_err = 3;

    //correct addr, wrong length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->erase(part_blockdev, 0, data_size+3));
    //wrong addr, correct length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->erase(part_blockdev, target_addr_err, part_blockdev->geometry.erase_size));

    //correct addr, correct dst buff, wrong length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->read(part_blockdev, test_data, data_size, 0, data_size+3));
    //wrong addr, correct dst buff, correct length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->read(part_blockdev, test_data, data_size, target_addr_err, data_size));
    //correct addr, wrong dst buff, correct length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, part_blockdev->ops->read(part_blockdev, test_data_err, sizeof(test_data_err), 0, data_size));

    //correct addr, wrong length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->write(part_blockdev, test_data, 0, data_size+3));
    //wrong addr, correct length
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, part_blockdev->ops->write(part_blockdev, test_data, target_addr_err, data_size));

    //release the BDL objects
    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST(partition_bdl, test_bdl_partition_readonly)
{
   //storage3 is readonly partition
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage3");
    TEST_ASSERT_NOT_NULL(partition);

    esp_blockdev_handle_t part_blockdev = NULL;
    TEST_ESP_OK(esp_partition_get_blockdev(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage3", &part_blockdev));

    //test flags
    TEST_ASSERT_EQUAL(1, part_blockdev->device_flags.read_only);
    const size_t expected_write_size = partition->encrypted ? 16 : 1;
    TEST_ASSERT_EQUAL(expected_write_size, part_blockdev->geometry.write_size);
    TEST_ASSERT_EQUAL(partition->erase_size, part_blockdev->geometry.erase_size);
    TEST_ASSERT_EQUAL(expected_write_size, part_blockdev->geometry.recommended_write_size);
    TEST_ASSERT_EQUAL(partition->erase_size, part_blockdev->geometry.recommended_erase_size);

    uint8_t dummy_test_buff[1024];

    //write & erase must fail and read must work normally
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, part_blockdev->ops->erase(part_blockdev, 0, sizeof(dummy_test_buff)));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, part_blockdev->ops->write(part_blockdev, dummy_test_buff, 0, sizeof(dummy_test_buff)));
    TEST_ESP_OK(part_blockdev->ops->read(part_blockdev, dummy_test_buff, sizeof(dummy_test_buff), 0, sizeof(dummy_test_buff)));

    TEST_ESP_OK(part_blockdev->ops->release(part_blockdev));
}

TEST_GROUP_RUNNER(partition_bdl)
{
    RUN_TEST_CASE(partition_bdl, test_partition_bdl_ops);
    RUN_TEST_CASE(partition_bdl, test_two_partitions_bdl_ops);
    RUN_TEST_CASE(partition_bdl, test_partition_bdl_limits);
    RUN_TEST_CASE(partition_bdl, test_bdl_partition_readonly);
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(partition_bdl);
}

int main(int argc, char **argv)
{
    UNITY_MAIN_FUNC(run_all_tests);
    return 0;
}
