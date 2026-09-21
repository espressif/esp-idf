/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include "esp_err.h"
#include "esp_radio_spinel.h"
#include "lib/spinel/spinel_interface.hpp"

/**
 * Optional UART open/close hooks. Used by the non-multipan transport.
 * The multipan transport ignores them (UART is owned by esp_radio_spinel_multipan_init()).
 */
typedef struct {
    esp_radio_spinel_uart_init_handler uart_init;     /* May be NULL. */
    esp_radio_spinel_uart_deinit_handler uart_deinit; /* May be NULL. */
} esp_radio_spinel_uart_transport_hooks_t;

/**
 * Open the UART spinel transport.
 *
 * Non-multipan: opens the real UART and returns that fd as wait_fd. @p iid is set to 0.
 * Multipan: allocates a free IID slot and returns an eventfd as wait_fd.
 *
 * @param[in]  config   UART config. Non-multipan uses it to open the port.
 *                      Multipan may ignore it if esp_radio_spinel_multipan_init() already ran.
 * @param[in]  hooks    Optional UART init/deinit hooks. May be NULL.
 * @param[out] iid      Allocated spinel interface id.
 * @param[out] wait_fd  Fd to select for a readable spinel frame.
 *
 * @return ESP_OK on success, an error code otherwise.
 */
esp_err_t esp_radio_spinel_uart_transport_open(const esp_radio_spinel_uart_config_t *config,
                                               const esp_radio_spinel_uart_transport_hooks_t *hooks, int8_t *iid,
                                               int *wait_fd);

/**
 * Close the transport opened for @p iid.
 */
esp_err_t esp_radio_spinel_uart_transport_close(int8_t iid);

/**
 * Bind the RadioSpinel receive buffer and callback for @p iid.
 *
 * Called from UartSpinelInterface::Init (and Enable if Init already ran).
 * Non-multipan attaches the buffer to the HDLC decoder.
 * Multipan stores them on the host slot and invokes the callback from read().
 */
esp_err_t esp_radio_spinel_uart_transport_bind_rx(int8_t iid,
                                                  ot::Spinel::SpinelInterface::ReceiveFrameCallback callback,
                                                  void *context,
                                                  ot::Spinel::SpinelInterface::RxFrameBuffer *frame_buffer);

/**
 * Undo bind_rx for @p iid.
 *
 * Non-multipan detaches the HDLC decoder from the RxFrameBuffer.
 * Multipan clears the host slot's buffer and callback.
 * Safe to call if nothing is bound.
 */
void esp_radio_spinel_uart_transport_unbind_rx(int8_t iid);

/**
 * Drain the transport for @p iid.
 *
 * Non-multipan: reads UART and HDLC-decodes into the bound RxFrameBuffer.
 * Complete frames are delivered via the callback registered with bind_rx.
 * Multipan: dequeues pending spinel frames, copies each into the bound
 * RxFrameBuffer, then invokes the same callback.
 *
 * @return 0 on success (including no complete frame), -1 on error.
 */
int esp_radio_spinel_uart_transport_read(int8_t iid);

/**
 * Write one complete spinel frame (HDLC-encoded internally).
 *
 * @return @p len on success, -1 on error.
 */
ssize_t esp_radio_spinel_uart_transport_write(int8_t iid, const void *buf, size_t len);

/**
 * Re-install the UART after a bus error. Client wait fds stay valid.
 */
esp_err_t esp_radio_spinel_uart_transport_recover(int8_t iid);

/**
 * UART baud rate in bits/second.
 */
uint32_t esp_radio_spinel_uart_transport_get_bus_speed(int8_t iid);
