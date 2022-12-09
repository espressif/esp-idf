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

/**
 * @brief Timings for NEC protocol
 *
 */
#define NEC_LEADING_CODE_HIGH_US (9000)
#define NEC_LEADING_CODE_LOW_US (4500)
#define NEC_PAYLOAD_ONE_HIGH_US (560)
#define NEC_PAYLOAD_ONE_LOW_US (1690)
#define NEC_PAYLOAD_ZERO_HIGH_US (560)
#define NEC_PAYLOAD_ZERO_LOW_US (560)
#define NEC_REPEAT_CODE_HIGH_US (9000)
#define NEC_REPEAT_CODE_LOW_US (2250)
#define NEC_ENDING_CODE_HIGH_US (560)

/**
 * @brief Timings for RC5 protocol
 *
 */
#define RC5_PULSE_DURATION_US (889)

#ifdef __cplusplus
}
#endif
