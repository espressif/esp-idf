#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>
#include <esp_partition.h>
#include <esp_flash_encrypt.h>

#include "test_config.h"

static uint32_t buffer[1024];

/* read-only region used for mmap tests */
static const uint32_t start = 0x100000;
static const uint32_t end = 0x200000;


TEST_CASE("Prepare data for mmap tests", "[mmap]")
{
    srand(0);
    for (int block = start / 0x10000; block < end / 0x10000; ++block) {
        printf("Writing block %d\n", block);
        for (int sector = 0; sector < 16; ++sector) {
            for (uint32_t word = 0; word < 1024; ++word) {
                uint32_t val = rand();
                if (block == start / 0x10000 && sector == 0 && word == 0) {
                    printf("first word: %08x\n", val);
                }
                buffer[word] = val;
            }
            uint32_t abs_sector = (block) * 16 + sector;
            printf("Writing sector %d\n", abs_sector);
            ESP_ERROR_CHECK( spi_flash_erase_sector((uint16_t) abs_sector) );
            ESP_ERROR_CHECK( spi_flash_write(abs_sector * SPI_FLASH_SEC_SIZE, (const uint8_t *) buffer, sizeof(buffer)) );
        }
    }
}

TEST_CASE("Can mmap into data address space", "[mmap]")
{

    printf("Mapping %x (+%x)\n", start, end - start);
    spi_flash_mmap_handle_t handle1;
    const void *ptr1;
    ESP_ERROR_CHECK( spi_flash_mmap(start, end - start, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res: handle=%d ptr=%p\n", handle1, ptr1);

    spi_flash_mmap_dump();

    srand(0);
    const uint32_t *data = (const uint32_t *) ptr1;
    for (int block = 0; block < (end - start) / 0x10000; ++block) {
        for (int sector = 0; sector < 16; ++sector) {
            for (uint32_t word = 0; word < 1024; ++word) {
                TEST_ASSERT_EQUAL_UINT32(rand(), data[(block * 16 + sector) * 1024 + word]);
            }
        }
    }
    printf("Mapping %x (+%x)\n", start - 0x10000, 0x20000);
    spi_flash_mmap_handle_t handle2;
    const void *ptr2;
    ESP_ERROR_CHECK( spi_flash_mmap(start - 0x10000, 0x20000, SPI_FLASH_MMAP_DATA, &ptr2, &handle2) );
    printf("mmap_res: handle=%d ptr=%p\n", handle2, ptr2);

    TEST_ASSERT_EQUAL_HEX32(start - 0x10000, spi_flash_cache2phys(ptr2));
    TEST_ASSERT_EQUAL_PTR(ptr2, spi_flash_phys2cache(start - 0x10000, SPI_FLASH_MMAP_DATA));
    TEST_ASSERT_EQUAL_PTR((intptr_t)ptr2 + 0x12000, spi_flash_phys2cache(start - 0x10000 + 0x12000, SPI_FLASH_MMAP_DATA));

    spi_flash_mmap_dump();

    printf("Mapping %x (+%x)\n", start, 0x10000);
    spi_flash_mmap_handle_t handle3;
    const void *ptr3;
    ESP_ERROR_CHECK( spi_flash_mmap(start, 0x10000, SPI_FLASH_MMAP_DATA, &ptr3, &handle3) );
    printf("mmap_res: handle=%d ptr=%p\n", handle3, ptr3);

    TEST_ASSERT_EQUAL_HEX32(start, spi_flash_cache2phys(ptr3));
    TEST_ASSERT_EQUAL_PTR(ptr3, spi_flash_phys2cache(start, SPI_FLASH_MMAP_DATA));
    TEST_ASSERT_EQUAL_PTR((intptr_t)ptr3 + 0x4444, spi_flash_phys2cache(start + 0x4444, SPI_FLASH_MMAP_DATA));

    spi_flash_mmap_dump();

    printf("Unmapping handle1\n");
    spi_flash_munmap(handle1);
    spi_flash_mmap_dump();

    printf("Unmapping handle2\n");
    spi_flash_munmap(handle2);
    spi_flash_mmap_dump();

    printf("Unmapping handle3\n");
    spi_flash_munmap(handle3);

    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(start, SPI_FLASH_MMAP_DATA));
}

TEST_CASE("flash_mmap invalidates just-written data", "[spi_flash]")
{
    spi_flash_mmap_handle_t handle1;
    const void *ptr1;

    const size_t test_size = 128;

    if (esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption enabled, spi_flash_write_encrypted() test won't pass as-is");
    }

    ESP_ERROR_CHECK( spi_flash_erase_sector(TEST_REGION_START / SPI_FLASH_SEC_SIZE) );

    /* map erased test region to ptr1 */
    ESP_ERROR_CHECK( spi_flash_mmap(TEST_REGION_START, test_size, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1: handle=%d ptr=%p\n", handle1, ptr1);

    /* verify it's all 0xFF */
    for (int i = 0; i < test_size; i++) {
        TEST_ASSERT_EQUAL_HEX(0xFF, ((uint8_t *)ptr1)[i]);
    }

    /* unmap the erased region */
    spi_flash_munmap(handle1);

    /* write flash region to 0xEE */
    uint8_t buf[test_size];
    memset(buf, 0xEE, test_size);
    ESP_ERROR_CHECK( spi_flash_write(TEST_REGION_START, buf, test_size) );

    /* re-map the test region at ptr1.

       this is a fresh mmap call so should trigger a cache flush,
       ensuring we see the updated flash.
    */
    ESP_ERROR_CHECK( spi_flash_mmap(TEST_REGION_START, test_size, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1 #2: handle=%d ptr=%p\n", handle1, ptr1);

    /* assert that ptr1 now maps to the new values on flash,
       ie contents of buf array.
    */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, ptr1, test_size);

    spi_flash_munmap(handle1);
}

TEST_CASE("phys2cache/cache2phys basic checks", "[spi_flash]")
{
    uint8_t buf_a[32], buf_b[32];

    /* esp_partition_find is in IROM */
    uint32_t phys = spi_flash_cache2phys(esp_partition_find);
    TEST_ASSERT_NOT_EQUAL(SPI_FLASH_CACHE2PHYS_FAIL, phys);
    TEST_ASSERT_EQUAL_PTR(esp_partition_find, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_INST));
    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_DATA));

    /* Read the flash @ 'phys' and compare it to the data we get via cache */
    memcpy(buf_a, esp_partition_find, sizeof(buf_a));
    spi_flash_read(phys, buf_b, sizeof(buf_b));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf_a, buf_b, sizeof(buf_b));

    /* spi_flash_mmap is in IRAM */
    printf("%p\n", spi_flash_mmap);
    TEST_ASSERT_EQUAL_HEX32(SPI_FLASH_CACHE2PHYS_FAIL,
                            spi_flash_cache2phys(spi_flash_mmap));

    /* 'start' should be in DROM */
    phys = spi_flash_cache2phys(&start);
    TEST_ASSERT_NOT_EQUAL(SPI_FLASH_CACHE2PHYS_FAIL, phys);
    TEST_ASSERT_EQUAL_PTR(&start,
                          spi_flash_phys2cache(phys, SPI_FLASH_MMAP_DATA));
    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_INST));

    /* Read the flash @ 'phys' and compare it to the data we get via cache */
    memcpy(buf_a, &start, sizeof(start));
    spi_flash_read(phys, buf_b, sizeof(start));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf_a, buf_b, sizeof(start));
}

TEST_CASE("mmap consistent with phys2cache/cache2phys", "[spi_flash]")
{
    spi_flash_mmap_handle_t handle = 0;
    const void *ptr = NULL;
    const size_t test_size = 2 * SPI_FLASH_MMU_PAGE_SIZE;

    TEST_ASSERT_EQUAL_HEX(SPI_FLASH_CACHE2PHYS_FAIL, spi_flash_cache2phys(ptr));

    ESP_ERROR_CHECK( spi_flash_mmap(TEST_REGION_START, test_size, SPI_FLASH_MMAP_DATA, &ptr, &handle) );
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_NOT_EQUAL(0, handle);

    TEST_ASSERT_EQUAL_HEX(TEST_REGION_START, spi_flash_cache2phys(ptr));
    TEST_ASSERT_EQUAL_HEX(TEST_REGION_START + 1024, spi_flash_cache2phys((void *)((intptr_t)ptr + 1024)));
    TEST_ASSERT_EQUAL_HEX(TEST_REGION_START + 3000, spi_flash_cache2phys((void *)((intptr_t)ptr + 3000)));
    /* this pointer lands in a different MMU table entry */
    TEST_ASSERT_EQUAL_HEX(TEST_REGION_START + test_size - 4, spi_flash_cache2phys((void *)((intptr_t)ptr + test_size - 4)));

    spi_flash_munmap(handle);

    TEST_ASSERT_EQUAL_HEX(SPI_FLASH_CACHE2PHYS_FAIL, spi_flash_cache2phys(ptr));
}
