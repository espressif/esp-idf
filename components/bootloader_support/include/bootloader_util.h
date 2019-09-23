// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if half-open intervals overlap
 *
 * @param start1  interval 1 start
 * @param end1    interval 1 end
 * @param start2  interval 2 start
 * @param end2    interval 2 end
 * @return true iff [start1; end1) overlaps [start2; end2)
 */
static inline bool bootloader_util_regions_overlap(
        const intptr_t start1, const intptr_t end1,
        const intptr_t start2, const intptr_t end2)
{
    assert(end1>start1);
    assert(end2>start2);
    return (end1 > start2 && end2 > start1);
}

#ifdef __cplusplus
}
#endif
