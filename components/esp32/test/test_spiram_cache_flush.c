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
#include "esp_heap_caps.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "test_utils.h"


#if CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MEMMAP

#define TSTSZ (16*1024)


volatile static int res[2], err[2];

void tstMem(void *arg) {
    volatile unsigned char *mem=(volatile unsigned char*)arg;
    int p=0;
    while(1) {
        for (int i=0; i<TSTSZ; i++) {
            mem[i]=(i^p);
        }
        for (int i=0; i<TSTSZ; i++) {
            if (mem[i]!=((i^p)&0xff)) {
                printf("Core %d mem err! Got %x espected %x at addr %p\n", xPortGetCoreID(), mem[i], (i^p)&0xff, &mem[i]);
                err[xPortGetCoreID()]++;
            }
        }
        p++;
        res[xPortGetCoreID()]++;
    }
}


TEST_CASE("Spiram cache flush on mmap", "[spiram][ignore]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
#if CONFIG_SPIRAM_USE_CAPS_ALLOC
    mem[0]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
#else
    mem[0]=(void*)0x3f800000;
    mem[1]=(void*)0x3f800000+TSTSZ;
#endif
    TaskHandle_t th[2];
    err[0]=0; err[1]=0;
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tstMem  , "tskone"  , 2048, mem[0], 3, &th[0], 0);
    xTaskCreatePinnedToCore(tstMem  , "tsktwo"  , 2048, mem[1], 3, &th[1], 1);

    const esp_partition_t* part = get_test_data_partition();
    for (int l=0; l<10; l++) {
        for (int p=0; p<4096*1024; p+=65536) {
            const void *out;
            spi_flash_mmap_handle_t h;
            spi_flash_mmap(p, 65536, SPI_FLASH_MMAP_DATA, &out, &h);
            spi_flash_munmap(h);
        }
        printf("%d/10\n", l);
    }

    printf("Checked memory %d and %d times. Errors: %d and %d\n", res[0], res[1], err[0], err[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
#if CONFIG_SPIRAM_USE_CAPS_ALLOC
    free(mem[0]);
    free(mem[1]);
#endif
    TEST_ASSERT(err[0]==0);
    TEST_ASSERT(err[1]==0);
}


#define CYCLES 1024

TEST_CASE("Spiram cache flush on write/read", "[spiram][ignore]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
#if CONFIG_SPIRAM_USE_CAPS_ALLOC
    mem[0]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=pvPortMallocCaps(TSTSZ, MALLOC_CAP_SPIRAM);
#else
    mem[0]=(void*)0x3f800000;
    mem[1]=(void*)0x3f800000+TSTSZ;
#endif
    TaskHandle_t th[2];
    const esp_partition_t* part = get_test_data_partition();
    assert(part!=NULL);

    printf("Erasing sector...\n");
    esp_partition_erase_range(part, 0, 64*1024);
    printf("Erased.\n");
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tstMem  , "tskone"  , 2048, mem[0], 3, &th[0], 0);
    xTaskCreatePinnedToCore(tstMem  , "tsktwo"  , 2048, mem[1], 3, &th[1], 1);
    char buf[512];

    const void *out;
    spi_flash_mmap_handle_t handle;
    esp_partition_mmap(part, 0, 512, SPI_FLASH_MMAP_DATA, &out, &handle);
    for (int i=0; i<CYCLES; i++) {
        printf("%d/%d\n", i, CYCLES);
        esp_partition_write(part, 0, buf, 512);
        esp_partition_read(part, 0, buf, 512);
        vTaskDelay(1);
    }
    spi_flash_munmap(handle);

    printf("Checked memory %d and %d times.\n", res[0], res[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
#if CONFIG_SPIRAM_USE_CAPS_ALLOC
    free(mem[0]);
    free(mem[1]);
#endif
}

#endif  //CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MEMMAP
