// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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

#include "esp_openthread_uart.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_types.h"
#include "esp_vfs_dev.h"
#include "common/logging.hpp"
#include "driver/uart.h"
#include "utils/uart.h"

static int s_uart_port;
static int s_uart_fd;
static uint8_t s_uart_buffer[ESP_OPENTHREAD_UART_BUFFER_SIZE];

otError otPlatUartEnable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *buf, uint16_t buf_length)
{
    int rval = write(s_uart_fd, buf, buf_length);

    if (rval != (int)buf_length) {
        return OT_ERROR_FAILED;
    }

    otPlatUartSendDone();

    return OT_ERROR_NONE;
}

esp_err_t esp_openthread_uart_init_port(const esp_openthread_uart_config_t *config)
{
    ESP_RETURN_ON_ERROR(uart_param_config(config->port, &config->uart_config), OT_PLAT_LOG_TAG,
                        "uart_param_config failed");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
        OT_PLAT_LOG_TAG, "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(config->port, ESP_OPENTHREAD_UART_BUFFER_SIZE, 0, 0, NULL, 0),
                        OT_PLAT_LOG_TAG, "uart_driver_install failed");
    esp_vfs_dev_uart_use_driver(config->port);
    return ESP_OK;
}

esp_err_t esp_openthread_uart_init(const esp_openthread_platform_config_t *config)
{
    char uart_path[16];

    // Disable IO buffer.
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    // Install UART driver for interrupt-driven reads and writes.
    s_uart_port = config->host_config.host_uart_config.port;
    ESP_RETURN_ON_ERROR(esp_openthread_uart_init_port(&config->host_config.host_uart_config), OT_PLAT_LOG_TAG,
                        "esp_openthread_uart_init_port failed");

    esp_vfs_dev_uart_port_set_rx_line_endings(s_uart_port, ESP_LINE_ENDINGS_LF);
    esp_vfs_dev_uart_port_set_tx_line_endings(s_uart_port, ESP_LINE_ENDINGS_CRLF);

    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", s_uart_port);
    s_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);

    return s_uart_fd >= 0 ? ESP_OK : ESP_FAIL;
}

void esp_openthread_uart_deinit()
{
    if (s_uart_fd != -1) {
        close(s_uart_fd);
        s_uart_fd = -1;
    }
    uart_driver_delete(s_uart_port);
}

void esp_openthread_uart_update(esp_openthread_mainloop_context_t *mainloop)
{
    FD_SET(s_uart_fd, &mainloop->read_fds);
    if (s_uart_fd > mainloop->max_fd) {
        mainloop->max_fd = s_uart_fd;
    }
}

esp_err_t esp_openthread_uart_process()
{
    int rval = read(s_uart_fd, s_uart_buffer, sizeof(s_uart_buffer));

    if (rval > 0) {
        otPlatUartReceived(s_uart_buffer, (uint16_t)rval);
    } else if (rval > 0) {
        if (errno != EAGAIN) {
            otLogWarnPlat("read uart failed: %d", errno);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}
