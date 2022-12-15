/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>

#include "run_tc.h"
#include "test_env.h"
#include "wifi_unit.h"
#include "ble_unit.h"
#include "sync.h"
#include "wifi_connect.h"

#define TAG "CASE"

QueueHandle_t xTaskQueue = 0;

static const char *coex_get_case_env(coex_test_env_t *test_env, const char *keyword)
{
    const char *ret = NULL;
    if (!strcmp(keyword, "ap_ssid")) {
        ret = test_env->ap_ssid;
    } else if (!strcmp(keyword, "ap_password")) {
        ret = test_env->ap_password;
    } else if (!strcmp(keyword, "test_port")) {
        ret = test_env->test_port;
    } else if (!strcmp(keyword, "server_ip")) {
        ret = test_env->server_ip;
    } else if (!strcmp(keyword, "duration")) {
        ret = test_env->duration;
    }

    return ret;
}

static void wifi_tc_sta_throughput_timeout(void *arg)
{
    static uint32_t statistic_count  = 0;
    static uint64_t accumulate_speed = 0;

    uint32_t now = utils_get_system_ts();
    uint32_t *report = (uint32_t *) arg;
    uint32_t last_timestamp = report[0];

    if (now > last_timestamp) {
        uint32_t speed = report[1] * 8 / (now - last_timestamp);
        accumulate_speed += speed;
        statistic_count += 1;
        printf("speed: %" PRIu32 " kbps average speed: %lld kbps\n", speed, accumulate_speed / statistic_count );
        report[1] = 0;
        report[0] = now;
    }
}

static esp_err_t create_statistic_timer(esp_timer_handle_t *timer_hdl , uint32_t statistic_date[])
{
    esp_err_t ret;
    esp_timer_create_args_t tca = {
        .callback = (esp_timer_cb_t)wifi_tc_sta_throughput_timeout,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "TCP_STATISTIC",
    };
    tca.arg = statistic_date;
    ret = esp_timer_create(&tca, timer_hdl);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "statistic_timer create failed");
        return ret;
    }
    esp_timer_start_periodic(*timer_hdl, 1000000); //1000ms
    return ret;
}


void wifi_tcp_tx_throught_start(void *param)
{
    esp_timer_handle_t timer_hdl = NULL;
    esp_err_t ret;
    int sock = -1;
    uint32_t statistic_date[2] = {0};

    const char *ssid   = coex_get_case_env(param, "ap_ssid");
    const char *passwd = coex_get_case_env(param, "ap_password");


    wifi_util_init();
    // wifi_unit_connect_ap(ssid, passwd);
    example_connect(ssid, passwd);

#if defined(CONFIG_EXAMPLE_AUTO)
    if (((coex_test_env_t *)param)->run_mutex != NULL) {
        //This will be blocked by the sync timer.
        xSemaphoreTake(((coex_test_env_t *)param)->run_mutex, portMAX_DELAY);
        xSemaphoreGive(((coex_test_env_t *)param)->run_mutex);
    }
#endif
    const char *ip = coex_get_case_env(param, "server_ip");
    const char *port = coex_get_case_env(param, "test_port");
    const char *duration = coex_get_case_env(param, "duration");
    wifi_unit_client_establish(&sock, ip, port);

    ret = create_statistic_timer(&timer_hdl, statistic_date);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "statistic_timer create failed");
        goto _stop;
    }

    ret = wifi_util_tcp_send(sock, 1460, 0, &statistic_date[1], atoi(duration));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "send failed, %x", ret);
    }

_stop:
    if (timer_hdl) {
        esp_timer_stop(timer_hdl);
        esp_timer_delete(timer_hdl);
    }

    if (sock > 0) {
        close(sock);
    }
}

void wifi_tcp_tx_throught_end(void)
{
    esp_wifi_disconnect();
}

void wifi_tcp_rx_throught_start(void *param)
{
    esp_timer_handle_t timer_hdl = NULL;
    esp_err_t ret;
    int sock = -1;
    uint32_t statistic_date[2] = {0};

    const char *ssid   = coex_get_case_env(param, "ap_ssid");
    const char *passwd = coex_get_case_env(param, "ap_password");
    const char *port = coex_get_case_env(param, "test_port");
    const char *duration = coex_get_case_env(param, "duration");

    wifi_util_init();
    // wifi_unit_connect_ap(ssid, passwd);
    example_connect(ssid, passwd);
    wifi_unit_server_establish( &sock, port);

    ret = create_statistic_timer(&timer_hdl, statistic_date);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "statistic_timer create failed");
        goto _stop;
    }
    ret = wifi_unit_tcp_recv(sock, duration, statistic_date);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "tcp receive failed");
        goto _stop;
    }
_stop:
    if (timer_hdl) {
        esp_timer_stop(timer_hdl);
        esp_timer_delete(timer_hdl);
    }

    if (sock > 0) {
        close(sock);
    }
}

void wifi_tcp_rx_throught_end(void)
{
    esp_wifi_disconnect();
}

void ble_adv_start(void *param)
{
    esp_err_t ret;
    const char *duration = coex_get_case_env(param, "duration");
#if defined(CONFIG_EXAMPLE_MANAUL)
    bt_test_init();
#endif
    ret = ble_gap_util_set_default_adv_data();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to set adv data");
        return;
    }

    ret = ble_gap_util_start_adv(&default_adv_param);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to start adv");
        return;
    }

    vTaskDelay(atoi(duration) / portTICK_PERIOD_MS);

}

void ble_adv_end(void)
{
    ble_gap_util_stop();
}


void ble_scan_start(void *param)
{
    esp_err_t ret;
#if defined(CONFIG_EXAMPLE_MANAUL)
    bt_test_init();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    init_ble_gap_test_util();
#endif


    ret = ble_gap_util_set_scan_param(&default_scan_param);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "set scan param fail");
        return;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);

    scan_count.adv_count = 0;
    scan_count.scan_res_count = 0;

    ret = esp_ble_gap_start_scanning(BLE_TC_SCAN_REPORT_PERIOD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_scanning error, %d", ret);
        return;
    }
}

void ble_scan_end(void)
{
    ESP_LOGI(TAG, "%s \n", __func__);
}

tc_t tc_case[] = {
    DECLARE_TC(TC_WIFI_COEX_TCP_TX_THROUGHPUT, 0, wifi_tcp_tx_throught_start, wifi_tcp_tx_throught_end, (void *)&test_env),
    DECLARE_TC(TC_WIFI_COEX_TCP_RX_THROUGHPUT, 1, wifi_tcp_rx_throught_start, wifi_tcp_rx_throught_end, (void *)&test_env),
    DECLARE_TC(TC_BLE_COEX_ADV,                2, ble_adv_start,              ble_adv_end,              (void *)&test_env),
    DECLARE_TC(TC_BLE_COEX_SCAN,               3, ble_scan_start,             ble_scan_end,             (void *)&test_env),
};

static void excute_case(void *arg)
{
    tc_t *run_case = (tc_t *) arg;
    if (run_case && run_case->func_start != NULL) {
        run_case->func_start(run_case->param_list);
    }

    if (run_case && run_case->func_stop != NULL ) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        run_case->func_stop();
    }
    vTaskDelete(NULL);
}

static void run_task(void *arg)
{
    tc_t *tc_case_table = (tc_t *) arg;
    run_task_msg_t msg;

    for (;;) {
        if (pdTRUE == xQueueReceive(xTaskQueue, &msg, (TickType_t)portMAX_DELAY)) {
            if ( msg.case_id < sizeof(tc_case) / sizeof(tc_case[0]) ) {
                xTaskCreatePinnedToCore(excute_case, tc_case_table->name, 4096, &tc_case_table[msg.case_id], RUN_TASK_PRIORITY, NULL, 0);
            } else {
                ESP_LOGW(TAG, "msg.case_id  %d\n", msg.case_id);
            }

        }
    }
    vTaskDelete(NULL);
}


void run_tc_init(void)
{
    xTaskQueue = xQueueCreate(RUN_TASK_QUEUE_LEN, sizeof(run_task_msg_t));
    if (!xTaskQueue) {
        ESP_LOGE(TAG, "xTaskQueue create failed");
        return;
    }
    xTaskCreatePinnedToCore(run_task, "run_task", 4096, tc_case, RUN_TASK_PRIORITY, NULL, 0);
}
