// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __ESP_ATTR_H__
#define __ESP_ATTR_H__

#define ROMFN_ATTR

//Normally, the linker script will put all code and rodata in flash,
//and all variables in shared RAM. These macros can be used to redirect
//particular functions/variables to other memory regions.

// Forces code into IRAM instead of flash.
#define IRAM_ATTR __attribute__((section(".iram1")))

// Forces data into DRAM instead of flash
#define DRAM_ATTR __attribute__((section(".dram1")))

// Forces data to be 4 bytes aligned
#define WORD_ALIGNED_ATTR __attribute__((aligned(4)))

// Forces data to be placed to DMA-capable places
#define DMA_ATTR WORD_ALIGNED_ATTR DRAM_ATTR

// Forces a string into DRAM instead of flash
// Use as ets_printf(DRAM_STR("Hello world!\n"));
#define DRAM_STR(str) (__extension__({static const DRAM_ATTR char __c[] = (str); (const char *)&__c;}))

// Forces code into RTC fast memory. See "docs/deep-sleep-stub.rst"
#define RTC_IRAM_ATTR __attribute__((section(".rtc.text")))

// Forces data into RTC slow memory. See "docs/deep-sleep-stub.rst"
// Any variable marked with this attribute will keep its value
// during a deep sleep / wake cycle.
#define RTC_DATA_ATTR __attribute__((section(".rtc.data")))

// Forces read-only data into RTC slow memory. See "docs/deep-sleep-stub.rst"
#define RTC_RODATA_ATTR __attribute__((section(".rtc.rodata")))

// Forces data into noinit section to avoid initialization after restart.
#define __NOINIT_ATTR __attribute__((section(".noinit")))

// Forces data into RTC slow memory of .noinit section.
// Any variable marked with this attribute will keep its value
// after restart or during a deep sleep / wake cycle.
#define RTC_NOINIT_ATTR  __attribute__((section(".rtc_noinit")))

#endif /* __ESP_ATTR_H__ */
