#include <string.h>
#include "unity.h"
#include "wear_levelling.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/cpu.h"

#include "esp32/clk.h"


#define TEST_SECTORS_COUNT 8

static void check_mem_data(wl_handle_t handle, uint32_t init_val, uint32_t* buff)
{
    size_t sector_size = wl_sector_size(handle);

    for (int m=0 ; m < TEST_SECTORS_COUNT ; m++) {
        TEST_ESP_OK(wl_read(handle, sector_size * m, buff, sector_size));
        for (int i=0 ; i< sector_size/sizeof(uint32_t) ; i++) {
            uint32_t compare_val = init_val + i +  m*sector_size;
            TEST_ASSERT_EQUAL( buff[i], compare_val);
        }
    }
}


// We write complete memory with defined data
// And then write one sector many times.
// A data in other secors should be the same.
// We do this also with unmount
TEST_CASE("multiple write is correct", "[wear_levelling]")
{
    const esp_partition_t *partition = get_test_data_partition();
    esp_partition_t fake_partition;
    memcpy(&fake_partition, partition, sizeof(fake_partition));

    fake_partition.size = SPI_FLASH_SEC_SIZE*(4 + TEST_SECTORS_COUNT);

    wl_handle_t handle;
    TEST_ESP_OK(wl_mount(&fake_partition, &handle));

    size_t sector_size = wl_sector_size(handle);
    // Erase 8 sectors
    TEST_ESP_OK(wl_erase_range(handle, 0, sector_size * TEST_SECTORS_COUNT));
    // Write data to all sectors
    printf("Check 1 sector_size=0x%08x\n", sector_size);
    // Set initial random value
    uint32_t init_val = rand();

    uint32_t* buff = (uint32_t*)malloc(sector_size);
    for (int m=0 ; m < TEST_SECTORS_COUNT ; m++) {
        for (int i=0 ; i< sector_size/sizeof(uint32_t) ; i++) {
            buff[i] = init_val + i +  m*sector_size;
        }
        TEST_ESP_OK(wl_erase_range(handle, sector_size*m, sector_size));
        TEST_ESP_OK(wl_write(handle, sector_size*m, buff, sector_size));
    }

    check_mem_data(handle, init_val, buff);

    uint32_t start;
    RSR(CCOUNT, start);


    for (int m=0 ; m< 100000 ; m++) {
        uint32_t sector = m % TEST_SECTORS_COUNT;
        for (int i=0 ; i< sector_size/sizeof(uint32_t) ; i++) {
            buff[i] = init_val + i +  sector*sector_size;
        }
        TEST_ESP_OK(wl_erase_range(handle, sector_size*sector, sector_size));
        TEST_ESP_OK(wl_write(handle, sector_size*sector, buff, sector_size));
        check_mem_data(handle, init_val, buff);

        uint32_t end;
        RSR(CCOUNT, end);
        uint32_t ms = (end - start) / (esp_clk_cpu_freq() / 1000);
        printf("loop %4i pass, time= %ims\n", m, ms);
        if (ms > 10000) {
            break;
        }
    }

    free(buff);
    wl_unmount(handle);
}