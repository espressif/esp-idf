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
#include "sdkconfig.h"
#if __XTENSA__
#include "xtensa/config/core.h"
#else
#error "Doesn't support other architectures"
#endif

static const char* TAG = "example";

typedef struct {
    uint32_t regs[11];
    uint32_t in[3];
    uint32_t out[4];
} arg_data_t;

void get_ps_other_cpu(void* arg);
void extended_ipc_isr_asm(void* arg);

void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    uint32_t ps_other_cpu = 0;
    ESP_LOGI(TAG, "call get_ps_other_cpu");
    esp_ipc_isr_call_blocking(get_ps_other_cpu, &ps_other_cpu);
    ESP_LOGI(TAG, "PS_INTLEVEL = 0x%"PRIx32, ps_other_cpu & XCHAL_PS_INTLEVEL_MASK);
    ESP_LOGI(TAG, "PS_EXCM = 0x%"PRIx32, (ps_other_cpu & XCHAL_PS_EXCM_MASK) >> XCHAL_PS_EXCM_SHIFT);
    ESP_LOGI(TAG, "PS_UM = 0x%"PRIx32, (ps_other_cpu & XCHAL_PS_UM_MASK) >> XCHAL_PS_UM_SHIFT);

    ESP_LOGI(TAG, "call extended_ipc_isr_asm");
    arg_data_t arg = { 0 };
    arg.in[0] = 0x01;
    arg.in[1] = 0x02;
    arg.in[2] = 0x03;
    ESP_LOGI(TAG, "in[0] = 0x%"PRIx32, arg.in[0]);
    ESP_LOGI(TAG, "in[1] = 0x%"PRIx32, arg.in[1]);
    ESP_LOGI(TAG, "in[2] = 0x%"PRIx32, arg.in[2]);
    esp_ipc_isr_call_blocking(extended_ipc_isr_asm, (void*)&arg);
    ESP_LOGI(TAG, "out[0] = (in[0] | in[1] | in[2]) = 0x%"PRIx32, arg.out[0]);
    assert(0x03 == arg.out[0]);
    ESP_LOGI(TAG, "out[1] = (in[0] + in[1] + in[2]) = 0x%"PRIx32, arg.out[1]);
    assert(0x06 == arg.out[1]);
    ESP_LOGI(TAG, "out[2] = in[2] = 0x%"PRIx32, arg.out[2]);
    assert(0x03 == arg.out[2]);
    ESP_LOGI(TAG, "out[3] = PS of other cpu = 0x%"PRIx32, arg.out[3]);
    assert(ps_other_cpu == arg.out[3]);
    ESP_LOGI(TAG, "End");
}
