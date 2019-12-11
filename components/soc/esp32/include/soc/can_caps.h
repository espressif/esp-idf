// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#if (CONFIG_ESP32_REV_MIN >= 2)
#define CAN_BRP_DIV_SUPPORTED       1
#define CAN_BRP_DIV_THRESH          128
//Any even number from 2 to 128, or multiples of 4 from 132 to 256
#define CAN_BRP_IS_VALID(brp)       (((brp) >= 2 && (brp) <= 128 && ((brp) & 0x1) == 0) || ((brp) >= 132 && (brp) <= 256 && ((brp) & 0x3) == 0))
#else
//Any even number from 2 to 128
#define CAN_BRP_IS_VALID(brp)       ((brp) >= 2 && (brp) <= 128 && ((brp) & 0x1) == 0)
#endif

//Todo: Add FIFO overrun errata workaround
//Todo: Add ECC decode capabilities
//Todo: Add ALC decode capability

#ifdef __cplusplus
}
#endif
