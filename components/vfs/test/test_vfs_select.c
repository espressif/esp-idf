// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include "unity.h"
#include "soc/uart_struct.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "test_utils.h"

typedef struct {
    int fd;
    int delay_ms;
    xSemaphoreHandle sem;
} test_task_param_t;

static const char message[] = "Hello world!";

static int open_dummy_socket()
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *res = NULL;

    const int err = getaddrinfo("localhost", "80", &hints, &res);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_NOT_NULL(res);

    const int dummy_socket_fd = socket(res->ai_family, res->ai_socktype, 0);
    TEST_ASSERT(dummy_socket_fd >= 0);

    return dummy_socket_fd;
}

static int socket_init()
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *res;
    int err;
    struct sockaddr_in saddr = { 0 };
    int socket_fd = -1;

    err = getaddrinfo("localhost", "80", &hints, &res);
    TEST_ASSERT_EQUAL(err, 0);
    TEST_ASSERT_NOT_NULL(res);

    socket_fd = socket(res->ai_family, res->ai_socktype, 0);
    TEST_ASSERT(socket_fd >= 0);

    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(80);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(socket_fd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in));
    TEST_ASSERT(err >= 0);

    err = connect(socket_fd, res->ai_addr, res->ai_addrlen);
    TEST_ASSERT_EQUAL_MESSAGE(err, 0, "Socket connection failed");

    freeaddrinfo(res);

    return socket_fd;
}

static void uart1_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_driver_install(UART_NUM_1, 256, 256, 0, NULL, 0);
}

static void send_task(void *param)
{
    const test_task_param_t *test_task_param = param;
    vTaskDelay(test_task_param->delay_ms / portTICK_PERIOD_MS);
    write(test_task_param->fd, message, sizeof(message));
    if (test_task_param->sem) {
        xSemaphoreGive(test_task_param->sem);
    }
    vTaskDelete(NULL);
}

static inline void start_task(const test_task_param_t *test_task_param)
{
    xTaskCreate(send_task, "send_task", 4*1024, (void *) test_task_param, 5, NULL);
}

static void init(int *uart_fd, int *socket_fd)
{
    test_case_uses_tcpip();

    uart1_init();
    UART1.conf0.loopback = 1;

    *uart_fd = open("/dev/uart/1", O_RDWR);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(*uart_fd, -1, "Cannot open UART");

    esp_vfs_dev_uart_use_driver(1);

    *socket_fd = socket_init();
}

static void deinit(int uart_fd, int socket_fd)
{
    esp_vfs_dev_uart_use_nonblocking(1);
    close(uart_fd);
    UART1.conf0.loopback = 0;
    uart_driver_delete(UART_NUM_1);

    close(socket_fd);
}

TEST_CASE("UART can do select()", "[vfs]")
{
    int uart_fd;
    int socket_fd;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 100000,
    };
    char recv_message[sizeof(message)];

    init(&uart_fd, &socket_fd);

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);
    //without socket in rfds it will not use the same signalization

    const test_task_param_t test_task_param = {
        .fd = uart_fd,
        .delay_ms = 50,
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(test_task_param.sem);
    start_task(&test_task_param);

    int s = select(uart_fd + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(s, 1);
    TEST_ASSERT(FD_ISSET(uart_fd, &rfds));
    TEST_ASSERT_UNLESS(FD_ISSET(socket_fd, &rfds));

    int read_bytes = read(uart_fd, recv_message, sizeof(message));
    TEST_ASSERT_EQUAL(read_bytes, sizeof(message));
    TEST_ASSERT_EQUAL_MEMORY(message, recv_message, sizeof(message));

    TEST_ASSERT_EQUAL(xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS), pdTRUE);

    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);
    FD_SET(socket_fd, &rfds);

    start_task(&test_task_param);

    s = select(MAX(uart_fd, socket_fd) + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(s, 1);
    TEST_ASSERT(FD_ISSET(uart_fd, &rfds));
    TEST_ASSERT_UNLESS(FD_ISSET(socket_fd, &rfds));

    read_bytes = read(uart_fd, recv_message, sizeof(message));
    TEST_ASSERT_EQUAL(read_bytes, sizeof(message));
    TEST_ASSERT_EQUAL_MEMORY(message, recv_message, sizeof(message));

    TEST_ASSERT_EQUAL(xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS), pdTRUE);
    vSemaphoreDelete(test_task_param.sem);

    deinit(uart_fd, socket_fd);
}

TEST_CASE("socket can do select()", "[vfs]")
{
    int uart_fd;
    int socket_fd;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 100000,
    };
    char recv_message[sizeof(message)];

    init(&uart_fd, &socket_fd);
    const int dummy_socket_fd = open_dummy_socket();

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);
    FD_SET(socket_fd, &rfds);
    FD_SET(dummy_socket_fd, &rfds);

    const test_task_param_t test_task_param = {
        .fd = socket_fd,
        .delay_ms = 50,
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(test_task_param.sem);
    start_task(&test_task_param);

    const int s = select(MAX(MAX(uart_fd, socket_fd), dummy_socket_fd) + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(1, s);
    TEST_ASSERT_UNLESS(FD_ISSET(uart_fd, &rfds));
    TEST_ASSERT_UNLESS(FD_ISSET(dummy_socket_fd, &rfds));
    TEST_ASSERT(FD_ISSET(socket_fd, &rfds));

    int read_bytes = read(socket_fd, recv_message, sizeof(message));
    TEST_ASSERT_EQUAL(read_bytes, sizeof(message));
    TEST_ASSERT_EQUAL_MEMORY(message, recv_message, sizeof(message));

    TEST_ASSERT_EQUAL(xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS), pdTRUE);
    vSemaphoreDelete(test_task_param.sem);

    deinit(uart_fd, socket_fd);
    close(dummy_socket_fd);
}

TEST_CASE("select() timeout", "[vfs]")
{
    int uart_fd;
    int socket_fd;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 100000,
    };

    init(&uart_fd, &socket_fd);

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);
    FD_SET(socket_fd, &rfds);

    int s = select(MAX(uart_fd, socket_fd) + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(s, 0);
    TEST_ASSERT_UNLESS(FD_ISSET(uart_fd, &rfds));
    TEST_ASSERT_UNLESS(FD_ISSET(socket_fd, &rfds));

    FD_ZERO(&rfds);

    s = select(MAX(uart_fd, socket_fd) + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(s, 0);
    TEST_ASSERT_UNLESS(FD_ISSET(uart_fd, &rfds));
    TEST_ASSERT_UNLESS(FD_ISSET(socket_fd, &rfds));

    deinit(uart_fd, socket_fd);
}

static void select_task(void *param)
{
    const test_task_param_t *test_task_param = param;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 100000,
    };

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(test_task_param->fd, &rfds);

    int s = select(test_task_param->fd + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(0, s); //timeout

    if (test_task_param->sem) {
        xSemaphoreGive(test_task_param->sem);
    }
    vTaskDelete(NULL);
}

TEST_CASE("concurent selects work", "[vfs]")
{
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 100000,//irrelevant
    };

    int uart_fd, socket_fd;
    init(&uart_fd, &socket_fd);

    const int dummy_socket_fd = open_dummy_socket();

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);

    test_task_param_t test_task_param = {
        .fd = uart_fd,
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(test_task_param.sem);

    xTaskCreate(select_task, "select_task", 4*1024, (void *) &test_task_param, 5, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS); //make sure the task has started and waits in select()

    int s = select(uart_fd + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(-1, s); //this select should fail because two selects are accessing UART
                              //(the other one is waiting for the timeout)
    TEST_ASSERT_EQUAL(EINTR, errno);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS));

    FD_ZERO(&rfds);
    FD_SET(socket_fd, &rfds);

    test_task_param.fd = dummy_socket_fd;

    xTaskCreate(select_task, "select_task", 4*1024, (void *) &test_task_param, 5, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS); //make sure the task has started and waits in select()

    s = select(socket_fd + 1, &rfds, NULL, NULL, &tv);
    TEST_ASSERT_EQUAL(0, s); //this select should timeout as well as the concurrent one because
                             //concurrent socket select should work

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS));
    vSemaphoreDelete(test_task_param.sem);

    deinit(uart_fd, socket_fd);
    close(dummy_socket_fd);
}
