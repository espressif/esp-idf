// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_RMT_CHANNEL_MEM_WORDS       (48) /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_TX_CHANNELS_NUM         (2)  /*!< Number of channels that capable of Transmit */
#define SOC_RMT_RX_CHANNELS_NUM         (2)  /*!< Number of channels that capable of Receive */
#define SOC_RMT_CHANNELS_NUM            (4)  /*!< Total 8 channels (each channel can be configured to either TX or RX) */
#define SOC_RMT_SUPPORT_RX_PINGPONG     (1)  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION (1)  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT   (1)  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_GROUP        (1)  /*!< Support a group of TX channels to transmit simultaneously */
#define SOC_RMT_SUPPORT_XTAL            (1)  /*!< Support set XTAL clock as the RMT clock source */

#ifdef __cplusplus
}
#endif
