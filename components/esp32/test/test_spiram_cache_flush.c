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

#if CONFIG_SPIRAM_SUPPORT

#if CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC
#define USE_CAPS_ALLOC 1
#endif // CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC

#define TSTSZ (16*1024)

#if !CONFIG_FREERTOS_UNICORE

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


TEST_CASE("Spiram cache flush on mmap", "[spiram]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
#if USE_CAPS_ALLOC
    printf("Allocating SPI RAM chunk...\n");
    mem[0]=heap_caps_malloc(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=heap_caps_malloc(TSTSZ, MALLOC_CAP_SPIRAM);
#else
    mem[0]=(void*)0x3f800000;
    mem[1]=(void*)0x3f800000+TSTSZ;
#endif
    assert(mem[0]);
    assert(mem[1]);
    TaskHandle_t th[2];
    err[0]=0; err[1]=0;
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tstMem  , "tskone"  , 2048, mem[0], 3, &th[0], 0);
    xTaskCreatePinnedToCore(tstMem  , "tsktwo"  , 2048, mem[1], 3, &th[1], 1);

    for (int l=0; l<10; l++) {
        for (int p=0; p<4096*1024; p+=65536) {
            const void *out;
            spi_flash_mmap_handle_t h;
            spi_flash_mmap(p, 65536, SPI_FLASH_MMAP_DATA, &out, &h);
            spi_flash_munmap(h);
        }
    }

    printf("Checked memory %d and %d times. Errors: %d and %d\n", res[0], res[1], err[0], err[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
#if USE_CAPS_ALLOC
    free(mem[0]);
    free(mem[1]);
#endif
    TEST_ASSERT(err[0]==0);
    TEST_ASSERT(err[1]==0);
}


#define CYCLES 1024

TEST_CASE("Spiram cache flush on write/read", "[spiram]")
{
    void *mem[2];
    res[0]=0; res[1]=0;
#if USE_CAPS_ALLOC
    printf("Allocating SPI RAM chunk...\n");
    mem[0]=heap_caps_malloc(TSTSZ, MALLOC_CAP_SPIRAM);
    mem[1]=heap_caps_malloc(TSTSZ, MALLOC_CAP_SPIRAM);
#else
    mem[0]=(void*)0x3f800000;
    mem[1]=(void*)0x3f800000+TSTSZ;
#endif
    assert(mem[0]);
    assert(mem[1]);
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
        esp_partition_write(part, 0, buf, 512);
        esp_partition_read(part, 0, buf, 512);
        vTaskDelay(1);
    }
    spi_flash_munmap(handle);

    printf("Checked memory %d and %d times.\n", res[0], res[1]);

    vTaskDelete(th[0]);
    vTaskDelete(th[1]);
#if USE_CAPS_ALLOC
    free(mem[0]);
    free(mem[1]);
#endif
}
#endif // !CONFIG_FREERTOS_UNICORE

IRAM_ATTR TEST_CASE("Spiram memcmp weirdness at 80MHz", "[spiram]") {
    char *mem1=malloc(0x10000);
#if USE_CAPS_ALLOC
    char *mem2=heap_caps_malloc(0x10000, MALLOC_CAP_SPIRAM);
#else
    char *mem2=(void*)0x3f800000;
#endif

#if !CONFIG_SPIRAM_SPEED_80M
    printf("**** WARNING **** Spi memory isn't running at 80MHz, so this test is somewhat meaningless.\n");
#endif

    printf("RAM: Got %p and %p\n", mem1, mem2);
    assert(mem1);
    assert(mem2);
    for (int i=0; i<0x10000; i++) mem1[i]=i^0xAAAAAAAA;
    
    for (int cycle=1; cycle<100; cycle++) {
        memcpy(mem2, mem1, 0x10000);
        if (memcmp(mem1, mem2, 0x10000)!=0) {
            printf("Memcmp failed! Cycle %d\n", cycle);
            for (int i=0; i<0x10000; i++) {
                if (mem1[i]!=mem2[i]) {
                    printf("Found real difference at index %d: 0x%x vs 0x%x\n", i, mem1[i], mem2[i]);
                    break;
                }
            }
        }
    }

    free(mem1);
#if USE_CAPS_ALLOC
    free(mem2);
#endif
}

#endif // CONFIG_SPIRAM_SUPPORT
