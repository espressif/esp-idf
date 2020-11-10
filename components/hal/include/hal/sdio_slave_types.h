// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "soc/soc.h"

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
                                         *   Normally there's no problem using this to work in DS mode.
                                         */
    SDIO_SLAVE_TIMING_NSEND_PSAMPLE    ,///< Send at negedge, and sample at posedge. Default value for DS mode and below.
    SDIO_SLAVE_TIMING_PSEND_NSAMPLE,    ///< Send at posedge, and sample at negedge
    SDIO_SLAVE_TIMING_NSEND_NSAMPLE,    ///< Send at negedge, and sample at negedge
} sdio_slave_timing_t;

/// Configuration of SDIO slave mode
typedef enum {
    SDIO_SLAVE_SEND_STREAM = 0, ///< Stream mode, all packets to send will be combined as one if possible
    SDIO_SLAVE_SEND_PACKET = 1, ///< Packet mode, one packets will be sent one after another (only increase packet_len if last packet sent).
} sdio_slave_sending_mode_t;
