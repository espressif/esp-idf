/*
This code tests the interaction between PSRAM and SPI flash routines.
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rom/ets_sys.h"
#include "esp_heap_alloc_caps.h"
#include "esp_spi_flash.h"


#define TSTSZ (16*1024)


volatile static int res[2];

void tstMem(void *arg) {
    volatile unsigned char *mem=(volatile unsigned char*)arg;
    int p=0;
    while(1) {
        for (int i=0; i<TSTSZ; i++) {
            mem[i]=(i^p);
        }
//      vTaskDelay(1);
        for (int i=0; i<TSTSZ; i++) {
            if (mem[i]!=((i^p)&0xff)) {
                printf("Core %d mem err! Got %x espected %x at addr %p\n", xPortGetCoreID(), mem[i], (i^p)&0xff, &mem[i]);
            }
        }
        p++;
        res[xPortGetCoreID()]++;
    }
}


TEST_CASE("PSram cache flush on mmap", "[psram][ignore]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
    mem[0]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    TaskHandle_t th[2];
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tstMem  , "tskone"  , 2048, mem[0], 3, &th[0], 0);
    xTaskCreatePinnedToCore(tstMem  , "tsktwo"  , 2048, mem[1], 3, &th[1], 1);
    char buf[512];

    for (int i=0; i<4*1024*1024; i+=512) {
        spi_flash_read(i, buf, 512);
        vTaskDelay(1);
    }

    printf("Checked memory %d and %d times.\n", res[0], res[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
    free(mem[0]);
    free(mem[1]);

}


#define FLASHPOS (2*1024*1024-512)
#define CYCLES 1024

TEST_CASE("PSram cache flush on write/read", "[psram][ignore]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
    mem[0]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    TaskHandle_t th[2];
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tstMem  , "tskone"  , 2048, mem[0], 3, &th[0], 0);
    xTaskCreatePinnedToCore(tstMem  , "tsktwo"  , 2048, mem[1], 3, &th[1], 1);
    char buf[512];

    printf("Erasing sector...\n");
    spi_flash_erase_sector(FLASHPOS/4096);
    printf("Erased.\n");
    for (int i=0; i<CYCLES; i++) {
        printf("%d/%d\n", i, CYCLES);
        spi_flash_write(FLASHPOS, buf, 512);
        spi_flash_read(i, buf, 512);
        vTaskDelay(1);
    }

    printf("Checked memory %d and %d times.\n", res[0], res[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
    free(mem[0]);
    free(mem[1]);
}

