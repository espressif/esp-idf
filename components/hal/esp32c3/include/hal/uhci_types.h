// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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



// Though the UHCI driver hasn't been published, some types are defined here
// for users to develop over the HAL. See example: controller_hci_uart_esp32c3

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief UHCI escape sequence
 */
typedef struct {
    uint8_t seper_chr;        /*!< escape sequence character */
    uint8_t sub_chr1;         /*!< escape sequence sub-character 1 */
    uint8_t sub_chr2;         /*!< escape sequence sub-character 2 */
    bool sub_chr_en;          /*!< enable use of sub-chaacter of escape sequence */
} uhci_seper_chr_t;

/**
 * @brief UHCI software flow control
 */
typedef struct {
    uint8_t xon_chr;          /*!< character for XON */
    uint8_t xon_sub1;         /*!< sub-character 1 for XON */
    uint8_t xon_sub2;         /*!< sub-character 2 for XON */
    uint8_t xoff_chr;         /*!< character 2 for XOFF */
    uint8_t xoff_sub1;        /*!< sub-character 1 for XOFF */
    uint8_t xoff_sub2;        /*!< sub-character 2 for XOFF */
    uint8_t flow_en;          /*!< enable use of software flow control */
} uhci_swflow_ctrl_sub_chr_t;

#ifdef __cplusplus
}
#endif
