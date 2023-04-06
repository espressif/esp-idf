/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_uart_spinel_interface.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/unistd.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_types.h"
#include "esp_openthread_uart.h"
#include "esp_vfs_dev.h"
#include "core/common/code_utils.hpp"
#include "core/common/logging.hpp"
#include "driver/uart.h"
#include "lib/platform/exit_code.h"
#include "openthread/platform/time.h"

namespace esp {
namespace openthread {

UartSpinelInterface::UartSpinelInterface(ot::Spinel::SpinelInterface::ReceiveFrameCallback callback,
                                         void *callback_context,
                                         ot::Spinel::SpinelInterface::RxFrameBuffer &frame_buffer)
    : m_receiver_frame_callback(callback)
    , m_receiver_frame_context(callback_context)
    , m_receive_frame_buffer(frame_buffer)
    , m_hdlc_decoder(frame_buffer, HandleHdlcFrame, this)
    , m_uart_fd(-1)
    , mRcpFailureHandler(nullptr)
{
}

UartSpinelInterface::~UartSpinelInterface(void)
{
}

esp_err_t UartSpinelInterface::Init(const esp_openthread_uart_config_t &radio_uart_config)
{
    esp_err_t error = ESP_OK;
    m_uart_rx_buffer = static_cast<uint8_t *>(heap_caps_malloc(kMaxFrameSize, MALLOC_CAP_8BIT));
    if (m_uart_rx_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    error = InitUart(radio_uart_config);
    ESP_LOGI(OT_PLAT_LOG_TAG, "spinel UART interface initialization completed");
    return error;
}

esp_err_t UartSpinelInterface::Deinit(void)
{
    if (m_uart_rx_buffer) {
        heap_caps_free(m_uart_rx_buffer);
    }
    m_uart_rx_buffer = NULL;

    return DeinitUart();
}

otError UartSpinelInterface::SendFrame(const uint8_t *frame, uint16_t length)
{
    otError error = OT_ERROR_NONE;
    ot::Hdlc::FrameBuffer<kMaxFrameSize> encoder_buffer;
    ot::Hdlc::Encoder hdlc_encoder(encoder_buffer);

    SuccessOrExit(error = hdlc_encoder.BeginFrame());
    SuccessOrExit(error = hdlc_encoder.Encode(frame, length));
    SuccessOrExit(error = hdlc_encoder.EndFrame());

    SuccessOrExit(error = Write(encoder_buffer.GetFrame(), encoder_buffer.GetLength()));

exit:
    if (error != OT_ERROR_NONE) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "send radio frame failed");
    } else {
        ESP_LOGD(OT_PLAT_LOG_TAG, "sent radio frame");
    }

    return error;
}

void UartSpinelInterface::Process(const esp_openthread_mainloop_context_t &mainloop)
{
    if (FD_ISSET(m_uart_fd, &mainloop.read_fds)) {
        ESP_LOGD(OT_PLAT_LOG_TAG, "radio uart read event");
        TryReadAndDecode();
    }
}

void UartSpinelInterface::Update(esp_openthread_mainloop_context_t &mainloop)
{
    // Register only READ events for radio UART and always wait
    // for a radio WRITE to complete.
    FD_SET(m_uart_fd, &mainloop.read_fds);
    if (m_uart_fd > mainloop.max_fd) {
        mainloop.max_fd = m_uart_fd;
    }
}

int UartSpinelInterface::TryReadAndDecode(void)
{
    uint8_t buffer[UART_FIFO_LEN];
    ssize_t rval;

    do {
        rval = read(m_uart_fd, buffer, sizeof(buffer));
        if (rval > 0) {
            m_hdlc_decoder.Decode(buffer, static_cast<uint16_t>(rval));
        }
    } while (rval > 0);

    if ((rval < 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        ESP_ERROR_CHECK(TryRecoverUart());
    }

    return rval;
}

otError UartSpinelInterface::WaitForWritable(void)
{
    otError error = OT_ERROR_NONE;
    struct timeval timeout = {kMaxWaitTime / MS_PER_S, (kMaxWaitTime % MS_PER_S) * US_PER_MS};
    uint64_t now = otPlatTimeGet();
    uint64_t end = now + kMaxWaitTime * US_PER_MS;
    fd_set write_fds;
    fd_set error_fds;
    int rval;

    while (true) {
        FD_ZERO(&write_fds);
        FD_ZERO(&error_fds);
        FD_SET(m_uart_fd, &write_fds);
        FD_SET(m_uart_fd, &error_fds);

        rval = select(m_uart_fd + 1, NULL, &write_fds, &error_fds, &timeout);

        if (rval > 0) {
            if (FD_ISSET(m_uart_fd, &write_fds)) {
                ExitNow();
            } else if (FD_ISSET(m_uart_fd, &error_fds)) {
                ExitNow(error = OT_ERROR_FAILED);
            }
        } else if ((rval < 0) && (errno != EINTR)) {
            ESP_ERROR_CHECK(TryRecoverUart());
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

otError UartSpinelInterface::Write(const uint8_t *aFrame, uint16_t length)
{
    otError error = OT_ERROR_NONE;

    while (length) {
        ssize_t rval;

        rval = write(m_uart_fd, aFrame, length);

        if (rval > 0) {
            assert(rval <= length);
            length -= static_cast<uint16_t>(rval);
            aFrame += static_cast<uint16_t>(rval);
            continue;
        } else if (rval < 0) {
            ESP_ERROR_CHECK(TryRecoverUart());
            ExitNow(error = OT_ERROR_FAILED);
        }

        SuccessOrExit(error = WaitForWritable());
    }

exit:
    return error;
}

otError UartSpinelInterface::WaitForFrame(uint64_t timeout_us)
{
    otError error = OT_ERROR_NONE;
    struct timeval timeout;
    fd_set read_fds;
    fd_set error_fds;
    int rval;

    FD_ZERO(&read_fds);
    FD_ZERO(&error_fds);
    FD_SET(m_uart_fd, &read_fds);
    FD_SET(m_uart_fd, &error_fds);

    timeout.tv_sec = static_cast<time_t>(timeout_us / US_PER_S);
    timeout.tv_usec = static_cast<suseconds_t>(timeout_us % US_PER_S);

    rval = select(m_uart_fd + 1, &read_fds, NULL, &error_fds, &timeout);

    if (rval > 0) {
        if (FD_ISSET(m_uart_fd, &read_fds)) {
            TryReadAndDecode();
        } else if (FD_ISSET(m_uart_fd, &error_fds)) {
            ESP_ERROR_CHECK(TryRecoverUart());
            ExitNow(error = OT_ERROR_FAILED);
        }
    } else if (rval == 0) {
        ExitNow(error = OT_ERROR_RESPONSE_TIMEOUT);
    } else {
        ESP_ERROR_CHECK(TryRecoverUart());
        ExitNow(error = OT_ERROR_FAILED);
    }

exit:
    return error;
}

void UartSpinelInterface::HandleHdlcFrame(void *context, otError error)
{
    static_cast<UartSpinelInterface *>(context)->HandleHdlcFrame(error);
}

void UartSpinelInterface::HandleHdlcFrame(otError error)
{
    if (error == OT_ERROR_NONE) {
        ESP_LOGD(OT_PLAT_LOG_TAG, "received hdlc radio frame");
        m_receiver_frame_callback(m_receiver_frame_context);
    } else {
        ESP_LOGE(OT_PLAT_LOG_TAG, "dropping radio frame: %s", otThreadErrorToString(error));
        m_receive_frame_buffer.DiscardFrame();
    }
}

esp_err_t UartSpinelInterface::InitUart(const esp_openthread_uart_config_t &radio_uart_config)
{
    char uart_path[16];

    m_uart_config = radio_uart_config;
    ESP_RETURN_ON_ERROR(esp_openthread_uart_init_port(&radio_uart_config), OT_PLAT_LOG_TAG,
                        "esp_openthread_uart_init_port failed");
    // We have a driver now installed so set up the read/write functions to use driver also.
    esp_vfs_dev_uart_port_set_tx_line_endings(m_uart_config.port, ESP_LINE_ENDINGS_LF);
    esp_vfs_dev_uart_port_set_rx_line_endings(m_uart_config.port, ESP_LINE_ENDINGS_LF);

    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", radio_uart_config.port);
    m_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);

    return m_uart_fd >= 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t UartSpinelInterface::DeinitUart(void)
{
    if (m_uart_fd != -1) {
        close(m_uart_fd);
        m_uart_fd = -1;
        return uart_driver_delete(m_uart_config.port);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t UartSpinelInterface::TryRecoverUart(void)
{
    ESP_RETURN_ON_ERROR(DeinitUart(), OT_PLAT_LOG_TAG, "DeInitUart failed");
    ESP_RETURN_ON_ERROR(InitUart(m_uart_config), OT_PLAT_LOG_TAG, "InitUart failed");
    return ESP_OK;
}

void UartSpinelInterface::OnRcpReset(void)
{
    if (mRcpFailureHandler) {
        mRcpFailureHandler();
        TryRecoverUart();
    }
}

} // namespace openthread
} // namespace esp
