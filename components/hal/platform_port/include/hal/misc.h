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
#pragma once

#define HAL_SWAP16(d) __builtin_bswap16((d))
#define HAL_SWAP32(d) __builtin_bswap32((d))
#define HAL_SWAP64(d) __builtin_bswap64((d))

/** @cond */    //Doxy command to hide preprocessor definitions from docs */

// In case the compiler optimise a 32bit instruction (e.g. s32i) into 8/16bit instruction with size optimization enabled
// which is not allowed on s2 and later chips (s2, s3, c3, h2)
// use these wrappers for manually read-modify-write with l32i and s32i

// modify register as uint32_t
#define HAL_FORCE_MODIFY_U32_REG_FIELD(base_reg, field, val)    \
{                                                           \
    typeof(base_reg) temp_reg = (base_reg);                 \
    temp_reg.field = (val);                                 \
    (base_reg) = temp_reg;                                  \
}

// read register as uint32_t
#define HAL_FORCE_READ_U32_REG_FIELD(base_reg, field)      \
( ((typeof(base_reg))((base_reg).val)).field )

/** @endcond */
