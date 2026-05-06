/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Test for LoadStore exception handlers. This test performs unaligned load and store in 32bit aligned addresses
*/

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ARCH_XTENSA && CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY

#include <esp_types.h>
#include <stdio.h>
#include <esp_heap_caps.h>
#include "esp_attr.h"
#include "esp_random.h"
#include "esp_intr_alloc.h"
#include "xtensa_api.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "unity.h"
#include "esp_log_buffer.h"

#define SW_ISR_NUM_L1  7    // CPU interrupt number for internal SW0 (level 1)
#define SW_ISR_NUM_L3  29   // CPU interrupt number for internal SW1 (level 3)

typedef struct {
    uint8_t  b8;
    uint16_t b16;
    uint8_t  b8_2;
    uint32_t b32;
} iram_data_t;

static volatile iram_data_t *s_iram;
static volatile int isr_hits;

TEST_CASE("LoadStore Exception handler", "[freertos]")
{
    int32_t val0 = 0xDEADBEEF;
    int32_t val1 = 0xBBAA9988;
    int32_t val2 = 0x77665544;
    int32_t val3 = 0x33221100;

    int8_t val8_0 = val0 & 0xff;
    int8_t val8_1 = val1 & 0xff;
    int8_t val8_2 = val2 & 0xff;
    int8_t val8_3 = val3 & 0xff;

    int16_t val16_0 = val0 & 0xffff;
    int16_t val16_1 = val1 & 0xffff;
    int16_t val16_2 = val2 & 0xffff;
    int16_t val16_3 = val3 & 0xffff;

    uint32_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_IRAM_8BIT);

    int8_t *arr = heap_caps_malloc(largest_free * sizeof(int8_t), MALLOC_CAP_IRAM_8BIT);
    TEST_ASSERT(arr != NULL);

    int8_t *arr8 =  arr;
    int16_t *arr16 = (int16_t *)arr;
    int32_t *arr32 = (int32_t *)arr;

    for (int i = 0; i < 1024; i++) {

        // LoadStoreError

        uint32_t offset = esp_random() % (largest_free - 20);
        uint32_t offset8, offset16, offset32;

        // Get word aligned offset
        offset8 = offset & ~3;
        offset16 = offset8 / 2;
        offset32 = offset8 / 4;

        arr8[offset8] = val8_0;
        arr8[offset8 + 1] = val8_1;
        arr8[offset8 + 2] = val8_2;
        arr8[offset8 + 3] = val8_3;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val8_0, arr8[offset8]);
        TEST_ASSERT_EQUAL(val8_1, arr8[offset8 + 1]);
        TEST_ASSERT_EQUAL(val8_2, arr8[offset8 + 2]);
        TEST_ASSERT_EQUAL(val8_3, arr8[offset8 + 3]);

        arr16[offset16] = val16_0;
        arr16[offset16 + 1] = val16_1;
        arr16[offset16 + 2] = val16_2;
        arr16[offset16 + 3] = val16_3;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val16_0, arr16[offset16]);
        TEST_ASSERT_EQUAL(val16_1, arr16[offset16 + 1]);
        TEST_ASSERT_EQUAL(val16_2, arr16[offset16 + 2]);
        TEST_ASSERT_EQUAL(val16_3, arr16[offset16 + 3]);

        // LoadStoreAlignement Error

        // Check that it doesn't write to adjacent bytes
        int8_t *ptr8_0 = (void *)&arr8[offset8];
        int8_t *ptr8_1 = (void *)&arr8[offset8] + 5;
        int8_t *ptr8_2 = (void *)&arr8[offset8] + 10;
        int8_t *ptr8_3 = (void *)&arr8[offset8] + 15;

        *ptr8_0 = 0x73;
        *ptr8_1 = 0x73;
        *ptr8_2 = 0x73;
        *ptr8_3 = 0x73;

        int16_t *ptr16_0 = (void *)&arr16[offset16] + 1;
        int16_t *ptr16_1 = (void *)&arr16[offset16] + 3;

        *ptr16_0 = val16_0;
        *ptr16_1 = val16_1;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val16_0, *ptr16_0);
        TEST_ASSERT_EQUAL(0x73, *ptr8_0);
        TEST_ASSERT_EQUAL(val16_1, *ptr16_1);
        TEST_ASSERT_EQUAL(0x73, *ptr8_1);

        int32_t *ptr32_0 = (void *)&arr32[offset32] + 1;
        int32_t *ptr32_1 = (void *)&arr32[offset32] + 6;
        int32_t *ptr32_2 = (void *)&arr32[offset32] + 11;
        *ptr32_0 = val0;
        *ptr32_1 = val1;
        *ptr32_2 = val2;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw");
        TEST_ASSERT_EQUAL(0x73, *ptr8_0);
        TEST_ASSERT_EQUAL(val0, *ptr32_0);
        TEST_ASSERT_EQUAL(0x73, *ptr8_1);
        TEST_ASSERT_EQUAL(val1, *ptr32_1);
        TEST_ASSERT_EQUAL(0x73, *ptr8_2);
        TEST_ASSERT_EQUAL(val2, *ptr32_2);
        TEST_ASSERT_EQUAL(0x73, *ptr8_3);
    }

    TEST_ASSERT_TRUE(heap_caps_check_integrity_all(true));
    heap_caps_free(arr);
}

static void IRAM_ATTR level_isr(void *arg)
{
    const int cpu_intr_num = *(const int *)arg;

    // Clear the SW interrupt source
    xt_set_intclear(1 << cpu_intr_num);

    // Schedule another pending level interrupt while still inside this ISR
    // to keep dispatch_c_isr in the interrupt loop handling. The pending
    // interrupt will be handled after this ISR returns.
    if (isr_hits == 0) {
        xt_set_intset(1 << cpu_intr_num);
    }

    // 8/16-bit accesses to s_iram will trigger LoadStore/Alignment exceptions which changes the EXCSAVE_1 value
    s_iram->b16++;
    s_iram->b8 = 0x5A;
    s_iram->b8_2 = 0xA5;

    isr_hits++;
}

static void run_sw_isr_level(int int_level)
{
    memset((void *)s_iram, 0, sizeof(iram_data_t));
    isr_hits = 0;

    intr_handle_t handle;
    int cpu_intr_num = (int_level == 1) ? SW_ISR_NUM_L1 : SW_ISR_NUM_L3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_intr_alloc((int_level == 1) ? ETS_INTERNAL_SW0_INTR_SOURCE : ETS_INTERNAL_SW1_INTR_SOURCE,
                                             (int_level == 1) ? ESP_INTR_FLAG_LEVEL1 : ESP_INTR_FLAG_LEVEL3,
                                             level_isr,
                                             (void *)&cpu_intr_num,
                                             &handle));

    // Trigger the first interrupt; ISR will queue one more while running
    xt_set_intset(1 << cpu_intr_num);

    // Wait for the ISR to be invoked twice
    TickType_t start = xTaskGetTickCount();
    while (isr_hits < 2 && (xTaskGetTickCount() - start) < pdMS_TO_TICKS(500)) {
        vTaskDelay(1);
    }

    TEST_ASSERT_EQUAL(2, isr_hits);
    TEST_ASSERT_EQUAL_HEX8(0x5A, s_iram->b8);
    TEST_ASSERT_EQUAL_HEX8(0xA5, s_iram->b8_2);
    TEST_ASSERT_EQUAL_UINT16(2, s_iram->b16);
    TEST_ASSERT_EQUAL_UINT32(0, s_iram->b32);

    esp_intr_free(handle);
}

TEST_CASE("LoadStore: 8/16-bit field access in IRAM from ISRs when pending interrupts are present", "[freertos]")
{
    s_iram = heap_caps_calloc(1, sizeof(*s_iram), MALLOC_CAP_IRAM_8BIT);
    TEST_ASSERT_NOT_NULL(s_iram);

    esp_rom_printf("Running test in level-1 ISR...\n");
    run_sw_isr_level(1);
    esp_rom_printf("test passed\n");

    esp_rom_printf("Running test in level-3 ISR...\n");
    run_sw_isr_level(3);
    esp_rom_printf("test passed\n");

    heap_caps_free((void *)s_iram);
    vTaskDelay(pdMS_TO_TICKS(100)); // Wait for memory to be freed, to avoid affecting other tests.
}

TEST_CASE("LoadStore: zero-overhead loop continues after IRAM 8-bit store", "[freertos]")
{
    const unsigned len = 32;

    uint8_t *dst = heap_caps_calloc(len, 1, MALLOC_CAP_IRAM_8BIT);
    TEST_ASSERT_NOT_NULL(dst);

    uint8_t *src = heap_caps_calloc(len, 1, MALLOC_CAP_IRAM_8BIT);
    TEST_ASSERT_NOT_NULL(src);

    for (unsigned i = 0; i < len; i++) {
        src[i] = i + 1;
        dst[i] = 0xCC;
    }

    ESP_LOG_BUFFER_HEX("dst before", dst, len);
    ESP_LOG_BUFFER_HEX("src       ", src, len);

    /*
    * Use Xtensa zero-overhead loop where the final instruction (LEND)
    * is an 8-bit store into IRAM. With CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
    * each store triggers the LoadStoreError handler. The handler must mimic
    * the loop hardware to keep iterating; otherwise the loop exits after one
    * iteration. This assembly below performs a simple byte copy from src to dst,
    * and the test verifies that all bytes are copied correctly,
    * indicating that the loop continued to execute after each store exception.
    */
    uint32_t tmp_val;
    uint8_t *dst_it = dst;
    uint8_t *src_it = src;
    unsigned cnt = len;
    __asm__ volatile(
        "addi       %0, %0, -1\n"   /* dst-- */
        "addi       %1, %1, -1\n"   /* src-- */
        "loopnez    %2, .endLoop\n"
        "addi       %1, %1, 1\n"   /* src++ */
        "l8ui       %3, %1, 0\n"   /* tmp_val = src[] */
        "addi       %0, %0, 1\n"   /* dst++ */
        "s8i        %3, %0, 0\n"   /* dst[] = tmp_val, LEND: store is last in loop body */
        ".endLoop:\n"
        : "+r"(dst_it), "+r"(src_it), "+r"(cnt), "=&r"(tmp_val)
        :
        : "memory"
    );

    ESP_LOG_BUFFER_HEX("dst  after", dst, len);

    for (unsigned i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_HEX8(src[i], dst[i]);
    }

    heap_caps_free(dst);
    heap_caps_free(src);

    vTaskDelay(pdMS_TO_TICKS(100)); // Wait for free to complete before running other tests.
}
#endif // CONFIG_IDF_TARGET_ARCH_XTENSA && CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
