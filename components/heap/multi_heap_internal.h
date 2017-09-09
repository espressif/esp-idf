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

/* Internal definitions for the "implementation" of the multi_heap API,
   as defined in multi_heap.c.

   If heap poisioning is disabled, these are aliased directly to the public API.

   If heap poisoning is enabled, wrapper functions call each of these.
*/
void *multi_heap_malloc_impl(multi_heap_handle_t heap, size_t size);
void multi_heap_free_impl(multi_heap_handle_t heap, void *p);
void *multi_heap_realloc_impl(multi_heap_handle_t heap, void *p, size_t size);
multi_heap_handle_t multi_heap_register_impl(void *start, size_t size);
void multi_heap_get_info_impl(multi_heap_handle_t heap, multi_heap_info_t *info);
size_t multi_heap_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_minimum_free_size_impl(multi_heap_handle_t heap);
size_t multi_heap_get_allocated_size_impl(multi_heap_handle_t heap, void *p);

/* Some internal functions for heap poisoning use */

/* Check an allocated block's poison bytes are correct. Called by multi_heap_check(). */
bool multi_heap_internal_check_block_poisoning(void *start, size_t size, bool is_free, bool print_errors);

/* Fill a region of memory with the free or malloced pattern.
   Called when merging blocks, to overwrite the old block header.
*/
void multi_heap_internal_poison_fill_region(void *start, size_t size, bool is_free);
