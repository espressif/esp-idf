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
//and all variables in shared RAM. This can be redirected to IRAM if
//needed using these macros.

// Forces code into IRAM instead of flash
#define IRAM_ATTR __attribute__((section(".iram1")))

// Forces data into DRAM instead of flash
#define DRAM_ATTR __attribute__((section(".dram1")))

#endif /* __ESP_ATTR_H__ */
