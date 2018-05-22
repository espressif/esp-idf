#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "esp_partition.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs_api.h"

#include "catch.hpp"

TEST_CASE("format disk, open file, write and read file", "[spiffs]")
{
    uint32_t size = 0x00400000;

    int flash_handle = esp_flash_create(size, CONFIG_WL_SECTOR_SIZE, 1);
    esp_partition_t partition = esp_partition_create(size, 0, flash_handle);

    spiffs fs;
    spiffs_config cfg;

    // Configure objects needed by SPIFFS
    esp_spiffs_t esp_user_data;
    esp_user_data.partition = &partition;
    fs.user_data = (void*)&esp_user_data;

    cfg.hal_erase_f = spiffs_api_erase;
    cfg.hal_read_f = spiffs_api_read;
    cfg.hal_write_f = spiffs_api_write;
    cfg.log_block_size = CONFIG_WL_SECTOR_SIZE;
    cfg.log_page_size = CONFIG_SPIFFS_PAGE_SIZE;
    cfg.phys_addr = 0;
    cfg.phys_erase_block = CONFIG_WL_SECTOR_SIZE;
    cfg.phys_size = partition.size;

    uint32_t max_files = 5;

    uint32_t fds_sz = max_files * sizeof(spiffs_fd);
    uint32_t work_sz = cfg.log_page_size * 2;
    uint32_t cache_sz = sizeof(spiffs_cache) + max_files * (sizeof(spiffs_cache_page)
                          + cfg.log_page_size);

    uint8_t *work = (uint8_t*) malloc(work_sz);
    uint8_t *fds = (uint8_t*) malloc(fds_sz); 
    uint8_t *cache = (uint8_t*) malloc(cache_sz); 

    s32_t spiffs_res;

    // Special mounting procedure: mount, format, mount as per
    // https://github.com/pellepl/spiffs/wiki/Using-spiffs
    spiffs_res = SPIFFS_mount(&fs, &cfg, work, fds, fds_sz, 
                            cache, cache_sz, spiffs_api_check);
    REQUIRE(spiffs_res == SPIFFS_ERR_NOT_A_FS);    

    spiffs_res = SPIFFS_format(&fs);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    spiffs_res = SPIFFS_mount(&fs, &cfg, work, fds, fds_sz, 
                            cache, cache_sz, spiffs_api_check);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    // Open test file
    spiffs_res = SPIFFS_open(&fs, "test.txt", SPIFFS_O_CREAT | SPIFFS_O_RDWR, 0);    
    REQUIRE(spiffs_res >= SPIFFS_OK);

    // Write to the test file
    spiffs_file file = spiffs_res;

    const char data[] = "Hello, World!";
    char *read = (char*) malloc(sizeof(data));

    s32_t bw;

    spiffs_res = SPIFFS_write(&fs, file, (void*)data, sizeof(data));    
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == sizeof(data));

    // Set the file object pointer to the beginning
    spiffs_res = SPIFFS_lseek(&fs, file, 0, SPIFFS_SEEK_SET);
    REQUIRE(spiffs_res >= SPIFFS_OK);
    
    // Set the file object pointer to the beginning
    spiffs_res = SPIFFS_read(&fs, file, (void*)read, sizeof(data));
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == sizeof(data));

    // Close the test file
    spiffs_res = SPIFFS_close(&fs, file);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    // Unmount
    SPIFFS_unmount(&fs);

    esp_flash_delete(flash_handle);
    free(read);
}