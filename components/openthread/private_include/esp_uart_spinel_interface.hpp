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

#pragma once

#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "hal/uart_types.h"
#include "lib/spinel/spinel_interface.hpp"

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
    UartSpinelInterface(ot::Spinel::SpinelInterface::ReceiveFrameCallback callback,
                        void                                             *callback_context,
                        ot::Spinel::SpinelInterface::RxFrameBuffer       &frame_buffer);

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
     * @note    This is blocking call, i.e., if the socket is not writable, this method waits for it to become writable for
     * up to `kMaxWaitTime` interval.
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
    void Process(const esp_openthread_mainloop_context_t &mainloop);

    /**
     * This methods updates the mainloop context.
     *
     * @param[inout] mainloop   The mainloop context.
     *
     */
    void Update(esp_openthread_mainloop_context_t &mainloop);

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
    void        HandleHdlcFrame(otError error);

    ot::Spinel::SpinelInterface::ReceiveFrameCallback m_receiver_frame_callback;
    void                                             *m_receiver_frame_context;
    ot::Spinel::SpinelInterface::RxFrameBuffer       &m_receive_frame_buffer;

    ot::Hdlc::Decoder m_hdlc_decoder;
    uint8_t          *m_uart_rx_buffer;

    esp_openthread_uart_config_t    m_uart_config;
    int                             m_uart_fd;

    // Non-copyable, intentionally not implemented.
    UartSpinelInterface(const UartSpinelInterface &);
    UartSpinelInterface &operator=(const UartSpinelInterface &);
};

} // namespace openthread
} // namespace esp
