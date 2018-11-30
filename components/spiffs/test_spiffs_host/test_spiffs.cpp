#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "esp_partition.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs_api.h"

#include "catch.hpp"

extern "C" void init_spi_flash(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);

TEST_CASE("format disk, open file, write and read file", "[spiffs]")
{
    init_spi_flash(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    spiffs fs;
    spiffs_config cfg;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");

    // Configure objects needed by SPIFFS
    esp_spiffs_t esp_user_data;
    esp_user_data.partition = partition;
    fs.user_data = (void*)&esp_user_data;

    cfg.hal_erase_f = spiffs_api_erase;
    cfg.hal_read_f = spiffs_api_read;
    cfg.hal_write_f = spiffs_api_write;
    cfg.log_block_size = CONFIG_WL_SECTOR_SIZE;
    cfg.log_page_size = CONFIG_SPIFFS_PAGE_SIZE;
    cfg.phys_addr = 0;
    cfg.phys_erase_block = CONFIG_WL_SECTOR_SIZE;
    cfg.phys_size = partition->size;

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

    // Generate data
    spiffs_file file = spiffs_res;

    uint32_t data_size = 100000;

    char *data = (char*) malloc(data_size);
    char *read = (char*) malloc(data_size);

    for(uint32_t i = 0; i < data_size; i += sizeof(i))
    {
        *((uint32_t*)(data + i)) = i;
    }

    s32_t bw;

    // Write data to file
    spiffs_res = SPIFFS_write(&fs, file, (void*)data, data_size);    
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == data_size);

    // Set the file object pointer to the beginning
    spiffs_res = SPIFFS_lseek(&fs, file, 0, SPIFFS_SEEK_SET);
    REQUIRE(spiffs_res >= SPIFFS_OK);
    
    // Read the file
    spiffs_res = SPIFFS_read(&fs, file, (void*)read, data_size);
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == data_size);

    // Close the test file
    spiffs_res = SPIFFS_close(&fs, file);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    REQUIRE(memcmp(data, read, data_size) == 0);

    // Unmount
    SPIFFS_unmount(&fs);

    free(read);
    free(data);
}
