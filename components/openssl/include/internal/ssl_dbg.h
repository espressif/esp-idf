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

#ifndef _SSL_DEBUG_H_
#define _SSL_DEBUG_H_

#include "platform/ssl_opt.h"
#include "platform/ssl_port.h"

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef SSL_DEBUG_ENBALE
#define SSL_DEBUG_ENBALE 0
#endif

#ifndef SSL_DEBUG_LEVEL
#define SSL_DEBUG_LEVEL 0
#endif

#ifndef SSL_ASSERT_ENABLE
#define SSL_ASSERT_ENABLE 0
#endif

#ifndef SSL_DEBUG_LOCATION_ENABLE
#define SSL_DEBUG_LOCATION_ENABLE 0
#endif

#if SSL_DEBUG_ENBALE
    #if !defined(SSL_PRINT_LOG) || !defined(SSL_ERROR_LOG) || !defined(SSL_LOCAL_LOG)
        #include "stdio.h"
        extern int printf(const char *fmt, ...);
        #ifndef SSL_PRINT_LOG
            #define SSL_PRINT_LOG printf
        #endif
        #ifndef SSL_ERROR_LOG
            #define SSL_ERROR_LOG printf
        #endif 
        #ifndef SSL_LOCAL_LOG
            #define SSL_LOCAL_LOG printf
        #endif
    #endif
#else
    #ifdef SSL_PRINT_LOG
        #undef SSL_PRINT_LOG
    #endif
    #define SSL_PRINT_LOG(...)
    
    #ifdef SSL_ERROR_LOG
        #undef SSL_ERROR_LOG
    #endif
    #define SSL_ERROR_LOG(...)
    #ifdef SSL_LOCAL_LOG
        #undef SSL_LOCAL_LOG
    #endif
    #define SSL_LOCAL_LOG(...)
#endif

#if SSL_DEBUG_LOCATION_ENABLE
    #define SSL_DEBUG_LOCATION() SSL_LOCAL_LOG("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__)
#else
    #define SSL_DEBUG_LOCATION()
#endif

#if SSL_ASSERT_ENABLE
    #define SSL_ASSERT(s) { if (!(s)) { SSL_DEBUG_LOCATION(); } }
#else
    #define SSL_ASSERT(s)
#endif

#define SSL_ERR(err, go, fmt, ...) { SSL_DEBUG_LOCATION(); SSL_ERROR_LOG(fmt, ##__VA_ARGS__); ret = err; goto go; }

#define SSL_RET(go, fmt, ...) {  SSL_DEBUG_LOCATION(); SSL_ERROR_LOG(fmt, ##__VA_ARGS__); goto go; }

#define SSL_DEBUG(level, fmt, ...) { if (level > SSL_DEBUG_LEVEL) {SSL_PRINT_LOG(fmt, ##__VA_ARGS__);} }

#ifdef __cplusplus
}
#endif

#endif
