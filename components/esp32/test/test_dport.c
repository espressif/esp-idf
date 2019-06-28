#include <stdio.h>
#include <stdlib.h>
#include "xtensa/core-macros.h"
#include "xtensa/hal.h"
#include "esp_types.h"
#include "esp32/clk.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_timer.h"
#include "soc/cpu.h"
#include "unity.h"
#include "test_utils.h"
#include "esp32/rom/uart.h"
#include "soc/uart_periph.h"
#include "soc/dport_reg.h"
#include "soc/rtc.h"
#include "esp_intr_alloc.h"
#include "driver/timer.h"

#define MHZ (1000000)
static volatile bool exit_flag;
static bool dport_test_result;
static bool apb_test_result;
uint32_t volatile apb_intr_test_result;

static void accessDPORT(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t dport_date = DPORT_REG_READ(DPORT_DATE_REG);

    dport_test_result = true;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        if (dport_date != DPORT_REG_READ(DPORT_DATE_REG)) {
            dport_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void accessAPB(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t uart_date = REG_READ(UART_DATE_REG(0));

    apb_test_result = true;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        if (uart_date != REG_READ(UART_DATE_REG(0))) {
            apb_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

void run_tasks(const char *task1_description, void (* task1_func)(void *), const char *task2_description, void (* task2_func)(void *), uint32_t delay_ms)
{
    apb_intr_test_result = 1;
    int i;
    TaskHandle_t th[2];
    xSemaphoreHandle exit_sema[2];

    for (i=0; i<2; i++) {
        if((task1_func != NULL && i == 0) || (task2_func != NULL && i == 1)){
            exit_sema[i] = xSemaphoreCreateBinary();
        }
    }

    exit_flag = false;

#ifndef CONFIG_FREERTOS_UNICORE
    printf("assign task accessing DPORT to core 0 and task accessing APB to core 1\n");
    if(task1_func != NULL) xTaskCreatePinnedToCore(task1_func, task1_description, 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0], 0);
    if(task2_func != NULL) xTaskCreatePinnedToCore(task2_func, task2_description, 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1], 1);
#else
    printf("assign task accessing DPORT and accessing APB\n");
    if(task1_func != NULL) xTaskCreate(task1_func, task1_description, 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0]);
    if(task2_func != NULL) xTaskCreate(task2_func, task2_description, 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1]);
#endif

    printf("start wait for %d seconds [Test %s and %s]\n", delay_ms/1000, task1_description, task2_description);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;

    for (i=0; i<2; i++) {
        if ((task1_func != NULL && i == 0) || (task2_func != NULL && i == 1)) {
            xSemaphoreTake(exit_sema[i], portMAX_DELAY);
            vSemaphoreDelete(exit_sema[i]);
        }
    }
    TEST_ASSERT(dport_test_result == true && apb_test_result == true && apb_intr_test_result == 1);
}

TEST_CASE("access DPORT and APB at same time", "[esp32]")
{
    dport_test_result   = false;
    apb_test_result     = false;
    printf("CPU_FREQ = %d MHz\n", esp_clk_cpu_freq());
    run_tasks("accessDPORT", accessDPORT, "accessAPB", accessAPB, 10000);
}

void run_tasks_with_change_freq_cpu(int cpu_freq_mhz)
{
    const int uart_num = CONFIG_ESP_CONSOLE_UART_NUM;
    const int uart_baud = CONFIG_ESP_CONSOLE_UART_BAUDRATE;
    dport_test_result = false;
    apb_test_result = false;
    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);

    printf("CPU_FREQ = %d MHz\n", old_config.freq_mhz);

    if (cpu_freq_mhz != old_config.freq_mhz) {
        rtc_cpu_freq_config_t new_config;
        bool res = rtc_clk_cpu_freq_mhz_to_config(cpu_freq_mhz, &new_config);
        assert(res && "invalid frequency value");

        uart_tx_wait_idle(uart_num);
        rtc_clk_cpu_freq_set_config(&new_config);
        uart_div_modify(uart_num, (rtc_clk_apb_freq_get() << 4) / uart_baud);
        /* adjust RTOS ticks */
        _xt_tick_divisor = cpu_freq_mhz * 1000000 / XT_TICK_PER_SEC;
        vTaskDelay(2);

        printf("CPU_FREQ switched to %d MHz\n", cpu_freq_mhz);
    }
    run_tasks("accessDPORT", accessDPORT, "accessAPB", accessAPB, 10000);

    // return old freq.
    uart_tx_wait_idle(uart_num);
    rtc_clk_cpu_freq_set_config(&old_config);
    uart_div_modify(uart_num, (rtc_clk_apb_freq_get() << 4) / uart_baud);
    _xt_tick_divisor = old_config.freq_mhz * 1000000 / XT_TICK_PER_SEC;
}

TEST_CASE("access DPORT and APB at same time (Freq CPU and APB = 80 MHz)", "[esp32] [ignore]")
{
    run_tasks_with_change_freq_cpu(80);
}

TEST_CASE("access DPORT and APB at same time (Freq CPU and APB = 40 MHz (XTAL))", "[esp32]")
{
    run_tasks_with_change_freq_cpu((int) rtc_clk_xtal_freq_get());
}

static uint32_t stall_other_cpu_counter;
static uint32_t pre_reading_apb_counter;
static uint32_t apb_counter;

static void accessDPORT_stall_other_cpu(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t dport_date = DPORT_REG_READ(DPORT_DATE_REG);
    uint32_t dport_date_cur;
    dport_test_result = true;
    stall_other_cpu_counter = 0;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        ++stall_other_cpu_counter;
        DPORT_STALL_OTHER_CPU_START();
        dport_date_cur = _DPORT_REG_READ(DPORT_DATE_REG);
        DPORT_STALL_OTHER_CPU_END();
        if (dport_date != dport_date_cur) {
            apb_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void accessAPB_measure_performance(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t uart_date = REG_READ(UART_DATE_REG(0));

    apb_test_result = true;
    apb_counter = 0;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        ++apb_counter;
        if (uart_date != REG_READ(UART_DATE_REG(0))) {
            apb_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void accessDPORT_pre_reading_apb(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t dport_date = DPORT_REG_READ(DPORT_DATE_REG);
    uint32_t dport_date_cur;
    dport_test_result = true;
    pre_reading_apb_counter = 0;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        ++pre_reading_apb_counter;
        dport_date_cur = DPORT_REG_READ(DPORT_DATE_REG);
        if (dport_date != dport_date_cur) {
            apb_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("test for DPORT access performance", "[esp32]")
{
    dport_test_result   = true;
    apb_test_result     = true;
    typedef struct {
        uint32_t dport;
        uint32_t apb;
        uint32_t summ;
    } test_performance_t;
    test_performance_t t[5] = {0};
    uint32_t delay_ms = 5000;

    run_tasks("-", NULL, "accessAPB", accessAPB_measure_performance, delay_ms);
    t[0].apb    = apb_counter;
    t[0].dport  = 0;
    t[0].summ   = t[0].apb + t[0].dport;

    run_tasks("accessDPORT_stall_other_cpu", accessDPORT_stall_other_cpu, "-", NULL, delay_ms);
    t[1].apb    = 0;
    t[1].dport  = stall_other_cpu_counter;
    t[1].summ   = t[1].apb + t[1].dport;

    run_tasks("accessDPORT_pre_reading_apb", accessDPORT_pre_reading_apb, "-", NULL, delay_ms);
    t[2].apb    = 0;
    t[2].dport  = pre_reading_apb_counter;
    t[2].summ   = t[2].apb + t[2].dport;

    run_tasks("accessDPORT_stall_other_cpu", accessDPORT_stall_other_cpu, "accessAPB", accessAPB_measure_performance, delay_ms);
    t[3].apb    = apb_counter;
    t[3].dport  = stall_other_cpu_counter;
    t[3].summ   = t[3].apb + t[3].dport;

    run_tasks("accessDPORT_pre_reading_apb", accessDPORT_pre_reading_apb, "accessAPB", accessAPB_measure_performance, delay_ms);
    t[4].apb    = apb_counter;
    t[4].dport  = pre_reading_apb_counter;
    t[4].summ   = t[4].apb + t[4].dport;

    printf("\nPerformance table: \n"
            "The number of simultaneous read operations of the APB and DPORT registers\n"
            "by different methods for %d seconds.\n", delay_ms/1000);
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|    Method read DPORT  |   DPORT  |    APB   |   SUMM   |\n");
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|1.Only accessAPB       |%10d|%10d|%10d|\n", t[0].dport, t[0].apb, t[0].summ);
    printf("|2.Only STALL_OTHER_CPU |%10d|%10d|%10d|\n", t[1].dport, t[1].apb, t[1].summ);
    printf("|3.Only PRE_READ_APB_REG|%10d|%10d|%10d|\n", t[2].dport, t[2].apb, t[2].summ);
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|4.STALL_OTHER_CPU      |%10d|%10d|%10d|\n", t[3].dport, t[3].apb, t[3].summ);
    printf("|5.PRE_READ_APB_REG     |%10d|%10d|%10d|\n", t[4].dport, t[4].apb, t[4].summ);
    printf("+-----------------------+----------+----------+----------+\n");
    printf("| ratio=PRE_READ/STALL  |%10f|%10f|%10f|\n", (float)t[4].dport/t[3].dport, (float)t[4].apb/t[3].apb, (float)t[4].summ/t[3].summ);
    printf("+-----------------------+----------+----------+----------+\n");
}

#define REPEAT_OPS 10000

static uint32_t start, end;

#define BENCHMARK_START() do {                                      \
        RSR(CCOUNT, start);                                         \
    } while(0)

#define BENCHMARK_END(OPERATION) do {                               \
        RSR(CCOUNT, end);                                           \
        printf("%s took %d cycles/op (%d cycles for %d ops)\n",     \
               OPERATION, (end - start)/REPEAT_OPS,                 \
               (end - start), REPEAT_OPS);                          \
    } while(0)

TEST_CASE("BENCHMARK for DPORT access performance", "[freertos]")
{
    BENCHMARK_START();
    for (int i = 0; i < REPEAT_OPS; i++) {
        DPORT_STALL_OTHER_CPU_START();
        _DPORT_REG_READ(DPORT_DATE_REG);
        DPORT_STALL_OTHER_CPU_END();
    }
    BENCHMARK_END("[old]DPORT access STALL OTHER CPU");


    BENCHMARK_START();
    for (int i = 0; i < REPEAT_OPS; i++) {
        DPORT_REG_READ(DPORT_DATE_REG);
    }
    BENCHMARK_END("[new]DPORT access PRE-READ APB REG");


    BENCHMARK_START();
    for (int i = 0; i < REPEAT_OPS; i++) {
        DPORT_SEQUENCE_REG_READ(DPORT_DATE_REG);
    }
    BENCHMARK_END("[seq]DPORT access PRE-READ APB REG");


    BENCHMARK_START();
    for (int i = 0; i < REPEAT_OPS; i++) {
        REG_READ(UART_DATE_REG(0));
    }
    BENCHMARK_END("REG_READ");


    BENCHMARK_START();
    for (int i = 0; i < REPEAT_OPS; i++) {
        _DPORT_REG_READ(DPORT_DATE_REG);
    }
    BENCHMARK_END("_DPORT_REG_READ");
}

uint32_t xt_highint5_read_apb;

#ifndef CONFIG_FREERTOS_UNICORE
timer_isr_handle_t inth;
xSemaphoreHandle sync_sema;

static void init_hi_interrupt(void *arg)
{
    printf("init hi_interrupt on CPU%d \n", xPortGetCoreID());
    TEST_ESP_OK(esp_intr_alloc(ETS_INTERNAL_TIMER2_INTR_SOURCE, ESP_INTR_FLAG_LEVEL5 | ESP_INTR_FLAG_IRAM, NULL, NULL, &inth));
    while (exit_flag == false);
    esp_intr_free(inth);
    printf("disable hi_interrupt on CPU%d \n", xPortGetCoreID());
    vTaskDelete(NULL);
}

static void accessDPORT2_stall_other_cpu(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    dport_test_result = true;
    while (exit_flag == false) {
        DPORT_STALL_OTHER_CPU_START();
        XTHAL_SET_CCOMPARE(2, XTHAL_GET_CCOUNT());
        xt_highint5_read_apb = 1;
        for (int i = 0; i < 200; ++i) {
            if (_DPORT_REG_READ(DPORT_DATE_REG) != _DPORT_REG_READ(DPORT_DATE_REG)) {
                apb_test_result = false;
                break;
            }
        }
        xt_highint5_read_apb = 0;
        DPORT_STALL_OTHER_CPU_END();
    }
    printf("accessDPORT2_stall_other_cpu finish\n");

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Check stall workaround DPORT and Hi-interrupt", "[esp32]")
{
    xt_highint5_read_apb = 0;
    dport_test_result    = false;
    apb_test_result      = true;
    TEST_ASSERT(xTaskCreatePinnedToCore(&init_hi_interrupt, "init_hi_intr", 2048, NULL, 6, NULL, 1) == pdTRUE);
    // Access DPORT(stall other cpu method) - CPU0
    // STALL                                - CPU1
    // Hi-interrupt                         - CPU1
    run_tasks("accessDPORT2_stall_other_cpu", accessDPORT2_stall_other_cpu, " - ", NULL, 10000);
}

static void accessDPORT2(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    dport_test_result = true;

    TEST_ESP_OK(esp_intr_alloc(ETS_INTERNAL_TIMER2_INTR_SOURCE, ESP_INTR_FLAG_LEVEL5 | ESP_INTR_FLAG_IRAM, NULL, NULL, &inth));

    while (exit_flag == false) {
        XTHAL_SET_CCOMPARE(2, XTHAL_GET_CCOUNT() + 21);
        for (int i = 0; i < 200; ++i) {
            if (DPORT_REG_READ(DPORT_DATE_REG) != DPORT_REG_READ(DPORT_DATE_REG)) {
                dport_test_result = false;
                break;
            }
        }
    }
    esp_intr_free(inth);
    printf("accessDPORT2 finish\n");

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Check pre-read workaround DPORT and Hi-interrupt", "[esp32]")
{
    xt_highint5_read_apb = 0;
    dport_test_result    = false;
    apb_test_result      = true;
    // Access DPORT(pre-read method) - CPU1
    // Hi-interrupt                  - CPU1
    run_tasks("accessAPB", accessAPB, "accessDPORT2", accessDPORT2, 10000);
}
#endif // CONFIG_FREERTOS_UNICORE
