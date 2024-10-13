/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "xtensa/core-macros.h"
#include "xtensa/hal.h"
#include "esp_types.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_clk_tree_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "xtensa_timer.h"
#include "driver/uart.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_rom_uart.h"
#include "hal/uart_types.h"
#include "hal/uart_ll.h"
#include "soc/dport_reg.h"
#include "dport_access.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
#include "esp_intr_alloc.h"
#include "esp_task.h"
#include "test_config.h"

#define MHZ (1000000)
static volatile bool exit_flag;
static bool dport_test_result;
static bool apb_test_result;
uint32_t volatile apb_intr_test_result;

static void accessDPORT(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
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
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
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
    SemaphoreHandle_t exit_sema[2];

    for (i=0; i<2; i++) {
        if((task1_func != NULL && i == 0) || (task2_func != NULL && i == 1)){
            exit_sema[i] = xSemaphoreCreateBinary();
        }
    }

    exit_flag = false;

#ifndef CONFIG_FREERTOS_UNICORE
    printf("assign task accessing DPORT to core 0 and task accessing APB to core 1\n");
    if(task1_func != NULL) xTaskCreatePinnedToCore(task1_func, task1_description, 2048, &exit_sema[0], TEST_TASK_PRIORITY - 1, &th[0], 0);
    if(task2_func != NULL) xTaskCreatePinnedToCore(task2_func, task2_description, 2048, &exit_sema[1], TEST_TASK_PRIORITY - 1, &th[1], 1);
#else
    printf("assign task accessing DPORT and accessing APB\n");
    if(task1_func != NULL) xTaskCreate(task1_func, task1_description, 2048, &exit_sema[0], TEST_TASK_PRIORITY - 1, &th[0]);
    if(task2_func != NULL) xTaskCreate(task2_func, task2_description, 2048, &exit_sema[1], TEST_TASK_PRIORITY - 1, &th[1]);
#endif

    printf("start wait for %"PRIu32" seconds [Test %s and %s]\n", delay_ms/1000, task1_description, task2_description);
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

    printf("CPU_FREQ = %"PRIu32" MHz\n", old_config.freq_mhz);

    if (cpu_freq_mhz != old_config.freq_mhz) {
        rtc_cpu_freq_config_t new_config;
        bool res = rtc_clk_cpu_freq_mhz_to_config(cpu_freq_mhz, &new_config);
        assert(res && "invalid frequency value");

        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
        rtc_clk_cpu_freq_set_config(&new_config);
        esp_clk_tree_enable_src((soc_module_clk_t)UART_SCLK_DEFAULT, true);
        uart_ll_set_sclk(UART_LL_GET_HW(uart_num), UART_SCLK_DEFAULT);

        uint32_t sclk_freq;
        TEST_ESP_OK(uart_get_sclk_freq(UART_SCLK_DEFAULT, &sclk_freq));
        uart_ll_set_baudrate(UART_LL_GET_HW(uart_num), uart_baud, sclk_freq);
        /* adjust RTOS ticks */
        _xt_tick_divisor = cpu_freq_mhz * 1000000 / XT_TICK_PER_SEC;
        vTaskDelay(2);

        printf("CPU_FREQ switched to %d MHz\n", cpu_freq_mhz);
    }
    run_tasks("accessDPORT", accessDPORT, "accessAPB", accessAPB, 10000);

    // return old freq.
    esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
    rtc_clk_cpu_freq_set_config(&old_config);
    esp_clk_tree_enable_src((soc_module_clk_t)UART_SCLK_DEFAULT, true);
    uart_ll_set_sclk(UART_LL_GET_HW(uart_num), UART_SCLK_DEFAULT);

    uint32_t sclk_freq;
    TEST_ESP_OK(uart_get_sclk_freq(UART_SCLK_DEFAULT, &sclk_freq));
    uart_ll_set_baudrate(UART_LL_GET_HW(uart_num), uart_baud, sclk_freq);
    _xt_tick_divisor = old_config.freq_mhz * 1000000 / XT_TICK_PER_SEC;
}

TEST_CASE("access DPORT and APB at same time (Freq CPU and APB = 80 MHz)", "[esp32] [ignore]")
{
    run_tasks_with_change_freq_cpu(80);
}

TEST_CASE("access DPORT and APB at same time (Freq CPU and APB = 40 MHz (XTAL))", "[esp32]")
{
    run_tasks_with_change_freq_cpu(esp_clk_xtal_freq() / MHZ);
}

static uint32_t stall_other_cpu_counter;
static uint32_t pre_reading_apb_counter;
static uint32_t apb_counter;

static void accessDPORT_stall_other_cpu(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
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
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
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
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
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
            "by different methods for %"PRIu32" seconds.\n", delay_ms/1000);
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|    Method read DPORT  |   DPORT  |    APB   |   SUMM   |\n");
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|1.Only accessAPB       |%10"PRIu32"|%10"PRIu32"|%10"PRIu32"|\n", t[0].dport, t[0].apb, t[0].summ);
    printf("|2.Only STALL_OTHER_CPU |%10"PRIu32"|%10"PRIu32"|%10"PRIu32"|\n", t[1].dport, t[1].apb, t[1].summ);
    printf("|3.Only PRE_READ_APB_REG|%10"PRIu32"|%10"PRIu32"|%10"PRIu32"|\n", t[2].dport, t[2].apb, t[2].summ);
    printf("+-----------------------+----------+----------+----------+\n");
    printf("|4.STALL_OTHER_CPU      |%10"PRIu32"|%10"PRIu32"|%10"PRIu32"|\n", t[3].dport, t[3].apb, t[3].summ);
    printf("|5.PRE_READ_APB_REG     |%10"PRIu32"|%10"PRIu32"|%10"PRIu32"|\n", t[4].dport, t[4].apb, t[4].summ);
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
        printf("%s took %"PRIu32" cycles/op (%"PRIu32" cycles for %d ops)\n",     \
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
intr_handle_t inth;
SemaphoreHandle_t sync_sema;

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
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    dport_test_result = true;
    while (exit_flag == false) {
        DPORT_STALL_OTHER_CPU_START();
        XTHAL_SET_CCOMPARE(2, esp_cpu_get_cycle_count());
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
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    dport_test_result = true;

    TEST_ESP_OK(esp_intr_alloc(ETS_INTERNAL_TIMER2_INTR_SOURCE, ESP_INTR_FLAG_LEVEL5 | ESP_INTR_FLAG_IRAM, NULL, NULL, &inth));

    while (exit_flag == false) {
        XTHAL_SET_CCOMPARE(2, esp_cpu_get_cycle_count() + 21);
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

static uint32_t s_shift_counter;

/*
The test_dport_access_reg_read() is similar DPORT_REG_READ() but has difference:
- generate an interrupt by SET_CCOMPARE
- additional branch command helps get good reproducing an issue with breaking the DPORT pre-read workaround
- uncomment (1) and comment (2) it allows seeing the broken pre-read workaround
For pre-reading the workaround, it is important that the two reading commands APB and DPORT
are executed without interruption. For this reason, it disables interrupts and to do reading inside the safe area.
But despite a disabling interrupt it was still possible that these two readings can be interrupted.
The reason is linked with work parallel execution commands in the pipeline (it is not a bug).
To resolve this issue (1) was moved to (2) position into the disabled interrupt part.
When the read command is interrupted after stage E(execute), the result of its execution will be saved in the internal buffer,
and after returning from the interrupt, this command takes this value from the buffer without repeating the reading,
which is critical for the DPORT pre-read workaround. To fix it we added additional command under safe area ((1)->(2)).
*/
static uint32_t IRAM_ATTR test_dport_access_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || defined(CONFIG_FREERTOS_UNICORE) || !SOC_DPORT_WORKAROUND
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    unsigned int intLvl;
    XTHAL_SET_CCOMPARE(2, esp_cpu_get_cycle_count() + s_shift_counter);
    __asm__ __volatile__ (\
                  /* "movi %[APB], "XTSTR(0x3ff40078)"\n" */ /* (1) uncomment for reproduce issue */ \
                  "bnez %[APB], kl1\n" /* this branch command helps get good reproducing */ \
                  "kl1:\n"\
                  "rsil %[LVL], "XTSTR(SOC_DPORT_WORKAROUND_DIS_INTERRUPT_LVL)"\n"\
                  "movi %[APB], "XTSTR(0x3ff40078)"\n" /* (2) comment for reproduce issue */ \
                  "l32i %[APB], %[APB], 0\n"\
                  "l32i %[REG], %[REG], 0\n"\
                  "wsr  %[LVL], "XTSTR(PS)"\n"\
                  "rsync\n"\
                  : [APB]"=a"(apb), [REG]"+a"(reg), [LVL]"=a"(intLvl)\
                  : \
                  : "memory" \
                  );
    return reg;
#endif
}

// The accessDPORT3 task is similar accessDPORT2 but uses test_dport_access_reg_read() instead of usual DPORT_REG_READ().
static void accessDPORT3(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    dport_test_result = true;

    TEST_ESP_OK(esp_intr_alloc(ETS_INTERNAL_TIMER2_INTR_SOURCE, ESP_INTR_FLAG_LEVEL5 | ESP_INTR_FLAG_IRAM, NULL, NULL, &inth));
    int i = 0;
    while (exit_flag == false) {
        if (test_dport_access_reg_read(DPORT_DATE_REG) != test_dport_access_reg_read(DPORT_DATE_REG)) {
            dport_test_result = false;
            break;
        }
        if ((++i % 100) == 0) {
            s_shift_counter = (s_shift_counter + 1) % 30;
        }
    }
    esp_intr_free(inth);
    printf("accessDPORT3 finish\n");

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Check pre-read workaround DPORT and Hi-interrupt (2)", "[esp32]")
{
    s_shift_counter = 1;
    xt_highint5_read_apb = 0;
    dport_test_result    = false;
    apb_test_result      = true;
    // Access DPORT(pre-read method) - CPU1
    // Hi-interrupt                  - CPU1
    run_tasks("accessAPB", accessAPB, "accessDPORT3", accessDPORT3, 10000);
}
#endif // CONFIG_FREERTOS_UNICORE

#endif // CONFIG_IDF_TARGET_ESP32
