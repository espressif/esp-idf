/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "driver/temperature_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const static char* TAG = "TSENS";
static temperature_sensor_handle_t s_temp_sensor = NULL;

static int tsens_init(int argc, char **argv)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &s_temp_sensor));
    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(s_temp_sensor));
    return 0;
}

static void register_tsens_init(void)
{
    const esp_console_cmd_t cmd = {
        .command = "tsens_init",
        .help = "Initialize temperature sensor",
        .hint = NULL,
        .func = &tsens_init,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

typedef struct {
    struct arg_int *count;
    struct arg_int *interval_ms;
    struct arg_end *end;
} tsens_read_args_t;
static tsens_read_args_t tsens_args;

typedef struct {
    int count;
    int interval_ms;
} tsens_read_param_t;

static void tsens_read_task(void *arg)
{
    tsens_read_param_t *param = (tsens_read_param_t*) arg;
    int count = param->count;
    int interval_ms = param->interval_ms;
    free(arg);
    float tsens_value;
    while (count-- > 0) {
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(s_temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
    }
    vTaskDelete(NULL);
}

static int do_tsens_read(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &tsens_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tsens_args.end, argv[0]);
        return 1;
    }

    tsens_read_param_t* params = (tsens_read_param_t*)malloc(sizeof(tsens_read_param_t));
    params->count = 1;
    params->interval_ms = 100;

    if (tsens_args.count->count > 0) {
        params->count = tsens_args.count->ival[0];
    }
    if (tsens_args.interval_ms->count > 0) {
        params->interval_ms = tsens_args.interval_ms->ival[0];
    }
    xTaskCreate(tsens_read_task, "tsens_read_task", 4096, (void*)params, 1, NULL);

    return 0;
}

static void register_tsens_read(void)
{
    tsens_args.count = arg_int0("c", "count", "<count>", "read count");
    tsens_args.interval_ms = arg_int0("i", "interval", "<interval>", "read interval in ms");
    tsens_args.end = arg_end(2);
    const esp_console_cmd_t cmd = {
        .command = "tsens_read",
        .help = "Temperature sensor read",
        .hint = NULL,
        .func = &do_tsens_read,
        .argtable = &tsens_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void register_tsens_cmd(void)
{
    register_tsens_init();
    register_tsens_read();
}
