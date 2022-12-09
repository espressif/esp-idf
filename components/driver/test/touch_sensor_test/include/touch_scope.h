// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @brief Send data info to digital scope.
 * @param data Pointer to send buff.
 * @param channel_num The number of channels to be displayed.
 * @return the length of successfully transmitted data.
 */
int test_tp_print_to_scope(float *data, unsigned char channel_num);

/**
 * @brief Initialize the UART so that the touch sensor data is output to the digital scope.
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL UART error
 */
esp_err_t test_tp_scope_debug_init(uint8_t uart_num, int tx_io_num, int rx_io_num, int baud_rate);