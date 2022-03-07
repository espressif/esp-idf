/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Linux host partition API test
 */

#include <string.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_private/partition_linux.h"

int main(int argc, char **argv)
{
    printf("Partition API Linux emulation test: ");

    ////////////////////////////////////////
    //PARTITION LOOKUP:

    //1. esp_partition_find (label=STORAGE)
    esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(iter);

    //2. esp_partition_get (label=STORAGE)
    const esp_partition_t *part = esp_partition_get(iter);
    assert(part);

    //3. esp_partition_iterator_release (label STORAGE iter): assumed OK
    esp_partition_iterator_release(iter);

    ////////////////////////////////////////
    //ITERATORS, PARTITION PROPERTIES:

    //4. esp_partition_find_first (type=APP, subtype=ANY)
    const esp_partition_t *partition_app = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    assert(partition_app);

    //5. enumerate all APP partitions
    iter = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    assert(iter);
    size_t counter = 0;

    while (iter != NULL) {
        const esp_partition_t *part_data = esp_partition_get(iter);
        counter++;
        assert(part_data);
        iter = esp_partition_next(iter);
    }
    esp_partition_iterator_release(iter);

    //6. enumerate all DATA partitions and print details for each
    iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    assert(iter);
    counter = 0;

    while (iter != NULL) {
        const esp_partition_t *part_data = esp_partition_get(iter);
        counter++;
        assert(part_data);
        iter = esp_partition_next(iter);
    }
    esp_partition_iterator_release(iter);

    //7. esp_partition_find_first (type=DATA, label=STORAGE)
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition_data);

    /////////////////////////////////////
    //OPERATIONS

    uint8_t buff[] = "ABCDEFGHIJKLMNOP";
    size_t bufsize = sizeof(buff);
    size_t off = 0x100;

    //8. esp_partition_write/raw
    esp_err_t err = esp_partition_write(partition_data, off, (const void *)buff, bufsize);
    assert(err == ESP_OK);

    //9. esp_partition_read/raw
    uint8_t buffout[32] = {0};
    err = esp_partition_read(partition_data, off, (void *)buffout, bufsize);
    assert(err == ESP_OK);

    //10. esp_partition_erase_range
    uint8_t buferase[bufsize];
    memset(buferase, 0xFF, bufsize);
    memset(buffout, 0, sizeof(buffout));
    size_t sector_off = 0; //erase works per whole sector - offset must be aligned to 4kB boundaries

    err = esp_partition_erase_range(partition_data, sector_off, SPI_FLASH_SEC_SIZE);
    assert(esp_partition_read(partition_data, off, (void *)buffout, bufsize) == ESP_OK);
    assert(err == ESP_OK && memcmp(buffout, buferase, bufsize) == 0);

    //11. esp_partition_verify (partition_data)
    const esp_partition_t *verified_partition = esp_partition_verify(partition_data);
    assert(verified_partition != NULL);

    //12. release SPI FLASH emulation block from memory
    err = esp_partition_file_munmap();
    assert(err == ESP_OK);

    printf("OK\n");

    return 0;
}
