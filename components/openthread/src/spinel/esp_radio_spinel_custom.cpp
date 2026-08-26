/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_radio_spinel_custom.hpp"

#include <cstring>
#include <errno.h>
#include <unistd.h>

#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "esp_radio_spinel.h"
#include "esp_vfs_eventfd.h"
#include "common/code_utils.hpp"
#include "hdlc.hpp"
#include "openthread/error.h"
#include "openthread/platform/time.h"
#include "platform/exit_code.h"

namespace esp {
namespace radio_spinel {

namespace {
static int s_transport_spinel_notify_fd = -1;
}

extern "C" void esp_openthread_transport_notify_host_rx(void)
{
    if (s_transport_spinel_notify_fd >= 0) {
        uint64_t one = 1;
        ssize_t w = write(s_transport_spinel_notify_fd, &one, sizeof(one));
        (void)w;
    }
}

CustomSpinelInterface::CustomSpinelInterface(void)
    : m_receiver_frame_callback(nullptr)
    , m_receiver_frame_context(nullptr)
    , m_receive_frame_buffer(nullptr)
    , m_rx_buffer(nullptr)
    , m_notify_fd(-1)
    , mRcpFailureHandler(nullptr)
{
    memset(&m_transport_cfg, 0, sizeof(m_transport_cfg));
    memset(&mInterfaceMetrics, 0, sizeof(mInterfaceMetrics));
}

CustomSpinelInterface::~CustomSpinelInterface(void)
{
    Disable();
    Deinit();
}

otError CustomSpinelInterface::Init(ReceiveFrameCallback aCallback, void *aCallbackContext, RxFrameBuffer &aFrameBuffer)
{
    m_receiver_frame_callback = aCallback;
    m_receiver_frame_context = aCallbackContext;
    m_receive_frame_buffer    = &aFrameBuffer;
    m_hdlc_decoder.Init(aFrameBuffer, HandleHdlcFrame, this);
    return OT_ERROR_NONE;
}

void CustomSpinelInterface::Deinit(void)
{
    m_receiver_frame_callback = nullptr;
    m_receiver_frame_context  = nullptr;
    m_receive_frame_buffer    = nullptr;
}

esp_err_t CustomSpinelInterface::Enable(const esp_openthread_transport_config_t &transport_config)
{
    ESP_RETURN_ON_FALSE((m_notify_fd < 0) && (m_rx_buffer == NULL), ESP_ERR_INVALID_STATE, ESP_SPINEL_LOG_TAG, "custom transport spinel already enabled");
    ESP_RETURN_ON_FALSE((transport_config.transport_tx != nullptr) && (transport_config.transport_rx != nullptr),
                        ESP_ERR_INVALID_ARG, ESP_SPINEL_LOG_TAG, "transport callbacks invalid");

    m_rx_buffer = static_cast<uint8_t *>(heap_caps_malloc(ESP_OPENTHREAD_TRANSPORT_BUFFER_SIZE, MALLOC_CAP_8BIT));
    if (m_rx_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    m_notify_fd = eventfd(0, EFD_SUPPORT_ISR);
    if (m_notify_fd < 0) {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "custom transport spinel eventfd failed");
        heap_caps_free(m_rx_buffer);
        m_rx_buffer = NULL;
        return ESP_FAIL;
    }

    s_transport_spinel_notify_fd = m_notify_fd;

    memcpy(&m_transport_cfg, &transport_config, sizeof(m_transport_cfg));

    return ESP_OK;
}

esp_err_t CustomSpinelInterface::Disable(void)
{
    if (m_rx_buffer) {
        heap_caps_free(m_rx_buffer);
    }
    m_rx_buffer = NULL;

    if (s_transport_spinel_notify_fd == m_notify_fd) {
        s_transport_spinel_notify_fd = -1;
    }

    if (m_notify_fd >= 0) {
        close(m_notify_fd);
        m_notify_fd = -1;
    }

    memset(&m_transport_cfg, 0, sizeof(m_transport_cfg));
    return ESP_OK;
}

otError CustomSpinelInterface::SendFrame(const uint8_t *frame, uint16_t length)
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

void CustomSpinelInterface::Process(const void *aMainloopContext)
{
    if (m_notify_fd < 0) {
        return;
    }
    auto *ctx = (esp_radio_spinel_mainloop_context_t *)aMainloopContext;

    if (m_notify_fd >= 0 && FD_ISSET(m_notify_fd, &ctx->read_fds)) {
        uint64_t v;
        ssize_t r = read(m_notify_fd, &v, sizeof(v));
        (void)r;
    }

    TryReadAndDecode();
}

int CustomSpinelInterface::TryReadAndDecode(void)
{
    if (!m_transport_cfg.transport_rx) {
        return 0;
    }

    for (;;) {
        size_t n = 0;
        esp_err_t err = m_transport_cfg.transport_rx(m_rx_buffer, CONFIG_OPENTHREAD_TRANSPORT_BUFFER_SIZE, &n);
        if ((err != ESP_OK) || (n == 0) || (n > CONFIG_OPENTHREAD_TRANSPORT_BUFFER_SIZE)) {
            break;
        }
        m_hdlc_decoder.Decode(m_rx_buffer, static_cast<uint16_t>(n));
    }
    return 0;
}

otError CustomSpinelInterface::Write(const uint8_t *aFrame, uint16_t length)
{
    if (!m_transport_cfg.transport_tx) {
        return OT_ERROR_FAILED;
    }
    esp_err_t err = m_transport_cfg.transport_tx(aFrame, length);
    return (err == ESP_OK) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

otError CustomSpinelInterface::WaitForFrame(uint64_t timeout_us)
{
    if (m_notify_fd < 0) {
        return OT_ERROR_FAILED;
    }

    otError          error = OT_ERROR_NONE;
    struct timeval   timeout;
    fd_set           read_fds;
    fd_set           error_fds;
    int              rval;
    uint64_t const   deadline = otPlatTimeGet() + timeout_us;

    TryReadAndDecode();

    while (true) {
        uint64_t now = otPlatTimeGet();
        if (now >= deadline) {
            ExitNow(error = OT_ERROR_RESPONSE_TIMEOUT);
        }
        uint64_t remain = deadline - now;

        FD_ZERO(&read_fds);
        FD_ZERO(&error_fds);
        FD_SET(m_notify_fd, &read_fds);
        FD_SET(m_notify_fd, &error_fds);

        timeout.tv_sec  = static_cast<time_t>(remain / US_PER_S);
        timeout.tv_usec = static_cast<suseconds_t>(remain % US_PER_S);

        rval = select(m_notify_fd + 1, &read_fds, NULL, &error_fds, &timeout);

        if (rval > 0) {
            if (FD_ISSET(m_notify_fd, &read_fds)) {
                uint64_t v;
                ssize_t r = read(m_notify_fd, &v, sizeof(v));
                (void)r;
                TryReadAndDecode();
                ExitNow(error = OT_ERROR_NONE);
            }
            if (FD_ISSET(m_notify_fd, &error_fds)) {
                ExitNow(error = OT_ERROR_FAILED);
            }
        } else if (rval == 0) {
            ExitNow(error = OT_ERROR_RESPONSE_TIMEOUT);
        } else if (errno != EINTR) {
            ExitNow(error = OT_ERROR_FAILED);
        }
    }

exit:
    return error;
}

void CustomSpinelInterface::HandleHdlcFrame(void *context, otError error)
{
    static_cast<CustomSpinelInterface *>(context)->HandleHdlcFrame(error);
}

void CustomSpinelInterface::HandleHdlcFrame(otError error)
{
    if (error == OT_ERROR_NONE) {
        if (m_receiver_frame_callback != nullptr) {
            m_receiver_frame_callback(m_receiver_frame_context);
        }
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "dropping radio frame: %s", otThreadErrorToString(error));
        if (m_receive_frame_buffer != nullptr) {
            m_receive_frame_buffer->DiscardFrame();
        }
    }
}

void CustomSpinelInterface::UpdateFdSet(void *aMainloopContext)
{
    if (m_notify_fd < 0) {
        return;
    }
    auto *ctx = (esp_radio_spinel_mainloop_context_t *)aMainloopContext;
    FD_SET(m_notify_fd, &ctx->read_fds);
    if (m_notify_fd > ctx->max_fd) {
        ctx->max_fd = m_notify_fd;
    }
}

uint32_t CustomSpinelInterface::GetBusSpeed(void) const
{
    return m_transport_cfg.bus_speed;
}

otError CustomSpinelInterface::HardwareReset(void)
{
    if (mRcpFailureHandler) {
        mRcpFailureHandler();
    }
    return OT_ERROR_NONE;
}

} // namespace radio_spinel
} // namespace esp
