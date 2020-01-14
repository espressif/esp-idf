#include <esp_types.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "esp_intr_alloc.h"
#include "xtensa/hal.h"
#include "unity.h"
#include "soc/cpu.h"
#include "test_utils.h"
#include "math.h"
  
#define SW_ISR_LEVEL_1  7

struct fp_test_context {
    SemaphoreHandle_t sync;
    float expected;
};

static void software_isr(void *arg) {
    (void)arg;
    BaseType_t yield;
    xt_set_intclear(1 << SW_ISR_LEVEL_1);

    struct fp_test_context *ctx = (struct fp_test_context *)arg;

    for(int i = 0; i < 10; i++) {
        ctx->expected = ctx->expected * 2.0f;
    }

    xSemaphoreGiveFromISR(ctx->sync, &yield);
    if(yield) {
        portYIELD_FROM_ISR();
    }
}


TEST_CASE("Floating point usage in ISR test", "[freertos] [ignore]")
{
    struct fp_test_context ctx;

    intr_handle_t handle;
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, &software_isr, &ctx, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);

    ctx.sync = xSemaphoreCreateBinary();
    TEST_ASSERT(ctx.sync != NULL);
    ctx.expected = 1.0f;

    xt_set_intset(1 << SW_ISR_LEVEL_1);
    xSemaphoreTake(ctx.sync, portMAX_DELAY);

    esp_intr_free(handle);
    vSemaphoreDelete(ctx.sync);

    TEST_ASSERT_FLOAT_WITHIN(0.1f, ctx.expected, 1024.0f);
}