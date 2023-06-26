/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "driver/spi_master.h"
#include "lib/spinel/spinel_interface.hpp"

namespace esp {
namespace openthread {

class SpiSpinelInterface {
public:
    /**
     * @brief   This constructor of object.
     *
     * @param[in] callback         Callback on frame received
     * @param[in] callback_context  Callback context
     * @param[in] frame_buffer      A reference to a `RxFrameBuffer` object.
     *
     */
    SpiSpinelInterface(ot::Spinel::SpinelInterface::ReceiveFrameCallback callback, void *callback_context,
                       ot::Spinel::SpinelInterface::RxFrameBuffer &frame_buffer);

    /**
     * @brief   This destructor of the object.
     *
     */
    ~SpiSpinelInterface(void);

    /**
     * @brief   This method initializes the spinel interface.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_STATE if already initialized
     *      - ESP_ERR_NO_MEM if allocation has failed
     *      - ESP_FAIL on failure
     */
    esp_err_t Init(const esp_openthread_spi_host_config_t &spi_config);

    /**
     * @brief  This method deinitializes the HDLC interface.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_FAIL on failure
     */
    esp_err_t Deinit(void);

    /**
     * @brief   This method encodes and sends a spinel frame to Radio Co-processor (RCP) over the socket.
     *
     * @param[in] frame     A pointer to buffer containing the spinel frame to send.
     * @param[in] length    The length (number of bytes) in the frame.
     *
     * @return
     *      -OT_ERROR_NONE     Successfully encoded and sent the spinel frame.
     *      -OT_ERROR_NO_BUFS  Insufficient buffer space available to encode the frame.
     *      -OT_ERROR_FAILED   Failed to send due to socket not becoming writable within `kMaxWaitTime`.
     *
     */
    otError SendFrame(const uint8_t *frame, uint16_t length);

    /**
     * This method waits for receiving part or all of spinel frame within specified timeout.
     *
     * @param[in]  timeout_us  The timeout value in microseconds.
     *
     * @return
     *      -OT_ERROR_NONE             Part or all of spinel frame is received.
     *      -OT_ERROR_RESPONSE_TIMEOUT No spinel frame is received within @p timeout_us.
     *
     */
    otError WaitForFrame(uint64_t timeout_us);

    /**
     * This method performs spi processing to the RCP.
     *
     * @param[in]  mainloop     The mainloop context
     *
     */
    void Process(const esp_openthread_mainloop_context_t &mainloop);

    /**
     * This methods updates the mainloop context.
     *
     * @param[inout] mainloop   The mainloop context.
     *
     */
    void Update(esp_openthread_mainloop_context_t &mainloop);

    /**
     * This methods registers the callback for RCP failure.
     *
     * @param[in] handler   The RCP failure handler.
     *
     */
    void RegisterRcpFailureHandler(esp_openthread_rcp_failure_handler handler) { mRcpFailureHandler = handler; }

    /**
     * This method is called when RCP is reset to recreate the connection with it.
     * Intentionally empty.
     *
     */
    otError ResetConnection(void) { return OT_ERROR_NONE; }

    /**
     * This method is called when RCP failure detected and resets internal states of the interface.
     *
     */
    void OnRcpReset(void);

private:
    static constexpr uint8_t kSPIFrameHeaderSize = 5;
    static constexpr uint16_t kSPIFrameSize = ot::Spinel::SpinelInterface::kMaxFrameSize + kSPIFrameHeaderSize;
    static constexpr uint8_t kSmallPacketSize = 32;
    static constexpr uint16_t kSPIDataEvent = 1;

    static void GpioIntrHandler(void *arg);
    esp_err_t ConductSPITransaction(bool reset, uint16_t tx_data_size, uint16_t rx_data_size);

    esp_openthread_spi_host_config_t m_spi_config;
    uint8_t m_tx_buffer[kSPIFrameSize];
    int m_event_fd;
    volatile uint16_t m_pending_data_len;

    ot::Spinel::SpinelInterface::ReceiveFrameCallback m_receiver_frame_callback;
    void *m_receiver_frame_context;
    ot::Spinel::SpinelInterface::RxFrameBuffer &m_receive_frame_buffer;
    bool m_has_pending_device_frame;

    spi_device_handle_t m_device;

    esp_openthread_rcp_failure_handler mRcpFailureHandler;
};

} // namespace openthread
} // namespace esp
