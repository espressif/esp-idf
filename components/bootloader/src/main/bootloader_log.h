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
#ifndef __BOOT_LOG_H__
#define __BOOT_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"

#define BOOT_LOG_LEVEL_NONE       (0)
#define BOOT_LOG_LEVEL_ERROR      (1)
#define BOOT_LOG_LEVEL_WARN       (2)
#define BOOT_LOG_LEVEL_INFO       (3)
#define BOOT_LOG_LEVEL_NOTICE     (4)
#define BOOT_LOG_LEVEL_DEBUG      (5)

#define Black   "30"
#define Red     "31"
#define Green   "32"
#define Brown   "33"
#define Blue    "34"
#define Purple  "35"
#define Cyan    "36"

#if CONFIG_BOOTLOADER_LOG_COLORS
#define LOG_COLOR(COLOR) "\033[0;"COLOR"m"
#define LOG_BOLD(COLOR)  "\033[1;"COLOR"m"
#define LOG_RESET_COLOR "\033[0m"
#else
#define LOG_COLOR(...)
#define LOG_BOLD(...)
#define LOG_RESET_COLOR ""
#endif

// BOOT_LOG_LEVEL defined by make menuconfig
#if CONFIG_BOOTLOADER_LOG_LEVEL_NONE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_NONE
#elif CONFIG_BOOTLOADER_LOG_LEVEL_ERROR
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_ERROR
#elif CONFIG_BOOTLOADER_LOG_LEVEL_WARN
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_WARN
#elif CONFIG_BOOTLOADER_LOG_LEVEL_INFO
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#elif CONFIG_BOOTLOADER_LOG_LEVEL_NOTICE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_NOTICE
#elif CONFIG_BOOTLOADER_LOG_LEVEL_DEBUG
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_DEBUG
#else
#error "No bootloader log level set in menuconfig!"
#endif

//printf("\033[0;36m[NOTICE][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_notice(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_NOTICE){\
            ets_printf(LOG_COLOR(Cyan) format "\r\n", ##__VA_ARGS__); \
            ets_printf(LOG_RESET_COLOR); \
        }\
    }while(0)

#define log_info(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_INFO){\
            ets_printf(LOG_BOLD(Cyan) format "\r\n", ##__VA_ARGS__); \
            ets_printf(LOG_RESET_COLOR); \
        }\
    }while(0)

//printf("\033[0;31m[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_error(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_ERROR){\
            ets_printf(LOG_COLOR(Red) "[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ets_printf(LOG_RESET_COLOR); \
        }\
    }while(0)

//printf("\033[1;33m[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_warn(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_WARN){\
            ets_printf(LOG_BOLD(Brown) "[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ets_printf(LOG_RESET_COLOR); \
        }\
    }while(0)

//printf("\033[1;32m[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_debug(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_DEBUG){\
            ets_printf(LOG_BOLD(Green) "[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ets_printf(LOG_RESET_COLOR); \
        }\
    }while(0)

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_LOGGING_H__ */
