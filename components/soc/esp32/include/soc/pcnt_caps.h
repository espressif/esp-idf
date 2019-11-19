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

#ifdef __cplusplus
extern "C" {
#endif

// ESP32 have 1 PCNT peripheral
#define PCNT_PORT_0            (0) /*!< PCNT port 0 */
#define PCNT_PORT_MAX          (1) /*!< PCNT port max */
#define SOC_PCNT_NUM           (PCNT_PORT_MAX)

#define PCNT_PIN_NOT_USED     (-1)  /*!< When selected for a pin, this pin will not be used */

#define PCNT_UNIT_0   (0)   /*!< PCNT unit 0 */
#define PCNT_UNIT_1   (1)   /*!< PCNT unit 1 */
#define PCNT_UNIT_2   (2)   /*!< PCNT unit 2 */
#define PCNT_UNIT_3   (3)   /*!< PCNT unit 3 */
#define PCNT_UNIT_4   (4)   /*!< PCNT unit 4 */
#define PCNT_UNIT_5   (5)   /*!< PCNT unit 5 */
#define PCNT_UNIT_6   (6)   /*!< PCNT unit 6 */
#define PCNT_UNIT_7   (7)   /*!< PCNT unit 7 */
#define PCNT_UNIT_MAX (8)

#ifdef __cplusplus
}
#endif
