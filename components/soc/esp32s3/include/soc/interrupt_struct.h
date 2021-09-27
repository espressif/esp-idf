// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_INTERRUPT_STRUCT_H_
#define _SOC_INTERRUPT_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct interrupt_dev_s {
} interrupt_dev_t;
extern interrupt_dev_t INTERRUPT;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_INTERRUPT_STRUCT_H_ */
