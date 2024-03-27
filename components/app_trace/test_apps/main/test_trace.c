/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "unity.h"
#include "driver/gptimer.h"
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_app_trace.h"
#include "esp_app_trace_util.h"

#define ESP_APPTRACE_TEST_USE_PRINT_LOCK        0
#define ESP_APPTRACE_TEST_PRN_WRERR_MAX         5
#define ESP_APPTRACE_TEST_BLOCKS_BEFORE_CRASH   100
#define ESP_APPTRACE_TEST_BLOCK_SIZE            1024

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
const static char *TAG = "esp_apptrace_test";

#if ESP_APPTRACE_TEST_USE_PRINT_LOCK == 1
#define ESP_APPTRACE_TEST_LOG( format, ... )   \
    do { \
        BaseType_t ret; \
        if (xPortInIsrContext()) \
            ret = xSemaphoreTakeFromISR(s_print_lock, NULL); \
        else \
            ret = xSemaphoreTake(s_print_lock, portMAX_DELAY); \
        if (ret == pdTRUE) { \
            esp_rom_printf(format, ##__VA_ARGS__); \
            if (xPortInIsrContext()) \
                xSemaphoreGiveFromISR(s_print_lock, NULL); \
            else \
                xSemaphoreGive(s_print_lock); \
        } \
    } while(0)
#else
#define ESP_APPTRACE_TEST_LOG( format, ... )   \
    do { \
        esp_rom_printf(format, ##__VA_ARGS__); \
    } while(0)
#endif

#define ESP_APPTRACE_TEST_LOG_LEVEL( _L_, level, format, ... )   \
    do { \
        if (LOG_LOCAL_LEVEL >= level) { \
            ESP_APPTRACE_TEST_LOG(LOG_FORMAT(_L_, format), esp_log_early_timestamp(), TAG, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_APPTRACE_TEST_LOGE( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(E, ESP_LOG_ERROR, format, ##__VA_ARGS__)
#define ESP_APPTRACE_TEST_LOGW( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(W, ESP_LOG_WARN, format, ##__VA_ARGS__)
#define ESP_APPTRACE_TEST_LOGI( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(I, ESP_LOG_INFO, format, ##__VA_ARGS__)
#define ESP_APPTRACE_TEST_LOGD( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(D, ESP_LOG_DEBUG, format, ##__VA_ARGS__)
#define ESP_APPTRACE_TEST_LOGV( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(V, ESP_LOG_VERBOSE, format, ##__VA_ARGS__)
#define ESP_APPTRACE_TEST_LOGO( format, ... )  ESP_APPTRACE_TEST_LOG_LEVEL(E, ESP_LOG_NONE, format, ##__VA_ARGS__)

#if CONFIG_APPTRACE_SV_ENABLE == 0
#define ESP_APPTRACE_TEST_WRITE(_b_, _s_)            esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, ESP_APPTRACE_TMO_INFINITE)
#define ESP_APPTRACE_TEST_WRITE_FROM_ISR(_b_, _s_)   esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, 0UL)
#define ESP_APPTRACE_TEST_WRITE_NOWAIT(_b_, _s_)     esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, 0)

typedef struct {
    uint8_t *buf;
    uint32_t buf_sz;
    uint8_t mask;
    uint32_t period; // trace write period in us
    uint32_t wr_err;
    uint32_t wr_cnt;
} esp_apptrace_test_gen_data_t;

typedef struct {
    gptimer_handle_t gptimer;
    bool (*isr_func)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
    esp_apptrace_test_gen_data_t data;
} esp_apptrace_test_timer_arg_t;

typedef struct {
    int  nowait;
    int  core;
    int  prio;
    void (*task_func)(void *);
    esp_apptrace_test_gen_data_t data;
    volatile int stop;
    SemaphoreHandle_t done;
    uint32_t timers_num;
    esp_apptrace_test_timer_arg_t *timers;
} esp_apptrace_test_task_arg_t;

typedef struct {
    uint32_t tasks_num;
    esp_apptrace_test_task_arg_t  *tasks;
} esp_apptrace_test_cfg_t;

#if ESP_APPTRACE_TEST_USE_PRINT_LOCK == 1
static SemaphoreHandle_t s_print_lock;
#endif

static uint64_t esp_apptrace_test_ts_get(void);

static bool esp_apptrace_test_timer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    esp_apptrace_test_timer_arg_t *tim_arg = (esp_apptrace_test_timer_arg_t *)user_ctx;

    uint32_t *ts = (uint32_t *)(tim_arg->data.buf + sizeof(uint32_t));
    *ts = (uint32_t)esp_apptrace_test_ts_get();
    memset(tim_arg->data.buf + 2 * sizeof(uint32_t), tim_arg->data.wr_cnt & tim_arg->data.mask, tim_arg->data.buf_sz - 2 * sizeof(uint32_t));
    int res = ESP_APPTRACE_TEST_WRITE_FROM_ISR(tim_arg->data.buf, tim_arg->data.buf_sz);
    if (res == ESP_OK) {
        tim_arg->data.wr_err = 0;
    }

    tim_arg->data.wr_cnt++;
    return true;
}

static bool esp_apptrace_test_timer_isr_crash(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    esp_apptrace_test_timer_arg_t *tim_arg = (esp_apptrace_test_timer_arg_t *)user_ctx;

    if (tim_arg->data.wr_cnt < ESP_APPTRACE_TEST_BLOCKS_BEFORE_CRASH) {
        uint32_t *ts = (uint32_t *)(tim_arg->data.buf + sizeof(uint32_t));
        *ts = (uint32_t)esp_apptrace_test_ts_get();//xthal_get_ccount();//xTaskGetTickCount();
        memset(tim_arg->data.buf + 2 * sizeof(uint32_t), tim_arg->data.wr_cnt & tim_arg->data.mask, tim_arg->data.buf_sz - 2 * sizeof(uint32_t));
        int res = ESP_APPTRACE_TEST_WRITE_FROM_ISR(tim_arg->data.buf, tim_arg->data.buf_sz);
        if (res != ESP_OK) {
            esp_rom_printf("tim-%p: Failed to write trace %d %" PRIx32 "!\n", tim_arg->gptimer, res, tim_arg->data.wr_cnt & tim_arg->data.mask);
        } else {
            esp_rom_printf("tim-%p: Written chunk%" PRIu32 " %" PRIu32 " bytes, %" PRIx32 "\n",
                           timer, tim_arg->data.wr_cnt, tim_arg->data.buf_sz, tim_arg->data.wr_cnt & tim_arg->data.mask);
            tim_arg->data.wr_cnt++;
        }
    } else {
        uint32_t *ptr = 0;
        *ptr = 1000;
    }
    return true;
}

static void esp_apptrace_dummy_task(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;
    TickType_t tmo_ticks = arg->data.period / (1000 * portTICK_PERIOD_MS);

    ESP_APPTRACE_TEST_LOGI("%p: run dummy task (period %" PRIu32 " us, %" PRIu32 " timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->timers_num);

    for (int i = 0; i < arg->timers_num; i++) {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1000000,
        };
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &arg->timers[i].gptimer));
        *(uint32_t *)arg->timers[i].data.buf = (uint32_t)arg->timers[i].gptimer | (1 << 31);
        ESP_APPTRACE_TEST_LOGI("%p: start timer %p period %" PRIu32 " us", xTaskGetCurrentTaskHandle(), arg->timers[i].gptimer, arg->timers[i].data.period);
        gptimer_alarm_config_t alarm_config = {
            .reload_count = 0,
            .alarm_count = arg->timers[i].data.period,
            .flags.auto_reload_on_alarm = true,
        };
        gptimer_event_callbacks_t cbs = {
            .on_alarm = arg->timers[i].isr_func,
        };
        TEST_ESP_OK(gptimer_register_event_callbacks(arg->timers[i].gptimer, &cbs, &arg->timers[i]));
        TEST_ESP_OK(gptimer_enable(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_set_alarm_action(arg->timers[i].gptimer, &alarm_config));
        TEST_ESP_OK(gptimer_start(arg->timers[i].gptimer));
    }

    int i = 0;
    while (!arg->stop) {
        ESP_APPTRACE_TEST_LOGD("%p: dummy task work %d.%d", xTaskGetCurrentTaskHandle(), esp_cpu_get_core_id(), i++);
        if (tmo_ticks) {
            vTaskDelay(tmo_ticks);
        }
    }

    for (int i = 0; i < arg->timers_num; i++) {
        TEST_ESP_OK(gptimer_stop(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_disable(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_del_timer(arg->timers[i].gptimer));
    }
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

static void esp_apptrace_test_task(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;
    int res;
    TickType_t tmo_ticks = arg->data.period / (1000 * portTICK_PERIOD_MS);

    ESP_APPTRACE_TEST_LOGI("%p: run (period %" PRIu32 " us, stamp mask %" PRIx8 ", %" PRIu32 " timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->data.mask, arg->timers_num);

    for (int i = 0; i < arg->timers_num; i++) {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1000000,
        };
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &arg->timers[i].gptimer));
        *(uint32_t *)arg->timers[i].data.buf = ((uint32_t)arg->timers[i].gptimer) | (1 << 31) | (esp_cpu_get_core_id() ? 0x1 : 0);
        ESP_APPTRACE_TEST_LOGI("%p: start timer %p period %" PRIu32 " us", xTaskGetCurrentTaskHandle(), arg->timers[i].gptimer, arg->timers[i].data.period);
        gptimer_alarm_config_t alarm_config = {
            .reload_count = 0,
            .alarm_count = arg->timers[i].data.period,
            .flags.auto_reload_on_alarm = true,
        };
        gptimer_event_callbacks_t cbs = {
            .on_alarm = arg->timers[i].isr_func,
        };
        TEST_ESP_OK(gptimer_register_event_callbacks(arg->timers[i].gptimer, &cbs, &arg->timers[i]));
        TEST_ESP_OK(gptimer_enable(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_set_alarm_action(arg->timers[i].gptimer, &alarm_config));
        TEST_ESP_OK(gptimer_start(arg->timers[i].gptimer));
    }

    *(uint32_t *)arg->data.buf = (uint32_t)xTaskGetCurrentTaskHandle() | (esp_cpu_get_core_id() ? 0x1 : 0);
    arg->data.wr_cnt = 0;
    arg->data.wr_err = 0;
    while (!arg->stop) {
        uint32_t *ts = (uint32_t *)(arg->data.buf + sizeof(uint32_t));
        *ts = (uint32_t)esp_apptrace_test_ts_get();
        memset(arg->data.buf + 2 * sizeof(uint32_t), arg->data.wr_cnt & arg->data.mask, arg->data.buf_sz - 2 * sizeof(uint32_t));
        // ESP_APPTRACE_TEST_LOGD("%x:%x: Write chunk%d %d bytes, %x", xTaskGetCurrentTaskHandle(), *ts, arg->data.wr_cnt, arg->data.buf_sz, arg->data.wr_cnt & arg->data.mask);
        if (arg->nowait) {
            res = ESP_APPTRACE_TEST_WRITE_NOWAIT(arg->data.buf, arg->data.buf_sz);
        } else {
            res = ESP_APPTRACE_TEST_WRITE(arg->data.buf, arg->data.buf_sz);
        }
        if (res) {
            if (1) { //arg->data.wr_err++ < ESP_APPTRACE_TEST_PRN_WRERR_MAX) {
                ESP_APPTRACE_TEST_LOGE("%p: Failed to write trace %d %" PRIx32 "!", xTaskGetCurrentTaskHandle(), res, arg->data.wr_cnt & arg->data.mask);
                if (arg->data.wr_err == ESP_APPTRACE_TEST_PRN_WRERR_MAX) {
                    ESP_APPTRACE_TEST_LOGE("\n");
                }
            }
        } else {
            if (0) {
                ESP_APPTRACE_TEST_LOGD("%p:%" PRIx32 ": Written chunk%" PRIu32 " %" PRIu32 " bytes, %" PRIx32, xTaskGetCurrentTaskHandle(), *ts, arg->data.wr_cnt, arg->data.buf_sz, arg->data.wr_cnt & arg->data.mask);
            }
            arg->data.wr_err = 0;
        }
        arg->data.wr_cnt++;
        if (tmo_ticks) {
            vTaskDelay(tmo_ticks);
        }
    }

    for (int i = 0; i < arg->timers_num; i++) {
        TEST_ESP_OK(gptimer_stop(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_disable(arg->timers[i].gptimer));
        TEST_ESP_OK(gptimer_del_timer(arg->timers[i].gptimer));
    }
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

static void esp_apptrace_test_task_crash(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;

    ESP_APPTRACE_TEST_LOGE("%p: run (period %" PRIu32 " us, stamp mask %" PRIx8 ", %" PRIu32 " timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->data.mask, arg->timers_num);

    arg->data.wr_cnt = 0;
    *(uint32_t *)arg->data.buf = (uint32_t)xTaskGetCurrentTaskHandle();
    for (int i = 0; i < ESP_APPTRACE_TEST_BLOCKS_BEFORE_CRASH; i++) {
        uint32_t *ts = (uint32_t *)(arg->data.buf + sizeof(uint32_t));
        *ts = (uint32_t)esp_apptrace_test_ts_get();
        memset(arg->data.buf + sizeof(uint32_t), arg->data.wr_cnt & arg->data.mask, arg->data.buf_sz - sizeof(uint32_t));
        int res = ESP_APPTRACE_TEST_WRITE(arg->data.buf, arg->data.buf_sz);
        if (res) {
            ESP_APPTRACE_TEST_LOGE("%p: Failed to write trace %d %" PRIx32 "!", xTaskGetCurrentTaskHandle(), res, arg->data.wr_cnt & arg->data.mask);
        } else {
            ESP_APPTRACE_TEST_LOGD("%p: Written chunk%" PRIu32 " %" PRIu32 " bytes, %" PRIx32, xTaskGetCurrentTaskHandle(), arg->data.wr_cnt, arg->data.buf_sz, arg->data.wr_cnt & arg->data.mask);
        }
        arg->data.wr_cnt++;
    }
    vTaskDelay(500);
    uint32_t *ptr = 0;
    *ptr = 1000;

    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

static gptimer_handle_t ts_gptimer;

static uint64_t esp_apptrace_test_ts_get(void)
{
    uint64_t ts = 0;
    gptimer_get_raw_count(ts_gptimer, &ts);
    return ts;
}

static void esp_apptrace_test_ts_init(void)
{
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 10000000,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &ts_gptimer));
    ESP_APPTRACE_TEST_LOGI("Use timer %p for TS", ts_gptimer);
    TEST_ESP_OK(gptimer_enable(ts_gptimer));
    TEST_ESP_OK(gptimer_start(ts_gptimer));
}

static void esp_apptrace_test_ts_cleanup(void)
{
    TEST_ESP_OK(gptimer_stop(ts_gptimer));
    TEST_ESP_OK(gptimer_disable(ts_gptimer));
    TEST_ESP_OK(gptimer_del_timer(ts_gptimer));
    ts_gptimer = NULL;
}

static void esp_apptrace_test(esp_apptrace_test_cfg_t *test_cfg)
{
    esp_apptrace_test_task_arg_t dummy_task_arg = {
        .core = 0,
        .prio = 3,
        .task_func = esp_apptrace_test_task_crash,
        .data.buf = NULL,
        .data.buf_sz = 0,
        .data.period = 500000,
        .timers_num = 0,
        .timers = NULL,
    };

#if ESP_APPTRACE_TEST_USE_PRINT_LOCK == 1
    s_print_lock = xSemaphoreCreateBinary();
    if (!s_print_lock) {
        esp_rom_printf("%s: Failed to create print lock!", TAG);
        return;
    }
    xSemaphoreGive(s_print_lock);
#else
#endif

    for (int i = 0; i < test_cfg->tasks_num; i++) {
        test_cfg->tasks[i].data.mask = 0xFF;
        test_cfg->tasks[i].stop = 0;
        test_cfg->tasks[i].done = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_NULL(test_cfg->tasks[i].done);
        for (int k = 0; k < test_cfg->tasks[i].timers_num; k++) {
            test_cfg->tasks[i].timers[k].data.mask = 0xFF;
        }
    }

    esp_apptrace_test_ts_init();

    for (int i = 0; i < test_cfg->tasks_num; i++) {
        char name[30];
        TaskHandle_t thnd;
        sprintf(name, "apptrace_test%d", i);
        xTaskCreatePinnedToCore(test_cfg->tasks[i].task_func, name, 2048, &test_cfg->tasks[i], test_cfg->tasks[i].prio, &thnd, test_cfg->tasks[i].core);
        ESP_APPTRACE_TEST_LOGI("Created task %p", thnd);
    }
    xTaskCreatePinnedToCore(esp_apptrace_dummy_task, "dummy0", 2048, &dummy_task_arg, dummy_task_arg.prio, NULL, 0);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_apptrace_dummy_task, "dummy1", 2048, &dummy_task_arg, dummy_task_arg.prio, NULL, 1);
#endif
    for (int i = 0; i < test_cfg->tasks_num; i++) {
        //arg1.stop = 1;
        xSemaphoreTake(test_cfg->tasks[i].done, portMAX_DELAY);
    }

    for (int i = 0; i < test_cfg->tasks_num; i++) {
        if (test_cfg->tasks[i].done) {
            vSemaphoreDelete(test_cfg->tasks[i].done);
        }
    }
    esp_apptrace_test_ts_cleanup();

#if ESP_APPTRACE_TEST_USE_PRINT_LOCK == 1
    vSemaphoreDelete(s_print_lock);
#else
#endif
}

static esp_apptrace_test_task_arg_t s_test_tasks[4];
static esp_apptrace_test_timer_arg_t s_test_timers[2];
static uint8_t s_bufs[6][ESP_APPTRACE_TEST_BLOCK_SIZE];

TEST_CASE("App trace test (1 task + 1 crashed timer ISR @ 1 core)", "[trace][ignore]")
{
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 1,
        .tasks = s_test_tasks,
    };

    memset(s_test_timers, 0, sizeof(s_test_timers));
    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_timers[0].isr_func = esp_apptrace_test_timer_isr_crash;
    s_test_timers[0].data.buf = s_bufs[0];
    s_test_timers[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_timers[0].data.period = 1000;

    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_dummy_task;
    s_test_tasks[0].data.buf = NULL;
    s_test_tasks[0].data.buf_sz = 0;
    s_test_tasks[0].data.period = 1000000;
    s_test_tasks[0].timers_num = 1;
    s_test_tasks[0].timers = s_test_timers;

    esp_apptrace_test(&test_cfg);
}

TEST_CASE("App trace test (1 crashed task)", "[trace][ignore]")
{
    esp_apptrace_test_task_arg_t s_test_tasks[1];
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 1,
        .tasks = s_test_tasks,
    };

    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_test_task_crash;
    s_test_tasks[0].data.buf = s_bufs[0];
    s_test_tasks[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_tasks[0].data.period = 6000;
    s_test_tasks[0].timers_num = 0;
    s_test_tasks[0].timers = NULL;

    esp_apptrace_test(&test_cfg);
}

#if CONFIG_FREERTOS_UNICORE == 0
TEST_CASE("App trace test (2 tasks + 1 timer @ each core", "[trace][ignore]")
{
    int ntask = 0;
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 4,
        .tasks = s_test_tasks,
    };

    memset(s_test_tasks, 0, sizeof(s_test_tasks));
    memset(s_test_timers, 0, sizeof(s_test_timers));

    s_test_timers[0].isr_func = esp_apptrace_test_timer_isr;
    s_test_timers[0].data.buf = s_bufs[0];
    s_test_timers[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_timers[0].data.period = 150;

    s_test_timers[1].isr_func = esp_apptrace_test_timer_isr;
    s_test_timers[1].data.buf = s_bufs[1];
    s_test_timers[1].data.buf_sz = sizeof(s_bufs[1]);
    s_test_timers[1].data.period = 150;

    s_test_tasks[ntask].core = 0;
    s_test_tasks[ntask].prio = 4;
    s_test_tasks[ntask].task_func = esp_apptrace_test_task;
    s_test_tasks[ntask].data.buf = s_bufs[2];
    s_test_tasks[ntask].data.buf_sz = sizeof(s_bufs[2]);
    s_test_tasks[ntask].data.period = 1000;
    s_test_tasks[ntask].timers_num = 1;
    s_test_tasks[ntask].timers = &s_test_timers[0];
    ntask++;
    s_test_tasks[ntask].core = 0;
    s_test_tasks[ntask].prio = 3;
    s_test_tasks[ntask].task_func = esp_apptrace_test_task;
    s_test_tasks[ntask].data.buf = s_bufs[3];
    s_test_tasks[ntask].data.buf_sz = sizeof(s_bufs[3]);
    s_test_tasks[ntask].data.period = 0;
    s_test_tasks[ntask].timers_num = 0;
    s_test_tasks[ntask].timers = NULL;
    ntask++;
    s_test_tasks[ntask].core = 1;
    s_test_tasks[ntask].prio = 4;
    s_test_tasks[ntask].task_func = esp_apptrace_test_task;
    s_test_tasks[ntask].data.buf = s_bufs[4];
    s_test_tasks[ntask].data.buf_sz = sizeof(s_bufs[4]);
    s_test_tasks[ntask].data.period = 1000;
    s_test_tasks[ntask].timers_num = 1;
    s_test_tasks[ntask].timers = &s_test_timers[1];
    ntask++;
    s_test_tasks[ntask].core = 1;
    s_test_tasks[ntask].prio = 3;
    s_test_tasks[ntask].task_func = esp_apptrace_test_task;
    s_test_tasks[ntask].data.buf = s_bufs[5];
    s_test_tasks[ntask].data.buf_sz = sizeof(s_bufs[5]);
    s_test_tasks[ntask].data.period = 0;
    s_test_tasks[ntask].timers_num = 0;
    s_test_tasks[ntask].timers = NULL;
    ntask++;
    esp_apptrace_test(&test_cfg);
}
#endif

TEST_CASE("App trace test (1 task + 1 timer @ 1 core)", "[trace][ignore]")
{
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 1,
        .tasks = s_test_tasks,
    };

    memset(s_test_timers, 0, sizeof(s_test_timers));
    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_timers[0].isr_func = esp_apptrace_test_timer_isr;
    s_test_timers[0].data.buf = s_bufs[0];
    s_test_timers[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_timers[0].data.period = 150;

    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_test_task;
    s_test_tasks[0].data.buf = s_bufs[1];
    s_test_tasks[0].data.buf_sz = sizeof(s_bufs[1]);
    s_test_tasks[0].data.period = 0;
    s_test_tasks[0].timers_num = 1;
    s_test_tasks[0].timers = s_test_timers;

    esp_apptrace_test(&test_cfg);
}

#if CONFIG_FREERTOS_UNICORE == 0
TEST_CASE("App trace test (2 tasks (nowait): 1 @ each core)", "[trace][ignore]")
{
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 2,
        .tasks = s_test_tasks,
    };

    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_tasks[0].nowait = 1;
    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_test_task;
    s_test_tasks[0].data.buf = s_bufs[0];
    s_test_tasks[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_tasks[0].data.period = 6700;
    s_test_tasks[0].timers_num = 0;
    s_test_tasks[0].timers = NULL;

    s_test_tasks[1].nowait = 1;
    s_test_tasks[1].core = 1;
    s_test_tasks[1].prio = 3;
    s_test_tasks[1].task_func = esp_apptrace_test_task;
    s_test_tasks[1].data.buf = s_bufs[1];
    s_test_tasks[1].data.buf_sz = sizeof(s_bufs[1]);
    s_test_tasks[1].data.period = 6700;
    s_test_tasks[1].timers_num = 0;
    s_test_tasks[1].timers = NULL;

    esp_apptrace_test(&test_cfg);
}

TEST_CASE("App trace test (2 tasks: 1 @ each core)", "[trace][ignore]")
{
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 2,
        .tasks = s_test_tasks,
    };

    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_test_task;
    s_test_tasks[0].data.buf = s_bufs[0];
    s_test_tasks[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_tasks[0].data.period = 0;
    s_test_tasks[0].timers_num = 0;
    s_test_tasks[0].timers = NULL;

    s_test_tasks[1].core = 1;
    s_test_tasks[1].prio = 3;
    s_test_tasks[1].task_func = esp_apptrace_test_task;
    s_test_tasks[1].data.buf = s_bufs[1];
    s_test_tasks[1].data.buf_sz = sizeof(s_bufs[1]);
    s_test_tasks[1].data.period = 0;
    s_test_tasks[1].timers_num = 0;
    s_test_tasks[1].timers = NULL;

    esp_apptrace_test(&test_cfg);
}
#endif

TEST_CASE("App trace test (1 task)", "[trace][ignore]")
{
    esp_apptrace_test_cfg_t test_cfg = {
        .tasks_num = 1,
        .tasks = s_test_tasks,
    };

    memset(s_test_tasks, 0, sizeof(s_test_tasks));

    s_test_tasks[0].core = 0;
    s_test_tasks[0].prio = 3;
    s_test_tasks[0].task_func = esp_apptrace_test_task;
    s_test_tasks[0].data.buf = s_bufs[0];
    s_test_tasks[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_tasks[0].data.period = 0;
    s_test_tasks[0].timers_num = 0;
    s_test_tasks[0].timers = NULL;

    esp_apptrace_test(&test_cfg);
}

static int esp_logtrace_printf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    int ret = esp_apptrace_vprintf_to(ESP_APPTRACE_DEST_TRAX, ESP_APPTRACE_TMO_INFINITE, fmt, ap);

    va_end(ap);

    return ret;
}

typedef struct {
    SemaphoreHandle_t done;
    uint32_t work_count;
} esp_logtrace_task_t;

static void esp_logtrace_task(void *p)
{
    esp_logtrace_task_t *arg = (esp_logtrace_task_t *) p;

    ESP_APPTRACE_TEST_LOGI("%p: run log test task", xTaskGetCurrentTaskHandle());

    int i = 0;
    while (1) {
        esp_logtrace_printf("sample print %lx %hx %c\n", 2 * i + 0x10, 2 * i + 0x20, (2 * i + 0x30) & 0xFF);
        esp_logtrace_printf("sample print %lx %hx %c %lu %hu %d %d %d %d\n", i, i + 0x10, (i + 0x20) & 0xFF, i + 0x30, i + 0x40, i + 0x50, i + 0x60, i + 0x70, i + 0x80);
        ESP_LOGI(TAG, "%p: sample print 1", xTaskGetCurrentTaskHandle());
        ESP_LOGI(TAG, "%p: sample print 2 %u", xTaskGetCurrentTaskHandle(), (unsigned)i);
        ESP_LOGI(TAG, "%p: sample print 4 %c", xTaskGetCurrentTaskHandle(), ((i & 0xFF) % 95) + 32);
        ESP_LOGI(TAG, "%p: sample print 5 %f", xTaskGetCurrentTaskHandle(), 1.0);
        ESP_LOGI(TAG, "%p: sample print 6 %f", xTaskGetCurrentTaskHandle(), 3.45);
        ESP_LOGI(TAG, "%p: logtrace task work %d.%d", xTaskGetCurrentTaskHandle(), esp_cpu_get_core_id(), i);
        if (++i == 10000) {
            break;
        }
    }
    esp_err_t ret = esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, ESP_APPTRACE_TMO_INFINITE);
    if (ret != ESP_OK) {
        ESP_APPTRACE_TEST_LOGE("Failed to flush printf buf (%d)!", ret);
    }

    ESP_APPTRACE_TEST_LOGI("%p: finished", xTaskGetCurrentTaskHandle());

    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("Log trace test (2 tasks)", "[trace][ignore]")
{
    TaskHandle_t thnd;

    esp_logtrace_task_t arg1 = {
        .done = xSemaphoreCreateBinary(),
    };
    esp_logtrace_task_t arg2 = {
        .done = xSemaphoreCreateBinary(),
    };

    xTaskCreatePinnedToCore(esp_logtrace_task, "logtrace0", 2048, &arg1, 3, &thnd, 0);
    ESP_APPTRACE_TEST_LOGI("Created task %p", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_logtrace_task, "logtrace1", 2048, &arg2, 3, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_logtrace_task, "logtrace1", 2048, &arg2, 3, &thnd, 0);
#endif
    ESP_APPTRACE_TEST_LOGI("Created task %p", thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
}

#else // #if CONFIG_APPTRACE_SV_ENABLE == 0

typedef struct {
    gptimer_handle_t gptimer;
    uint32_t period;
    int flags;
    uint32_t id;
} esp_sysviewtrace_timer_arg_t;

typedef struct {
    SemaphoreHandle_t done;
    SemaphoreHandle_t *sync;
    esp_sysviewtrace_timer_arg_t *timer;
    uint32_t work_count;
    uint32_t sleep_tmo;
    uint32_t id;
} esp_sysviewtrace_task_arg_t;

static bool esp_sysview_test_timer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    esp_sysviewtrace_timer_arg_t *tim_arg = (esp_sysviewtrace_timer_arg_t *)user_ctx;
    (void) tim_arg;
    return false;
}

static void esp_sysviewtrace_test_task(void *p)
{
    esp_sysviewtrace_task_arg_t *arg = (esp_sysviewtrace_task_arg_t *) p;
    volatile uint32_t tmp = 0;
    printf("%p: run sysview task\n", xTaskGetCurrentTaskHandle());

    if (arg->timer) {
        gptimer_alarm_config_t alarm_config = {
            .reload_count = 0,
            .alarm_count = arg->timer->period,
            .flags.auto_reload_on_alarm = true,
        };
        gptimer_event_callbacks_t cbs = {
            .on_alarm = esp_sysview_test_timer_isr,
        };
        TEST_ESP_OK(gptimer_register_event_callbacks(arg->timer->gptimer, &cbs, arg->timer));
        TEST_ESP_OK(gptimer_enable(arg->timer->gptimer));
        TEST_ESP_OK(gptimer_set_alarm_action(arg->timer->gptimer, &alarm_config));
        TEST_ESP_OK(gptimer_start(arg->timer->gptimer));
    }

    int i = 0;
    while (1) {
        static uint32_t count;
        printf("%" PRIu32, arg->id);
        if ((++count % 80) == 0) {
            printf("\n");
        }
        if (arg->sync) {
            xSemaphoreTake(*arg->sync, portMAX_DELAY);
        }
        for (uint32_t k = 0; k < arg->work_count; k++) {
            tmp++;
        }
        vTaskDelay(arg->sleep_tmo / portTICK_PERIOD_MS);
        i++;
        if (arg->sync) {
            xSemaphoreGive(*arg->sync);
        }
    }
    ESP_APPTRACE_TEST_LOGI("%p: finished", xTaskGetCurrentTaskHandle());

    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("SysView trace test 1", "[trace][ignore]")
{
    TaskHandle_t thnd;

    esp_sysviewtrace_timer_arg_t tim_arg1 = {
        .flags = ESP_INTR_FLAG_SHARED,
        .id = 0,
        .period = 500,
    };
    esp_sysviewtrace_task_arg_t arg1 = {
        .done = xSemaphoreCreateBinary(),
        .sync = NULL,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = &tim_arg1,
        .id = 0,
    };
    esp_sysviewtrace_timer_arg_t tim_arg2 = {
        .flags = 0,
        .id = 1,
        .period = 100,
    };
    esp_sysviewtrace_task_arg_t arg2 = {
        .done = xSemaphoreCreateBinary(),
        .sync = NULL,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = &tim_arg2,
        .id = 1,
    };

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    timer_config.flags.intr_shared = (tim_arg1.flags & ESP_INTR_FLAG_SHARED) == ESP_INTR_FLAG_SHARED;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &tim_arg1.gptimer));
    timer_config.flags.intr_shared = (tim_arg2.flags & ESP_INTR_FLAG_SHARED) == ESP_INTR_FLAG_SHARED;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &tim_arg2.gptimer));

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace0", 2048, &arg1, 3, &thnd, 0);
    ESP_APPTRACE_TEST_LOGI("Created task %p", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 5, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 5, &thnd, 0);
#endif
    ESP_APPTRACE_TEST_LOGI("Created task %p", thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
    TEST_ESP_OK(gptimer_stop(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_disable(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_del_timer(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_stop(tim_arg2.gptimer));
    TEST_ESP_OK(gptimer_disable(tim_arg2.gptimer));
    TEST_ESP_OK(gptimer_del_timer(tim_arg2.gptimer));
}

TEST_CASE("SysView trace test 2", "[trace][ignore]")
{
    TaskHandle_t thnd;

    esp_sysviewtrace_timer_arg_t tim_arg1 = {
        .flags = ESP_INTR_FLAG_SHARED,
        .id = 0,
        .period = 500,
    };
    esp_sysviewtrace_task_arg_t arg1 = {
        .done = xSemaphoreCreateBinary(),
        .sync = NULL,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = &tim_arg1,
        .id = 0,
    };
    esp_sysviewtrace_timer_arg_t tim_arg2 = {
        .flags = 0,
        .id = 1,
        .period = 100,
    };
    esp_sysviewtrace_task_arg_t arg2 = {
        .done = xSemaphoreCreateBinary(),
        .sync = NULL,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = &tim_arg2,
        .id = 1,
    };

    SemaphoreHandle_t test_sync = xSemaphoreCreateBinary();
    xSemaphoreGive(test_sync);
    esp_sysviewtrace_task_arg_t arg3 = {
        .done = xSemaphoreCreateBinary(),
        .sync = &test_sync,
        .work_count = 1000,
        .sleep_tmo = 1,
        .timer = NULL,
        .id = 2,
    };
    esp_sysviewtrace_task_arg_t arg4 = {
        .done = xSemaphoreCreateBinary(),
        .sync = &test_sync,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = NULL,
        .id = 3,
    };

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    timer_config.flags.intr_shared = (tim_arg1.flags & ESP_INTR_FLAG_SHARED) == ESP_INTR_FLAG_SHARED;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &tim_arg1.gptimer));
    timer_config.flags.intr_shared = (tim_arg2.flags & ESP_INTR_FLAG_SHARED) == ESP_INTR_FLAG_SHARED;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &tim_arg2.gptimer));

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace0", 2048, &arg1, 3, &thnd, 0);
    printf("Created task %p\n", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 4, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 4, &thnd, 0);
#endif
    printf("Created task %p\n", thnd);

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync0", 2048, &arg3, 3, &thnd, 0);
    printf("Created task %p\n", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync1", 2048, &arg4, 5, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync1", 2048, &arg4, 5, &thnd, 0);
#endif
    printf("Created task %p\n", thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
    xSemaphoreTake(arg3.done, portMAX_DELAY);
    vSemaphoreDelete(arg3.done);
    xSemaphoreTake(arg4.done, portMAX_DELAY);
    vSemaphoreDelete(arg4.done);
    vSemaphoreDelete(test_sync);
    TEST_ESP_OK(gptimer_stop(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_disable(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_del_timer(tim_arg1.gptimer));
    TEST_ESP_OK(gptimer_stop(tim_arg2.gptimer));
    TEST_ESP_OK(gptimer_disable(tim_arg2.gptimer));
    TEST_ESP_OK(gptimer_del_timer(tim_arg2.gptimer));
}
#endif // #if CONFIG_APPTRACE_SV_ENABLE == 0
