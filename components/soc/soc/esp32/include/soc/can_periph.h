// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#warning soc/can_periph.h is deprecated, please use soc/twai_periph.h instead

#if CONFIG_IDF_TARGET_ESP32
#include "soc/soc_caps.h"
#include "soc/can_struct.h"
#endif

#ifdef __cplusplus
}
#endif
