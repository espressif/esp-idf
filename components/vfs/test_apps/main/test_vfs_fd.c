/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/fcntl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_vfs.h"
#include "unity.h"
#include "esp_log.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define VFS_PREF1       "/vfs1"
#define VFS_PREF2       "/vfs2"
#define FILE1           "/file1"

typedef struct {
    const char *path;
    int fd;
} collision_test_vfs_param_t;

static int collision_test_vfs_open(void* ctx, const char * path, int flags, int mode)
{
    const collision_test_vfs_param_t *param = (collision_test_vfs_param_t *) ctx;
    if (strcmp(param->path, path) == 0) {
        return param->fd;
    }
    errno = ENOENT;
    return -1;
}

static int collision_test_vfs_close(void* ctx, int fd)
{
    const collision_test_vfs_param_t *param = (collision_test_vfs_param_t *) ctx;
    if (fd == param->fd) {
        return 0;
    }
    errno = EBADF;
    return -1;
}

TEST_CASE("FDs from different VFSs don't collide", "[vfs]")
{
    collision_test_vfs_param_t param = {
        .path = FILE1,
        .fd = 1,
    };

    esp_vfs_t desc = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .open_p = collision_test_vfs_open,
        .close_p = collision_test_vfs_close,
    };
    TEST_ESP_OK( esp_vfs_register(VFS_PREF1, &desc, &param) );
    TEST_ESP_OK( esp_vfs_register(VFS_PREF2, &desc, &param) );

    const int fd1 = open(VFS_PREF1 FILE1, 0, 0);
    const int fd2 = open(VFS_PREF2 FILE1, 0, 0);

    TEST_ASSERT_NOT_EQUAL(fd1, -1);
    TEST_ASSERT_NOT_EQUAL(fd2, -1);
    // Both VFS drivers return local FD 1 but the global FDs returned by
    // open() should not be the same
    TEST_ASSERT_NOT_EQUAL(fd1, fd2);

    TEST_ASSERT_NOT_EQUAL(close(fd1), -1);
    TEST_ASSERT_NOT_EQUAL(close(fd2), -1);

    TEST_ESP_OK( esp_vfs_unregister(VFS_PREF1) );
    TEST_ESP_OK( esp_vfs_unregister(VFS_PREF2) );
}

#define FILE2                       "/file2"
#define FILE3                       "/file3"
#define FILE4                       "/file4"
#define CONCURRENT_TEST_STACK_SIZE  (2*1024)
#define CONCURRENT_TEST_MAX_WAIT    (1000 / portTICK_PERIOD_MS)

typedef struct {
    const char *path;
    SemaphoreHandle_t done;
} concurrent_test_task_param_t;

typedef struct {
    const char *path;
    int local_fd;
} concurrent_test_path_to_fd_t;

static concurrent_test_path_to_fd_t concurrent_test_path_to_fd[] = {
    { .path = FILE1, .local_fd = 1 },
    { .path = FILE2, .local_fd = 2 },
    { .path = FILE3, .local_fd = 3 },
    { .path = FILE4, .local_fd = 4 },
};

static int concurrent_test_vfs_open(const char * path, int flags, int mode)
{
    for (size_t i = 0; i < sizeof(concurrent_test_path_to_fd)/sizeof(concurrent_test_path_to_fd[0]); ++i) {
        if (strcmp(concurrent_test_path_to_fd[i].path, path) == 0) {
            // This behaves like UART: opening the same file gives always the
            // same local FD (even when opening at the same time multiple FDs)
            return concurrent_test_path_to_fd[i].local_fd;
        }
    }

    errno = ENOENT;
    return -1;
}

static int concurrent_test_vfs_close(int fd)
{
    for (size_t i = 0; i < sizeof(concurrent_test_path_to_fd)/sizeof(concurrent_test_path_to_fd[0]); ++i) {
        if (concurrent_test_path_to_fd[i].local_fd == fd) {
            return 0;
        }
    }
    errno = EBADF;
    return -1;
}

static inline void test_delay_rand_ms(int ms)
{
    vTaskDelay((rand() % ms) / portTICK_PERIOD_MS);
}

static void concurrent_task(void *param)
{
    concurrent_test_task_param_t *task_param = (concurrent_test_task_param_t *) param;

    test_delay_rand_ms(10);
    for (int i = 0; i < 10; ++i) {
        const int global_fd = open(task_param->path, 0, 0);
        TEST_ASSERT_NOT_EQUAL(global_fd, -1);
        test_delay_rand_ms(10);
        TEST_ASSERT_NOT_EQUAL(close(global_fd), -1);
        test_delay_rand_ms(10);
    }
    xSemaphoreGive(task_param->done);
    vTaskDelete(NULL);
}

TEST_CASE("VFS can handle concurrent open/close requests", "[vfs]")
{
    esp_vfs_t desc = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .open = concurrent_test_vfs_open,
        .close = concurrent_test_vfs_close,
    };

    TEST_ESP_OK( esp_vfs_register(VFS_PREF1, &desc, NULL) );

    concurrent_test_task_param_t param1 = { .path = VFS_PREF1 FILE1, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param2 = { .path = VFS_PREF1 FILE1, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param3 = { .path = VFS_PREF1 FILE2, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param4 = { .path = VFS_PREF1 FILE2, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param5 = { .path = VFS_PREF1 FILE3, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param6 = { .path = VFS_PREF1 FILE3, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param7 = { .path = VFS_PREF1 FILE4, .done = xSemaphoreCreateBinary() };
    concurrent_test_task_param_t param8 = { .path = VFS_PREF1 FILE4, .done = xSemaphoreCreateBinary() };

    TEST_ASSERT_NOT_NULL(param1.done);
    TEST_ASSERT_NOT_NULL(param2.done);
    TEST_ASSERT_NOT_NULL(param3.done);
    TEST_ASSERT_NOT_NULL(param4.done);
    TEST_ASSERT_NOT_NULL(param5.done);
    TEST_ASSERT_NOT_NULL(param6.done);
    TEST_ASSERT_NOT_NULL(param7.done);
    TEST_ASSERT_NOT_NULL(param8.done);

    const int cpuid0 = 0;
    const int cpuid1 = CONFIG_FREERTOS_NUMBER_OF_CORES - 1;

    srand(time(NULL));

    xTaskCreatePinnedToCore(concurrent_task, "t1", CONCURRENT_TEST_STACK_SIZE, &param1, 3, NULL, cpuid0);
    xTaskCreatePinnedToCore(concurrent_task, "t2", CONCURRENT_TEST_STACK_SIZE, &param2, 3, NULL, cpuid1);
    xTaskCreatePinnedToCore(concurrent_task, "t3", CONCURRENT_TEST_STACK_SIZE, &param3, 3, NULL, cpuid0);
    xTaskCreatePinnedToCore(concurrent_task, "t4", CONCURRENT_TEST_STACK_SIZE, &param4, 3, NULL, cpuid1);
    xTaskCreatePinnedToCore(concurrent_task, "t5", CONCURRENT_TEST_STACK_SIZE, &param5, 3, NULL, cpuid0);
    xTaskCreatePinnedToCore(concurrent_task, "t6", CONCURRENT_TEST_STACK_SIZE, &param6, 3, NULL, cpuid1);
    xTaskCreatePinnedToCore(concurrent_task, "t7", CONCURRENT_TEST_STACK_SIZE, &param7, 3, NULL, cpuid0);
    xTaskCreatePinnedToCore(concurrent_task, "t8", CONCURRENT_TEST_STACK_SIZE, &param8, 3, NULL, cpuid1);

    TEST_ASSERT_EQUAL(xSemaphoreTake(param1.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param2.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param3.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param4.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param5.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param6.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param7.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);
    TEST_ASSERT_EQUAL(xSemaphoreTake(param8.done, CONCURRENT_TEST_MAX_WAIT), pdTRUE);

    vSemaphoreDelete(param1.done);
    vSemaphoreDelete(param2.done);
    vSemaphoreDelete(param3.done);
    vSemaphoreDelete(param4.done);
    vSemaphoreDelete(param5.done);
    vSemaphoreDelete(param6.done);
    vSemaphoreDelete(param7.done);
    vSemaphoreDelete(param8.done);

    TEST_ESP_OK( esp_vfs_unregister(VFS_PREF1) );
}

static int time_test_vfs_open(const char *path, int flags, int mode)
{
    return 1;
}

static int time_test_vfs_close(int fd)
{
    return 1;
}

static int time_test_vfs_write(int fd, const void *data, size_t size)
{
    return size;
}

TEST_CASE("Open & write & close through VFS passes performance test", "[vfs]")
{
    esp_vfs_t desc = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .open = time_test_vfs_open,
        .close = time_test_vfs_close,
        .write = time_test_vfs_write,
    };

    TEST_ESP_OK( esp_vfs_register(VFS_PREF1, &desc, NULL) );

    ccomp_timer_start();
    const int iter_count = 5000;

    for (int i = 0; i < iter_count; ++i) {
        const int fd = open(VFS_PREF1 FILE1, 0, 0);
        TEST_ASSERT_NOT_EQUAL(fd, -1);

        write(fd, "a", 1);

        TEST_ASSERT_NOT_EQUAL(close(fd), -1);
    }

    const int64_t time_diff_us = ccomp_timer_stop();
    const int ns_per_iter = (int) (time_diff_us * 1000 / iter_count);
    TEST_ESP_OK( esp_vfs_unregister(VFS_PREF1) );
#ifdef CONFIG_SPIRAM
    TEST_PERFORMANCE_CCOMP_LESS_THAN(VFS_OPEN_WRITE_CLOSE_TIME_PSRAM, "%dns", ns_per_iter);
#else
    TEST_PERFORMANCE_CCOMP_LESS_THAN(VFS_OPEN_WRITE_CLOSE_TIME, "%dns", ns_per_iter);
#endif

}

static int vfs_overlap_test_open(const char * path, int flags, int mode)
{
    return 0;
}

static int vfs_overlap_test_close(int fd)
{
    return 0;
}


TEST_CASE("esp_vfs_register_fd_range checks for overlap", "[vfs]")
{
    esp_vfs_t vfs1 = {
        .open = vfs_overlap_test_open,
        .close = vfs_overlap_test_close
    };


    TEST_ESP_OK(esp_vfs_register("/test", &vfs1, NULL));
    int fd = open("/test/1", 0, 0);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    esp_vfs_t vfs2 = { };
    esp_err_t err = esp_vfs_register_fd_range(&vfs2, NULL, fd, fd + 1);
    close(fd);

    TEST_ESP_OK(esp_vfs_unregister("/test"));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, err);
}

static void socket_init(int *socket_fd)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *res;
    int err;

    err = getaddrinfo("localhost", "80", &hints, &res);
    TEST_ASSERT_EQUAL(err, 0);
    TEST_ASSERT_NOT_NULL(res);

    *socket_fd = socket(res->ai_family, res->ai_socktype, 0);
    TEST_ASSERT(*socket_fd >= 0);
    freeaddrinfo(res);
}

TEST_CASE("esp_vfs_dump_fds displays all registered fds in vfs", "[vfs]")
{
    int uart_fd0, uart_fd1, uart_fd2;
    int socket_fd=-1;

    uart_fd0 = open("/dev/uart/0", O_RDWR);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(uart_fd0, -1, "Cannot open UART");
    uart_fd1 = open("/dev/uart/1", O_RDWR);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(uart_fd1, -1, "Cannot open UART");
    uart_fd2 = open("/dev/uart/1", O_RDWR);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(uart_fd2, -1, "Cannot open UART");

    test_case_uses_tcpip();
    socket_init(&socket_fd);

    esp_vfs_dump_fds(stdout);

    close(uart_fd0);
    close(uart_fd1);
    close(uart_fd2);
    close(socket_fd);
}
