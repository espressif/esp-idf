#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct {
    int delay_us;
    int method;
} delay_test_arg_t;

static void test_delay_task(void* p)
{
    const delay_test_arg_t* arg = (delay_test_arg_t*) p;
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, NULL);
    switch (arg->method) {
        case 0:
            ets_delay_us(arg->delay_us);
            break;
        case 1:
            vTaskDelay(arg->delay_us / portTICK_PERIOD_MS / 1000);
            break;
        default:
            TEST_FAIL();
    }
    gettimeofday(&tv_stop, NULL);
    int real_delay_us = (tv_stop.tv_sec - tv_start.tv_sec) * 1000000 +
            tv_stop.tv_usec - tv_start.tv_usec;
    printf("%s core=%d expected=%d actual=%d\n", arg->method ? "vTaskDelay" : "ets_delay_us",
            xPortGetCoreID(), arg->delay_us, real_delay_us);
    TEST_ASSERT_TRUE(abs(real_delay_us - arg->delay_us) < 1000);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("ets_delay produces correct delay on both CPUs", "[delay][ignore]")
{
    int delay_ms = 50;
    const delay_test_arg_t args = { .delay_us = delay_ms * 1000, .method = 0 };
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 0);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS + 1);
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 1);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS + 1);
}

TEST_CASE("vTaskDelay produces correct delay on both CPUs", "[delay]")
{
    int delay_ms = 50;
    const delay_test_arg_t args = { .delay_us = delay_ms * 1000, .method = 1 };
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 0);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS + 1);
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 1);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS + 1);
}
