// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "hal/interrupt_controller_hal.h"

int_type_t interrupt_controller_hal_desc_type(int interrupt_number)
{
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return(int_desc[interrupt_number].type);
}

int interrupt_controller_hal_desc_level(int interrupt_number)
{
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return(int_desc[interrupt_number].level);
}

int_desc_flag_t interrupt_controller_hal_desc_flags(int interrupt_number, int cpu_number)
{
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return(int_desc[interrupt_number].cpuflags[cpu_number]);
}
