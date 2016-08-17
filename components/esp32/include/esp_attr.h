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

//Forces data and flash into IRAM instead of flash / shared RAM
#define IRAM_ATTR __attribute__((section(".iram1")))
#define DRAM_ATTR __attribute__((section(".dram1")))

//Forces data and flash into the IRAM section of a specific core.
//Normally, you shouldn't have to use this: the linker will take care of
//only linking in the functions for that specific core.
#define IRAM_C0_ATTR __attribute__((section(".iram1pro")))
#define DRAM_C0_ATTR __attribute__((section(".iram1pro")))
#define IRAM_C1_ATTR __attribute__((section(".dram1app")))
#define DRAM_C1_ATTR __attribute__((section(".dram1app")))

#endif /* __ESP_ATTR_H__ */
