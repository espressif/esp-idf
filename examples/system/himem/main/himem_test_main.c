/* Himem API example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_heap_caps.h"
#include "esp32/spiram.h"
#include "sdkconfig.h"
#include "esp32/himem.h"


//Fill memory with pseudo-random data generated from the given seed.
//Fills the memory in 32-bit words for speed.
static void fill_mem_seed(int seed, void *mem, int len)
{
    uint32_t *p = (uint32_t *)mem;
    unsigned int rseed = seed ^ 0xa5a5a5a5;
    for (int i = 0; i < len / 4; i++) {
        *p++ = rand_r(&rseed);
    }
}

//Check the memory filled by fill_mem_seed. Returns true if the data matches the data
//that fill_mem_seed wrote (when given the same seed).
//Returns true if there's a match, false when the region differs from what should be there.
static bool check_mem_seed(int seed, void *mem, int len, int phys_addr)
{
    uint32_t *p = (uint32_t *)mem;
    unsigned int rseed = seed ^ 0xa5a5a5a5;
    for (int i = 0; i < len / 4; i++) {
        uint32_t ex = rand_r(&rseed);
        if (ex != *p) {
            printf("check_mem_seed: %x has 0x%08x expected 0x%08x\n", phys_addr+((char*)p-(char*)mem), *p, ex);
            return false;
        }
        p++;
    }
    return true;
}

//Allocate a himem region, fill it with data, check it and release it.
static bool test_region(int check_size, int seed)
{
    esp_himem_handle_t mh; //Handle for the address space we're using
    esp_himem_rangehandle_t rh; //Handle for the actual RAM.
    bool ret = true;

    //Allocate the memory we're going to check.
    ESP_ERROR_CHECK(esp_himem_alloc(check_size, &mh));
    //Allocate a block of address range
    ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &rh));
    for (int i = 0; i < check_size; i += ESP_HIMEM_BLKSZ) {
        uint32_t *ptr = NULL;
        //Map in block, write pseudo-random data, unmap block.
        ESP_ERROR_CHECK(esp_himem_map(mh, rh, i, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
        fill_mem_seed(i ^ seed, ptr, ESP_HIMEM_BLKSZ); //
        ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
    }
    vTaskDelay(5); //give the OS some time to do things so the task watchdog doesn't bark
    for (int i = 0; i < check_size; i += ESP_HIMEM_BLKSZ) {
        uint32_t *ptr;
        //Map in block, check against earlier written pseudo-random data, unmap block.
        ESP_ERROR_CHECK(esp_himem_map(mh, rh, i, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
        if (!check_mem_seed(i ^ seed, ptr, ESP_HIMEM_BLKSZ, i)) {
            printf("Error in block %d\n", i / ESP_HIMEM_BLKSZ);
            ret = false;
        }
        ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
        if (!ret) break; //don't check rest of blocks if error occurred
    }
    //Okay, all done!
    ESP_ERROR_CHECK(esp_himem_free(mh));
    ESP_ERROR_CHECK(esp_himem_free_map_range(rh));
    return ret;
}


void app_main(void)
{
    size_t memcnt=esp_himem_get_phys_size();
    size_t memfree=esp_himem_get_free_size();
    printf("Himem has %dKiB of memory, %dKiB of which is free. Testing the free memory...\n", (int)memcnt/1024, (int)memfree/1024);
    assert(test_region(memfree, 0xaaaa));
    printf("Done!\n");
}

