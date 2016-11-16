// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __AUDIO_LOG_H__
#define __AUDIO_LOG_H__
#include "esp_types.h"
#include "sdkconfig.h"

//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif

void __log(const char *level, const char * func, uint32_t line, const char * restrict format, ...);

#ifdef CONFIG_AUDIO_LOG_INFO_EN
#define LOGI(format, ...)  __log("I", __func__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOGI(format, ...)
#endif

#ifdef CONFIG_AUDIO_LOG_DEBUG_EN
#define LOGD(format, ...)  __log("D", __func__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOGD(format, ...)
#endif

#ifdef CONFIG_AUDIO_LOG_WARN_EN
#define LOGW(format, ...)  __log("W", __func__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOGW(format, ...)
#endif

#ifdef CONFIG_AUDIO_LOG_ERROR_EN
#define LOGE(format, ...)  __log("E", __func__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOGE(format, ...)
#endif



#ifdef __cplusplus
}
#endif

#endif //  __AUDIO_LOG_H__
