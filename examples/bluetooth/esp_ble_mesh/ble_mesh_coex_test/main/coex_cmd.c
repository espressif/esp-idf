/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "run_tc.h"
#include "test_env.h"


#define TAG_CNSL "CNSL"

typedef struct {
    struct arg_str *wifi_tc_idx;
    struct arg_str *bt_tc_idx;
    struct arg_end *end;
} tc_run_args_t;

typedef struct {
    struct arg_lit *set;
    struct arg_lit *get;
    struct arg_str *key;
    struct arg_str *value;
    struct arg_end *end;
} env_param_cmd_args_t;

static tc_run_args_t tc_run_args;
static env_param_cmd_args_t env_param_cmd_args;

static int process_env_parameter_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &env_param_cmd_args);
    int ret;
    const char *env_value = "";

    if (nerrors != 0) {
        arg_print_errors(stderr, env_param_cmd_args.end, argv[0]);
        return 1;
    }
    if (env_param_cmd_args.set->count == 1) {
        if (env_param_cmd_args.key->count == 1) {
            if (env_param_cmd_args.value->count == 1) {
                env_value = env_param_cmd_args.value->sval[0];
            }
            ret = coex_set_test_env(env_param_cmd_args.key->sval[0], env_value, strlen(env_param_cmd_args.value->sval[0]));
            if (ret == ESP_ERR_NOT_SUPPORTED) {
                ESP_LOGE(TAG_CNSL, "Not supported env key");
            } else if (ret == ESP_ERR_INVALID_ARG) {
                ESP_LOGE(TAG_CNSL, "Invalid value");
            }
            ESP_LOGI(TAG_CNSL, "env set done");
        } else {
            ESP_LOGE(TAG_CNSL, "env key not set correctly");
        }
    } else if (env_param_cmd_args.get->count == 1) {
        coex_print_test_env();
    }
    return 0;
}

static int process_restart_cmd(int argc, char **argv)
{
    ESP_LOGI(TAG_CNSL, "restarting...");
    esp_restart();
    return 0;
}


static int process_run_tc_cmd(int argc, char **argv)
{
    run_task_msg_t msg;
    int nerrors = arg_parse(argc, argv, (void **) &tc_run_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tc_run_args.end, argv[0]);
        return 1;
    }

    if (tc_run_args.wifi_tc_idx->count == 1) {
        msg.case_id = atoi(tc_run_args.wifi_tc_idx->sval[0]);
        if (xQueueSend(xTaskQueue, &msg, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(TAG_CNSL, "xTaskQueue Post failed\n");
        }
    }

    if (tc_run_args.bt_tc_idx->count == 1) {
        msg.case_id = atoi(tc_run_args.bt_tc_idx->sval[0]);
        if (xQueueSend(xTaskQueue, &msg, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(TAG_CNSL, "xTaskQueue Post failed\n");
        }
    }

    return 0;
}

static int process_get_mac_addr_cmd(int argc, char **argv)
{
    const uint8_t *mac = esp_bt_dev_get_address();

    if (mac != NULL) {
        ESP_LOGI(TAG_CNSL, "+BTMAC:"MACSTR"\n", MAC2STR(mac));
    }
    return 0;
}
void register_coex_cmd(void)
{
    const esp_console_cmd_t restart_cmd = {
        .command = "restart",
        .help = "restart cmd",
        .hint = NULL,
        .func = &process_restart_cmd,
        .argtable = NULL
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&restart_cmd) );

    const esp_console_cmd_t get_mac_cmd = {
        .command = "mac",
        .help = "Get DUT mac address",
        .hint = NULL,
        .func = &process_get_mac_addr_cmd,
        .argtable = NULL
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&get_mac_cmd) );

    tc_run_args.wifi_tc_idx = arg_str0("w", "wifi", "<str>", "0 : wifi_tcp_tx_throught 1 : wifi_tcp_rx_throught\n");
    tc_run_args.bt_tc_idx   = arg_str0("b", "bluetooth", "<str>", "2  :ble_adv  3 : ble_scan\n");
    tc_run_args.end = arg_end(2);

    const esp_console_cmd_t run_tc_cmd = {
        .command = "run_tc",
        .help = "run wifi bt test case command",
        .hint = NULL,
        .func = &process_run_tc_cmd,
        .argtable = &tc_run_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&run_tc_cmd) );

    env_param_cmd_args.set = arg_lit0("s", "set", "set env parameter");
    env_param_cmd_args.get = arg_lit0("g", "get", "get env parameter");
    env_param_cmd_args.key = arg_str0("k", "key", "<str>", "env parameter key");
    env_param_cmd_args.value = arg_str0("v", "value", "<str>", "env parameter value (only used with set)");
    env_param_cmd_args.end = arg_end(4);

    const esp_console_cmd_t env_cmd = {
        .command = "env",
        .help = "Set or get test environment parameters",
        .hint = NULL,
        .func = &process_env_parameter_cmd,
        .argtable = &env_param_cmd_args,
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&env_cmd) );
}
