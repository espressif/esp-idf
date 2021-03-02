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

#include <errno.h>
#include <sys/select.h>

#include "driver/timer.h"
#include "esp_vfs.h"
#include "sys/_stdint.h"
#include "unity.h"

TEST_CASE("Test eventfd create and close", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(0, close(fd));

    fd = eventfd(0, EFD_SUPPORT_ISR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd reject unknown flags", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 1);
    TEST_ASSERT_LESS_THAN(0, fd);
    TEST_ASSERT_EQUAL(EINVAL, errno);

    fd = eventfd(0, INT_MAX);
    TEST_ASSERT_LESS_THAN(0, fd);
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd read", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    unsigned int initval = 123;
    int fd = eventfd(initval, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint64_t val = 0;
    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(initval, val);
    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(0, val);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd read invalid size", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint32_t val = 0;
    TEST_ASSERT_LESS_THAN(0, read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd write invalid size", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint32_t val = 0;
    TEST_ASSERT_LESS_THAN(0, write(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd write then read", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint64_t val = 123;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(123, val);
    val = 4;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    val = 5;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(9, val);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("Test eventfd instant select", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    struct timeval zero_time;
    fd_set read_fds, write_fds, error_fds;

    zero_time.tv_sec = 0;
    zero_time.tv_usec = 0;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    int ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &zero_time);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT(!FD_ISSET(fd, &read_fds));

    uint64_t val = 1;
    printf("Write to fd\n");
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &zero_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd, &read_fds));

    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &zero_time);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT(!FD_ISSET(fd, &read_fds));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

static void signal_task(void *arg)
{
    int fd = *((int *)arg);
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint64_t val = 1;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    vTaskDelete(NULL);
}

TEST_CASE("Test eventfd signal from task", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    xTaskCreate(signal_task, "signal_task", 2048, &fd, 5, NULL);
    struct timeval wait_time;
    struct timeval zero_time;
    fd_set read_fds, write_fds, error_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    wait_time.tv_sec = 2;
    wait_time.tv_usec = 0;
    zero_time.tv_sec = 0;
    zero_time.tv_usec = 0;

    FD_SET(fd, &read_fds);
    int ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &wait_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd, &read_fds));

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &zero_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd, &read_fds));

    uint64_t val;
    TEST_ASSERT_EQUAL(sizeof(val), read(fd, &val, sizeof(val)));
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &zero_time);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT(!FD_ISSET(fd, &read_fds));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

static void IRAM_ATTR eventfd_select_test_isr(void *arg)
{
    int fd = *((int *)arg);
    uint64_t val = 1;
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    int ret = write(fd, &val, sizeof(val));
    assert(ret == sizeof(val));
}

TEST_CASE("Test eventfd signal from ISR", "[vfs][eventfd]")
{
    TEST_ESP_OK(esp_vfs_eventfd_register());
    int fd = eventfd(0, EFD_SUPPORT_ISR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    timer_config_t config = {
        .divider = 16,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = false,
    };
    TEST_ESP_OK(timer_init(TIMER_GROUP_0, TIMER_0, &config));

    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL));

    TEST_ESP_OK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_BASE_CLK / 16));
    TEST_ESP_OK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    TEST_ESP_OK(timer_isr_register(TIMER_GROUP_0, TIMER_0, eventfd_select_test_isr,
                                   &fd, ESP_INTR_FLAG_IRAM, NULL));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));

    struct timeval wait_time;
    fd_set read_fds, write_fds, error_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    wait_time.tv_sec = 2;
    wait_time.tv_usec = 0;

    FD_SET(fd, &read_fds);
    int ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &wait_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd, &read_fds));
    timer_deinit(TIMER_GROUP_0, TIMER_0);
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}
