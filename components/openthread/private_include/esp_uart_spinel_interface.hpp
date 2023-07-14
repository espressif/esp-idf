/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "hal/uart_types.h"
#include "lib/spinel/spinel_interface.hpp"
#include "lib/hdlc/hdlc.hpp"
#include "openthread/error.h"

namespace esp {
namespace openthread {

/**
 * This class defines an UART interface to the Radio Co-processor (RCP).
 *
 */
class UartSpinelInterface {
public:
    /**
     * @brief   This constructor of object.
     *
     * @param[in] callback         Callback on frame received
     * @param[in] callback_context  Callback context
     * @param[in] frame_buffer      A reference to a `RxFrameBuffer` object.
     *
     */
    UartSpinelInterface(ot::Spinel::SpinelInterface::ReceiveFrameCallback callback, void *callback_context,
                        ot::Spinel::SpinelInterface::RxFrameBuffer &frame_buffer);

    /**
     * @brief   This destructor of the object.
     *
     */
    ~UartSpinelInterface(void);

    /**
     * @brief   This method initializes the HDLC interface.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_NO_MEM if allocation has failed
     *      - ESP_ERROR on failure
     */
    esp_err_t Init(const esp_openthread_uart_config_t &radio_uart_config);

    /**
     * @brief  This method deinitializes the HDLC interface.
     *
     */
    esp_err_t Deinit(void);

    /**
     * @brief   This method encodes and sends a spinel frame to Radio Co-processor (RCP) over the socket.
     *
     * @note    This is blocking call, i.e., if the socket is not writable, this method waits for it to become writable
     * for up to `kMaxWaitTime` interval.
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
     * This method performs uart processing to the RCP.
     *
     * @param[in]  mainloop     The mainloop context
     *
     */
    void Process(const void *mainloop);

    /**
     * This methods updates the mainloop context.
     *
     * @param[inout] mainloop   The mainloop context.
     *
     */
    void Update(void *mainloop);

    /**
     * This methods registers the callback for RCP failure.
     *
     * @param[in] handler   The RCP failure handler.
     *
     */
    void RegisterRcpFailureHandler(esp_openthread_rcp_failure_handler handler) { mRcpFailureHandler = handler; }

    /**
     * This method is called when RCP failure detected and resets internal states of the interface.
     *
     */
    otError HardwareReset(void);

    /**
     * This method is called when RCP is reset to recreate the connection with it.
     * Intentionally empty.
     *
     */
    otError ResetConnection(void) { return OT_ERROR_NONE; }

private:
    enum {
        /**
         * Maximum spinel frame size.
         *
         */
        kMaxFrameSize = ot::Spinel::SpinelInterface::kMaxFrameSize,

        /**
         * Maximum wait time in Milliseconds for socket to become writable (see `SendFrame`).
         *
         */
        kMaxWaitTime = 2000,
    };

    esp_err_t InitUart(const esp_openthread_uart_config_t &radio_uart_config);

    esp_err_t DeinitUart(void);

    int TryReadAndDecode(void);

    otError WaitForWritable(void);

    otError Write(const uint8_t *frame, uint16_t length);

    esp_err_t TryRecoverUart(void);

    static void HandleHdlcFrame(void *context, otError error);
    void HandleHdlcFrame(otError error);

    ot::Spinel::SpinelInterface::ReceiveFrameCallback m_receiver_frame_callback;
    void *m_receiver_frame_context;
    ot::Spinel::SpinelInterface::RxFrameBuffer &m_receive_frame_buffer;

    ot::Hdlc::Decoder m_hdlc_decoder;
    uint8_t *m_uart_rx_buffer;

    esp_openthread_uart_config_t m_uart_config;
    int m_uart_fd;

    // Non-copyable, intentionally not implemented.
    UartSpinelInterface(const UartSpinelInterface &);
    UartSpinelInterface &operator=(const UartSpinelInterface &);

    esp_openthread_rcp_failure_handler mRcpFailureHandler;
};

} // namespace openthread
} // namespace esp
