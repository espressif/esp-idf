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
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <soc/soc_memory_layout.h>
#include "multi_heap.h"
#include "rom/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some common heap registration data structures used
   for heap_caps_init.c to share heap information with heap_caps.c
*/

/* Type for describing each registered heap */
typedef struct heap_t_ {
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS]; ///< Capabilities for the type of memory in this heap (as a prioritised set). Copied from soc_memory_types so it's in RAM not flash.
    intptr_t start;
    intptr_t end;
    portMUX_TYPE heap_mux;
    multi_heap_handle_t heap;
    SLIST_ENTRY(heap_t_) next;
} heap_t;

/* All registered heaps.

   Forms a single linked list, even though most entries are contiguous.
   This means at the expense of 4 bytes per heap, new heaps can be
   added at runtime in a fast & thread-safe way.
*/
extern SLIST_HEAD(registered_heap_ll, heap_t_) registered_heaps;

bool heap_caps_match(const heap_t *heap, uint32_t caps);

#ifdef __cplusplus
}
#endif
