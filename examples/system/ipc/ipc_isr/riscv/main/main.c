/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_ipc_isr.h"
#include "callbacks.h"
#include "sdkconfig.h"

static const char* TAG = "example";

void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    uint32_t mstatus_other_cpu = 0;
    ESP_LOGI(TAG, "call get_mstatus_other_cpu");
    esp_ipc_isr_call_blocking(get_mstatus_other_cpu, &mstatus_other_cpu);
    ESP_LOGI(TAG, "MSTATUS = 0x%"PRIx32, mstatus_other_cpu);

    ESP_LOGI(TAG, "call extended_ipc_isr_func");
    arg_data_t arg = { 0 };
    arg.in[0] = 0x01;
    arg.in[1] = 0x02;
    arg.in[2] = 0x03;
    ESP_LOGI(TAG, "in[0] = 0x%"PRIx32, arg.in[0]);
    ESP_LOGI(TAG, "in[1] = 0x%"PRIx32, arg.in[1]);
    ESP_LOGI(TAG, "in[2] = 0x%"PRIx32, arg.in[2]);
    esp_ipc_isr_call_blocking(extended_ipc_isr_func, (void*)&arg);
    ESP_LOGI(TAG, "out[0] = (in[0] | in[1] | in[2]) = 0x%"PRIx32, arg.out[0]);
    assert(0x03 == arg.out[0]);
    ESP_LOGI(TAG, "out[1] = (in[0] + in[1] + in[2]) = 0x%"PRIx32, arg.out[1]);
    assert(0x06 == arg.out[1]);
    ESP_LOGI(TAG, "out[2] = MCAUSE of other cpu = 0x%"PRIx32, arg.out[2]);
    assert(0xb800002c == arg.out[2]);
    ESP_LOGI(TAG, "out[3] = MSTATUS of other cpu = 0x%"PRIx32, arg.out[3]);
    assert(mstatus_other_cpu == arg.out[3]);
    ESP_LOGI(TAG, "End");
}
