/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_riscv_trace.h"

#define WORKLOAD_STACK_SIZE 2048
#define WORKLOAD_PRIORITY   1
#define WORKLOAD_ITERATIONS 1000
#define CMD_MAX_LEN         32
#define CMD_POLL_MS         10

static volatile uint32_t s_sink;

/* Branchy loop that fills and wraps the trace buffer within milliseconds. */
static void IRAM_ATTR branchy_work(void)
{
    uint32_t acc = s_sink;
    for (volatile int i = 0; i < WORKLOAD_ITERATIONS; i++) {
        if ((i & 3) == 0) {
            acc += (uint32_t)i;
        } else {
            acc ^= ((uint32_t)i << 1);
        }
    }
    s_sink = acc;
}

static void workload_task(void *arg)
{
    (void)arg;
    for (;;) {
        branchy_work();
    }
}

static bool core_is_traced(int core)
{
    return (CONFIG_ESP_RISCV_TRACE_CORE_MASK & BIT(core)) != 0;
}

/* Reads one command line from the console. Polls, so no UART driver is needed. */
static void read_command(char *buf, size_t size)
{
    size_t len = 0;
    for (;;) {
        int c = getchar();
        if (c == EOF) {
            /* The end-of-file flag is sticky, so later reads would not poll the console */
            clearerr(stdin);
            vTaskDelay(pdMS_TO_TICKS(CMD_POLL_MS));
        } else if (c == '\r' || c == '\n') {
            if (len > 0) {
                buf[len] = '\0';
                return;
            }
        } else if (len < size - 1) {
            buf[len++] = (char)c;
        }
    }
}

static void print_status(void)
{
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        if (!core_is_traced(core)) {
            continue;
        }
        esp_riscv_trace_status_t status = {0};
        esp_err_t err = esp_riscv_trace_get_status((esp_riscv_trace_core_t)core, &status);
        printf("status: core %d err %s work %d overflowed %d\n", core, esp_err_to_name(err),
               (int)status.work_status, (int)status.fifo_overflowed);
    }
}

void app_main(void)
{
    for (int core = 0; core < SOC_CPU_CORES_NUM && core < portNUM_PROCESSORS; core++) {
        if (core_is_traced(core)) {
            xTaskCreatePinnedToCore(workload_task, "workload", WORKLOAD_STACK_SIZE, NULL, WORKLOAD_PRIORITY, NULL,
                                    core);
        }
    }
    printf("riscv_trace_ocd: ready\n");

    char cmd[CMD_MAX_LEN];
    for (;;) {
        read_command(cmd, sizeof(cmd));
        if (strcmp(cmd, "panic") == 0) {
            abort();
        } else if (strcmp(cmd, "api_start") == 0) {
            esp_err_t err = esp_riscv_trace_start(ESP_RISCV_TRACE_CORE_0);
            printf("api_start: %s\n", esp_err_to_name(err));
        } else if (strcmp(cmd, "status") == 0) {
            print_status();
        } else {
            printf("unknown command: %s\n", cmd);
        }
    }
}
