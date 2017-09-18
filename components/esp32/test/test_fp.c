#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

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
        "maddn.s f2, f0, f3 \n"
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
    int done;
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
    state->done++;
    vTaskDelete(NULL);
}

TEST_CASE("context switch saves FP registers", "[fp]")
{
    struct TestFPState state;
    state.done = 0;
    state.fail = 0;
    xTaskCreatePinnedToCore(tskTestFP, "tsk1", 2048, &state, 3, NULL, 0);
    xTaskCreatePinnedToCore(tskTestFP, "tsk2", 2048, &state, 3, NULL, 0);
    xTaskCreatePinnedToCore(tskTestFP, "tsk3", 2048, &state, 3, NULL, portNUM_PROCESSORS - 1);
    xTaskCreatePinnedToCore(tskTestFP, "tsk4", 2048, &state, 3, NULL, 0);
    while (state.done != 4) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    if (state.fail) {
        const int total = testFpIter * 4;
        printf("Failed: %d, total: %d\r\n", state.fail, total);
    }
    TEST_ASSERT(state.fail == 0);
}
