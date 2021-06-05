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

#define ESP_ROM_HAS_CRC_LE       (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE       (1) // ROM CRC library supports Big Endian
#define ESP_ROM_HAS_JPEG_DECODE  (1) // ROM has JPEG decode library
#define ESP_ROM_UART_CLK_IS_XTAL (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_USB_SERIAL_DEVICE_NUM (3) // UART uses USB_SERIAL_JTAG port in ROM.
