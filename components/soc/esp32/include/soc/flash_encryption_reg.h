// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "soc.h"

#define FLASH_ENCRYPTION_BUFFER_REG   (PERIPHS_SPI_ENCRYPT_BASEADDR)
#define FLASH_ENCRYPTION_START_REG    (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x20)
#define FLASH_ENCRYPTION_ADDRESS_REG  (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x24)
#define FLASH_ENCRYPTION_DONE_REG     (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x28)
