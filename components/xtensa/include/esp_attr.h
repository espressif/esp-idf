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

#include "sdkconfig.h"

#define ROMFN_ATTR

//Normally, the linker script will put all code and rodata in flash,
//and all variables in shared RAM. These macros can be used to redirect
//particular functions/variables to other memory regions.

// Forces code into IRAM instead of flash
#define IRAM_ATTR _SECTION_ATTR_IMPL(".iram1", __COUNTER__)

// Forces data into DRAM instead of flash
#define DRAM_ATTR _SECTION_ATTR_IMPL(".dram1", __COUNTER__)

// Forces data to be 4 bytes aligned
#define WORD_ALIGNED_ATTR __attribute__((aligned(4)))

// Forces data to be placed to DMA-capable places
#define DMA_ATTR WORD_ALIGNED_ATTR DRAM_ATTR

// Forces a function to be inlined
#define FORCE_INLINE_ATTR static inline __attribute__((always_inline))

// Forces a string into DRAM instead of flash
// Use as ets_printf(DRAM_STR("Hello world!\n"));
#define DRAM_STR(str) (__extension__({static const DRAM_ATTR char __c[] = (str); (const char *)&__c;}))

// Forces code into RTC fast memory. See "docs/deep-sleep-stub.rst"
#define RTC_IRAM_ATTR _SECTION_ATTR_IMPL(".rtc.text", __COUNTER__)

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
// Forces bss variable into external memory. "
#define EXT_RAM_ATTR _SECTION_ATTR_IMPL(".ext_ram.bss", __COUNTER__)
#else
#define EXT_RAM_ATTR
#endif

// Forces data into RTC slow memory. See "docs/deep-sleep-stub.rst"
// Any variable marked with this attribute will keep its value
// during a deep sleep / wake cycle.
#define RTC_DATA_ATTR _SECTION_ATTR_IMPL(".rtc.data", __COUNTER__)

// Forces read-only data into RTC memory. See "docs/deep-sleep-stub.rst"
#define RTC_RODATA_ATTR _SECTION_ATTR_IMPL(".rtc.rodata", __COUNTER__)

// Allows to place data into RTC_SLOW memory.
#define RTC_SLOW_ATTR _SECTION_ATTR_IMPL(".rtc.force_slow", __COUNTER__)

// Allows to place data into RTC_FAST memory.
#define RTC_FAST_ATTR _SECTION_ATTR_IMPL(".rtc.force_fast", __COUNTER__)

// Forces data into noinit section to avoid initialization after restart.
#define __NOINIT_ATTR _SECTION_ATTR_IMPL(".noinit", __COUNTER__)

// Forces data into RTC slow memory of .noinit section.
// Any variable marked with this attribute will keep its value
// after restart or during a deep sleep / wake cycle.
#define RTC_NOINIT_ATTR  _SECTION_ATTR_IMPL(".rtc_noinit", __COUNTER__)

// Forces to not inline function
#define NOINLINE_ATTR __attribute__((noinline))

// This allows using enum as flags in C++
// Format: FLAG_ATTR(flag_enum_t)
#ifdef __cplusplus

// Inline is required here to avoid multiple definition error in linker
#define FLAG_ATTR_IMPL(TYPE, INT_TYPE) \
FORCE_INLINE_ATTR constexpr TYPE operator~ (TYPE a) { return (TYPE)~(INT_TYPE)a; } \
FORCE_INLINE_ATTR constexpr TYPE operator| (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a | (INT_TYPE)b); } \
FORCE_INLINE_ATTR constexpr TYPE operator& (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a & (INT_TYPE)b); } \
FORCE_INLINE_ATTR constexpr TYPE operator^ (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a ^ (INT_TYPE)b); } \
FORCE_INLINE_ATTR constexpr TYPE operator>> (TYPE a, int b) { return (TYPE)((INT_TYPE)a >> b); } \
FORCE_INLINE_ATTR constexpr TYPE operator<< (TYPE a, int b) { return (TYPE)((INT_TYPE)a << b); } \
FORCE_INLINE_ATTR TYPE& operator|=(TYPE& a, TYPE b) { a = a | b; return a; } \
FORCE_INLINE_ATTR TYPE& operator&=(TYPE& a, TYPE b) { a = a & b; return a; } \
FORCE_INLINE_ATTR TYPE& operator^=(TYPE& a, TYPE b) { a = a ^ b; return a; } \
FORCE_INLINE_ATTR TYPE& operator>>=(TYPE& a, int b) { a >>= b; return a; } \
FORCE_INLINE_ATTR TYPE& operator<<=(TYPE& a, int b) { a <<= b; return a; }

#define FLAG_ATTR_U32(TYPE) FLAG_ATTR_IMPL(TYPE, uint32_t)
#define FLAG_ATTR FLAG_ATTR_U32

#else
#define FLAG_ATTR(TYPE)
#endif

// Implementation for a unique custom section
//
// This prevents gcc producing "x causes a section type conflict with y"
// errors if two variables in the same source file have different linkage (maybe const & non-const) but are placed in the same custom section
//
// Using unique sections also means --gc-sections can remove unused
// data with a custom section type set
#define _SECTION_ATTR_IMPL(SECTION, COUNTER) __attribute__((section(SECTION "." _COUNTER_STRINGIFY(COUNTER))))

#define _COUNTER_STRINGIFY(COUNTER) #COUNTER

/* Use IDF_DEPRECATED attribute to mark anything deprecated from use in
   ESP-IDF's own source code, but not deprecated for external users.
*/
#ifdef IDF_CI_BUILD
#define IDF_DEPRECATED(REASON) __attribute__((deprecated(REASON)))
#else
#define IDF_DEPRECATED(REASON)
#endif

#endif /* __ESP_ATTR_H__ */
