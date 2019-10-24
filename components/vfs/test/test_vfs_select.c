// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "test_utils.h"

typedef struct {
    int fd;
    int delay_ms;
    xSemaphoreHandle sem;
} test_task_param_t;

typedef struct {
    fd_set *rdfds;
    fd_set *wrfds;
    fd_set *errfds;
    int maxfds;
    struct timeval *tv;
    int select_ret;
    xSemaphoreHandle sem;
} test_select_task_param_t;

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

TEST_CASE("UART can do poll()", "[vfs]")
{
    int uart_fd;
    int socket_fd;
    char recv_message[sizeof(message)];

    init(&uart_fd, &socket_fd);

    struct pollfd poll_fds[] = {
        {
            .fd = uart_fd,
            .events = POLLIN,
        },
        {
            .fd = -1,  // should be ignored according to the documentation of poll()
        },
    };

    const test_task_param_t test_task_param = {
        .fd = uart_fd,
        .delay_ms = 50,
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(test_task_param.sem);
    start_task(&test_task_param);

    int s = poll(poll_fds, sizeof(poll_fds)/sizeof(poll_fds[0]), 100);
    TEST_ASSERT_EQUAL(s, 1);
    TEST_ASSERT_EQUAL(uart_fd, poll_fds[0].fd);
    TEST_ASSERT_EQUAL(POLLIN, poll_fds[0].revents);
    TEST_ASSERT_EQUAL(-1, poll_fds[1].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[1].revents);

    int read_bytes = read(uart_fd, recv_message, sizeof(message));
    TEST_ASSERT_EQUAL(read_bytes, sizeof(message));
    TEST_ASSERT_EQUAL_MEMORY(message, recv_message, sizeof(message));

    TEST_ASSERT_EQUAL(xSemaphoreTake(test_task_param.sem, 1000 / portTICK_PERIOD_MS), pdTRUE);

    poll_fds[1].fd = socket_fd;
    poll_fds[1].events = POLLIN;

    start_task(&test_task_param);

    s = poll(poll_fds, sizeof(poll_fds)/sizeof(poll_fds[0]), 100);
    TEST_ASSERT_EQUAL(s, 1);
    TEST_ASSERT_EQUAL(uart_fd, poll_fds[0].fd);
    TEST_ASSERT_EQUAL(POLLIN, poll_fds[0].revents);
    TEST_ASSERT_EQUAL(socket_fd, poll_fds[1].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[1].revents);

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

TEST_CASE("socket can do poll()", "[vfs]")
{
    int uart_fd;
    int socket_fd;
    char recv_message[sizeof(message)];

    init(&uart_fd, &socket_fd);
    const int dummy_socket_fd = open_dummy_socket();

    struct pollfd poll_fds[] = {
        {
            .fd = uart_fd,
            .events = POLLIN,
        },
        {
            .fd = socket_fd,
            .events = POLLIN,
        },
        {
            .fd = dummy_socket_fd,
            .events = POLLIN,
        },
    };

    const test_task_param_t test_task_param = {
        .fd = socket_fd,
        .delay_ms = 50,
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(test_task_param.sem);
    start_task(&test_task_param);

    int s = poll(poll_fds, sizeof(poll_fds)/sizeof(poll_fds[0]), 100);
    TEST_ASSERT_EQUAL(s, 1);
    TEST_ASSERT_EQUAL(uart_fd, poll_fds[0].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[0].revents);
    TEST_ASSERT_EQUAL(socket_fd, poll_fds[1].fd);
    TEST_ASSERT_EQUAL(POLLIN, poll_fds[1].revents);
    TEST_ASSERT_EQUAL(dummy_socket_fd, poll_fds[2].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[2].revents);

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

TEST_CASE("poll() timeout", "[vfs]")
{
    int uart_fd;
    int socket_fd;

    init(&uart_fd, &socket_fd);

    struct pollfd poll_fds[] = {
        {
            .fd = uart_fd,
            .events = POLLIN,
        },
        {
            .fd = socket_fd,
            .events = POLLIN,
        },
    };

    int s = poll(poll_fds, sizeof(poll_fds)/sizeof(poll_fds[0]), 100);
    TEST_ASSERT_EQUAL(s, 0);
    TEST_ASSERT_EQUAL(uart_fd, poll_fds[0].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[0].revents);
    TEST_ASSERT_EQUAL(socket_fd, poll_fds[1].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[1].revents);

    poll_fds[0].fd = -1;
    poll_fds[1].fd = -1;

    s = poll(poll_fds, sizeof(poll_fds)/sizeof(poll_fds[0]), 100);
    TEST_ASSERT_EQUAL(s, 0);
    TEST_ASSERT_EQUAL(-1, poll_fds[0].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[0].revents);
    TEST_ASSERT_EQUAL(-1, poll_fds[1].fd);
    TEST_ASSERT_EQUAL(0, poll_fds[1].revents);

    deinit(uart_fd, socket_fd);
}

static void select_task(void *task_param)
{
    const test_select_task_param_t *param = task_param;

    int s = select(param->maxfds, param->rdfds, param->wrfds, param->errfds, param->tv);
    TEST_ASSERT_EQUAL(param->select_ret, s);

    if (param->sem) {
        xSemaphoreGive(param->sem);
    }
    vTaskDelete(NULL);
}

static void inline start_select_task(test_select_task_param_t *param)
{
    xTaskCreate(select_task, "select_task", 4*1024, (void *) param, 5, NULL);
}

TEST_CASE("concurrent selects work", "[vfs]")
{
    int uart_fd, socket_fd;
    init(&uart_fd, &socket_fd);
    const int dummy_socket_fd = open_dummy_socket();

    {
        // Two tasks will wait for the same UART FD for reading and they will time-out

        struct timeval tv = {
            .tv_sec = 0,
            .tv_usec = 100000,
        };

        fd_set rdfds1;
        FD_ZERO(&rdfds1);
        FD_SET(uart_fd, &rdfds1);

        test_select_task_param_t param = {
            .rdfds = &rdfds1,
            .wrfds = NULL,
            .errfds = NULL,
            .maxfds = uart_fd + 1,
            .tv = &tv,
            .select_ret = 0, // expected timeout
            .sem = xSemaphoreCreateBinary(),
        };
        TEST_ASSERT_NOT_NULL(param.sem);

        fd_set rdfds2;
        FD_ZERO(&rdfds2);
        FD_SET(uart_fd, &rdfds2);
        FD_SET(socket_fd, &rdfds2);
        FD_SET(dummy_socket_fd, &rdfds2);

        start_select_task(&param);
        vTaskDelay(10 / portTICK_PERIOD_MS); //make sure the task has started and waits in select()

        int s = select(MAX(MAX(uart_fd, dummy_socket_fd), socket_fd) + 1, &rdfds2, NULL, NULL, &tv);
        TEST_ASSERT_EQUAL(0, s); // timeout here as well

        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(param.sem, 1000 / portTICK_PERIOD_MS));
        vSemaphoreDelete(param.sem);
    }

    {
        // One tasks waits for UART reading and one for writing. The former will be successful and latter will
        // time-out.

        struct timeval tv = {
            .tv_sec = 0,
            .tv_usec = 100000,
        };

        fd_set wrfds1;
        FD_ZERO(&wrfds1);
        FD_SET(uart_fd, &wrfds1);

        test_select_task_param_t param = {
            .rdfds = NULL,
            .wrfds = &wrfds1,
            .errfds = NULL,
            .maxfds = uart_fd + 1,
            .tv = &tv,
            .select_ret = 0, // expected timeout
            .sem = xSemaphoreCreateBinary(),
        };
        TEST_ASSERT_NOT_NULL(param.sem);

        start_select_task(&param);

        fd_set rdfds2;
        FD_ZERO(&rdfds2);
        FD_SET(uart_fd, &rdfds2);
        FD_SET(socket_fd, &rdfds2);
        FD_SET(dummy_socket_fd, &rdfds2);

        const test_task_param_t send_param = {
            .fd = uart_fd,
            .delay_ms = 50,
            .sem = xSemaphoreCreateBinary(),
        };
        TEST_ASSERT_NOT_NULL(send_param.sem);
        start_task(&send_param);        // This task will write to UART which will be detected by select()

        int s = select(MAX(MAX(uart_fd, dummy_socket_fd), socket_fd) + 1, &rdfds2, NULL, NULL, &tv);
        TEST_ASSERT_EQUAL(1, s);
        TEST_ASSERT(FD_ISSET(uart_fd, &rdfds2));
        TEST_ASSERT_UNLESS(FD_ISSET(socket_fd, &rdfds2));
        TEST_ASSERT_UNLESS(FD_ISSET(dummy_socket_fd, &rdfds2));

        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(param.sem, 1000 / portTICK_PERIOD_MS));
        vSemaphoreDelete(param.sem);

        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(send_param.sem, 1000 / portTICK_PERIOD_MS));
        vSemaphoreDelete(send_param.sem);
    }

    deinit(uart_fd, socket_fd);
    close(dummy_socket_fd);
}

TEST_CASE("select() works with concurrent mount", "[vfs][fatfs]")
{
    wl_handle_t test_wl_handle;
    int uart_fd, socket_fd;

    init(&uart_fd, &socket_fd);
    const int dummy_socket_fd = open_dummy_socket();

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 2
    };

    // select() will be waiting for a socket & UART and FATFS mount will occur in parallel

    struct timeval tv = {
        .tv_sec = 1,
        .tv_usec = 0,
    };

    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(uart_fd, &rdfds);
    FD_SET(dummy_socket_fd, &rdfds);

    test_select_task_param_t param = {
        .rdfds = &rdfds,
        .wrfds = NULL,
        .errfds = NULL,
        .maxfds = MAX(uart_fd, dummy_socket_fd) + 1,
        .tv = &tv,
        .select_ret = 0, // expected timeout
        .sem = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(param.sem);

    start_select_task(&param);
    vTaskDelay(10 / portTICK_PERIOD_MS); //make sure the task has started and waits in select()

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &test_wl_handle));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(param.sem, 1500 / portTICK_PERIOD_MS));

    // select() will be waiting for a socket & UART and FATFS unmount will occur in parallel

    FD_ZERO(&rdfds);
    FD_SET(uart_fd, &rdfds);
    FD_SET(dummy_socket_fd, &rdfds);

    start_select_task(&param);
    vTaskDelay(10 / portTICK_PERIOD_MS); //make sure the task has started and waits in select()

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount("/spiflash", test_wl_handle));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(param.sem, 1500 / portTICK_PERIOD_MS));

    vSemaphoreDelete(param.sem);

    deinit(uart_fd, socket_fd);
    close(dummy_socket_fd);
}
