/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32

#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"

/* Note: these functions are included here for unit test purposes. They are not needed for writing
 * normal code. If writing standard C floating point code, libgcc should correctly include implementations
 * that use the floating point registers correctly. */

static float addsf(float a, float b)
{
    float result;
    asm volatile (
        "wfr f0, %1\n"
        "wfr f1, %2\n"
        "add.s f2, f0, f1\n"
        "rfr %0, f2\n"
        :"=r"(result):"r"(a), "r"(b)
    );
    return result;
}

static float mulsf(float a, float b)
{
    float result;
    asm volatile (
        "wfr f0, %1\n"
        "wfr f1, %2\n"
        "mul.s f2, f0, f1\n"
        "rfr %0, f2\n"
        :"=r"(result):"r"(a), "r"(b)
    );
    return result;
}

static float divsf(float a, float b)
{
    float result;
    asm volatile (
        "wfr f0, %1\n"
        "wfr f1, %2\n"
        "div0.s f3, f1 \n"
        "nexp01.s f4, f1 \n"
        "const.s f5, 1 \n"
        "maddn.s f5, f4, f3 \n"
        "mov.s f6, f3 \n"
        "mov.s f7, f1 \n"
        "nexp01.s f8, f0 \n"
        "maddn.s f6, f5, f3 \n"
        "const.s f5, 1 \n"
        "const.s f2, 0 \n"
        "neg.s f9, f8 \n"
        "maddn.s f5,f4,f6 \n"
        "maddn.s f2, f9, f3 \n"
        "mkdadj.s f7, f0 \n"
        "maddn.s f6,f5,f6 \n"
        "maddn.s f9,f4,f2 \n"
        "const.s f5, 1 \n"
        "maddn.s f5,f4,f6 \n"
        "maddn.s f2,f9,f6 \n"
        "neg.s f9, f8 \n"
        "maddn.s f6,f5,f6 \n"
        "maddn.s f9,f4,f2 \n"
        "addexpm.s f2, f7 \n"
        "addexp.s f6, f7 \n"
        "divn.s f2,f9,f6\n"
        "rfr %0, f2\n"
        :"=r"(result):"r"(a), "r"(b)
    );
    return result;
}

static float sqrtsf(float a)
{
    float result;
    asm volatile (
        "wfr f0, %1\n"
        "sqrt0.s f2, f0\n"
        "const.s f5, 0\n"
        "maddn.s f5, f2, f2\n"
        "nexp01.s f3, f0\n"
        "const.s f4, 3\n"
        "addexp.s f3, f4\n"
        "maddn.s f4, f5, f3\n"
        "nexp01.s f5, f0\n"
        "neg.s f6, f5\n"
        "maddn.s f2, f4, f2\n"
        "const.s f1, 0\n"
        "const.s f4, 0\n"
        "const.s f7, 0\n"
        "maddn.s f1, f6, f2\n"
        "maddn.s f4, f2, f3\n"
        "const.s f6, 3\n"
        "maddn.s f7, f6, f2\n"
        "maddn.s f5, f1, f1\n"
        "maddn.s f6, f4, f2\n"
        "neg.s f3, f7\n"
        "maddn.s f1, f5, f3\n"
        "maddn.s f7, f6, f7\n"
        "mksadj.s f2, f0\n"
        "nexp01.s f5, f0\n"
        "maddn.s f5, f1, f1\n"
        "neg.s f3, f7\n"
        "addexpm.s f1, f2\n"
        "addexp.s f3, f2\n"
        "divn.s f1, f5, f3\n"
        "rfr %0, f1\n"
        :"=r"(result):"r"(a)
    );
    return result;
}

TEST_CASE("test FP add", "[fp]")
{
    float a = 100.0f;
    float b = 0.5f;
    float c = addsf(a, b);
    float eps = c - 100.5f;
    printf("a=%g b=%g c=%g eps=%g\r\n", a, b, c, eps);
    TEST_ASSERT_TRUE(fabs(eps) < 0.000001);
}

TEST_CASE("test FP mul", "[fp]")
{
    float a = 100.0f;
    float b = 0.05f;
    float c = mulsf(a, b);
    float eps = c - 5.0f;
    printf("a=%g b=%g c=%g eps=%g\r\n", a, b, c, eps);
    TEST_ASSERT_TRUE(fabs(eps) < 0.000001);
}

TEST_CASE("test FP div", "[fp]")
{
    float a = 100.0f;
    float b = 5.0f;
    float c = divsf(a, b);
    float eps = c - 20.0f;
    printf("a=%g b=%g c=%g eps=%g\r\n", a, b, c, eps);
    TEST_ASSERT_TRUE(fabs(eps) < 0.000001);
}

TEST_CASE("test FP sqrt", "[fp]")
{
    float a = 100.0f;
    float c = sqrtsf(a);
    float eps = c - 10.0f;
    printf("a=%g c=%g eps=%g\r\n", a, c, eps);
    TEST_ASSERT_TRUE(fabs(eps) < 0.000001);
}


struct TestFPState {
    int fail;
    SemaphoreHandle_t done;
};

static const int testFpIter = 100000;

static void tskTestFP(void *pvParameters)
{
    struct TestFPState *state = (struct TestFPState *) pvParameters;
    for (int i = 0; i < testFpIter; ++i) {
        // calculate zero in a slightly obscure way
        float y = sqrtsf(addsf(1.0f, divsf(mulsf(sqrtsf(2), sqrtsf(2)), 2.0f)));
        y = mulsf(y, y);
        y = addsf(y, -2.0f);
        // check that result is not far from zero
        float eps = fabs(y);
        if (eps > 1e-6f) {
            state->fail++;
            printf("%s: i=%d y=%f eps=%f\r\n", __func__, i, y, eps);
        }
    }
    TEST_ASSERT(xSemaphoreGive(state->done));
    vTaskDelete(NULL);
}

TEST_CASE("context switch saves FP registers", "[fp]")
{
    struct TestFPState state = {
        .done = xSemaphoreCreateCounting(4, 0)
    };
    TEST_ASSERT_NOT_NULL(state.done);
    const int prio = UNITY_FREERTOS_PRIORITY + 1;
    TEST_ASSERT(xTaskCreatePinnedToCore(tskTestFP, "tsk1", 2048, &state, prio, NULL, 0));
    TEST_ASSERT(xTaskCreatePinnedToCore(tskTestFP, "tsk2", 2048, &state, prio, NULL, 0));
    TEST_ASSERT(xTaskCreatePinnedToCore(tskTestFP, "tsk3", 2048, &state, prio, NULL, portNUM_PROCESSORS - 1));
    TEST_ASSERT(xTaskCreatePinnedToCore(tskTestFP, "tsk4", 2048, &state, prio, NULL, 0));
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT(xSemaphoreTake(state.done, pdMS_TO_TICKS(5000)));
    }
    vSemaphoreDelete(state.done);
    if (state.fail) {
        const int total = testFpIter * 4;
        printf("Failed: %d, total: %d\r\n", state.fail, total);
    }
    TEST_ASSERT(state.fail == 0);
}

/* Note: not static, to avoid optimisation of const result */
float IRAM_ATTR test_fp_benchmark_fp_divide(int counts, unsigned *cycles)
{
    float f = MAXFLOAT;
    uint32_t before, after;
    RSR(CCOUNT, before);

    for (int i = 0; i < counts; i++) {
        f /= 1.000432f;
    }

    RSR(CCOUNT, after);
    *cycles = (after - before) / counts;

    return f;
}


extern void set_leak_threshold(int threshold);

TEST_CASE("floating point division performance", "[fp]")
{
    const unsigned COUNTS = 1000;
    unsigned cycles = 0;

    /*From lazy allocating resources when printing floats*/
    set_leak_threshold(-850);

    // initialize fpu
    volatile __attribute__((unused)) float dummy = sqrtf(rand());

    float f = test_fp_benchmark_fp_divide(COUNTS, &cycles);

    printf("%d divisions from %f = %f\n", COUNTS, MAXFLOAT, f);
    printf("Per division = %d cycles\n", cycles);

    TEST_PERFORMANCE_LESS_THAN(CYCLES_PER_DIV, "%d cycles", cycles);
}

/* Note: not static, to avoid optimisation of const result */
float IRAM_ATTR test_fp_benchmark_fp_sqrt(int counts, unsigned *cycles)
{
    float f = MAXFLOAT;
    uint32_t before, after;
    RSR(CCOUNT, before);

    for (int i = 0; i < counts; i++) {
        f = sqrtf(f);
    }

    RSR(CCOUNT, after);
    *cycles = (after - before) / counts;

    return f;
}

TEST_CASE("floating point square root performance", "[fp]")
{
    const unsigned COUNTS = 200;
    unsigned cycles = 0;

    /*From lazy allocating resources when printing floats*/
    set_leak_threshold(-850);

    // initialize fpu
    volatile float __attribute__((unused)) dummy = sqrtf(rand());

    float f = test_fp_benchmark_fp_sqrt(COUNTS, &cycles);

    printf("%d square roots from %f = %f\n", COUNTS, MAXFLOAT, f);
    printf("Per sqrt = %d cycles\n", cycles);

    TEST_PERFORMANCE_LESS_THAN(CYCLES_PER_SQRT, "%d cycles", cycles);
}

#endif // CONFIG_IDF_TARGET_ESP32
