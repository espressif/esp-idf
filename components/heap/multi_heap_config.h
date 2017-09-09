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

#ifdef ESP_PLATFORM

#include "sdkconfig.h"

/* Configuration macros for multi-heap */

#ifdef CONFIG_HEAP_POISONING_LIGHT
#define MULTI_HEAP_POISONING
#endif

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
#define MULTI_HEAP_POISONING
#define MULTI_HEAP_POISONING_SLOW
#endif

#else /* !ESP_PLATFORM */

/* Host-side tests, enable full poisoning */
#define MULTI_HEAP_POISONING
#define MULTI_HEAP_POISONING_SLOW

#endif
