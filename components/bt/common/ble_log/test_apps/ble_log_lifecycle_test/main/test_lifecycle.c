/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "esp_bt.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_rt.h"
#include "ble_log_prph_test.h"

#if CONFIG_IDF_TARGET_ESP32S31 || CONFIG_IDF_TARGET_ESP32H4
#define CONTROLLER_TASK_NAME "btdm"
#elif CONFIG_IDF_TARGET_ESP32C2
#define CONTROLLER_TASK_NAME "ll"
#elif CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C6 || \
      CONFIG_IDF_TARGET_ESP32C61 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32H21
#define CONTROLLER_TASK_NAME "ble_ll_task"
#else
#error "This lifecycle case requires a supported BLE Log v2 controller"
#endif

static bool s_armed;
static bool s_inside_log;
static TaskHandle_t s_blocked_task;
static uint32_t s_callback_flag;
static uint8_t s_read_buf[BLE_LOG_POOL_TRANS_SIZE];

void __real_ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                               uint32_t append_len, const uint8_t *append, uint32_t flag);
void __wrap_ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                               uint32_t append_len, const uint8_t *append, uint32_t flag)
{
    if (__atomic_load_n(&s_armed, __ATOMIC_ACQUIRE)) {
        /* Inject only on a real controller-task callback; keep the command sender runnable. */
        if (xPortInIsrContext() || !xPortCanYield() || strcmp(pcTaskGetName(NULL), CONTROLLER_TASK_NAME) ||
                (flag & BIT(BLE_LOG_LL_FLAG_ISR))) {
            return;
        }
        /* One injected call per round; preserve the controller's original blocking policy. */
        if (__atomic_load_n(&s_blocked_task, __ATOMIC_ACQUIRE)) {
            return;
        }
        s_callback_flag = flag;
        __atomic_store_n(&s_blocked_task, xTaskGetCurrentTaskHandle(), __ATOMIC_RELEASE);
        static const uint8_t marker = 0x55;
        __atomic_store_n(&s_inside_log, true, __ATOMIC_RELEASE);
        __real_ble_log_write_hex_ll(1, &marker, 0, NULL, flag);
        __atomic_store_n(&s_inside_log, false, __ATOMIC_RELEASE);
        return;
    }
    __real_ble_log_write_hex_ll(len, addr, append_len, append, flag);
}

#ifdef LOG_STOP_NEGATIVE_CONTROL
void __wrap_ble_log_pause(void)
{
    /* Deliberately reproduce deletion before cancellation, without changing production code. */
}
#endif

static void recycle(void *arg)
{
    (void)arg;
}

static void fill_pool(void)
{
    uint32_t handle;
    unsigned filled = 0;
    const size_t len = BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t);
    uint8_t *data;
    while ((data = ble_log_claim(BLE_LOG_SRC_ENCODE, len, &handle, false))) {
        memset(data, 0x5a, len);
        ble_log_commit(handle, len);
        filled++;
    }
    assert(filled > 0);
    ble_log_lbm_flush_open_trans();
    assert(ble_log_rt_drain());
}

static void send_available(void) {}
static int receive(uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;
    return 0;
}

static void run_round(unsigned round)
{
    esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    unsigned controller_core = round % CONFIG_FREERTOS_NUMBER_OF_CORES;
#if CONFIG_IDF_TARGET_ESP32S31 || CONFIG_IDF_TARGET_ESP32H4
    /* Exercise both runtime affinities, including the non-default core. */
    cfg.btdm.task_run_cpu = controller_core;
#else
    cfg.controller_run_cpu = controller_core;
#endif
    assert(ble_log_init());
    ble_log_prph_test_set_auto_recycle_hook(recycle, NULL);
    ESP_ERROR_CHECK(esp_bt_controller_init(&cfg));
    static const esp_vhci_host_callback_t callbacks = {
        .notify_host_send_available = send_available,
        .notify_host_recv = receive,
    };
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_vhci_host_register_callback(&callbacks));
    vTaskDelay(pdMS_TO_TICKS(20));
    assert(ble_log_rt_drain());
    ble_log_prph_test_set_auto_recycle_hook(NULL, NULL);
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL)) {}
    __atomic_store_n(&s_blocked_task, NULL, __ATOMIC_RELEASE);
    __atomic_store_n(&s_inside_log, false, __ATOMIC_RELEASE);
    __atomic_store_n(&s_armed, true, __ATOMIC_RELEASE);
    fill_pool();

    /* HCI Reset makes the controller process a command and log from its own task. */
    uint8_t reset[] = {0x01, 0x03, 0x0c, 0x00};
    assert(esp_vhci_host_check_send_available());
    esp_vhci_host_send_packet(reset, sizeof(reset));
    int64_t deadline = esp_timer_get_time() + 2000000;
    TaskHandle_t task = NULL;
    do {
        task = __atomic_load_n(&s_blocked_task, __ATOMIC_ACQUIRE);
        if (task && __atomic_load_n(&s_inside_log, __ATOMIC_ACQUIRE) && eTaskGetState(task) == eBlocked) {
            break;
        }
        vTaskDelay(1);
    } while (esp_timer_get_time() < deadline);
    assert(task && __atomic_load_n(&s_inside_log, __ATOMIC_ACQUIRE) && eTaskGetState(task) == eBlocked);
    vTaskDelay(pdMS_TO_TICKS(20));
    assert(__atomic_load_n(&s_inside_log, __ATOMIC_ACQUIRE) && eTaskGetState(task) == eBlocked);
    printf("CASE round=%u controller_core=%u blocked_in_log=1 original_flag=0x%lx\n",
           round, controller_core, (unsigned long)s_callback_flag);
#ifdef LOG_STOP_RECYCLE_CONTROL
    /* Complete the parked write normally before deletion; do not cancel it. */
    deadline = esp_timer_get_time() + 1000000;
    while (__atomic_load_n(&s_inside_log, __ATOMIC_ACQUIRE) && esp_timer_get_time() < deadline) {
        (void)ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL);
        vTaskDelay(1);
    }
    assert(!__atomic_load_n(&s_inside_log, __ATOMIC_ACQUIRE));
    puts("RECYCLED: parked log call returned before task deletion");
#endif
    int64_t start = esp_timer_get_time();
    ESP_ERROR_CHECK(esp_bt_controller_disable());
    int64_t elapsed = esp_timer_get_time() - start;
#ifndef LOG_STOP_NEGATIVE_CONTROL
    assert(!ble_log_lbm_is_enabled());
#endif
    printf("DISABLED round=%u disable_us=%lld\n", round, (long long)elapsed);
    __atomic_store_n(&s_armed, false, __ATOMIC_RELEASE);
    /* Transport completion remains separate from stopping the controller task. */
    assert(ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0);
    /* Drain queued transport data before deinit. */
    ble_log_prph_test_set_auto_recycle_hook(recycle, NULL);
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL)) {}
    puts("DEINIT: transport recycling restored");
    ESP_ERROR_CHECK(esp_bt_controller_deinit());
    assert(esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE);
    printf("PASS round=%u disable_us=%lld deinit=OK\n", round, (long long)elapsed);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    for (unsigned round = 0; round < 20; round++) {
        run_round(round);
    }
    puts("LIFECYCLE ALL PASS (20 rounds)");
}
