/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "hal/mmu_hal.h"
#include "soc/soc.h"

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

#define TEST_SIZE     CONFIG_MMU_PAGE_SIZE
#define TEST_PADDR    0x10000
#define TEST_VADDR    SOC_DROM_LOW

static void s_test_ext_vaddr(void)
{
    uint32_t actual_mapped_len = 0;
    mmu_hal_map_region(0, MMU_TARGET_FLASH0, TEST_VADDR, TEST_PADDR, TEST_SIZE, &actual_mapped_len);
    printf("actual_mapped_len is 0x%"PRIx32" bytes\n", actual_mapped_len);

    /**
     * we should work here, without any other Cache/MMU configs
     * If you found here we need some Cache/MMU configs, you should fix it in the driver
     */
    for (int i = 0; i < TEST_SIZE; i = i+4) {
        volatile uint32_t my_var = *(uint32_t *)(TEST_VADDR + i);
        //we actually don't care the value, but we need to make sure above line won't lead to Cache/MMU error
        assert(my_var || my_var == 0);
    }
}
#endif //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;

    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    printf("silicon revision %d, ", chip_info.revision);
    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
    printf("App is running in RAM !\n");

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    s_test_ext_vaddr();
#endif

    uint32_t uptime = 0;
    while (1) {
        printf("Time since boot: %"PRIu32" seconds...\n", uptime++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
