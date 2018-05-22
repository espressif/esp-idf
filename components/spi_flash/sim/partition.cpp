#include <assert.h>

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "Flash_Emulator.h"

#define EMULATORS_MAX                  8

static Flash_Emulator* emulators[EMULATORS_MAX];

int esp_flash_create(uint32_t size, uint32_t sector_size, uint32_t write_size)
{
    int handle = -1;

    for (int i = 0; i < EMULATORS_MAX; i++) {
        if (emulators[i] == NULL) {
            emulators[i] = new Flash_Emulator(size, sector_size, write_size);
            handle = i;
            break;
        }
    }
    
    assert(handle != -1);

    return handle;
}

esp_partition_t esp_partition_create(uint32_t size, uint32_t start, int flash)
{ 
    esp_partition_t partition;

    // Fills partition with default values, and attaches the flash_emulator reference
    // to the struct
    partition.type = ESP_PARTITION_TYPE_DATA;
    partition.subtype = ESP_PARTITION_SUBTYPE_ANY;
    partition.address = start;
    partition.size = size;
    partition.handle = flash;

    return partition;
}

void esp_flash_delete(int handle)
{
    delete emulators[handle];
    emulators[handle] = NULL;
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
