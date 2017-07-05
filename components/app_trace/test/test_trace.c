#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "unity.h"
#include "driver/timer.h"
#include "soc/cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#if CONFIG_ESP32_APPTRACE_ENABLE == 1
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
            ets_printf(format, ##__VA_ARGS__); \
            if (xPortInIsrContext()) \
                xSemaphoreGiveFromISR(s_print_lock, NULL); \
            else \
                xSemaphoreGive(s_print_lock); \
        } \
    } while(0)
#else
#define ESP_APPTRACE_TEST_LOG( format, ... )   \
    do { \
        ets_printf(format, ##__VA_ARGS__); \
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

static void esp_apptrace_test_timer_init(int timer_group, int timer_idx, uint32_t period)
{
    timer_config_t config;
    uint64_t alarm_val = (period * (TIMER_BASE_CLK / 1000000UL)) / 2;

    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = 1;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, alarm_val);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
}

#if CONFIG_SYSVIEW_ENABLE == 0
#define ESP_APPTRACE_TEST_WRITE(_b_, _s_)            esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, ESP_APPTRACE_TMO_INFINITE)
#define ESP_APPTRACE_TEST_WRITE_FROM_ISR(_b_, _s_)   esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, 0UL)
#define ESP_APPTRACE_TEST_WRITE_NOWAIT(_b_, _s_)     esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, _b_, _s_, 0)

#define ESP_APPTRACE_TEST_CPUTICKS2US(_t_)       ((_t_)/(XT_CLOCK_FREQ/1000000))

typedef struct {
    uint8_t *buf;
    uint32_t buf_sz;
    uint8_t mask;
    uint32_t period; // trace write period in us
    uint32_t wr_err;
    uint32_t wr_cnt;
} esp_apptrace_test_gen_data_t;

typedef struct {
    int  group;
    int  id;
    void (*isr_func)(void *);
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

static uint64_t esp_apptrace_test_ts_get();

static void esp_apptrace_test_timer_isr(void *arg)
{
    esp_apptrace_test_timer_arg_t *tim_arg = (esp_apptrace_test_timer_arg_t *)arg;

    uint32_t *ts = (uint32_t *)(tim_arg->data.buf + sizeof(uint32_t));
    *ts = (uint32_t)esp_apptrace_test_ts_get();
    memset(tim_arg->data.buf + 2 * sizeof(uint32_t), tim_arg->data.wr_cnt & tim_arg->data.mask, tim_arg->data.buf_sz - 2 * sizeof(uint32_t));
    int res = ESP_APPTRACE_TEST_WRITE_FROM_ISR(tim_arg->data.buf, tim_arg->data.buf_sz);
    if (res != ESP_OK) {
    } else {
        if (0) {
            ets_printf("tim-%d-%d: Written chunk%d %d bytes, %x\n",
                       tim_arg->group, tim_arg->id, tim_arg->data.wr_cnt, tim_arg->data.buf_sz, tim_arg->data.wr_cnt & tim_arg->data.mask);
        }
        tim_arg->data.wr_err = 0;
    }

    tim_arg->data.wr_cnt++;
    if (tim_arg->group == 0) {
        if (tim_arg->id == 0) {
            TIMERG0.int_clr_timers.t0 = 1;
            TIMERG0.hw_timer[0].update = 1;
            TIMERG0.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG0.int_clr_timers.t1 = 1;
            TIMERG0.hw_timer[1].update = 1;
            TIMERG0.hw_timer[1].config.alarm_en = 1;
        }
    }
    if (tim_arg->group == 1) {
        if (tim_arg->id == 0) {
            TIMERG1.int_clr_timers.t0 = 1;
            TIMERG1.hw_timer[0].update = 1;
            TIMERG1.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG1.int_clr_timers.t1 = 1;
            TIMERG1.hw_timer[1].update = 1;
            TIMERG1.hw_timer[1].config.alarm_en = 1;
        }
    }
}

static void esp_apptrace_test_timer_isr_crash(void *arg)
{
    esp_apptrace_test_timer_arg_t *tim_arg = (esp_apptrace_test_timer_arg_t *)arg;

    if (tim_arg->group == 0) {
        if (tim_arg->id == 0) {
            TIMERG0.int_clr_timers.t0 = 1;
            TIMERG0.hw_timer[0].update = 1;
            TIMERG0.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG0.int_clr_timers.t1 = 1;
            TIMERG0.hw_timer[1].update = 1;
            TIMERG0.hw_timer[1].config.alarm_en = 1;
        }
    }
    if (tim_arg->group == 1) {
        if (tim_arg->id == 0) {
            TIMERG1.int_clr_timers.t0 = 1;
            TIMERG1.hw_timer[0].update = 1;
            TIMERG1.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG1.int_clr_timers.t1 = 1;
            TIMERG1.hw_timer[1].update = 1;
            TIMERG1.hw_timer[1].config.alarm_en = 1;
        }
    }
    if (tim_arg->data.wr_cnt < ESP_APPTRACE_TEST_BLOCKS_BEFORE_CRASH) {
        uint32_t *ts = (uint32_t *)(tim_arg->data.buf + sizeof(uint32_t));
        *ts = (uint32_t)esp_apptrace_test_ts_get();//xthal_get_ccount();//xTaskGetTickCount();
        memset(tim_arg->data.buf + 2 * sizeof(uint32_t), tim_arg->data.wr_cnt & tim_arg->data.mask, tim_arg->data.buf_sz - 2 * sizeof(uint32_t));
        int res = ESP_APPTRACE_TEST_WRITE_FROM_ISR(tim_arg->data.buf, tim_arg->data.buf_sz);
        if (res != ESP_OK) {
            ets_printf("tim-%d-%d: Failed to write trace %d %x!\n", tim_arg->group, tim_arg->id, res, tim_arg->data.wr_cnt & tim_arg->data.mask);
        } else {
            ets_printf("tim-%d-%d: Written chunk%d %d bytes, %x\n",
                       tim_arg->group, tim_arg->id, tim_arg->data.wr_cnt, tim_arg->data.buf_sz, tim_arg->data.wr_cnt & tim_arg->data.mask);
            tim_arg->data.wr_cnt++;
        }
    } else {
        uint32_t *ptr = 0;
        *ptr = 1000;
    }
}

static void esp_apptrace_dummy_task(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;
    int res, flags = 0, i;
    timer_isr_handle_t *inth = NULL;
    TickType_t tmo_ticks = arg->data.period / (1000 * portTICK_PERIOD_MS);

    ESP_APPTRACE_TEST_LOGI("%x: run dummy task (period %u us, %u timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->timers_num);

    if (arg->timers_num > 0) {
        inth = pvPortMalloc(arg->timers_num * sizeof(timer_isr_handle_t));
        if (!inth) {
            ESP_APPTRACE_TEST_LOGE("Failed to alloc timer ISR handles!");
            goto on_fail;
        }
        memset(inth, 0, arg->timers_num * sizeof(timer_isr_handle_t));
        for (int i = 0; i < arg->timers_num; i++) {
            esp_apptrace_test_timer_init(arg->timers[i].group, arg->timers[i].id, arg->timers[i].data.period);
            res = timer_isr_register(arg->timers[i].group, arg->timers[i].id, arg->timers[i].isr_func, &arg->timers[i], flags, &inth[i]);
            if (res != ESP_OK) {
                ESP_APPTRACE_TEST_LOGE("Failed to timer_isr_register (%d)!", res);
                goto on_fail;
            }
            *(uint32_t *)arg->timers[i].data.buf = (uint32_t)inth[i] | (1 << 31);
            ESP_APPTRACE_TEST_LOGI("%x: start timer %x period %u us", xTaskGetCurrentTaskHandle(), inth[i], arg->timers[i].data.period);
            res = timer_start(arg->timers[i].group, arg->timers[i].id);
            if (res != ESP_OK) {
                ESP_APPTRACE_TEST_LOGE("Failed to timer_start (%d)!", res);
                goto on_fail;
            }
        }
    }

    i = 0;
    while (!arg->stop) {
        ESP_APPTRACE_TEST_LOGD("%x: dummy task work %d.%d", xTaskGetCurrentTaskHandle(), xPortGetCoreID(), i++);
        if (tmo_ticks) {
            vTaskDelay(tmo_ticks);
        }
    }

on_fail:
    if (inth) {
        for (int i = 0; i < arg->timers_num; i++) {
            timer_pause(arg->timers[i].group, arg->timers[i].id);
            timer_disable_intr(arg->timers[i].group, arg->timers[i].id);
            if (inth[i]) {
                esp_intr_free(inth[i]);
            }
        }
        vPortFree(inth);
    }
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

static void esp_apptrace_test_task(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;
    int res, flags = 0;
    timer_isr_handle_t *inth = NULL;
    TickType_t tmo_ticks = arg->data.period / (1000 * portTICK_PERIOD_MS);

    ESP_APPTRACE_TEST_LOGI("%x: run (period %u us, stamp mask %x, %u timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->data.mask, arg->timers_num);

    if (arg->timers_num > 0) {
        inth = pvPortMalloc(arg->timers_num * sizeof(timer_isr_handle_t));
        if (!inth) {
            ESP_APPTRACE_TEST_LOGE("Failed to alloc timer ISR handles!");
            goto on_fail;
        }
        memset(inth, 0, arg->timers_num * sizeof(timer_isr_handle_t));
        for (int i = 0; i < arg->timers_num; i++) {
            esp_apptrace_test_timer_init(arg->timers[i].group, arg->timers[i].id, arg->timers[i].data.period);
            res = timer_isr_register(arg->timers[i].group, arg->timers[i].id, arg->timers[i].isr_func, &arg->timers[i], flags, &inth[i]);
            if (res != ESP_OK) {
                ESP_APPTRACE_TEST_LOGE("Failed to timer_isr_register (%d)!", res);
                goto on_fail;
            }
            *(uint32_t *)arg->timers[i].data.buf = ((uint32_t)inth[i]) | (1 << 31) | (xPortGetCoreID() ? 0x1 : 0);
            ESP_APPTRACE_TEST_LOGI("%x: start timer %x period %u us", xTaskGetCurrentTaskHandle(), inth[i], arg->timers[i].data.period);
            res = timer_start(arg->timers[i].group, arg->timers[i].id);
            if (res != ESP_OK) {
                ESP_APPTRACE_TEST_LOGE("Failed to timer_start (%d)!", res);
                goto on_fail;
            }
        }
    }

    *(uint32_t *)arg->data.buf = (uint32_t)xTaskGetCurrentTaskHandle() | (xPortGetCoreID() ? 0x1 : 0);
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
            if (1){//arg->data.wr_err++ < ESP_APPTRACE_TEST_PRN_WRERR_MAX) {
                ESP_APPTRACE_TEST_LOGE("%x: Failed to write trace %d %x!", xTaskGetCurrentTaskHandle(), res, arg->data.wr_cnt & arg->data.mask);
                if (arg->data.wr_err == ESP_APPTRACE_TEST_PRN_WRERR_MAX) {
                    ESP_APPTRACE_TEST_LOGE("\n");
                }
            }
        } else {
            if (0) {
                ESP_APPTRACE_TEST_LOGD("%x:%x: Written chunk%d %d bytes, %x", xTaskGetCurrentTaskHandle(), *ts, arg->data.wr_cnt, arg->data.buf_sz, arg->data.wr_cnt & arg->data.mask);
            }
            arg->data.wr_err = 0;
        }
        arg->data.wr_cnt++;
        if (tmo_ticks) {
            vTaskDelay(tmo_ticks);
        }
    }

on_fail:
    if (inth) {
        for (int i = 0; i < arg->timers_num; i++) {
            timer_pause(arg->timers[i].group, arg->timers[i].id);
            timer_disable_intr(arg->timers[i].group, arg->timers[i].id);
            if (inth[i]) {
                esp_intr_free(inth[i]);
            }
        }
        vPortFree(inth);
    }
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

static void esp_apptrace_test_task_crash(void *p)
{
    esp_apptrace_test_task_arg_t *arg = (esp_apptrace_test_task_arg_t *) p;
    int res, i;

    ESP_APPTRACE_TEST_LOGE("%x: run (period %u us, stamp mask %x, %u timers)", xTaskGetCurrentTaskHandle(), arg->data.period, arg->data.mask, arg->timers_num);

    arg->data.wr_cnt = 0;
    *(uint32_t *)arg->data.buf = (uint32_t)xTaskGetCurrentTaskHandle();
    for (i = 0; i < ESP_APPTRACE_TEST_BLOCKS_BEFORE_CRASH; i++) {
        uint32_t *ts = (uint32_t *)(arg->data.buf + sizeof(uint32_t));
        *ts = (uint32_t)esp_apptrace_test_ts_get();
        memset(arg->data.buf + sizeof(uint32_t), arg->data.wr_cnt & arg->data.mask, arg->data.buf_sz - sizeof(uint32_t));
        res = ESP_APPTRACE_TEST_WRITE(arg->data.buf, arg->data.buf_sz);
        if (res) {
            ESP_APPTRACE_TEST_LOGE("%x: Failed to write trace %d %x!", xTaskGetCurrentTaskHandle(), res, arg->data.wr_cnt & arg->data.mask);
        } else {
            ESP_APPTRACE_TEST_LOGD("%x: Written chunk%d %d bytes, %x", xTaskGetCurrentTaskHandle(), arg->data.wr_cnt, arg->data.buf_sz, arg->data.wr_cnt & arg->data.mask);
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

static int s_ts_timer_group, s_ts_timer_idx;

static uint64_t esp_apptrace_test_ts_get()
{
    uint64_t ts = 0;
    timer_get_counter_value(s_ts_timer_group, s_ts_timer_idx, &ts);
    return ts;
}

static void esp_apptrace_test_ts_init(int timer_group, int timer_idx)
{
    timer_config_t config;
    //uint64_t alarm_val = period * (TIMER_BASE_CLK / 1000000UL);

    ESP_APPTRACE_TEST_LOGI("Use timer%d.%d for TS", timer_group, timer_idx);

    s_ts_timer_group = timer_group;
    s_ts_timer_idx = timer_idx;

    config.alarm_en = 0;
    config.auto_reload = 0;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = 1;
    config.counter_en = 0;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Enable timer interrupt*/
    timer_start(timer_group, timer_idx);
}

static void esp_apptrace_test_ts_cleanup()
{
    timer_config_t config;

    config.alarm_en = 0;
    config.auto_reload = 0;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = 1;
    config.counter_en = 0;
    /*Configure timer*/
    timer_init(s_ts_timer_group, s_ts_timer_idx, &config);
}

static void esp_apptrace_test(esp_apptrace_test_cfg_t *test_cfg)
{
    int i, k;
    int tims_in_use[TIMER_GROUP_MAX][TIMER_MAX] = {{0, 0}, {0, 0}};
    esp_apptrace_test_task_arg_t dummy_task_arg[1];

    memset(dummy_task_arg, 0, sizeof(dummy_task_arg));
    dummy_task_arg[0].core = 0;
    dummy_task_arg[0].prio = 3;
    dummy_task_arg[0].task_func = esp_apptrace_test_task_crash;
    dummy_task_arg[0].data.buf = NULL;
    dummy_task_arg[0].data.buf_sz = 0;
    dummy_task_arg[0].data.period = 500000;
    dummy_task_arg[0].timers_num = 0;
    dummy_task_arg[0].timers = NULL;
#if ESP_APPTRACE_TEST_USE_PRINT_LOCK == 1
    s_print_lock = xSemaphoreCreateBinary();
    if (!s_print_lock) {
        ets_printf("%s: Failed to create print lock!", TAG);
        return;
    }
    xSemaphoreGive(s_print_lock);
#else
#endif

    for (i = 0; i < test_cfg->tasks_num; i++) {
        test_cfg->tasks[i].data.mask = 0xFF;
        test_cfg->tasks[i].stop = 0;
        test_cfg->tasks[i].done = xSemaphoreCreateBinary();
        if (!test_cfg->tasks[i].done) {
            ESP_APPTRACE_TEST_LOGE("Failed to create task completion semaphore!");
            goto on_fail;
        }
        for (k = 0; k < test_cfg->tasks[i].timers_num; k++) {
            test_cfg->tasks[i].timers[k].data.mask = 0xFF;
            tims_in_use[test_cfg->tasks[i].timers[k].group][test_cfg->tasks[i].timers[k].id] = 1;
        }
    }

    int found = 0;
    for (i = 0; i < TIMER_GROUP_MAX; i++) {
        for (k = 0; k < TIMER_MAX; k++) {
            if (!tims_in_use[i][k]) {
                ESP_APPTRACE_TEST_LOGD("Found timer%d.%d", i, k);
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        ESP_APPTRACE_TEST_LOGE("No free timer for TS!");
        goto on_fail;
    }
    esp_apptrace_test_ts_init(i, k);

    for (int i = 0; i < test_cfg->tasks_num; i++) {
        char name[30];
        TaskHandle_t thnd;
        sprintf(name, "apptrace_test%d", i);
        xTaskCreatePinnedToCore(test_cfg->tasks[i].task_func, name, 2048, &test_cfg->tasks[i], test_cfg->tasks[i].prio, &thnd, test_cfg->tasks[i].core);
        ESP_APPTRACE_TEST_LOGI("Created task %x", thnd);
    }
    xTaskCreatePinnedToCore(esp_apptrace_dummy_task, "dummy0", 2048, &dummy_task_arg[0], dummy_task_arg[0].prio, NULL, 0);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_apptrace_dummy_task, "dummy1", 2048, &dummy_task_arg[0], dummy_task_arg[0].prio, NULL, 1);
#endif
    for (int i = 0; i < test_cfg->tasks_num; i++) {
        //arg1.stop = 1;
        xSemaphoreTake(test_cfg->tasks[i].done, portMAX_DELAY);
    }

on_fail:
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

    s_test_timers[0].group = TIMER_GROUP_0;
    s_test_timers[0].id = TIMER_0;
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

    s_test_timers[0].group = TIMER_GROUP_0;
    s_test_timers[0].id = TIMER_0;
    s_test_timers[0].isr_func = esp_apptrace_test_timer_isr;
    s_test_timers[0].data.buf = s_bufs[0];
    s_test_timers[0].data.buf_sz = sizeof(s_bufs[0]);
    s_test_timers[0].data.period = 150;

    s_test_timers[1].group = TIMER_GROUP_1;
    s_test_timers[1].id = TIMER_0;
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

    s_test_timers[0].group = TIMER_GROUP_0;
    s_test_timers[0].id = TIMER_0;
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

    ESP_APPTRACE_TEST_LOGI("%x: run log test task", xTaskGetCurrentTaskHandle());

    int i = 0;
    while (1) {
        esp_logtrace_printf("sample print %lx %hx %c\n", 2 * i + 0x10, 2 * i + 0x20, (2 * i + 0x30) & 0xFF);
        esp_logtrace_printf("sample print %lx %hx %c %lu %hu %d %d %d %d\n", i, i + 0x10, (i + 0x20) & 0xFF, i + 0x30, i + 0x40, i + 0x50, i + 0x60, i + 0x70, i + 0x80);
        ESP_LOGI(TAG, "%p: sample print 1", xTaskGetCurrentTaskHandle());
        ESP_LOGI(TAG, "%p: sample print 2 %u", xTaskGetCurrentTaskHandle(), (unsigned)i);
        ESP_LOGI(TAG, "%p: sample print 4 %c", xTaskGetCurrentTaskHandle(), ((i & 0xFF) % 95) + 32);
        ESP_LOGI(TAG, "%p: sample print 5 %f", xTaskGetCurrentTaskHandle(), 1.0);
        ESP_LOGI(TAG, "%p: sample print 6 %f", xTaskGetCurrentTaskHandle(), 3.45);
        ESP_LOGI(TAG, "%p: logtrace task work %d.%d", xTaskGetCurrentTaskHandle(), xPortGetCoreID(), i);
        if (++i == 10000) {
            break;
        }
    }
    esp_err_t ret = esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, ESP_APPTRACE_TMO_INFINITE);
    if (ret != ESP_OK) {
        ESP_APPTRACE_TEST_LOGE("Failed to flush printf buf (%d)!", ret);
    }

    ESP_APPTRACE_TEST_LOGI("%x: finished", xTaskGetCurrentTaskHandle());

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
    ESP_APPTRACE_TEST_LOGI("Created task %x", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_logtrace_task, "logtrace1", 2048, &arg2, 3, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_logtrace_task, "logtrace1", 2048, &arg2, 3, &thnd, 0);
#endif
    ESP_APPTRACE_TEST_LOGI("Created task %x", thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
}

#else

typedef struct {
    int group;
    int timer;
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

static void esp_sysview_test_timer_isr(void *arg)
{
    esp_sysviewtrace_timer_arg_t *tim_arg = (esp_sysviewtrace_timer_arg_t *)arg;

    //ESP_APPTRACE_TEST_LOGI("tim-%d: IRQ %d/%d\n", tim_arg->id, tim_arg->group, tim_arg->timer);

    if (tim_arg->group == 0) {
        if (tim_arg->timer == 0) {
            TIMERG0.int_clr_timers.t0 = 1;
            TIMERG0.hw_timer[0].update = 1;
            TIMERG0.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG0.int_clr_timers.t1 = 1;
            TIMERG0.hw_timer[1].update = 1;
            TIMERG0.hw_timer[1].config.alarm_en = 1;
        }
    }
    if (tim_arg->group == 1) {
        if (tim_arg->timer == 0) {
            TIMERG1.int_clr_timers.t0 = 1;
            TIMERG1.hw_timer[0].update = 1;
            TIMERG1.hw_timer[0].config.alarm_en = 1;
        } else {
            TIMERG1.int_clr_timers.t1 = 1;
            TIMERG1.hw_timer[1].update = 1;
            TIMERG1.hw_timer[1].config.alarm_en = 1;
        }
    }
}

static void esp_sysviewtrace_test_task(void *p)
{
    esp_sysviewtrace_task_arg_t *arg = (esp_sysviewtrace_task_arg_t *) p;
    volatile uint32_t tmp = 0;
    timer_isr_handle_t inth;

    printf("%x: run sysview task\n", (uint32_t)xTaskGetCurrentTaskHandle());

    if (arg->timer) {
        esp_err_t res = timer_isr_register(arg->timer->group, arg->timer->timer, esp_sysview_test_timer_isr, arg->timer, arg->timer->flags, &inth);
        if (res != ESP_OK) {
            printf("%x: failed to register timer ISR\n", (uint32_t)xTaskGetCurrentTaskHandle());
        }
        else {
            res = timer_start(arg->timer->group, arg->timer->timer);
            if (res != ESP_OK) {
                printf("%x: failed to start timer\n", (uint32_t)xTaskGetCurrentTaskHandle());
            }
        }
    }

    int i = 0;
    while (1) {
        static uint32_t count;
        printf("%d", arg->id);
        if((++count % 80) == 0)
            printf("\n");
        if (arg->sync) {
            xSemaphoreTake(*arg->sync, portMAX_DELAY);
        }
        for (uint32_t k = 0; k < arg->work_count; k++) {
            tmp++;
        }
        vTaskDelay(arg->sleep_tmo/portTICK_PERIOD_MS);
        i++;
        if (arg->sync) {
            xSemaphoreGive(*arg->sync);
        }
    }
    ESP_APPTRACE_TEST_LOGI("%x: finished", xTaskGetCurrentTaskHandle());

    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("SysView trace test 1", "[trace][ignore]")
{
    TaskHandle_t thnd;

    esp_sysviewtrace_timer_arg_t tim_arg1 = {
        .group = TIMER_GROUP_1,
        .timer = TIMER_1,
        .flags = ESP_INTR_FLAG_SHARED,
        .id = 0,
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
        .group = TIMER_GROUP_1,
        .timer = TIMER_0,
        .flags = 0,
        .id = 1,
    };
    esp_sysviewtrace_task_arg_t arg2 = {
        .done = xSemaphoreCreateBinary(),
        .sync = NULL,
        .work_count = 10000,
        .sleep_tmo = 1,
        .timer = &tim_arg2,
        .id = 1,
    };

    esp_apptrace_test_timer_init(TIMER_GROUP_1, TIMER_1, 500);
    esp_apptrace_test_timer_init(TIMER_GROUP_1, TIMER_0, 100);

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace0", 2048, &arg1, 3, &thnd, 0);
    ESP_APPTRACE_TEST_LOGI("Created task %x", thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 5, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 5, &thnd, 0);
#endif
    ESP_APPTRACE_TEST_LOGI("Created task %x", thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
}

TEST_CASE("SysView trace test 2", "[trace][ignore]")
{
    TaskHandle_t thnd;

    esp_sysviewtrace_timer_arg_t tim_arg1 = {
        .group = TIMER_GROUP_1,
        .timer = TIMER_1,
        .flags = ESP_INTR_FLAG_SHARED,
        .id = 0,
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
        .group = TIMER_GROUP_1,
        .timer = TIMER_0,
        .flags = 0,
        .id = 1,
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

    esp_apptrace_test_timer_init(TIMER_GROUP_1, TIMER_1, 500);
    esp_apptrace_test_timer_init(TIMER_GROUP_1, TIMER_0, 100);

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace0", 2048, &arg1, 3, &thnd, 0);
    printf("Created task %x\n", (uint32_t)thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 4, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svtrace1", 2048, &arg2, 4, &thnd, 0);
#endif
    printf("Created task %x\n", (uint32_t)thnd);

    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync0", 2048, &arg3, 3, &thnd, 0);
    printf("Created task %x\n", (uint32_t)thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync1", 2048, &arg4, 5, &thnd, 1);
#else
    xTaskCreatePinnedToCore(esp_sysviewtrace_test_task, "svsync1", 2048, &arg4, 5, &thnd, 0);
#endif
    printf("Created task %x\n", (uint32_t)thnd);

    xSemaphoreTake(arg1.done, portMAX_DELAY);
    vSemaphoreDelete(arg1.done);
    xSemaphoreTake(arg2.done, portMAX_DELAY);
    vSemaphoreDelete(arg2.done);
    xSemaphoreTake(arg3.done, portMAX_DELAY);
    vSemaphoreDelete(arg3.done);
    xSemaphoreTake(arg4.done, portMAX_DELAY);
    vSemaphoreDelete(arg4.done);
    vSemaphoreDelete(test_sync);
}
#endif
#endif
