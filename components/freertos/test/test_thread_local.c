/*
 Test for thread local storage support.
*/

#include <string.h>
#include <esp_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "sdkconfig.h"

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
    uint32_t tp = (uint32_t)-1;
    int test_var1_old = 0;
    uint8_t test_var2_old = 0;
    uint16_t test_var3_old = 0;
    int f32_old = 0;
    uint8_t f8_old = 0;
    uint16_t f16_old = 0;

    asm volatile ("rur.threadptr %0":"=r"(tp));
    for (int i = 0; i < 5; i++) {
        printf("Task[%x]: var = 0x%x 0x%x\n", tp, tl_test_var1, tl_test_var2);
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
            TEST_ASSERT_EQUAL(test_var1_old+1, tl_test_var1);
            TEST_ASSERT_EQUAL(test_var2_old+1, tl_test_var2);
            TEST_ASSERT_EQUAL(test_var3_old+1, tl_test_var3);
            for (int k = 0; k < sizeof(tl_test_arr_var); k++) {
                TEST_ASSERT_EQUAL(i-1, tl_test_arr_var[k]);
            }
            TEST_ASSERT_EQUAL(f32_old+1, tl_test_struct_var.f32);
            TEST_ASSERT_EQUAL(f8_old+1, tl_test_struct_var.f8);
            TEST_ASSERT_EQUAL(f16_old+1, tl_test_struct_var.f16);
            for (int k = 0; k < sizeof(tl_test_struct_var.farr); k++) {
                TEST_ASSERT_EQUAL(i-1, tl_test_struct_var.farr[k]);
            }
        }
        test_var1_old = tl_test_var1;
        test_var2_old = tl_test_var2;
        test_var3_old = tl_test_var3;
        f32_old = tl_test_struct_var.f32;
        f8_old = tl_test_struct_var.f8;
        f16_old = tl_test_struct_var.f16;
        tl_test_var1++;
        tl_test_var2++;
        tl_test_var3++;
        memset(tl_test_arr_var, i, sizeof(tl_test_arr_var));
        tl_test_struct_var.f32++;
        tl_test_struct_var.f8++;
        tl_test_struct_var.f16++;
        memset(tl_test_struct_var.farr, i, sizeof(tl_test_struct_var.farr));
        vTaskDelay(10);
    }

    if (running) {
        *running = false;
        vTaskDelete(NULL);
    }
}

TEST_CASE("TLS test", "[freertos]")
{
    static StackType_t s_stack[2048];
    StaticTask_t s_task;
    bool running[2] = {true, true};
#if CONFIG_FREERTOS_UNICORE == 0
    int other_core = 1;
#else
    int other_core = 0;
#endif

    xTaskCreatePinnedToCore((TaskFunction_t)&task_test_tls, "task_test_tls", 3072, &running[0], 5, NULL, 0);
    xTaskCreateStaticPinnedToCore((TaskFunction_t)&task_test_tls, "task_test_tls", sizeof(s_stack), &running[1],
        5, s_stack, &s_task, other_core);
    while (running[0] || running[1]) {
        vTaskDelay(10);
    }
}
