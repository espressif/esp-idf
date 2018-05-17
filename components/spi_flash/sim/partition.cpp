#include <assert.h>

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "Flash_Emulator.h"

#define PARTITIONS_MAX                  8

static Flash_Emulator* emulators[PARTITIONS_MAX];

esp_partition_t esp_partition_create(uint32_t size, uint32_t start)
{ 
    int handle = -1;

    for (int i = 0; i < PARTITIONS_MAX; i++) {
        if (emulators[i] == NULL) {
            emulators[i] = new Flash_Emulator(start + size, SPI_FLASH_SEC_SIZE, SPI_FLASH_WRITE_SIZE);
            handle = i;
            break;
        }
    }
    
    assert(handle != -1);

    esp_partition_t partition;

    // Fills partition with default values, and attaches the flash_emulator reference
    // to the struct
    partition.type = ESP_PARTITION_TYPE_DATA;
    partition.subtype = ESP_PARTITION_SUBTYPE_DATA_FAT;
    partition.address = start;
    partition.size = size;
    partition.handle = handle;

    return partition;
}

void esp_partition_delete(esp_partition_t partition)
{
    delete emulators[partition.handle];
    emulators[partition.handle] = NULL;
}

esp_err_t esp_partition_write(const esp_partition_t* partition,
                             size_t dst_offset, const void* src, size_t size)
{
    emulators[partition->handle]->write(dst_offset, src, size);
    return ESP_OK;
}

esp_err_t esp_partition_erase_range(const esp_partition_t* partition,
                                    uint32_t start_addr, uint32_t size)
{
    emulators[partition->handle]->erase_range(start_addr, size);
    return ESP_OK;
}                                    

esp_err_t esp_partition_read(const esp_partition_t* partition,
                             size_t src_offset, void* dst, size_t size)
{
    emulators[partition->handle]->read(src_offset, dst, size);
    return ESP_OK;
}