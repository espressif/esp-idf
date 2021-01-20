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

#define RTC_CNTL_CPU_PD_DMA_BUS_WIDTH       (128)
#define RTC_CNTL_CPU_PD_REG_FILE_NUM        (108)
#define RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN      (RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)
#define RTC_CNTL_CPU_PD_DMA_BLOCK_SIZE      (RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)

#define RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE  (RTC_CNTL_CPU_PD_REG_FILE_NUM * (RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3))
