/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_spi_spinel_interface.hpp"

#include "error.h"
#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "esp_rom_sys.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"
#include <stdint.h>
#include "common/logging.hpp"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "hal/gpio_types.h"
#include "ncp/ncp_spi.hpp"

using ot::Spinel::SpiFrame;
using ot::Spinel::SpinelInterface;

namespace esp {
namespace openthread {

SpiSpinelInterface::SpiSpinelInterface(void)
    : m_event_fd(-1)
    , m_receiver_frame_callback(nullptr)
    , m_receiver_frame_context(nullptr)
    , m_receive_frame_buffer(nullptr)
    , mRcpFailureHandler(nullptr)
{
}

SpiSpinelInterface::~SpiSpinelInterface(void)
{
    Deinit();
}

otError SpiSpinelInterface::Init(ReceiveFrameCallback aCallback, void *aCallbackContext, RxFrameBuffer &aFrameBuffer)
{
    otError error = OT_ERROR_NONE;

    m_receiver_frame_callback = aCallback;
    m_receiver_frame_context = aCallbackContext;
    m_receive_frame_buffer = &aFrameBuffer;

    return error;
}

void SpiSpinelInterface::Deinit(void)
{
    m_receiver_frame_callback = nullptr;
    m_receiver_frame_context = nullptr;
    m_receive_frame_buffer = nullptr;
}

esp_err_t SpiSpinelInterface::Enable(const esp_openthread_spi_host_config_t &spi_config)
{
    ESP_RETURN_ON_FALSE(m_event_fd < 0, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG, "event fd was initialized");
    m_spi_config = spi_config;
    ESP_RETURN_ON_ERROR(spi_bus_initialize(spi_config.host_device, &spi_config.spi_interface, SPI_DMA_CH_AUTO),
                        OT_PLAT_LOG_TAG, "fail to initialize spi bus");
    ESP_RETURN_ON_ERROR(spi_bus_add_device(spi_config.host_device, &spi_config.spi_device, &m_device), OT_PLAT_LOG_TAG,
                        "fail to add spi bus device");

    gpio_config_t io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << spi_config.intr_pin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), OT_PLAT_LOG_TAG, "fail to config spi gpio");
    gpio_install_isr_service(0); // The gpio isr service may has been installed.
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(spi_config.intr_pin, GpioIntrHandler, this), OT_PLAT_LOG_TAG,
                        "fail to add gpio isr handler");
    m_has_pending_device_frame = false;
    m_event_fd = eventfd(0, EFD_SUPPORT_ISR);
    m_pending_data_len = 0;

    ESP_RETURN_ON_FALSE(m_event_fd >= 0, ESP_FAIL, OT_PLAT_LOG_TAG, "fail to get event fd");

    ESP_LOGI(OT_PLAT_LOG_TAG, "spinel SPI interface initialization completed");

    return ESP_OK;
}

esp_err_t SpiSpinelInterface::AfterRadioInit(void)
{
    return ConductSPITransaction(true, 0, 0);
}


esp_err_t SpiSpinelInterface::Disable(void)
{
    if (m_event_fd >= 0) {
        close(m_event_fd);
        m_event_fd = -1;

        ESP_RETURN_ON_ERROR(gpio_isr_handler_remove(m_spi_config.intr_pin), OT_PLAT_LOG_TAG,
                            "fail to remove gpio isr handler");
        ESP_RETURN_ON_ERROR(spi_bus_remove_device(m_device), OT_PLAT_LOG_TAG, "fail to remove spi bus device");
        ESP_RETURN_ON_ERROR(spi_bus_free(m_spi_config.host_device), OT_PLAT_LOG_TAG, "fail to free spi bus");
        gpio_uninstall_isr_service();
    }
    return ESP_OK;
}

otError SpiSpinelInterface::SendFrame(const uint8_t *frame, uint16_t length)
{
    ESP_RETURN_ON_FALSE(frame, OT_ERROR_INVALID_ARGS, OT_PLAT_LOG_TAG, "empty frame");
    ESP_RETURN_ON_FALSE(length <= SpinelInterface::kMaxFrameSize, OT_ERROR_NO_BUFS, OT_PLAT_LOG_TAG,
                        "send frame is too long");

    memcpy(&m_tx_buffer[kSPIFrameHeaderSize], frame, length);
    uint16_t rx_data_size =
        length < kSmallPacketSize ? kSmallPacketSize : length; // We'll use tx_size to receive small packets piggybacked
    if (ConductSPITransaction(false, length, rx_data_size) == ESP_OK) {
        return OT_ERROR_NONE;
    } else {
        return OT_ERROR_FAILED;
    }
}

esp_err_t SpiSpinelInterface::ConductSPITransaction(bool reset, uint16_t tx_data_size, uint16_t rx_data_size)
{
    ESP_RETURN_ON_FALSE(tx_data_size <= kSPIFrameSize && rx_data_size <= kSPIFrameSize, ESP_ERR_INVALID_ARG,
                        OT_PLAT_LOG_TAG, "invalid arguments");

    SpiFrame tx_frame(m_tx_buffer);

    tx_frame.SetHeaderFlagByte(reset);
    tx_frame.SetHeaderDataLen(tx_data_size);
    tx_frame.SetHeaderAcceptLen(rx_data_size);

    uint8_t *rx_buffer;
    otError err = m_receive_frame_buffer->SetSkipLength(kSPIFrameHeaderSize);

    ESP_RETURN_ON_FALSE(err == OT_ERROR_NONE, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "buffer space is insufficient");

    rx_buffer = m_receive_frame_buffer->GetFrame() - kSPIFrameHeaderSize;
    if (m_receive_frame_buffer->GetFrameMaxLength() < rx_data_size) {
        rx_data_size = m_receive_frame_buffer->GetFrameMaxLength();
    }
    uint16_t data_size = tx_data_size > rx_data_size ? tx_data_size : rx_data_size;
    data_size += kSPIFrameHeaderSize;

    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = data_size * CHAR_BIT;
    transaction.rxlength = (rx_data_size + kSPIFrameHeaderSize) * CHAR_BIT;
    transaction.tx_buffer = m_tx_buffer;
    transaction.rx_buffer = rx_buffer;

    ESP_RETURN_ON_ERROR(spi_device_polling_transmit(m_device, &transaction), OT_PLAT_LOG_TAG, "SPI transaction failed");
    SpiFrame rx_frame(rx_buffer);

    if (!rx_frame.IsValid() || rx_frame.GetHeaderAcceptLen() > kSPIFrameSize ||
        rx_frame.GetHeaderDataLen() > kSPIFrameSize) {
        vTaskDelay(pdMS_TO_TICKS(15));
        ESP_RETURN_ON_ERROR(spi_device_polling_transmit(m_device, &transaction), OT_PLAT_LOG_TAG,
                            "fail to retry SPI invalid transaction");
    }

    if (rx_frame.IsResetFlagSet()) {
        ESP_LOGW(OT_PLAT_LOG_TAG, "RCP Reset");
        m_receive_frame_buffer->DiscardFrame();
        return ESP_OK;
    }
    if (rx_frame.GetHeaderDataLen() == 0 && rx_frame.GetHeaderAcceptLen() == 0) {
        vTaskDelay(pdMS_TO_TICKS(15));
        ESP_RETURN_ON_ERROR(spi_device_polling_transmit(m_device, &transaction), OT_PLAT_LOG_TAG,
                            "fail to retry SPI empty transaction");
    }

    if (rx_frame.GetHeaderDataLen() > 0 && rx_frame.GetHeaderDataLen() < tx_frame.GetHeaderAcceptLen()) {
        if (gpio_get_level(m_spi_config.intr_pin) == 1) {
            m_pending_data_len = 0;
        }
        if (m_receive_frame_buffer->SetLength(rx_frame.GetHeaderDataLen()) != OT_ERROR_NONE) {
            ESP_LOGW(OT_PLAT_LOG_TAG, "insufficient buffer space to hold a frame of length %d...",
                     rx_frame.GetHeaderDataLen());
            m_receive_frame_buffer->DiscardFrame();
            return ESP_ERR_NO_MEM;
        }
        m_receiver_frame_callback(m_receiver_frame_context);
    } else {
        m_pending_data_len = 0;
        m_receive_frame_buffer->DiscardFrame();
    }
    m_pending_data_len = 0;

    return ESP_OK;
}

void SpiSpinelInterface::GpioIntrHandler(void *arg)
{
    SpiSpinelInterface *instance = static_cast<SpiSpinelInterface *>(arg);
    instance->m_pending_data_len = SpinelInterface::kMaxFrameSize;
    uint64_t event = SpinelInterface::kMaxFrameSize;
    write(instance->m_event_fd, &event, sizeof(event));
}

void SpiSpinelInterface::UpdateFdSet(void *aMainloopContext)
{
    if (m_pending_data_len > 0) {
        ((esp_openthread_mainloop_context_t *)aMainloopContext)->timeout.tv_sec = 0;
        ((esp_openthread_mainloop_context_t *)aMainloopContext)->timeout.tv_usec = 0;
    }
    FD_SET(m_event_fd, &((esp_openthread_mainloop_context_t *)aMainloopContext)->read_fds);
    FD_SET(m_event_fd, &((esp_openthread_mainloop_context_t *)aMainloopContext)->error_fds);
    if (m_event_fd > ((esp_openthread_mainloop_context_t *)aMainloopContext)->max_fd) {
        ((esp_openthread_mainloop_context_t *)aMainloopContext)->max_fd = m_event_fd;
    }
}

void SpiSpinelInterface::Process(const void *aMainloopContext)
{
    if (FD_ISSET(m_event_fd, &((esp_openthread_mainloop_context_t *)aMainloopContext)->error_fds)) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "SPI INTR GPIO error event");
        return;
    }
    if (FD_ISSET(m_event_fd, &((esp_openthread_mainloop_context_t *)aMainloopContext)->read_fds)) {
        uint64_t event;
        read(m_event_fd, &event, sizeof(event));
        m_pending_data_len = SpinelInterface::kMaxFrameSize;

        if (ConductSPITransaction(false, 0, m_pending_data_len) != ESP_OK) {
            ESP_LOGW(OT_PLAT_LOG_TAG, "fail to process SPI transaction");
        }
    }
    return;
}

otError SpiSpinelInterface::WaitForFrame(uint64_t timeout_us)
{
    fd_set read_fds, error_fds;
    struct timeval timeout;
    uint64_t event = 0;
    if (m_pending_data_len == 0) {
        FD_ZERO(&read_fds);
        FD_ZERO(&error_fds);
        FD_SET(m_event_fd, &read_fds);
        FD_SET(m_event_fd, &error_fds);
        timeout.tv_sec = timeout_us / US_PER_S;
        timeout.tv_usec = timeout_us % US_PER_S;

        int ret = select(m_event_fd + 1, &read_fds, NULL, &error_fds, &timeout);
        if (ret <= 0 || !FD_ISSET(m_event_fd, &read_fds)) {
            if (FD_ISSET(m_event_fd, &error_fds)) {
                ESP_LOGW(OT_PLAT_LOG_TAG, "FD error!");
            }
            ESP_LOGW(OT_PLAT_LOG_TAG, "SPI transaction timeout for %llu us, result %d", timeout_us, ret);
            return OT_ERROR_RESPONSE_TIMEOUT;
        }
        read(m_event_fd, &event, sizeof(event));
    }

    ESP_RETURN_ON_FALSE(ConductSPITransaction(false, 0, SpinelInterface::kMaxFrameSize) == ESP_OK, OT_ERROR_FAILED,
                        OT_PLAT_LOG_TAG, "fail to complete SPI transaction during wait for frame");
    return OT_ERROR_NONE;
}

otError SpiSpinelInterface::HardwareReset(void)
{
    if (mRcpFailureHandler) {
        mRcpFailureHandler();
        ConductSPITransaction(true, 0, 0); // clear
    }
    return OT_ERROR_NONE;
}

uint32_t SpiSpinelInterface::GetBusSpeed(void) const
{
    return m_spi_config.spi_device.clock_speed_hz;
}

} // namespace openthread
} // namespace esp
