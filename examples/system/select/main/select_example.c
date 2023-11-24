/* Select Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/param.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart_vfs.h"
#include "driver/uart.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

static const char* TAG = "uart_select_example";

static int uart_fd = -1;
static int socket_fd = -1;

static void socket_deinit(void)
{
    close(socket_fd);
    socket_fd = -1;
}

static void socket_init(void)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *res;
    int err;
    struct sockaddr_in saddr = { 0 };

    ESP_ERROR_CHECK(esp_netif_init());

    err = getaddrinfo("localhost", "80", &hints, &res);

    if (err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed: %d", errno);
        return;
    }

    socket_fd = socket(res->ai_family, res->ai_socktype, 0);

    if (socket_fd < 0) {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return;
    }

    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(80);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(socket_fd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to bind socket");
        freeaddrinfo(res);
        socket_deinit();
        return;
    }

    if (connect(socket_fd, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "Socket connection failed: %d", errno);
        freeaddrinfo(res);
        socket_deinit();
        return;
    }

    freeaddrinfo(res);
}

static void uart1_deinit(void)
{
    close(uart_fd);
    uart_fd = -1;
    uart_driver_delete(UART_NUM_1);
}

static void uart1_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_loop_back(UART_NUM_1, true);

    if ((uart_fd = open("/dev/uart/1", O_RDWR | O_NONBLOCK)) == -1) {
        ESP_LOGE(TAG, "Cannot open UART1");
        uart1_deinit();
    }

    uart_vfs_dev_use_driver(1);
}

static void uart1_write_task(void *param)
{
    char buf[20];

    uart1_init();

    for (uint8_t i = 1;; ++i) {
        vTaskDelay(4000 / portTICK_PERIOD_MS);

        snprintf(buf, sizeof(buf), "UART message U%d", i);
        int write_bytes = write(uart_fd, buf, strlen(buf));
        if (write_bytes < 0) {
            ESP_LOGE(TAG, "UART1 write error");
        } else {
            ESP_LOGI(TAG, "%d bytes were sent to UART1: %s", write_bytes, buf);
        }
    }

    uart1_deinit();
    vTaskDelete(NULL);
}

static void socket_write_task(void *param)
{
    char buf[20];

    socket_init();

    for (uint8_t i = 1;; ++i) {
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        snprintf(buf, sizeof(buf), "Socket message S%d", i);
        int write_bytes = write(socket_fd, buf, strlen(buf));
        if (write_bytes < 0) {
            ESP_LOGE(TAG, "Socket write error");
        } else {
            ESP_LOGI(TAG, "%d bytes were written to socket: %s", write_bytes, buf);
        }
    }

    socket_deinit();
    vTaskDelete(NULL);
}

static void check_and_print(int fd, const fd_set *rfds, const char *src_msg)
{
    char buf[100];
    int read_bytes;

    if (FD_ISSET(fd, rfds)) {
        if ((read_bytes = read(fd, buf, sizeof(buf)-1)) > 0) {
            buf[read_bytes] = '\0';
            ESP_LOGI(TAG, "%d bytes were received through %s: %s", read_bytes, src_msg, buf);
        } else {
            ESP_LOGE(TAG, "%s read error", src_msg);
        }
    }
}

static void select_task(void *param)
{
    while (1) {
        int s;
        fd_set rfds;
        struct timeval tv = {
            .tv_sec = 1,
            .tv_usec = 0,
        };

        FD_ZERO(&rfds);
        FD_SET(uart_fd, &rfds);
        FD_SET(socket_fd, &rfds);

        s = select(MAX(uart_fd, socket_fd) + 1, &rfds, NULL, NULL, &tv);

        if (s < 0) {
            ESP_LOGE(TAG, "Select failed: errno %d", errno);
        } else if (s == 0) {
            ESP_LOGI(TAG, "Timeout has been reached and nothing has been received");
        } else {
            check_and_print(socket_fd, &rfds, "socket");
            check_and_print(uart_fd, &rfds, "UART1");
        }
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(uart1_write_task, "uart1_write_task", 4*1024, NULL, 5, NULL);
    xTaskCreate(socket_write_task, "socket_write_task", 4*1024, NULL, 5, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreate(select_task, "select_task", 4*1024, NULL, 5, NULL);
}
