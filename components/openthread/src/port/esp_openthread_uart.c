/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_uart.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_vfs_dev.h"
#include "common/logging.hpp"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "utils/uart.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag.h"

static int s_uart_port;
static int s_uart_fd;
static uint8_t s_uart_buffer[ESP_OPENTHREAD_UART_BUFFER_SIZE];
static const char *uart_workflow = "uart";

#if (CONFIG_OPENTHREAD_CLI || (CONFIG_OPENTHREAD_RADIO && CONFIG_OPENTHREAD_RCP_UART))
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
#endif

esp_err_t esp_openthread_uart_init_port(const esp_openthread_uart_config_t *config)
{
    ESP_RETURN_ON_ERROR(uart_param_config(config->port, &config->uart_config), OT_PLAT_LOG_TAG,
                        "uart_param_config failed");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
        OT_PLAT_LOG_TAG, "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(config->port, ESP_OPENTHREAD_UART_BUFFER_SIZE, 0, 0, NULL, 0),
                        OT_PLAT_LOG_TAG, "uart_driver_install failed");
    uart_vfs_dev_use_driver(config->port);
    return ESP_OK;
}

#if CONFIG_OPENTHREAD_CONSOLE_TYPE_USB_SERIAL_JTAG
esp_err_t esp_openthread_host_cli_usb_init(const esp_openthread_platform_config_t *config)
{
    esp_err_t ret = ESP_OK;
    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable non-blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, O_NONBLOCK);
    fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);

    ret = usb_serial_jtag_driver_install((usb_serial_jtag_driver_config_t *)&config->host_config.host_usb_config);
    usb_serial_jtag_vfs_use_driver();
    uart_vfs_dev_register();
    return ret;
}
#endif

#if CONFIG_OPENTHREAD_CONSOLE_TYPE_UART
esp_err_t esp_openthread_host_cli_uart_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_ERROR(esp_openthread_uart_init_port(&config->host_config.host_uart_config), OT_PLAT_LOG_TAG,
                        "esp_openthread_uart_init_port failed");
    return ESP_OK;
}
#endif

esp_err_t esp_openthread_host_rcp_uart_init(const esp_openthread_platform_config_t *config)
{
    esp_err_t ret = ESP_OK;
    // Install UART driver for interrupt-driven reads and writes.
    char uart_path[16];
    s_uart_port = config->host_config.host_uart_config.port;
    ESP_RETURN_ON_ERROR(esp_openthread_uart_init_port(&config->host_config.host_uart_config), OT_PLAT_LOG_TAG,
                        "esp_openthread_uart_init_port failed");

    uart_vfs_dev_port_set_rx_line_endings(s_uart_port, ESP_LINE_ENDINGS_LF);
    uart_vfs_dev_port_set_tx_line_endings(s_uart_port, ESP_LINE_ENDINGS_LF);
    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", s_uart_port);
    s_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);
    ESP_RETURN_ON_FALSE(s_uart_fd >= 0, ESP_FAIL, OT_PLAT_LOG_TAG, "open uart_path failed");
    ret = esp_openthread_platform_workflow_register(&esp_openthread_uart_update, &esp_openthread_uart_process,
                                                    uart_workflow);

    return ret;
}

void esp_openthread_uart_deinit()
{
    if (s_uart_fd != -1) {
        close(s_uart_fd);
        s_uart_fd = -1;
    }
    uart_driver_delete(s_uart_port);
    esp_openthread_platform_workflow_unregister(uart_workflow);
}

void esp_openthread_uart_update(esp_openthread_mainloop_context_t *mainloop)
{
    FD_SET(s_uart_fd, &mainloop->read_fds);
    if (s_uart_fd > mainloop->max_fd) {
        mainloop->max_fd = s_uart_fd;
    }
}

esp_err_t esp_openthread_uart_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    int rval = read(s_uart_fd, s_uart_buffer, sizeof(s_uart_buffer));

    if (rval > 0) {
#if (CONFIG_OPENTHREAD_CLI || (CONFIG_OPENTHREAD_RADIO && CONFIG_OPENTHREAD_RCP_UART))
        otPlatUartReceived(s_uart_buffer, (uint16_t)rval);
#endif
    } else if (rval < 0) {
        if (errno != EAGAIN) {
            ESP_LOGW(OT_PLAT_LOG_TAG, "read uart failed: %d", errno);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}
