/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_radio_spinel_uart_transport.hpp"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "sdkconfig.h"
#include "esp_vfs_dev.h"
#include "hdlc.hpp"
#include "lib/spinel/spinel_interface.hpp"
#include "openthread/error.h"
#include "openthread/platform/time.h"

static const char *TAG = ESP_SPINEL_LOG_TAG;

static constexpr uint16_t kMaxFrameSize = ot::Spinel::SpinelInterface::kMaxFrameSize;
static constexpr uint32_t kMaxWaitTimeMs = 2000;

static esp_radio_spinel_uart_config_t s_uart_config;
static esp_radio_spinel_uart_transport_hooks_t s_hooks;
static int s_uart_fd = -1;
static int *s_wait_fd_out = nullptr;

static ot::Spinel::FrameBuffer<kMaxFrameSize> s_encoder_buffer;
static ot::Hdlc::Decoder s_hdlc_decoder;
static ot::Spinel::SpinelInterface::RxFrameBuffer *s_rx_buffer = nullptr;
static ot::Spinel::SpinelInterface::ReceiveFrameCallback s_rx_callback = nullptr;
static void *s_rx_context = nullptr;

static void handle_hdlc_frame(void *context, otError error)
{
    (void)context;
    if (error == OT_ERROR_NONE) {
        if (s_rx_callback != nullptr) {
            ESP_LOGD(TAG, "received spinel frame");
            s_rx_callback(s_rx_context);
        } else if (s_rx_buffer != nullptr) {
            s_rx_buffer->DiscardFrame();
        }
        return;
    }
    ESP_LOGE(TAG, "dropping radio frame: %s", otThreadErrorToString(error));
    if (s_rx_buffer != nullptr) {
        s_rx_buffer->DiscardFrame();
    }
}

static void decoder_attach(void)
{
    if (s_rx_buffer == nullptr) {
        return;
    }
    s_hdlc_decoder.Init(*s_rx_buffer, handle_hdlc_frame, nullptr);
}

static esp_err_t uart_init_port(const esp_radio_spinel_uart_config_t *config)
{
    char uart_path[16];
    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", config->port);
    bool is_uart_registered = (access(uart_path, F_OK) == 0);
    if (!is_uart_registered) {
        uart_vfs_dev_register();
    }

    ESP_RETURN_ON_ERROR(uart_param_config(config->port, &config->uart_config), TAG, "uart_param_config failed");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE), TAG,
        "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(config->port, CONFIG_OPENTHREAD_SPINEL_UART_DRIVER_BUFFER_SIZE, 0, 0, NULL, 0),
                        TAG, "uart_driver_install failed");
    uart_vfs_dev_use_driver(config->port);
    return ESP_OK;
}

static esp_err_t uart_open(void)
{
    if (s_hooks.uart_init) {
        return s_hooks.uart_init(&s_uart_config, &s_uart_fd);
    }

    char uart_path[16];
    ESP_RETURN_ON_ERROR(uart_init_port(&s_uart_config), TAG, "uart_init_port failed");
    uart_vfs_dev_port_set_tx_line_endings(s_uart_config.port, ESP_LINE_ENDINGS_LF);
    uart_vfs_dev_port_set_rx_line_endings(s_uart_config.port, ESP_LINE_ENDINGS_LF);

    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", s_uart_config.port);
    s_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);
    if (s_uart_fd < 0) {
        (void)uart_driver_delete(s_uart_config.port);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t uart_close(void)
{
    if (s_hooks.uart_deinit) {
        return s_hooks.uart_deinit(&s_uart_config, &s_uart_fd);
    }
    if (s_uart_fd == -1) {
        return ESP_ERR_INVALID_STATE;
    }
    close(s_uart_fd);
    s_uart_fd = -1;
    return uart_driver_delete(s_uart_config.port);
}

static void publish_wait_fd(void)
{
    if (s_wait_fd_out) {
        *s_wait_fd_out = s_uart_fd;
    }
}

static otError wait_for_writable(void)
{
    otError error = OT_ERROR_NONE;
    struct timeval timeout = {static_cast<time_t>(kMaxWaitTimeMs / MS_PER_S),
                              static_cast<suseconds_t>((kMaxWaitTimeMs % MS_PER_S) * US_PER_MS)};
    uint64_t now = otPlatTimeGet();
    uint64_t end = now + static_cast<uint64_t>(kMaxWaitTimeMs) * US_PER_MS;

    while (true) {
        fd_set write_fds;
        fd_set error_fds;
        FD_ZERO(&write_fds);
        FD_ZERO(&error_fds);
        FD_SET(s_uart_fd, &write_fds);
        FD_SET(s_uart_fd, &error_fds);

        int rval = select(s_uart_fd + 1, NULL, &write_fds, &error_fds, &timeout);
        if (rval > 0) {
            if (FD_ISSET(s_uart_fd, &write_fds)) {
                ExitNow();
            }
            if (FD_ISSET(s_uart_fd, &error_fds)) {
                ExitNow(error = OT_ERROR_FAILED);
            }
        } else if ((rval < 0) && (errno != EINTR)) {
            ExitNow(error = OT_ERROR_FAILED);
        }

        now = otPlatTimeGet();
        if (end > now) {
            uint64_t remain = end - now;
            timeout.tv_sec = static_cast<time_t>(remain / 1000000);
            timeout.tv_usec = static_cast<suseconds_t>(remain % 1000000);
        } else {
            break;
        }
    }

    error = OT_ERROR_FAILED;

exit:
    return error;
}

static otError uart_write_bytes(const uint8_t *frame, uint16_t length)
{
    otError error = OT_ERROR_NONE;

    while (length) {
        ssize_t rval = write(s_uart_fd, frame, length);
        if (rval > 0) {
            assert(rval <= length);
            length -= static_cast<uint16_t>(rval);
            frame += static_cast<uint16_t>(rval);
            continue;
        }
        if (rval < 0) {
            ExitNow(error = OT_ERROR_FAILED);
        }
        SuccessOrExit(error = wait_for_writable());
    }

exit:
    return error;
}

esp_err_t esp_radio_spinel_uart_transport_open(const esp_radio_spinel_uart_config_t *config,
                                               const esp_radio_spinel_uart_transport_hooks_t *hooks, int8_t *iid,
                                               int *wait_fd)
{
    ESP_RETURN_ON_FALSE(config != nullptr && wait_fd != nullptr && iid != nullptr, ESP_ERR_INVALID_ARG, TAG,
                        "invalid transport open args");
    ESP_RETURN_ON_FALSE(s_uart_fd == -1, ESP_ERR_INVALID_STATE, TAG, "uart transport already open");

    *wait_fd = -1;
    s_uart_config = *config;
    memset(&s_hooks, 0, sizeof(s_hooks));
    if (hooks) {
        s_hooks = *hooks;
    }
    s_wait_fd_out = wait_fd;

    ESP_RETURN_ON_ERROR(uart_open(), TAG, "uart open failed");
    *iid = 0;
    publish_wait_fd();
    ESP_LOGI(TAG, "spinel UART transport initialization completed");
    return ESP_OK;
}

esp_err_t esp_radio_spinel_uart_transport_close(int8_t iid)
{
    (void)iid;
    esp_err_t err = ESP_OK;
    if (s_uart_fd != -1 || s_hooks.uart_deinit) {
        err = uart_close();
    }
    publish_wait_fd();
    s_wait_fd_out = nullptr;
    return err;
}

int esp_radio_spinel_uart_transport_read(int8_t iid)
{
    (void)iid;
    if (s_uart_fd < 0 || s_rx_buffer == nullptr) {
        return 0;
    }

    uint8_t buffer[UART_HW_FIFO_LEN(s_uart_config.port)];
    ssize_t rval = 0;
    do {
        rval = read(s_uart_fd, buffer, sizeof(buffer));
        if (rval > 0) {
            s_hdlc_decoder.Decode(buffer, static_cast<uint16_t>(rval));
        }
    } while (rval > 0);

    if ((rval < 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        return -1;
    }
    return 0;
}

ssize_t esp_radio_spinel_uart_transport_write(int8_t iid, const void *buf, size_t len)
{
    (void)iid;
    ESP_RETURN_ON_FALSE(buf != nullptr, (errno = EINVAL, -1), TAG, "invalid write buffer");
    ESP_RETURN_ON_FALSE(s_uart_fd >= 0, (errno = EBADF, -1), TAG, "uart transport is not open");
    if (len == 0) {
        return 0;
    }
    if (len > kMaxFrameSize) {
        return -1;
    }

    otError error = OT_ERROR_NONE;
    s_encoder_buffer.Clear();
    ot::Hdlc::Encoder hdlc_encoder(s_encoder_buffer);
    SuccessOrExit(error = hdlc_encoder.BeginFrame());
    SuccessOrExit(error = hdlc_encoder.Encode(static_cast<const uint8_t *>(buf), static_cast<uint16_t>(len)));
    SuccessOrExit(error = hdlc_encoder.EndFrame());
    SuccessOrExit(error = uart_write_bytes(s_encoder_buffer.GetFrame(), s_encoder_buffer.GetLength()));
    return static_cast<ssize_t>(len);

exit:
    return -1;
}

esp_err_t esp_radio_spinel_uart_transport_recover(int8_t iid)
{
    (void)iid;
    s_hdlc_decoder.Reset();
    ESP_RETURN_ON_ERROR(uart_close(), TAG, "uart close failed during recover");
    ESP_RETURN_ON_ERROR(uart_open(), TAG, "uart open failed during recover");
    decoder_attach();
    publish_wait_fd();
    return ESP_OK;
}

uint32_t esp_radio_spinel_uart_transport_get_bus_speed(int8_t iid)
{
    (void)iid;
    return s_uart_config.uart_config.baud_rate;
}

esp_err_t esp_radio_spinel_uart_transport_bind_rx(int8_t iid,
                                                  ot::Spinel::SpinelInterface::ReceiveFrameCallback callback,
                                                  void *context,
                                                  ot::Spinel::SpinelInterface::RxFrameBuffer *frame_buffer)
{
    (void)iid;
    ESP_RETURN_ON_FALSE(frame_buffer != nullptr, ESP_ERR_INVALID_ARG, TAG, "invalid rx buffer");
    s_rx_callback = callback;
    s_rx_context = context;
    s_rx_buffer = frame_buffer;
    s_hdlc_decoder.Reset();
    decoder_attach();
    return ESP_OK;
}

void esp_radio_spinel_uart_transport_unbind_rx(int8_t iid)
{
    (void)iid;
    s_rx_callback = nullptr;
    s_rx_context = nullptr;
    s_rx_buffer = nullptr;
    s_hdlc_decoder.Reset();
}
