// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ESP32-S3 have 1 SIGMADELTA peripheral.
#define SIGMADELTA_PORT_0             (0) /*!< SIGMADELTA port 0 */
#define SIGMADELTA_PORT_MAX           (1) /*!< SIGMADELTA port max */
#define SOC_SIGMADELTA_NUM            (SIGMADELTA_PORT_MAX)

#define SIGMADELTA_CHANNEL_0   (0)    /*!< Sigma-delta channel 0 */
#define SIGMADELTA_CHANNEL_1   (1)    /*!< Sigma-delta channel 1 */
#define SIGMADELTA_CHANNEL_2   (2)    /*!< Sigma-delta channel 2 */
#define SIGMADELTA_CHANNEL_3   (3)    /*!< Sigma-delta channel 3 */
#define SIGMADELTA_CHANNEL_4   (4)    /*!< Sigma-delta channel 4 */
#define SIGMADELTA_CHANNEL_5   (5)    /*!< Sigma-delta channel 5 */
#define SIGMADELTA_CHANNEL_6   (6)    /*!< Sigma-delta channel 6 */
#define SIGMADELTA_CHANNEL_7   (7)    /*!< Sigma-delta channel 7 */
#define SIGMADELTA_CHANNEL_MAX (8)

#ifdef __cplusplus
}
#endif
