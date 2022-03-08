/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sync.h"
#include "run_tc.h"
#include "wifi_unit.h"
#define TAG "SYNC"

SemaphoreHandle_t client_mutex;

struct sync_t sync_obj = {
    .except_recv_wifi_id = false,
    .except_recv_bt_id   = false,
    .recv_param_bit = 0x0,
    .start_time     = 0x0,

#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    .state = ASSIGN_CASE,
#else
    .state = WAIT_CASE,
#endif
#if defined(CONFIG_EXAMPLE_COEX_TX_ADV)
    .own_wifi_case = WIFI_TCP_TX_CASE,
    .own_ble_case  = BLE_ADV_CASE,
#elif defined(CONFIG_EXAMPLE_COEX_RX_ADV)
    .own_wifi_case = WIFI_TCP_RX_CASE,
    .own_ble_case  = BLE_ADV_CASE,
#elif defined(CONFIG_EXAMPLE_COEX_TX_SCAN)
    .own_wifi_case = WIFI_TCP_TX_CASE,
    .own_ble_case  = BLE_SCAN_CASE,
#elif defined(CONFIG_EXAMPLE_COEX_RX_SCAN)
    .own_wifi_case = WIFI_TCP_RX_CASE,
    .own_ble_case  = BLE_SCAN_CASE,
#else
    .own_wifi_case = NOT_CASE,
    .own_ble_case  = NOT_CASE,
#endif
};


auto_tc auto_tb[6] = {
    {WIFI_TCP_TX_CASE,  PARAMTER(0b011011)},     //need paramter: wifi_case_id ssid password server ip
    {WIFI_TCP_RX_CASE,  PARAMTER(0b010011)},     //need paramter: wifi_case_id ssid password
    {BLE_ADV_CASE,      PARAMTER(0b100000)},     //need paramter: ble_case_id
    {BLE_SCAN_CASE,     PARAMTER(0b100000)},     //need paramter: ble_case_id
};


esp_err_t send_adv(uint8_t *raw_data, uint32_t raw_data_len)
{
    esp_err_t ret;

    esp_ble_adv_params_t adv_param = {
        .adv_int_min        = 0x40,
        .adv_int_max        = 0x40,
        .adv_type           = ADV_TYPE_NONCONN_IND,
        .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
        .channel_map        = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    ret = esp_ble_gap_config_adv_data_raw(raw_data, raw_data_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_config_adv_data_raw error, %d", ret);
        return ret;
    }

    ret = esp_ble_gap_start_advertising(&adv_param);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_advertising error, %d", ret);
        return ret;
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);  //every 30ms send one packet

    ret = esp_ble_gap_stop_advertising();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_stop_advertising error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t send_adv_data(sync_msg *msg)
{
    send_adv((uint8_t *)msg, sizeof(sync_msg));
    return 0;
}

void send_start_msg(uint8_t start_time)
{
    sync_msg msg = {
        .length = 30,
        .type   = MSG_TYPE,
        .head   = MSG_HEAD,
        .msg_id = 0x4,
        .ctl  = MSG_CONTINUE,
        .param_bit = 0b000100 << PARAM_MAX,
        .reserve   = 0xff,
        .data = {start_time},
    };
    send_adv_data(&msg);
}
void send_start_countdown(void)
{
    if (sync_obj.except_recv_bt_id == true && sync_obj.except_recv_wifi_id == true) {
        esp_timer_stop(sync_obj.sync_timer);
        esp_timer_start_once(sync_obj.sync_timer,  1000000);
        sync_obj.state = START_CASE;
    }
}
void send_tcp_rx_inited_msg(void)
{
    uint8_t hex_ip[4];
    uint32_t ip = wifi_util_get_ip();
    memcpy(hex_ip, &ip, sizeof(hex_ip));
    if (hex_ip[0] == 0x0) {
        return;
    }
    sync_msg msg = {
        .length = 30,
        .type   = MSG_TYPE,
        .head   = MSG_HEAD,
        .msg_id = MSG_ID_WIFI_DEV_INIT_FINISH,
        .ctl    = MSG_CONTINUE,
        .param_bit = 0b001000 << PARAM_MAX,
        .reserve   = 0xff,
        .data = {hex_ip[0], hex_ip[1], hex_ip[2], hex_ip[3]},
    };
    send_adv_data(&msg);
}
void send_case_inited_msg(uint8_t msg_id)
{
    sync_msg msg = {
        .length = 30,
        .type   = MSG_TYPE,
        .head = MSG_HEAD,
        .msg_id = msg_id,
        .ctl  = MSG_CONTINUE,
        .param_bit = 0b000000 << PARAM_MAX,
        .reserve   = 0xff,
        .data = {0},
    };
    send_adv_data(&msg);
}

void assign_case_to_dev(uint8_t ble_id, uint8_t wifi_id)
{
    if ( sync_obj.own_wifi_case == WIFI_TCP_RX_CASE ) {
        uint8_t hex_ip[4];
        uint32_t ip = wifi_util_get_ip();
        ESP_LOGI(TAG, "ip:%s", inet_ntoa(ip));
        memcpy(hex_ip, &ip, sizeof(hex_ip));
        if (hex_ip[0] == 0x0) {
            return;
        }
        sync_msg msg = {
            .length = 30,
            .type   = MSG_TYPE,
            .head = MSG_HEAD,
            .msg_id = 0x1,
            .ctl  = MSG_CONTINUE,
            .param_bit = 0b111000 << PARAM_MAX,
            .reserve   = 0xff,
            .data = {ble_id, wifi_id, hex_ip[0], hex_ip[1], hex_ip[2], hex_ip[3]},
        };
        send_adv_data(&msg);
    } else if ( sync_obj.own_wifi_case == WIFI_TCP_TX_CASE ) {
        sync_msg msg1 = {
            .length = 30,
            .type   = MSG_TYPE,
            .head = MSG_HEAD,
            .msg_id = 0x1,
            .ctl  = MSG_CONTINUE,
            .param_bit = 0b110000 << PARAM_MAX,
            .reserve   = 0xff,
            .data = {ble_id, wifi_id},
        };
        send_adv_data(&msg1);
    }
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    if (strlen(CONFIG_EXAMPLE_WIFI_SSID) < 20) {
        sync_msg msg = {
            .length = 30,
            .type   = MSG_TYPE,
            .head = MSG_HEAD,
            .msg_id = 0x1,
            .ctl  = MSG_CONTINUE,
            .param_bit = 0b000010 << PARAM_MAX,
            .reserve   = 0xff,
            .data[0] = strlen(CONFIG_EXAMPLE_WIFI_SSID),
        };
        for (uint8_t i = 0 ; i < strlen(CONFIG_EXAMPLE_WIFI_SSID); i++) {
            msg.data[i + 1] = CONFIG_EXAMPLE_WIFI_SSID[i] - '0';
        }
        send_adv_data(&msg);
    }

    if (strlen(CONFIG_EXAMPLE_WIFI_PASSWORD) < 20) {
        sync_msg msg = {
            .length = 30,
            .type   = MSG_TYPE,
            .head = MSG_HEAD,
            .msg_id = 0x1,
            .ctl  = MSG_END,
            .param_bit = 0b000001 << PARAM_MAX,
            .reserve   = 0xff,
            .data[0] = strlen(CONFIG_EXAMPLE_WIFI_PASSWORD),
        };
        for (int i = 0 ; i < strlen(CONFIG_EXAMPLE_WIFI_PASSWORD); i++) {
            msg.data[i + 1] = CONFIG_EXAMPLE_WIFI_PASSWORD[i] - '0';
        }
        send_adv_data(&msg);
    }
#endif
}


void excute_case(uint8_t run_case)
{
    run_task_msg_t msg;
    msg.case_id = run_case;
    if (xQueueSend(xTaskQueue, &msg, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "xTaskQueue Post failed\n");
    }
}


void assign_test_case(void)
{
#if defined(CONFIG_EXAMPLE_COEX_TX_ADV)
    assign_case_to_dev(BLE_SCAN_CASE, WIFI_TCP_RX_CASE);
#elif defined(CONFIG_EXAMPLE_COEX_RX_ADV)
    assign_case_to_dev(BLE_SCAN_CASE, WIFI_TCP_TX_CASE);
#elif defined(CONFIG_EXAMPLE_COEX_TX_SCAN)
    assign_case_to_dev(BLE_ADV_CASE, WIFI_TCP_RX_CASE);
#elif defined(CONFIG_EXAMPLE_COEX_RX_SCAN)
    assign_case_to_dev(BLE_ADV_CASE, WIFI_TCP_TX_CASE);
#endif
}


void analys_param(uint16_t param_bit, uint8_t data[], uint16_t *recv_param_bit)
{

    uint8_t data_ptr = 0;

    for (int i = 0 ; i < PARAM_MAX; i++ ) {
        switch (GET_PARAM(param_bit, 0x1000 >> i)) {
        case BLE_CASE_ID:
            ESP_LOGD(TAG, "BLE_CASE_ID\n");
#if defined(CONFIG_EXAMPLE_BT_ROLE)
            sync_obj.own_ble_case = data[data_ptr];
            (*recv_param_bit) |= BLE_CASE_ID;
#endif
            data_ptr += 1;
            break;
        case WIFI_CASE_ID:
            ESP_LOGD(TAG, "WIFI_CASE_ID\n");
#if defined(CONFIG_EXAMPLE_WIFI_ROLE)
            sync_obj.own_wifi_case = data[data_ptr];
            (*recv_param_bit) |= WIFI_CASE_ID;
#endif
            data_ptr += 1;
            break;
        case START_TIME:
            ESP_LOGD(TAG, "START_TIME\n");
            sync_obj.start_time   = data[data_ptr];
            data_ptr += 1;
            break;
        case SERVER_IP: {
            ESP_LOGD(TAG, "SERVER_IP\n");
            char server_ip[16];
            memset(server_ip, '0', sizeof(server_ip));
            sprintf(server_ip, "%d.%d.%d.%d", data[data_ptr], data[data_ptr + 1], data[data_ptr + 2], data[data_ptr + 3]); // size conversion
            coex_set_test_env("server_ip", server_ip, sizeof(server_ip));
            coex_print_test_env();
            data_ptr += 4;
            (*recv_param_bit) |= SERVER_IP;
            break;
        }
        case WIFI_SSID: {
            ESP_LOGD(TAG, "WIFI_SSID\n");
            uint8_t length = data[data_ptr];
            data_ptr += 1;
            if ( length > 20) {
                ESP_LOGE(TAG, "ssid length error");
                break;
            }
            char *ssid = malloc(length + 1);
            if (ssid == NULL) {
                ESP_LOGE(TAG, "%s malloc fail\n", __func__);
                return ;
            }
            memset(ssid, '0', sizeof(length + 1));

            for (int i = length - 1 ; i >= 0 ; i--) {
                ssid[i] = data[data_ptr + i] + '0';
            }
            ssid[length] = '\n';
            coex_set_test_env("ap_ssid", ssid, length);
            coex_print_test_env();
            (*recv_param_bit) |= WIFI_SSID;
            free(ssid);
            break;
        }
        case WIFI_PASSWIRD: {
            ESP_LOGD(TAG, "WIFI_PASSWIRD\n");
            uint8_t length = data[data_ptr];
            data_ptr += 1;
            if ( length > 20) {
                ESP_LOGE(TAG, "password length error");
                break;
            }
            char *password = malloc(length + 1);
            if (password == NULL) {
                ESP_LOGE(TAG, "%s malloc fail\n", __func__);
                return ;
            }
            memset(password, '0', sizeof(length + 1));
            for (int i = length - 1 ; i >= 0 ; i--) {
                password[i] = data[data_ptr + i] + '0';
            }
            password[length] = '\n';
            coex_set_test_env("ap_password", password, length);
            coex_print_test_env();
            (*recv_param_bit) |= WIFI_PASSWIRD;
            free(password);
            break;
        }
        default:
            break;
        }
    }

}

void sync_cmd_recv(uint8_t *raw_data, uint32_t raw_data_len)
{
    if (raw_data_len < MSG_MIN_LEN) {
        ESP_LOGD(TAG, "msg length is low");
    }
    sync_msg_head msg_head = {0};
    memcpy(&msg_head, raw_data, sizeof(sync_msg_head));

    if (msg_head.type != MSG_TYPE || msg_head.head != MSG_HEAD) {
        ESP_LOGD(TAG, "msg is unknown");
        return;
    }
    ESP_LOGD(TAG, "msg_id: %x\n", msg_head.msg_id);
    // ESP_LOG_BUFFER_HEX("sync recv:", raw_data, raw_data_len);
    switch (sync_obj.state) {
#if defined(CONFIG_EXAMPLE_WIFI_ROLE) || defined(CONFIG_EXAMPLE_BT_ROLE)
    case WAIT_CASE: {
        ESP_LOGD(TAG, "WAIT_CASE\n");
        if (msg_head.msg_id == MSG_ID_ASSIGN_CASE) {
            analys_param(msg_head.param_bit, raw_data + MSG_DATA_BASE, &sync_obj.recv_param_bit );
#if defined(CONFIG_EXAMPLE_WIFI_ROLE)
            if (sync_obj.own_wifi_case != NOT_CASE) {
                if ((sync_obj.recv_param_bit & auto_tb[sync_obj.own_wifi_case].excpet_param_bit) == auto_tb[sync_obj.own_wifi_case].excpet_param_bit) {
                    excute_case(sync_obj.own_wifi_case);
                    sync_obj.state = WAIT_START;
                    esp_timer_start_periodic(sync_obj.sync_timer, SYNC_TIMEOUT );
                }
            }
#endif
#if defined(CONFIG_EXAMPLE_BT_ROLE)
            if (sync_obj.own_ble_case != NOT_CASE) {
                if ((sync_obj.recv_param_bit & auto_tb[sync_obj.own_ble_case].excpet_param_bit) == auto_tb[sync_obj.own_ble_case].excpet_param_bit) {
                    sync_obj.state = WAIT_START;
                    esp_timer_start_periodic(sync_obj.sync_timer, SYNC_TIMEOUT );
                }
            }
#endif
        }
        break;
    }
    case WAIT_START:
        ESP_LOGD(TAG, "WAIT_START\n");
        ESP_LOGD(TAG, "WAIT_START %x\n", msg_head.msg_id);
        if (msg_head.msg_id == MSG_ID_START_CASE) {
            analys_param(msg_head.param_bit, raw_data + MSG_DATA_BASE, &sync_obj.recv_param_bit );
            sync_obj.state = START_CASE;
            ble_gap_util_stop();
#if defined(CONFIG_EXAMPLE_WIFI_ROLE)
            bt_test_deinit();
#endif
            esp_timer_stop(sync_obj.sync_timer);
            esp_timer_start_once(sync_obj.sync_timer, sync_obj.start_time * 1000000);
        }
        break;
#endif
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    case ASSIGN_CASE:
        ESP_LOGD(TAG, "ASSIGN_CASE\n");
        switch (msg_head.msg_id) {
        case MSG_ID_WIFI_DEV_INIT_FINISH:
            if (msg_head.param_bit != 0x0) {
                analys_param(msg_head.param_bit, raw_data + MSG_DATA_BASE, &sync_obj.recv_param_bit );
            }
            sync_obj.except_recv_wifi_id = true;
            break;
        case MSG_ID_BT_DEV_INIT_FINISH:
            sync_obj.except_recv_bt_id = true;
            break;
        default:
            break;
        }
        send_start_countdown();
        break;
#endif
    default:
        ESP_LOGD(TAG, "state is unknown %s", __func__);
        break;
    }
}

static void handle_sync_timeout(void *arg)
{
    static bool run_first = true;
    if (run_first == true) {
        xSemaphoreTake((SemaphoreHandle_t)arg, (TickType_t)portMAX_DELAY);
        esp_timer_start_periodic( (esp_timer_handle_t)arg, 1000000);
        run_first = false;
    }
    switch (sync_obj.state) {
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    case ASSIGN_CASE:
        ESP_LOGD(TAG, "ASSIGN_CASE\n");
        assign_test_case();
        esp_timer_start_periodic(sync_obj.sync_timer, SYNC_TIMEOUT);
        break;
#endif

#if defined(CONFIG_EXAMPLE_WIFI_ROLE) || defined(CONFIG_EXAMPLE_BT_ROLE)
    case WAIT_START: {
        ESP_LOGD(TAG, "WAIT_START\n");
#if defined(CONFIG_EXAMPLE_WIFI_ROLE)
        if ( WIFI_TCP_RX_CASE == sync_obj.own_wifi_case ) {
            send_tcp_rx_inited_msg();
        } else {
            send_case_inited_msg(MSG_ID_WIFI_DEV_INIT_FINISH);
        }
#elif defined(CONFIG_EXAMPLE_BT_ROLE)
        send_case_inited_msg(MSG_ID_BT_DEV_INIT_FINISH);
#endif
        esp_timer_start_periodic(sync_obj.sync_timer, SYNC_TIMEOUT);
        break;
    }
#endif


    case START_CASE: {
        ESP_LOGD(TAG, "START_CASE\n");
#if defined(CONFIG_EXAMPLE_BT_ROLE)
        excute_case(sync_obj.own_ble_case);

#elif defined(CONFIG_EXAMPLE_WIFI_ROLE)
        ESP_LOGD(TAG, "START_CASE\n");
        if (arg != NULL) {
            xSemaphoreGive((SemaphoreHandle_t)arg);
            run_first = true;
            esp_timer_stop(sync_obj.sync_timer);

        }
#else
        static uint8_t send_start_count = 10;
        if (send_start_count == 0) {
            excute_case(sync_obj.own_ble_case);
            xSemaphoreGive((SemaphoreHandle_t)arg);
            run_first = true;
            esp_timer_stop(sync_obj.sync_timer);
            break;
        }

        send_start_msg(send_start_count);
        send_start_count -= 1;
        esp_timer_start_once(sync_obj.sync_timer,  1000000);
#endif
        break;
    }
    default:
        ESP_LOGD(TAG, "state is unknown%s", __func__);
        break;
    }
}

esp_err_t create_sync_timer(esp_timer_handle_t *timer_hdl)
{
    esp_err_t ret;
    esp_timer_create_args_t tca = {
        .callback = (esp_timer_cb_t)handle_sync_timeout,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "SYNC_TIMER",
    };
    tca.arg = client_mutex;

    ret = esp_timer_create(&tca, timer_hdl);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "timer create failed %d %x\n", __LINE__, ret);
        return ret;
    }
    esp_timer_start_once( *timer_hdl, 10);

    return ret;
}


void sync_init(void)
{
    esp_err_t ret;
    sync_obj.cmd_recv = &sync_cmd_recv;
    client_mutex = xSemaphoreCreateMutex();
    if (!client_mutex) {
        ESP_LOGE(TAG, "client_mutex Create failed ");
        return;
    }
    coex_set_test_env("mutex", NULL, 0);

    bt_test_init();
    init_ble_gap_test_util();
    ret = esp_ble_gap_start_scanning(BLE_TC_SCAN_REPORT_PERIOD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_scanning error, %d", ret);
        return ;
    }
    ret = create_sync_timer(&sync_obj.sync_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "sync timer create failed");
        return ;
    }

#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    excute_case(sync_obj.own_wifi_case);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
#endif

}
