/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_radio_spinel_uart_interface.hpp"
#include <errno.h>
#include <fcntl.h>
#include <sys/unistd.h>
#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "openthread/platform/time.h"
#include "hdlc.hpp"
#include "common/code_utils.hpp"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
#include "esp_openthread_types.h"
#endif

namespace esp {
namespace radio_spinel {

static esp_err_t RadioSpinelUartInitPort(const esp_radio_spinel_uart_config_t *config)
{
    char uart_path[16];
    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", config->port);
    bool is_uart_registered = (access(uart_path, F_OK) == 0);
    if (!is_uart_registered) {
        // Register UART VFS devices before opening /dev/uart/x if not already present.
        uart_vfs_dev_register();
    }

    ESP_RETURN_ON_ERROR(uart_param_config(config->port, &config->uart_config), ESP_SPINEL_LOG_TAG,
                        "uart_param_config failed");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
        ESP_SPINEL_LOG_TAG, "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(config->port, CONFIG_OPENTHREAD_SPINEL_UART_DRIVER_BUFFER_SIZE, 0, 0, NULL, 0),
                        ESP_SPINEL_LOG_TAG, "uart_driver_install failed");
    uart_vfs_dev_use_driver(config->port);
    return ESP_OK;
}

UartSpinelInterface::UartSpinelInterface(void)
    : m_receiver_frame_callback(nullptr)
    , m_receiver_frame_context(nullptr)
    , m_receive_frame_buffer(nullptr)
    , m_uart_rx_buffer(nullptr)
    , m_uart_fd(-1)
    , mRcpFailureHandler(nullptr)
    , mUartInitHandler(nullptr)
    , mUartDeinitHandler(nullptr)
{
}

UartSpinelInterface::~UartSpinelInterface(void)
{
    // Ensure UART resources are released even if caller forgets Disable().
    Disable();
    Deinit();
}

otError UartSpinelInterface::Init(ReceiveFrameCallback aCallback, void *aCallbackContext, RxFrameBuffer &aFrameBuffer)
{
    otError error = OT_ERROR_NONE;

    m_receiver_frame_callback = aCallback;
    m_receiver_frame_context = aCallbackContext;
    m_receive_frame_buffer = &aFrameBuffer;
    m_hdlc_decoder.Init(aFrameBuffer, HandleHdlcFrame, this);

    return error;
}

void UartSpinelInterface::Deinit(void)
{
    m_receiver_frame_callback = nullptr;
    m_receiver_frame_context = nullptr;
    m_receive_frame_buffer = nullptr;
}

esp_err_t UartSpinelInterface::Enable(const esp_radio_spinel_uart_config_t &radio_uart_config)
{
    esp_err_t error = ESP_OK;

    if (m_uart_fd != -1 || m_uart_rx_buffer != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    m_uart_rx_buffer = static_cast<uint8_t *>(heap_caps_malloc(kMaxFrameSize, MALLOC_CAP_8BIT));
    if (m_uart_rx_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    error = InitUart(radio_uart_config);
    if (error == ESP_OK) {
        ESP_LOGI(ESP_SPINEL_LOG_TAG, "spinel UART interface initialization completed");
    } else {
        heap_caps_free(m_uart_rx_buffer);
        m_uart_rx_buffer = NULL;
    }
    return error;
}

#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
// NOTE: This overload bridges config types; keep field-wise copy and avoid storing references/pointers to the input to prevent potential lifetime-related memory risks.
esp_err_t UartSpinelInterface::Enable(const esp_openthread_uart_config_t &radio_uart_config)
{
    esp_radio_spinel_uart_config_t spinel_uart_config = {
        .port = radio_uart_config.port,
        .uart_config = radio_uart_config.uart_config,
        .rx_pin = radio_uart_config.rx_pin,
        .tx_pin = radio_uart_config.tx_pin,
    };
    return Enable(spinel_uart_config);
}
#endif

esp_err_t UartSpinelInterface::Disable(void)
{
    if (m_uart_rx_buffer) {
        heap_caps_free(m_uart_rx_buffer);
    }
    m_uart_rx_buffer = NULL;

    if (m_uart_fd == -1) {
        return ESP_OK;
    }

    return DeinitUart();
}

otError UartSpinelInterface::SendFrame(const uint8_t *frame, uint16_t length)
{
    otError error = OT_ERROR_NONE;
    encoder_buffer.Clear();
    ot::Hdlc::Encoder hdlc_encoder(encoder_buffer);

    SuccessOrExit(error = hdlc_encoder.BeginFrame());
    SuccessOrExit(error = hdlc_encoder.Encode(frame, length));
    SuccessOrExit(error = hdlc_encoder.EndFrame());

    SuccessOrExit(error = Write(encoder_buffer.GetFrame(), encoder_buffer.GetLength()));

exit:
    if (error != OT_ERROR_NONE) {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "send radio frame failed");
    } else {
        ESP_LOGD(ESP_SPINEL_LOG_TAG, "sent radio frame");
    }

    return error;
}

void UartSpinelInterface::Process(const void *aMainloopContext)
{
    if (FD_ISSET(m_uart_fd, &((esp_radio_spinel_mainloop_context_t *)aMainloopContext)->read_fds)) {
        ESP_LOGD(ESP_SPINEL_LOG_TAG, "radio uart read event");
        TryReadAndDecode();
    }
}

int UartSpinelInterface::TryReadAndDecode(void)
{
    uint8_t buffer[UART_HW_FIFO_LEN(m_uart_config.port)];
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
        ESP_LOGD(ESP_SPINEL_LOG_TAG, "received hdlc radio frame");
        m_receiver_frame_callback(m_receiver_frame_context);
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "dropping radio frame: %s", otThreadErrorToString(error));
        m_receive_frame_buffer->DiscardFrame();
    }
}

esp_err_t UartSpinelInterface::InitUart(const esp_radio_spinel_uart_config_t &radio_uart_config)
{
    if (mUartInitHandler) {
        m_uart_config = radio_uart_config;
        return mUartInitHandler(&m_uart_config, &m_uart_fd);
    } else {
        char uart_path[16];
        esp_err_t err = ESP_OK;

        m_uart_config = radio_uart_config;
        ESP_RETURN_ON_ERROR(RadioSpinelUartInitPort(&radio_uart_config), ESP_SPINEL_LOG_TAG,
                            "RadioSpinelUartInitPort failed");
        // We have a driver now installed so set up the read/write functions to use driver also.
        uart_vfs_dev_port_set_tx_line_endings(m_uart_config.port, ESP_LINE_ENDINGS_LF);
        uart_vfs_dev_port_set_rx_line_endings(m_uart_config.port, ESP_LINE_ENDINGS_LF);

        snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", radio_uart_config.port);
        m_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);

        if (m_uart_fd < 0) {
            err = uart_driver_delete(m_uart_config.port);
            ESP_RETURN_ON_ERROR(err, ESP_SPINEL_LOG_TAG, "uart_driver_delete failed after open");
            return ESP_FAIL;
        }
        return ESP_OK;
    }
}

esp_err_t UartSpinelInterface::DeinitUart(void)
{
    if (mUartDeinitHandler) {
        return mUartDeinitHandler(&m_uart_config, &m_uart_fd);
    } else {
        if (m_uart_fd != -1) {
            close(m_uart_fd);
            m_uart_fd = -1;
            return uart_driver_delete(m_uart_config.port);
        } else {
            return ESP_ERR_INVALID_STATE;
        }
    }
}

esp_err_t UartSpinelInterface::TryRecoverUart(void)
{
    ESP_RETURN_ON_ERROR(DeinitUart(), ESP_SPINEL_LOG_TAG, "DeInitUart failed");
    ESP_RETURN_ON_ERROR(InitUart(m_uart_config), ESP_SPINEL_LOG_TAG, "InitUart failed");
    return ESP_OK;
}

otError UartSpinelInterface::HardwareReset(void)
{
    if (mRcpFailureHandler) {
        TryRecoverUart();
        mRcpFailureHandler();
    }
    return OT_ERROR_NONE;
}

void UartSpinelInterface::UpdateFdSet(void *aMainloopContext)
{
    // Register only READ events for radio UART and always wait
    // for a radio WRITE to complete.
    FD_SET(m_uart_fd, &((esp_radio_spinel_mainloop_context_t *)aMainloopContext)->read_fds);
    if (m_uart_fd > ((esp_radio_spinel_mainloop_context_t *)aMainloopContext)->max_fd) {
        ((esp_radio_spinel_mainloop_context_t *)aMainloopContext)->max_fd = m_uart_fd;
    }
}

uint32_t UartSpinelInterface::GetBusSpeed(void) const
{
    return m_uart_config.uart_config.baud_rate;
}
} // namespace radio_spinel
} // namespace esp
