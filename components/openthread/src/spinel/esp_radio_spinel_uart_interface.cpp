/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_radio_spinel_uart_interface.hpp"

#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "esp_radio_spinel_uart_transport.hpp"

namespace esp {
namespace radio_spinel {

UartSpinelInterface::UartSpinelInterface(void)
    : m_receiver_frame_callback(nullptr)
    , m_receiver_frame_context(nullptr)
    , m_receive_frame_buffer(nullptr)
    , m_wait_fd(-1)
    , m_iid(-1)
    , mRcpFailureHandler(nullptr)
{
    memset(&mInterfaceMetrics, 0, sizeof(mInterfaceMetrics));
}

UartSpinelInterface::~UartSpinelInterface(void)
{
    Disable();
    Deinit();
}

otError UartSpinelInterface::Init(ReceiveFrameCallback aCallback, void *aCallbackContext, RxFrameBuffer &aFrameBuffer)
{
    m_receiver_frame_callback = aCallback;
    m_receiver_frame_context = aCallbackContext;
    m_receive_frame_buffer = &aFrameBuffer;
    if (m_iid < 0 ||
        esp_radio_spinel_uart_transport_bind_rx(m_iid, aCallback, aCallbackContext, m_receive_frame_buffer) != ESP_OK) {
        return OT_ERROR_FAILED;
    }
    return OT_ERROR_NONE;
}

void UartSpinelInterface::Deinit(void)
{
    esp_radio_spinel_uart_transport_unbind_rx(m_iid);
    m_receiver_frame_callback = nullptr;
    m_receiver_frame_context = nullptr;
    m_receive_frame_buffer = nullptr;
}

esp_err_t UartSpinelInterface::Enable(const esp_radio_spinel_uart_config_t &radio_uart_config,
                                      const esp_radio_spinel_uart_transport_hooks_t *hooks)
{
    if (m_wait_fd != -1) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(esp_radio_spinel_uart_transport_open(&radio_uart_config, hooks, &m_iid, &m_wait_fd),
                        ESP_SPINEL_LOG_TAG, "spinel UART transport open failed");
    if (m_receive_frame_buffer != nullptr) {
        esp_err_t err = esp_radio_spinel_uart_transport_bind_rx(
            m_iid, m_receiver_frame_callback, m_receiver_frame_context, m_receive_frame_buffer);
        if (err != ESP_OK) {
            ESP_LOGE(ESP_SPINEL_LOG_TAG, "spinel UART transport bind failed");
            (void)esp_radio_spinel_uart_transport_close(m_iid);
            m_wait_fd = -1;
            m_iid = -1;
            return err;
        }
    }
    ESP_LOGI(ESP_SPINEL_LOG_TAG, "spinel UART interface initialization completed, iid=%d", m_iid);
    return ESP_OK;
}

#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
esp_err_t UartSpinelInterface::Enable(const esp_openthread_uart_config_t &radio_uart_config,
                                      const esp_radio_spinel_uart_transport_hooks_t *hooks)
{
    esp_radio_spinel_uart_config_t spinel_uart_config = {
        .port = radio_uart_config.port,
        .uart_config = radio_uart_config.uart_config,
        .rx_pin = radio_uart_config.rx_pin,
        .tx_pin = radio_uart_config.tx_pin,
    };
    return Enable(spinel_uart_config, hooks);
}
#endif

esp_err_t UartSpinelInterface::Disable(void)
{
    if (m_wait_fd == -1) {
        return ESP_OK;
    }
    esp_err_t err = esp_radio_spinel_uart_transport_close(m_iid);
    m_wait_fd = -1;
    m_iid = -1;
    return err;
}

otError UartSpinelInterface::SendFrame(const uint8_t *frame, uint16_t length)
{
    ssize_t rval = esp_radio_spinel_uart_transport_write(m_iid, frame, length);
    if (rval == static_cast<ssize_t>(length)) {
        ESP_LOGD(ESP_SPINEL_LOG_TAG, "sent radio frame");
        return OT_ERROR_NONE;
    }
    ESP_LOGE(ESP_SPINEL_LOG_TAG, "send radio frame failed");
    ESP_ERROR_CHECK(esp_radio_spinel_uart_transport_recover(m_iid));
    return OT_ERROR_FAILED;
}

int UartSpinelInterface::TryReadSpinel(void)
{
    int rval = esp_radio_spinel_uart_transport_read(m_iid);
    if (rval < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        ESP_ERROR_CHECK(esp_radio_spinel_uart_transport_recover(m_iid));
    }
    return rval;
}

void UartSpinelInterface::Process(const void *aMainloopContext)
{
    if (m_wait_fd < 0) {
        return;
    }
    if (FD_ISSET(m_wait_fd, &((esp_radio_spinel_mainloop_context_t *)aMainloopContext)->read_fds)) {
        ESP_LOGD(ESP_SPINEL_LOG_TAG, "radio spinel read event");
        TryReadSpinel();
    }
}

otError UartSpinelInterface::WaitForFrame(uint64_t timeout_us)
{
    otError error = OT_ERROR_NONE;
    struct timeval timeout;
    fd_set read_fds;
    fd_set error_fds;
    int rval;

    if (m_wait_fd < 0) {
        return OT_ERROR_FAILED;
    }

    FD_ZERO(&read_fds);
    FD_ZERO(&error_fds);
    FD_SET(m_wait_fd, &read_fds);
    FD_SET(m_wait_fd, &error_fds);

    timeout.tv_sec = static_cast<time_t>(timeout_us / US_PER_S);
    timeout.tv_usec = static_cast<suseconds_t>(timeout_us % US_PER_S);

    rval = select(m_wait_fd + 1, &read_fds, NULL, &error_fds, &timeout);

    if (rval > 0) {
        if (FD_ISSET(m_wait_fd, &read_fds)) {
            TryReadSpinel();
        } else if (FD_ISSET(m_wait_fd, &error_fds)) {
            ESP_ERROR_CHECK(esp_radio_spinel_uart_transport_recover(m_iid));
            ExitNow(error = OT_ERROR_FAILED);
        }
    } else if (rval == 0) {
        ExitNow(error = OT_ERROR_RESPONSE_TIMEOUT);
    } else {
        ESP_ERROR_CHECK(esp_radio_spinel_uart_transport_recover(m_iid));
        ExitNow(error = OT_ERROR_FAILED);
    }

exit:
    return error;
}

otError UartSpinelInterface::HardwareReset(void)
{
    if (mRcpFailureHandler) {
        ESP_ERROR_CHECK(esp_radio_spinel_uart_transport_recover(m_iid));
        mRcpFailureHandler();
    }
    return OT_ERROR_NONE;
}

void UartSpinelInterface::UpdateFdSet(void *aMainloopContext)
{
    if (m_wait_fd < 0) {
        return;
    }
    auto *ctx = static_cast<esp_radio_spinel_mainloop_context_t *>(aMainloopContext);
    FD_SET(m_wait_fd, &ctx->read_fds);
    if (m_wait_fd > ctx->max_fd) {
        ctx->max_fd = m_wait_fd;
    }
}

uint32_t UartSpinelInterface::GetBusSpeed(void) const
{
    return esp_radio_spinel_uart_transport_get_bus_speed(m_iid);
}

} // namespace radio_spinel
} // namespace esp
