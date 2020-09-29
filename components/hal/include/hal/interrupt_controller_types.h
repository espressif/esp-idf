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

#pragma once 

#include "soc/soc_caps.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INTDESC_NORMAL=0,
    INTDESC_RESVD,
    INTDESC_SPECIAL 
} int_desc_flag_t;

typedef enum {
    INTTP_LEVEL=0,
    INTTP_EDGE,
    INTTP_NA
} int_type_t;

typedef struct {
    int level;
    int_type_t type;
    int_desc_flag_t cpuflags[SOC_CPU_CORES_NUM];
} int_desc_t;

typedef void (*interrupt_handler_t)(void *arg);

#ifdef __cplusplus
}
#endif