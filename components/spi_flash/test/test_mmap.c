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

#include "test_utils.h"

static uint32_t buffer[1024];

/* read-only region used for mmap tests, intialised in setup_mmap_tests() */
static uint32_t start;
static uint32_t end;

static spi_flash_mmap_handle_t handle1, handle2, handle3;

static void setup_mmap_tests()
{
    if (start == 0) {
        const esp_partition_t *part = get_test_data_partition();
        start = part->address;
        end = part->address + part->size;
        printf("Test data partition @ 0x%x - 0x%x\n", start, end);
    }
    TEST_ASSERT(end > start);
    TEST_ASSERT(end - start >= 512*1024);

    /* clean up any mmap handles left over from failed tests */
    if (handle1) {
        spi_flash_munmap(handle1);
        handle1 = 0;
    }
    if (handle2) {
        spi_flash_munmap(handle2);
        handle2 = 0;
    }
    if (handle3) {
        spi_flash_munmap(handle3);
        handle3 = 0;
    }

    /* prepare flash contents */
    srand(0);
    for (int block = start / 0x10000; block < end / 0x10000; ++block) {
        for (int sector = 0; sector < 16; ++sector) {
            uint32_t abs_sector = (block * 16) + sector;
            uint32_t sector_offs = abs_sector * SPI_FLASH_SEC_SIZE;
            bool sector_needs_write = false;

            ESP_ERROR_CHECK( spi_flash_read(sector_offs, buffer, sizeof(buffer)) );

            for (uint32_t word = 0; word < 1024; ++word) {
                uint32_t val = rand();
                if (block == start / 0x10000 && sector == 0 && word == 0) {
                    printf("setup_mmap_tests(): first prepped word: 0x%08x (flash holds 0x%08x)\n", val, buffer[word]);
                }
                if (buffer[word] != val) {
                    buffer[word] = val;
                    sector_needs_write = true;
                }
            }
            /* Only rewrite the sector if it has changed */
            if (sector_needs_write) {
                printf("setup_mmap_tests(): Prepping sector %d\n", abs_sector);
                ESP_ERROR_CHECK( spi_flash_erase_sector((uint16_t) abs_sector) );
                ESP_ERROR_CHECK( spi_flash_write(sector_offs, (const uint8_t *) buffer, sizeof(buffer)) );
            }
        }
    }
}

TEST_CASE("Can mmap into data address space", "[spi_flash]")
{
    setup_mmap_tests();

    printf("Mapping %x (+%x)\n", start, end - start);
    const void *ptr1;
    ESP_ERROR_CHECK( spi_flash_mmap(start, end - start, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res: handle=%d ptr=%p\n", handle1, ptr1);

    spi_flash_mmap_dump();

    srand(0);
    const uint32_t *data = (const uint32_t *) ptr1;
    for (int block = 0; block < (end - start) / 0x10000; ++block) {
        printf("block %d\n", block);
        for (int sector = 0; sector < 16; ++sector) {
            printf("sector %d\n", sector);
            for (uint32_t word = 0; word < 1024; ++word) {
                TEST_ASSERT_EQUAL_HEX32(rand(), data[(block * 16 + sector) * 1024 + word]);
            }
        }
    }
    printf("Mapping %x (+%x)\n", start - 0x10000, 0x20000);
    const void *ptr2;
    ESP_ERROR_CHECK( spi_flash_mmap(start - 0x10000, 0x20000, SPI_FLASH_MMAP_DATA, &ptr2, &handle2) );
    printf("mmap_res: handle=%d ptr=%p\n", handle2, ptr2);

    TEST_ASSERT_EQUAL_HEX32(start - 0x10000, spi_flash_cache2phys(ptr2));
    TEST_ASSERT_EQUAL_PTR(ptr2, spi_flash_phys2cache(start - 0x10000, SPI_FLASH_MMAP_DATA));

    spi_flash_mmap_dump();

    printf("Mapping %x (+%x)\n", start, 0x10000);
    const void *ptr3;
    ESP_ERROR_CHECK( spi_flash_mmap(start, 0x10000, SPI_FLASH_MMAP_DATA, &ptr3, &handle3) );
    printf("mmap_res: handle=%d ptr=%p\n", handle3, ptr3);

    TEST_ASSERT_EQUAL_HEX32(start, spi_flash_cache2phys(ptr3));
    TEST_ASSERT_EQUAL_PTR(ptr3, spi_flash_phys2cache(start, SPI_FLASH_MMAP_DATA));
    TEST_ASSERT_EQUAL_PTR((intptr_t)ptr3 + 0x4444, spi_flash_phys2cache(start + 0x4444, SPI_FLASH_MMAP_DATA));

    spi_flash_mmap_dump();

    printf("Unmapping handle1\n");
    spi_flash_munmap(handle1);
    handle1 = 0;
    spi_flash_mmap_dump();

    printf("Unmapping handle2\n");
    spi_flash_munmap(handle2);
    handle2 = 0;
    spi_flash_mmap_dump();

    printf("Unmapping handle3\n");
    spi_flash_munmap(handle3);
    handle3 = 0;

    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(start, SPI_FLASH_MMAP_DATA));
}

TEST_CASE("Can mmap into instruction address space", "[mmap]")
{
    setup_mmap_tests();

    printf("Mapping %x (+%x)\n", start, end - start);
    spi_flash_mmap_handle_t handle1;
    const void *ptr1;
    ESP_ERROR_CHECK( spi_flash_mmap(start, end - start, SPI_FLASH_MMAP_INST, &ptr1, &handle1) );
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
    spi_flash_mmap_dump();

    printf("Mapping %x (+%x)\n", start, 0x10000);
    spi_flash_mmap_handle_t handle3;
    const void *ptr3;
    ESP_ERROR_CHECK( spi_flash_mmap(start, 0x10000, SPI_FLASH_MMAP_DATA, &ptr3, &handle3) );
    printf("mmap_res: handle=%d ptr=%p\n", handle3, ptr3);
    spi_flash_mmap_dump();

    printf("Unmapping handle1\n");
    spi_flash_munmap(handle1);
    spi_flash_mmap_dump();

    printf("Unmapping handle2\n");
    spi_flash_munmap(handle2);
    spi_flash_mmap_dump();

    printf("Unmapping handle3\n");
    spi_flash_munmap(handle3);

}

TEST_CASE("Can mmap unordered pages into contiguous memory", "[spi_flash]")
{
    int nopages;
    int *pages;
    int startpage;

    setup_mmap_tests();
    nopages=(end-start)/SPI_FLASH_MMU_PAGE_SIZE;
    pages=alloca(sizeof(int)*nopages);

    startpage=start/SPI_FLASH_MMU_PAGE_SIZE;

    //make inverse mapping: virt 0 -> page (nopages-1), virt 1 -> page (nopages-2), ...
    for (int i=0; i<nopages; i++) {
        pages[i]=startpage+(nopages-1)-i;
        printf("Offset %x page %d\n", i*0x10000, pages[i]);
    }
    
    printf("Attempting mapping of unordered pages to contiguous memory area\n");

    spi_flash_mmap_handle_t handle1;
    const void *ptr1;
    ESP_ERROR_CHECK( spi_flash_mmap_pages(pages, nopages, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res: handle=%d ptr=%p\n", handle1, ptr1);

    spi_flash_mmap_dump();

    srand(0);
    const uint32_t *data = (const uint32_t *) ptr1;
    for (int block = 0; block < nopages; ++block) {
        for (int sector = 0; sector < 16; ++sector) {
            for (uint32_t word = 0; word < 1024; ++word) {
                TEST_ASSERT_EQUAL_UINT32(rand(), data[(((nopages-1)-block) * 16 + sector) * 1024 + word]);
            }
        }
    }

    printf("Unmapping handle1\n");
    spi_flash_munmap(handle1);
    spi_flash_mmap_dump();
}


TEST_CASE("flash_mmap invalidates just-written data", "[spi_flash]")
{
    const void *ptr1;

    const size_t test_size = 128;

    setup_mmap_tests();

    if (esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption enabled, spi_flash_write_encrypted() test won't pass as-is");
    }

    ESP_ERROR_CHECK( spi_flash_erase_sector(start / SPI_FLASH_SEC_SIZE) );

    /* map erased test region to ptr1 */
    ESP_ERROR_CHECK( spi_flash_mmap(start, test_size, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1: handle=%d ptr=%p\n", handle1, ptr1);

    /* verify it's all 0xFF */
    for (int i = 0; i < test_size; i++) {
        TEST_ASSERT_EQUAL_HEX(0xFF, ((uint8_t *)ptr1)[i]);
    }

    /* unmap the erased region */
    spi_flash_munmap(handle1);
    handle1 = 0;

    /* write flash region to 0xEE */
    uint8_t buf[test_size];
    memset(buf, 0xEE, test_size);
    ESP_ERROR_CHECK( spi_flash_write(start, buf, test_size) );

    /* re-map the test region at ptr1.

       this is a fresh mmap call so should trigger a cache flush,
       ensuring we see the updated flash.
    */
    ESP_ERROR_CHECK( spi_flash_mmap(start, test_size, SPI_FLASH_MMAP_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1 #2: handle=%d ptr=%p\n", handle1, ptr1);

    /* assert that ptr1 now maps to the new values on flash,
       ie contents of buf array.
    */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, ptr1, test_size);

    spi_flash_munmap(handle1);
    handle1 = 0;
}

TEST_CASE("phys2cache/cache2phys basic checks", "[spi_flash]")
{
    uint8_t buf[64];

    static const uint8_t constant_data[] = { 1, 2, 3, 7, 11, 16, 3, 88 };

    /* esp_partition_find is in IROM */
    uint32_t phys = spi_flash_cache2phys(esp_partition_find);
    TEST_ASSERT_NOT_EQUAL(SPI_FLASH_CACHE2PHYS_FAIL, phys);
    TEST_ASSERT_EQUAL_PTR(esp_partition_find, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_INST));
    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_DATA));

    /* Read the flash @ 'phys' and compare it to the data we get via regular cache access */
    spi_flash_read(phys, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX32_ARRAY((void *)esp_partition_find, buf, sizeof(buf)/sizeof(uint32_t));

    /* spi_flash_mmap is in IRAM */
    printf("%p\n", spi_flash_mmap);
    TEST_ASSERT_EQUAL_HEX32(SPI_FLASH_CACHE2PHYS_FAIL,
                            spi_flash_cache2phys(spi_flash_mmap));

    /* 'constant_data' should be in DROM */
    phys = spi_flash_cache2phys(&constant_data);
    TEST_ASSERT_NOT_EQUAL(SPI_FLASH_CACHE2PHYS_FAIL, phys);
    TEST_ASSERT_EQUAL_PTR(&constant_data,
                          spi_flash_phys2cache(phys, SPI_FLASH_MMAP_DATA));
    TEST_ASSERT_EQUAL_PTR(NULL, spi_flash_phys2cache(phys, SPI_FLASH_MMAP_INST));

    /* Read the flash @ 'phys' and compare it to the data we get via normal cache access */
    spi_flash_read(phys, buf, sizeof(constant_data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(constant_data, buf, sizeof(constant_data));
}

TEST_CASE("mmap consistent with phys2cache/cache2phys", "[spi_flash]")
{
    const void *ptr = NULL;
    const size_t test_size = 2 * SPI_FLASH_MMU_PAGE_SIZE;

    setup_mmap_tests();

    TEST_ASSERT_EQUAL_HEX(SPI_FLASH_CACHE2PHYS_FAIL, spi_flash_cache2phys(ptr));

    ESP_ERROR_CHECK( spi_flash_mmap(start, test_size, SPI_FLASH_MMAP_DATA, &ptr, &handle1) );
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_NOT_EQUAL(0, handle1);

    TEST_ASSERT_EQUAL_HEX(start, spi_flash_cache2phys(ptr));
    TEST_ASSERT_EQUAL_HEX(start + 1024, spi_flash_cache2phys((void *)((intptr_t)ptr + 1024)));
    TEST_ASSERT_EQUAL_HEX(start + 3000, spi_flash_cache2phys((void *)((intptr_t)ptr + 3000)));
    /* this pointer lands in a different MMU table entry */
    TEST_ASSERT_EQUAL_HEX(start + test_size - 4, spi_flash_cache2phys((void *)((intptr_t)ptr + test_size - 4)));

    spi_flash_munmap(handle1);
    handle1 = 0;

    TEST_ASSERT_EQUAL_HEX(SPI_FLASH_CACHE2PHYS_FAIL, spi_flash_cache2phys(ptr));
}

TEST_CASE("munmap followed by mmap flushes cache", "[spi_flash]")
{
    setup_mmap_tests();

    const esp_partition_t *p = get_test_data_partition();

    const uint32_t* data;
    spi_flash_mmap_handle_t handle;
    TEST_ESP_OK( esp_partition_mmap(p, 0, SPI_FLASH_MMU_PAGE_SIZE,
            SPI_FLASH_MMAP_DATA, (const void **) &data, &handle) );
    uint32_t buf[16];
    memcpy(buf, data, sizeof(buf));

    spi_flash_munmap(handle);
    TEST_ESP_OK( esp_partition_mmap(p, SPI_FLASH_MMU_PAGE_SIZE, SPI_FLASH_MMU_PAGE_SIZE,
            SPI_FLASH_MMAP_DATA, (const void **) &data, &handle) );
    TEST_ASSERT_NOT_EQUAL(0, memcmp(buf, data, sizeof(buf)));
}

