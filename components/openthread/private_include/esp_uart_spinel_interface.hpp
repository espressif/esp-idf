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
class UartSpinelInterface : public ot::Spinel::SpinelInterface {
public:
    /**
     * @brief   This constructor of object.
     */
    UartSpinelInterface(void);

    /**
     * @brief   This destructor of the object.
     *
     */
    ~UartSpinelInterface(void);

    /**
     * Initializes the interface to the Radio Co-processor (RCP).
     *
     * @note This method should be called before reading and sending spinel frames to the interface.
     *
     * @param[in] aCallback         Callback on frame received
     * @param[in] aCallbackContext  Callback context
     * @param[in] aFrameBuffer      A reference to a `RxFrameBuffer` object.
     *
     * @retval OT_ERROR_NONE       The interface is initialized successfully
     * @retval OT_ERROR_ALREADY    The interface is already initialized.
     * @retval OT_ERROR_FAILED     Failed to initialize the interface.
     *
     */
    otError Init(ReceiveFrameCallback aCallback, void *aCallbackContext, RxFrameBuffer &aFrameBuffer);

    /**
     * Deinitializes the interface to the RCP.
     *
     */
    void Deinit(void);

    /**
     * Encodes and sends a spinel frame to Radio Co-processor (RCP) over the socket.
     *
     * @param[in] aFrame     A pointer to buffer containing the spinel frame to send.
     * @param[in] aLength    The length (number of bytes) in the frame.
     *
     * @retval OT_ERROR_NONE     Successfully encoded and sent the spinel frame.
     * @retval OT_ERROR_BUSY     Failed due to another operation is on going.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to encode the frame.
     * @retval OT_ERROR_FAILED   Failed to call the SPI driver to send the frame.
     *
     */
    otError SendFrame(const uint8_t *aFrame, uint16_t aLength);

    /**
     * Waits for receiving part or all of spinel frame within specified interval.
     *
     * @param[in]  aTimeout  The timeout value in microseconds.
     *
     * @retval OT_ERROR_NONE             Part or all of spinel frame is received.
     * @retval OT_ERROR_RESPONSE_TIMEOUT No spinel frame is received within @p aTimeout.
     *
     */
    otError WaitForFrame(uint64_t aTimeoutUs);

    /**
     * Updates the file descriptor sets with file descriptors used by the radio driver.
     *
     * @param[in,out]   aMainloopContext  A pointer to the mainloop context.
     *
     */
    void UpdateFdSet(void *aMainloopContext);

    /**
     * Performs radio driver processing.
     *
     * @param[in]   aMainloopContext  A pointer to the mainloop context.
     *
     */
    void Process(const void *aMainloopContext);

    /**
     * Returns the bus speed between the host and the radio.
     *
     * @returns   Bus speed in bits/second.
     *
     */
    uint32_t GetBusSpeed(void) const;

    /**
     * Hardware resets the RCP.
     *
     * @retval OT_ERROR_NONE            Successfully reset the RCP.
     * @retval OT_ERROR_NOT_IMPLEMENT   The hardware reset is not implemented.
     *
     */
    otError HardwareReset(void);

    /**
     * Returns the RCP interface metrics.
     *
     * @returns The RCP interface metrics.
     *
     */
    const otRcpInterfaceMetrics *GetRcpInterfaceMetrics(void) const { return &mInterfaceMetrics; }

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
     * @brief   This method enable the HDLC interface.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_NO_MEM if allocation has failed
     *      - ESP_ERROR on failure
     */
    esp_err_t Enable(const esp_openthread_uart_config_t &radio_uart_config);

    /**
     * @brief  This method disable the HDLC interface.
     *
     */
    esp_err_t Disable(void);

private:

    enum {
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

    ReceiveFrameCallback m_receiver_frame_callback;
    void *m_receiver_frame_context;
    RxFrameBuffer *m_receive_frame_buffer;

    ot::Hdlc::Decoder m_hdlc_decoder;
    uint8_t *m_uart_rx_buffer;

    esp_openthread_uart_config_t m_uart_config;
    int m_uart_fd;

    otRcpInterfaceMetrics mInterfaceMetrics;

    // Non-copyable, intentionally not implemented.
    UartSpinelInterface(const UartSpinelInterface &);
    UartSpinelInterface &operator=(const UartSpinelInterface &);

    esp_openthread_rcp_failure_handler mRcpFailureHandler;

    ot::Spinel::FrameBuffer<kMaxFrameSize> encoder_buffer;
};

} // namespace openthread
} // namespace esp
