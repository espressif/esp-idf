/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Test for thread local storage support.
*/

#include <string.h>
#include <stdlib.h>
#include <esp_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "sdkconfig.h"

#if defined(__XTENSA__)
#define GET_THREADPTR(tp_dest)  do { asm volatile ("rur.threadptr %0":"=r"(tp_dest)); } while(0)
#elif defined (__riscv)
#define GET_THREADPTR(tp_dest)  do { register uint32_t _tp asm("tp"); tp_dest = _tp; } while(0)
#endif


static __thread int tl_test_var1;
static __thread uint8_t tl_test_var2 = 55;
static __thread uint16_t tl_test_var3 = 44;
static __thread uint8_t tl_test_arr_var[10];
static __thread struct test_tls_var {
    int f32;
    uint8_t f8;
    uint16_t f16;
    uint8_t farr[10];
} tl_test_struct_var;

static void task_test_tls(void *arg)
{
    bool *running = (bool *)arg;
    uint32_t tp = (uint32_t) -1;
    int test_var1_old = 0;
    uint8_t test_var2_old = 0;
    uint16_t test_var3_old = 0;
    int f32_old = 0;
    uint8_t f8_old = 0;
    uint16_t f16_old = 0;
    srand((int) xTaskGetCurrentTaskHandle());
    int step = (rand() % 10) + 1;

    GET_THREADPTR(tp);
    for (int i = 0; i < 5; i++) {
        printf("Task[%x]: var = 0x%x 0x%x step=%d\n", tp, tl_test_var1, tl_test_var2, step);
        if (i == 0) {
            TEST_ASSERT_EQUAL(0, tl_test_var1);
            TEST_ASSERT_EQUAL(55, tl_test_var2);
            TEST_ASSERT_EQUAL(44, tl_test_var3);
            for (int k = 0; k < sizeof(tl_test_arr_var); k++) {
                TEST_ASSERT_EQUAL(0, tl_test_arr_var[k]);
            }
            TEST_ASSERT_EQUAL(0, tl_test_struct_var.f32);
            TEST_ASSERT_EQUAL(0, tl_test_struct_var.f8);
            TEST_ASSERT_EQUAL(0, tl_test_struct_var.f16);
            for (int k = 0; k < sizeof(tl_test_struct_var.farr); k++) {
                TEST_ASSERT_EQUAL(0, tl_test_struct_var.farr[k]);
            }
        } else {
            TEST_ASSERT_EQUAL(test_var1_old + step, tl_test_var1);
            TEST_ASSERT_EQUAL(test_var2_old + step, tl_test_var2);
            TEST_ASSERT_EQUAL(test_var3_old + step, tl_test_var3);
            for (int k = 0; k < sizeof(tl_test_arr_var); k++) {
                TEST_ASSERT_EQUAL((i - 1) * step, tl_test_arr_var[k]);
            }
            TEST_ASSERT_EQUAL(f32_old + step, tl_test_struct_var.f32);
            TEST_ASSERT_EQUAL(f8_old + step, tl_test_struct_var.f8);
            TEST_ASSERT_EQUAL(f16_old + step, tl_test_struct_var.f16);
            for (int k = 0; k < sizeof(tl_test_struct_var.farr); k++) {
                TEST_ASSERT_EQUAL((i - 1) * step, tl_test_struct_var.farr[k]);
            }
        }

        test_var1_old = tl_test_var1;
        test_var2_old = tl_test_var2;
        test_var3_old = tl_test_var3;
        f32_old = tl_test_struct_var.f32;
        f8_old = tl_test_struct_var.f8;
        f16_old = tl_test_struct_var.f16;
        tl_test_var1 += step;
        tl_test_var2 += step;
        tl_test_var3 += step;
        memset(tl_test_arr_var, i * step, sizeof(tl_test_arr_var));
        tl_test_struct_var.f32 += step;
        tl_test_struct_var.f8 += step;
        tl_test_struct_var.f16 += step;
        memset(tl_test_struct_var.farr, i * step, sizeof(tl_test_struct_var.farr));
        vTaskDelay(10);
    }

    if (running) {
        *running = false;
        vTaskDelete(NULL);
    }
}

TEST_CASE("TLS test", "[freertos]")
{
    const size_t stack_size = 3072;
    StackType_t s_stack[stack_size]; /* with 8KB test task stack (default) this test still has ~3KB headroom */
    StaticTask_t s_task;
    bool running[2] = {true, true};
#if CONFIG_FREERTOS_UNICORE == 0
    int other_core = 1;
#else
    int other_core = 0;
#endif

    xTaskCreatePinnedToCore((TaskFunction_t)&task_test_tls, "task_test_tls", stack_size, &running[0],
                            UNITY_FREERTOS_PRIORITY, NULL, 0);
    xTaskCreateStaticPinnedToCore((TaskFunction_t)&task_test_tls, "task_test_tls", stack_size, &running[1],
                                  UNITY_FREERTOS_PRIORITY, s_stack, &s_task, other_core);
    while (running[0] || running[1]) {
        vTaskDelay(10);
    }
    vTaskDelay(10); /* Make sure idle task can clean up s_task, before it goes out of scope */
}
