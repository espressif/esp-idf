/* test longjmp

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_task.h>

#include <setjmp.h>

#define LUAI_NOIPA __attribute__((__noipa__))
#define LUAI_THROW(c)		longjmp((c)->b, 1)
#define LUAI_TRY(c,a)		if (setjmp((c)->b) == 0) { a }

#define TIMEOUT 50

#define RECURSION 19

static esp_timer_handle_t crash_timer;

static uint32_t result = 0;

uint32_t calc_fac(uint32_t n) {
    if (n == 1 || n == 0) {
        return 1;
    } else {
        return n * calc_fac(n - 1);
    }
}

static void timer_cb(void *arg) {
    result = calc_fac(RECURSION);
}

typedef struct {
    jmp_buf b;
} jmp_ctx;

LUAI_NOIPA
static void pret(jmp_ctx *jc) {
    LUAI_THROW(jc);
}

LUAI_NOIPA
static void precurse(jmp_ctx *jc, int n) {
    if (n) precurse(jc, n - 1);
    else pret(jc);
}

LUAI_NOIPA
static void ptest(jmp_ctx *jc) {
    precurse(jc, 64);
}

LUAI_NOIPA
void pcall(void (*func)(jmp_ctx *ctx)) {
    jmp_ctx jc;
    LUAI_TRY(&jc,
        ptest(&jc);
    );
}

static void sjlj_task(void *ctx) {
    uint32_t start = xTaskGetTickCount();
    for (;;) {
        pcall(ptest);
        uint32_t end = xTaskGetTickCount();

        uint32_t dt = end - start;
        if (dt >= 1000) {
            start = end;

            printf("[%u] sjlj tick %d\n", end, (int)ctx);
        }

        if (end > 9800) {
            break;
        }
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    const esp_timer_create_args_t timer_args = {
        timer_cb,
        NULL,
        ESP_TIMER_TASK,
        "crash_timer",
    };

    esp_timer_create(&timer_args, &crash_timer);
    esp_timer_start_periodic(crash_timer, TIMEOUT);

    printf("Hello world!\n");
    printf("Free heap: %d\n", esp_get_free_heap_size());

    for (size_t i = 0; i < 16; i++) {
        xTaskCreate(sjlj_task, "sjlj_task", 4096, (void *) i, tskIDLE_PRIORITY + 0, NULL);
    }

    vTaskDelay(10000);
    printf("stopping timers...\n");
    esp_timer_stop(crash_timer);
    esp_timer_delete(crash_timer);

    printf("Test successful\n");
}
