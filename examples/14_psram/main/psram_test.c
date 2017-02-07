/* Timer group-hardware timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "psram.h"

#define TEST_DATA_LEN           1024
#define PSRAM_START_ADR         (0x3F800000)        //0x3F800000 is psram start address

#define mutex_t                 xSemaphoreHandle
mutex_t psram_mem_mutex         = NULL;

uint8_t inRam0[TEST_DATA_LEN]   = {0};
uint8_t inRam1[TEST_DATA_LEN]   = {0};

int  mutex_init(mutex_t *pxMutex);
void mutex_lock(mutex_t *pxMutex);
void mutex_unlock(mutex_t *pxMutex);

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return 0: successed;1:failed;
 */
int mutex_init(mutex_t *pxMutex)
{
    int xReturn = -1;
    *pxMutex = xSemaphoreCreateMutex();
    if (*pxMutex != NULL) {
        xReturn = 0;
    }
    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock
 */
void mutex_lock(mutex_t *pxMutex)
{
    while (xSemaphoreTake(*pxMutex, portMAX_DELAY) != pdPASS);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock
 */
void mutex_unlock(mutex_t *pxMutex)
{
    xSemaphoreGive(*pxMutex);
}

/** 
 * @brief Memcpy with a cache, atomic protected, speaking, reading and writing
 */
void psram_cache_memcpy(uint8_t *dst, uint8_t *src, uint16_t len)
{
    if(psram_mem_mutex == NULL)
    {
        mutex_init(&psram_mem_mutex);
    }
    mutex_lock(&psram_mem_mutex);
    memcpy(dst, src, len);
    mutex_unlock(&psram_mem_mutex);
}

/** 
 * @brief Memset with a cache, atomic protected, speaking, reading and writing
 */
void psram_cache_memset(uint8_t *addr, uint8_t c, uint32_t len)
{
    if(psram_mem_mutex == NULL)
    {
        mutex_init(&psram_mem_mutex);
    }
    mutex_lock(&psram_mem_mutex);
    memset(addr, c, len);
    mutex_unlock(&psram_mem_mutex);
}

/**
 * @brief Print multiple bytes
 */
void printfByteS(uint8_t *indata, uint16_t len)
{
    int i = 0;
    for(; i < len ; i++)
    {
        printf("%02X", indata[i]);
    }
    printf("\n");
}

/**
 * @brief psram test task
 */
void psram_test_task()
{    
    uint8_t *psram_buf_ptr = (uint8_t*) PSRAM_START_ADR;
    printf("*****************psram demo start*****************\n");
    psram_enable(PSRAM_CACHE_F40M_S40M);
    
    psram_cache_memset(psram_buf_ptr, 0x00, TEST_DATA_LEN);             //clear zero
    
    memset(inRam0, 0x5A, TEST_DATA_LEN);
    memset(inRam1, 0xA5, TEST_DATA_LEN);
    
    printf("1.write psram data\n");
    psram_cache_memcpy(psram_buf_ptr, inRam0,   TEST_DATA_LEN);

    printf("2.read psram data (expect 0x5A)\n");
    printfByteS(psram_buf_ptr, TEST_DATA_LEN);
    
    printf("3.write psram data\n");
    psram_cache_memcpy(psram_buf_ptr, inRam1,   TEST_DATA_LEN);
    
    printf("4.read psram data (expect 0xA5)\n");
    printfByteS(psram_buf_ptr,      TEST_DATA_LEN);
    printf("*****************psram demo end******************\n");
    
    while(1)
    {
        vTaskDelay(100);
    }
}

/**
 * @brief In this test
 */
void app_main()
{
    xTaskCreate(psram_test_task, "psram_test_task", 1024 * 5, NULL, 5, NULL);
}

