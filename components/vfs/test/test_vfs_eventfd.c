// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include "esp_vfs_eventfd.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>

#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "hal/timer_types.h"
#include "unity.h"

#define TIMER_DIVIDER         16
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)
#define TIMER_INTERVAL0_SEC   (0.25)
#define TEST_WITHOUT_RELOAD   0
#define PROGRESS_INTERVAL_MS  350
#define TIMER_SIGNAL          1
#define PROGRESS_SIGNAL       2

int s_timer_fd;
int s_progress_fd;

/*
 * A simple helper function to print the raw timer counter value
 * and the counter value converted to seconds
 */
static void inline print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\n", (uint32_t) (counter_value >> 32),
           (uint32_t) (counter_value));
    printf("Time   : %.8f s\n", (double) counter_value / TIMER_SCALE);
}

void IRAM_ATTR timer_group0_isr(void *para)
{
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

    if (timer_intr & TIMER_INTR_T0) {
        timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
        timer_counter_value += (uint64_t) (TIMER_INTERVAL0_SEC * TIMER_SCALE);
        timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, timer_idx, timer_counter_value);
    }

    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    uint64_t signal = TIMER_SIGNAL;
    ssize_t val = write(s_timer_fd, &signal, sizeof(signal));
    assert(val == sizeof(signal));
    timer_spinlock_give(TIMER_GROUP_0);
}

static void eventfd_timer_init(int timer_idx, double timer_interval_sec)
{
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = false,
    };
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}

static void eventfd_timer_deinit(int timer_idx)
{
    timer_pause(TIMER_GROUP_0, timer_idx);
    timer_deinit(TIMER_GROUP_0, timer_idx);
}

static void worker_task(void *arg)
{
    for (int i = 0; i < 3; i++) {
        vTaskDelay(pdMS_TO_TICKS(PROGRESS_INTERVAL_MS));
        uint64_t signal = PROGRESS_SIGNAL;
        ssize_t val = write(s_progress_fd, &signal, sizeof(signal));
        assert(val == sizeof(signal));
    }
    vTaskDelete(NULL);
}

TEST_CASE("Test eventfd triggered correctly", "[vfs][eventfd]")
{
    xTaskCreate(worker_task, "worker_task", 1024, NULL, 5, NULL);
    TEST_ESP_OK(esp_vfs_eventfd_register());
    s_timer_fd = eventfd(0, EFD_SUPPORT_ISR);
    s_progress_fd = eventfd(0, 0);
    int maxFd = s_progress_fd > s_timer_fd ? s_progress_fd : s_timer_fd;
    printf("Timer fd %d progress fd %d\n", s_timer_fd, s_progress_fd);
    eventfd_timer_init(TIMER_0, TIMER_INTERVAL0_SEC);

    int selectTimeoutCount = 0;
    int timerTriggerCount = 0;
    int progressTriggerCount = 0;

    for (size_t i = 0; i < 10; i++) {
        struct timeval timeout;
        uint64_t signal;

        timeout.tv_sec = 0;
        timeout.tv_usec = 200 * 1000;

        fd_set readfds;
        fd_set writefds;
        fd_set errorfds;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&errorfds);

        FD_SET(s_timer_fd, &readfds);
        FD_SET(s_progress_fd, &readfds);

        select(maxFd + 1, &readfds, &writefds, &errorfds, &timeout);

        printf("-------- TASK TIME --------\n");
        uint64_t task_counter_value;
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
        print_timer_counter(task_counter_value);

        if (FD_ISSET(s_progress_fd, &readfds)) {
            ssize_t ret = read(s_progress_fd, &signal, sizeof(signal));
            TEST_ASSERT(ret == sizeof(signal));
            TEST_ASSERT(signal == PROGRESS_SIGNAL);
            progressTriggerCount++;
            printf("Progress fd\n");
        } else if (FD_ISSET(s_timer_fd, &readfds)) {
            ssize_t ret = read(s_timer_fd, &signal, sizeof(signal));
            TEST_ASSERT(ret == sizeof(signal));
            TEST_ASSERT(signal == TIMER_SIGNAL);
            timerTriggerCount++;
            printf("TimerEvent fd\n");
        } else {
            selectTimeoutCount++;
            printf("Select timeout\n");
        }
    }

    printf("Select timeout: %d\n", selectTimeoutCount);
    printf("Timer trigger: %d\n", timerTriggerCount);
    printf("Progress trigger: %d\n", progressTriggerCount);
    TEST_ASSERT(selectTimeoutCount == 3);
    TEST_ASSERT(timerTriggerCount == 4);
    TEST_ASSERT(progressTriggerCount == 3);
    printf("Test done\n");
    close(s_progress_fd);
    close(s_timer_fd);
    eventfd_timer_deinit(TIMER_0);
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}
