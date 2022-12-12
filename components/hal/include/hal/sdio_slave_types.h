/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

/// Mask of interrupts sending to the host.
typedef enum {
    SDIO_SLAVE_HOSTINT_BIT0 = BIT(0),   ///< General purpose interrupt bit 0.
    SDIO_SLAVE_HOSTINT_BIT1 = BIT(1),
    SDIO_SLAVE_HOSTINT_BIT2 = BIT(2),
    SDIO_SLAVE_HOSTINT_BIT3 = BIT(3),
    SDIO_SLAVE_HOSTINT_BIT4 = BIT(4),
    SDIO_SLAVE_HOSTINT_BIT5 = BIT(5),
    SDIO_SLAVE_HOSTINT_BIT6 = BIT(6),
    SDIO_SLAVE_HOSTINT_BIT7 = BIT(7),
    SDIO_SLAVE_HOSTINT_SEND_NEW_PACKET = BIT(23), ///< New packet available
} sdio_slave_hostint_t;


/// Timing of SDIO slave
typedef enum {
    SDIO_SLAVE_TIMING_PSEND_PSAMPLE = 0,/**< Send at posedge, and sample at posedge. Default value for HS mode.
                                         *   If :c:macro:`SDIO_SLAVE_FLAG_HIGH_SPEED` is specified in
                                         *   :cpp:class:`sdio_slave_config_t`, this should be selected.
                                         *   Normally there's no problem using this to work in DS mode.
                                         */
    SDIO_SLAVE_TIMING_NSEND_PSAMPLE,    /**< Send at negedge, and sample at posedge. Default value for DS mode and
                                         *   below. If :c:macro:`SDIO_SLAVE_FLAG_DEFAULT_SPEED` is specified in
                                         *   :cpp:class:`sdio_slave_config_t`, this should be selected.
                                         */
    SDIO_SLAVE_TIMING_PSEND_NSAMPLE,    ///< Send at posedge, and sample at negedge
    SDIO_SLAVE_TIMING_NSEND_NSAMPLE,    ///< Send at negedge, and sample at negedge
} sdio_slave_timing_t;

/// Configuration of SDIO slave mode
typedef enum {
    SDIO_SLAVE_SEND_STREAM = 0, ///< Stream mode, all packets to send will be combined as one if possible
    SDIO_SLAVE_SEND_PACKET = 1, ///< Packet mode, one packets will be sent one after another (only increase packet_len if last packet sent).
} sdio_slave_sending_mode_t;
