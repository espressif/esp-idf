/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "driver/gptimer.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"

TEST_CASE("eventfd create and close", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));
    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(0, close(fd));

    fd = eventfd(0, EFD_SUPPORT_ISR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("eventfd reject unknown flags", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, 1);
    TEST_ASSERT_LESS_THAN(0, fd);
    TEST_ASSERT_EQUAL(EINVAL, errno);

    fd = eventfd(0, INT_MAX);
    TEST_ASSERT_LESS_THAN(0, fd);
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("eventfd read", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

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

TEST_CASE("eventfd read invalid size", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint32_t val = 0;
    TEST_ASSERT_LESS_THAN(0, read(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("eventfd write invalid size", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    uint32_t val = 0;
    TEST_ASSERT_LESS_THAN(0, write(fd, &val, sizeof(val)));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

TEST_CASE("eventfd write then read", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

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

TEST_CASE("eventfd instant select", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

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
    TEST_ASSERT_EQUAL(0, close(fd));
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

TEST_CASE("eventfd signal from task", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd0 = eventfd(0, 0);
    int fd1 = eventfd(0, 0);
    int max_fd = fd1 > fd0 ? fd1 : fd0;
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd1);

    xTaskCreate(signal_task, "signal_task", 2048, &fd0, 5, NULL);
    struct timeval wait_time;
    struct timeval zero_time;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd0, &read_fds);
    FD_SET(fd1, &read_fds);
    wait_time.tv_sec = 2;
    wait_time.tv_usec = 0;
    zero_time.tv_sec = 0;
    zero_time.tv_usec = 0;

    int ret = select(max_fd + 1, &read_fds, NULL, NULL, &wait_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd0, &read_fds));

    uint64_t val = 1;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd1, &val, sizeof(val)));

    FD_ZERO(&read_fds);
    FD_SET(fd0, &read_fds);
    FD_SET(fd1, &read_fds);
    ret = select(max_fd + 1, &read_fds, NULL, NULL, &zero_time);
    TEST_ASSERT_EQUAL(2, ret);
    TEST_ASSERT(FD_ISSET(fd0, &read_fds));
    TEST_ASSERT(FD_ISSET(fd1, &read_fds));

    TEST_ASSERT_EQUAL(0, close(fd0));
    TEST_ASSERT_EQUAL(0, close(fd1));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

static bool eventfd_select_test_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    gptimer_stop(timer);
    int fd = *((int *)user_ctx);
    uint64_t val = 1;
    int ret = write(fd, &val, sizeof(val));
    assert(ret == sizeof(val));
    return true;
}

TEST_CASE("eventfd signal from ISR", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, EFD_SUPPORT_ISR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 200000,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = eventfd_select_test_isr,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, &fd));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

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
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}

static void close_task(void *arg)
{
    int fd = *((int *)arg);
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ASSERT_EQUAL(0, close(fd));
    vTaskDelete(NULL);
}

TEST_CASE("eventfd select closed fd", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    xTaskCreate(close_task, "close_task", 2048, &fd, 5, NULL);
    struct timeval wait_time;
    fd_set read_fds, write_fds, error_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
    FD_SET(fd, &read_fds);
    FD_SET(fd, &error_fds);
    wait_time.tv_sec = 2;
    wait_time.tv_usec = 0;

    int ret = select(fd + 1, &read_fds, &write_fds, &error_fds, &wait_time);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT(FD_ISSET(fd, &error_fds));

    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}

typedef struct {
    QueueHandle_t queue;
    int fd;
} select_task_args_t;

static void select_task(void *arg)
{
    select_task_args_t *select_arg = (select_task_args_t *)arg;
    int fd = select_arg->fd;
    struct timeval wait_time;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    wait_time.tv_sec = 2;
    wait_time.tv_usec = 0;

    int ret = select(fd + 1, &read_fds, NULL, NULL, &wait_time);
    assert(ret == 1);
    xQueueSend(select_arg->queue, select_arg, 0);
    vTaskDelete(NULL);
}

TEST_CASE("eventfd multiple selects", "[vfs][eventfd]")
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_vfs_eventfd_register(&config));

    int fd = eventfd(0, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    select_task_args_t args = {
        .queue = xQueueCreate(10, sizeof(select_task_args_t)),
        .fd = fd,
    };
    select_task_args_t ret_args;

    xTaskCreate(select_task, "select_task0", 2048, &args, 5, NULL);
    xTaskCreate(select_task, "select_task1", 2048, &args, 5, NULL);

    uint64_t val = 1;
    TEST_ASSERT_EQUAL(sizeof(val), write(fd, &val, sizeof(val)));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ASSERT(xQueueReceive(args.queue, &ret_args, 0));
    TEST_ASSERT_EQUAL(ret_args.fd, fd);
    TEST_ASSERT(xQueueReceive(args.queue, &ret_args, 0));
    TEST_ASSERT_EQUAL(ret_args.fd, fd);

    vQueueDelete(args.queue);
    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ESP_OK(esp_vfs_eventfd_unregister());
}
