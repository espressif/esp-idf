/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log.h"
#include "esp_radio_spinel.h"
#include "esp_radio_spinel_uart_transport.hpp"
#include "lib/spinel/spinel_interface.hpp"
#include "openthread/error.h"
#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
#include "esp_openthread_types.h"
#endif

namespace esp {
namespace radio_spinel {

/**
 * This class defines a UART spinel interface to the Radio Co-processor (RCP).
 *
 * HDLC and UART I/O are owned by the transport layer. This class only waits on
 * the transport wait fd. Complete spinel frames are delivered by the transport
 * into the RxFrameBuffer bound at Init, via the receive callback.
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
     * @retval OT_ERROR_FAILED   Failed to call the UART driver to send the frame.
     *
     */
    otError SendFrame(const uint8_t *aFrame, uint16_t aLength);

    /**
     * Waits for receiving part or all of spinel frame within specified interval.
     *
     * @param[in]  aTimeoutUs  The timeout value in microseconds.
     *
     * @retval OT_ERROR_NONE             Part or all of spinel frame is received.
     * @retval OT_ERROR_RESPONSE_TIMEOUT No spinel frame is received within @p aTimeoutUs.
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
    void RegisterRcpFailureHandler(esp_radio_spinel_rcp_failure_handler handler) { mRcpFailureHandler = handler; }

    /**
     * This method is called when RCP is reset to recreate the connection with it.
     * Intentionally empty.
     *
     */
    otError ResetConnection(void) { return OT_ERROR_NONE; }

    /**
     * Enable the spinel UART transport.
     *
     * Multipan: allocates a free spinel IID. Use GetIid() after Enable succeeds.
     *
     * @param[in] radio_uart_config  UART configuration.
     * @param[in] hooks              Optional UART init/deinit hooks. May be nullptr.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_STATE if already enabled
     *      - ESP_ERR_NO_MEM if a multipan host slot or rx queue cannot be allocated
     *      - ESP_FAIL on failure
     */
    esp_err_t Enable(const esp_radio_spinel_uart_config_t &radio_uart_config,
                     const esp_radio_spinel_uart_transport_hooks_t *hooks = nullptr);
#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
    /**
     * Enable the spinel UART transport using OpenThread UART config.
     *
     * @param[in] radio_uart_config  OpenThread UART configuration.
     * @param[in] hooks              Optional UART init/deinit hooks. May be nullptr.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_STATE if already enabled
     *      - ESP_ERR_NO_MEM if a multipan host slot or rx queue cannot be allocated
     *      - ESP_FAIL on failure
     */
    esp_err_t Enable(const esp_openthread_uart_config_t &radio_uart_config,
                     const esp_radio_spinel_uart_transport_hooks_t *hooks = nullptr);
#endif

    /**
     * Disable the spinel UART transport.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_FAIL on failure
     */
    esp_err_t Disable(void);

    /**
     * Returns the allocated spinel IID.
     *
     * Valid after Enable() succeeds. Returns -1 if the interface is not enabled.
     *
     */
    int8_t GetIid(void) const { return m_iid; }

private:
    int TryReadSpinel(void);

    ReceiveFrameCallback m_receiver_frame_callback;
    void *m_receiver_frame_context;
    RxFrameBuffer *m_receive_frame_buffer;
    int m_wait_fd;
    int8_t m_iid;
    otRcpInterfaceMetrics mInterfaceMetrics;
    esp_radio_spinel_rcp_failure_handler mRcpFailureHandler;

    // Non-copyable, intentionally not implemented.
    UartSpinelInterface(const UartSpinelInterface &);
    UartSpinelInterface &operator=(const UartSpinelInterface &);
};

} // namespace radio_spinel
} // namespace esp
