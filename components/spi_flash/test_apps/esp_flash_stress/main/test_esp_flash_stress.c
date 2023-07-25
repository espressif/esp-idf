/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "test_flash_utils.h"

portMUX_TYPE static s_test_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*---------------------------------------------------------------
            ESP Flash API Concurrency Pressure Test
---------------------------------------------------------------*/
#define TEST_FLASH_CONCURRENCY_TASK_NUM    12

static int s_test_concurrency_id;
//only used for finite loop for CI test
SemaphoreHandle_t s_test_concurrency_smphr;

typedef struct {
    bool is_pressure_test;
    const esp_partition_t *part;
} test_concurrency_ctx_t;

static void s_test_flash_ops_task(void *arg)
{
    test_concurrency_ctx_t *test_ctx = (test_concurrency_ctx_t *)arg;
    bool is_pressure_test = test_ctx->is_pressure_test;;
    esp_flash_t *chip = test_ctx->part->flash_chip;
    uint32_t offs = test_ctx->part->address;

    portENTER_CRITICAL(&s_test_spinlock);
    int id = s_test_concurrency_id;
    s_test_concurrency_id++;
    portEXIT_CRITICAL(&s_test_spinlock);

    int flash_block_sz = 4096;
    char *buffer = calloc(1, flash_block_sz);
    TEST_ASSERT(buffer);
    srand(id + 299);
    int delay_us = rand()  % 1000;
    printf("delay_us: %d\n", delay_us);

    if (is_pressure_test) {
        printf("INFINITE_LOOP\n");
        while (1) {
            printf("%s %d is running\n", __func__, id);
            TEST_ESP_OK(esp_flash_erase_region(chip, id * flash_block_sz + offs, flash_block_sz));
            TEST_ESP_OK(esp_flash_write(chip, buffer, id * flash_block_sz + offs, flash_block_sz));
            TEST_ESP_OK(esp_flash_read(chip, buffer, id * flash_block_sz + offs, flash_block_sz));
            vTaskDelay(pdMS_TO_TICKS(delay_us));
        }
    } else {
        printf("FINITE_LOOP\n");
        for (int i = 0; i < 10; i++) {
            TEST_ESP_OK(esp_flash_erase_region(chip, id * flash_block_sz + offs, 4096));
            TEST_ESP_OK(esp_flash_write(chip, buffer, id * flash_block_sz + offs, flash_block_sz));
            TEST_ESP_OK(esp_flash_read(chip, buffer, id * flash_block_sz + offs, flash_block_sz));
            vTaskDelay(pdMS_TO_TICKS(delay_us));
        }
        printf("%s %d finishes\n", __func__, id);
        free(buffer);
        xSemaphoreGive(s_test_concurrency_smphr);
        vTaskDelete(NULL);
    }
}

/*-------------------------------------------
            Uni Core
-------------------------------------------*/
TEST_CASE("Flash UniCore: Test ESP Flash API Concurrency", "[esp_flash]")
{
    const esp_partition_t* part = get_test_flash_partition();

    s_test_concurrency_id = 0;
    s_test_concurrency_smphr = xSemaphoreCreateCounting(TEST_FLASH_CONCURRENCY_TASK_NUM, 0);
    TEST_ASSERT(s_test_concurrency_smphr);

    char flash_task_name[32];
    test_concurrency_ctx_t ctx = {
        .is_pressure_test = false,
        .part = part,
    };

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM; i++) {
        int l = snprintf(flash_task_name, 32, "flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 0);
    }

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM; i++) {
        TEST_ASSERT(xSemaphoreTake(s_test_concurrency_smphr, portMAX_DELAY) == pdTRUE);
    }
    vTaskDelay(1);
    vSemaphoreDelete(s_test_concurrency_smphr);
}

/**
 * esp_flash APIs concurrency pressure test
 * This test is for manually test
 */
TEST_CASE("Flash UniCore: Test ESP Flash API Concurrency [Stress]", "[esp_flash][stress][manual][ignore]")
{
    const esp_partition_t* part = get_test_flash_partition();

    s_test_concurrency_id = 0;
    char flash_task_name[32];
    test_concurrency_ctx_t ctx = {
        .is_pressure_test = true,
        .part = part,
    };

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM; i++) {
        int l = snprintf(flash_task_name, 32, "flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 0);
    }

    while(1);
}

#if !CONFIG_FREERTOS_UNICORE
/*-------------------------------------------
            Dual Core
-------------------------------------------*/
TEST_CASE("Flash DualCore: Test ESP Flash API Concurrency", "[esp_flash]")
{
    const esp_partition_t* part = get_test_flash_partition();

    s_test_concurrency_id = 0;
    s_test_concurrency_smphr = xSemaphoreCreateCounting(TEST_FLASH_CONCURRENCY_TASK_NUM, 0);
    TEST_ASSERT(s_test_concurrency_smphr);

    char flash_task_name[32];
    test_concurrency_ctx_t ctx = {
        .is_pressure_test = false,
        .part = part,
    };

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM / 2; i++) {
        int l = snprintf(flash_task_name, 32, "core0_flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 0);
    }

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM / 2; i++) {
        int l = snprintf(flash_task_name, 32, "core1_flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 1);
    }

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM; i++) {
        TEST_ASSERT(xSemaphoreTake(s_test_concurrency_smphr, portMAX_DELAY) == pdTRUE);
    }
    vTaskDelay(1);
    vSemaphoreDelete(s_test_concurrency_smphr);
}


/**
 * esp_flash APIs concurrency pressure test
 * This test is for manually test
 */
TEST_CASE("Flash DualCore: Test ESP Flash API Concurrency [Stress]", "[esp_flash][stress][manual][ignore]")
{
    const esp_partition_t* part = get_test_flash_partition();

    s_test_concurrency_id = 0;
    char flash_task_name[32];
    test_concurrency_ctx_t ctx = {
        .is_pressure_test = true,
        .part = part,
    };

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM / 2; i++) {
        int l = snprintf(flash_task_name, 32, "core0_flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 0);
    }

    for (int i = 0; i < TEST_FLASH_CONCURRENCY_TASK_NUM / 2; i++) {
        int l = snprintf(flash_task_name, 32, "core1_flash_ops_task%d", i);
        flash_task_name[l] = '\0';
        xTaskCreatePinnedToCore(&s_test_flash_ops_task, flash_task_name, 4096, (void *)(&ctx), 5, NULL, 1);
    }

    while(1);
}
#endif  //#if !CONFIG_FREERTOS_UNICORE
