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


#define BOOT_LOG_LEVEL_ERROR      (1)
#define BOOT_LOG_LEVEL_WARN       (2)
#define BOOT_LOG_LEVEL_INFO       (3)
#define BOOT_LOG_LEVEL_NOTICE     (4)
#define BOOT_LOG_LEVEL_DEBUG      (5)

#define Black   0;30
#define Red     0;31
#define Green   0;32
#define Brown   0;33
#define Blue    0;34
#define Purple  0;35
#define Cyan    0;36

// TODO: move BOOT_LOG_LEVEL into menuconfig
//#define BOOT_LOG_LEVEL  BOOT_LOG_LEVEL_ERROR
#define BOOT_LOG_LEVEL  BOOT_LOG_LEVEL_NOTICE

//printf("\033[0;36m[NOTICE][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_notice(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_NOTICE){\
            ets_printf("\033[0;36m" format "\r\n", ##__VA_ARGS__);\
            ets_printf("\033[0m"); \
        }\
    }while(0)

#define log_info(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_INFO){\
            ets_printf("\033[1;36m" format "\r\n", ##__VA_ARGS__);\
            ets_printf("\033[0m"); \
        }\
    }while(0)

//printf("\033[0;31m[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_error(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_ERROR){\
            ets_printf("\033[0;31m[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            ets_printf("\033[0m"); \
        }\
    }while(0)

//printf("\033[1;33m[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_warn(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_WARN){\
            ets_printf("\033[1;33m[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            ets_printf("\033[0m"); \
        }\
    }while(0)

//printf("\033[1;32m[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define log_debug(format, ...) \
    do{\
        if(BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_DEBUG){\
	   		ets_printf("\033[1;32m[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            ets_printf("\033[0m"); \
        }\
    }while(0)

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_LOGGING_H__ */