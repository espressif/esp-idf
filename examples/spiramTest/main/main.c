/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_heap_caps.h"
#include "esp_task_wdt.h"
#include "include/soc/soc.h"

static void memRegTest(uint8_t *data, size_t size) {

    printf("%p: ", data);

    for (int i = 0; i < size/sizeof(uint8_t); ++i) {
        uint8_t *ptr8 = data + i;
        int var = *ptr8;
        *ptr8 = var + 1;
    }

    for (int i = 0; i < size/sizeof(uint16_t); ++i) {
        uint16_t *ptr16 = (uint16_t*)data + i;
        int var = *ptr16;
        *ptr16 = var + 1;
    }

    for (int i = 0; i < size/sizeof(uint32_t); ++i) {
        uint32_t *ptr32 = (uint32_t*)data + i;
        int var = *ptr32;
        *ptr32 = var + 1;
    }

    int off = 0;
    while (off < (size - 8)) {
        uint8_t *ptr8 = data + off;
        uint16_t *ptr16 = data + off;
        uint32_t *ptr32 = data + off;

        int var = *ptr8;
        *ptr8 = var + 1;

        var = *ptr16;
        *ptr16 = var + 1;

        var = *ptr32;
        *ptr32 = var + 1;
        ++off;
    }
    printf("ok \n");
}

static void memTest(size_t size) {
    uint8_t *data = malloc(size);
    if (!data) {
        printf("ALLOC \n");
        return;
    }
    memRegTest(data, size);
    free(data);
}

void app_main(void) {

#if 01
    memTest(1024);
    memTest(1024 * 1024);
#else
    static const size_t off = 1024;
    uint8_t *data = SOC_EXTRAM_DATA_HIGH - off;

    while (data > SOC_EXTRAM_DATA_LOW) {
        esp_task_wdt_reset();
        memRegTest(data, 128);
        data -= 1024;
    }
#endif

}
