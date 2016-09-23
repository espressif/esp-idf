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

#ifdef __cplusplus
 extern "C" {
#endif

#define SSL_DEBUG_ENBALE 0
#define SSL_DEBUG_LEVEL 0
#define SSL_ASSERT_ENABLE 0
#define SSL_DEBUG_LOCATION_ENABLE 0

#if SSL_DEBUG_ENBALE
    extern int ets_printf(const char *fmt, ...);

    #define SSL_PRINT ets_printf
#else
    #define SSL_PRINT(...)
#endif

#if SSL_DEBUG_LOCATION_ENABLE
    #define SSL_DEBUG_LOCATION() SSL_PRINT("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__)
#else
    #define SSL_DEBUG_LOCATION()
#endif

#if SSL_ASSERT_ENABLE
    #define SSL_ASSERT(s) { if (!(s)) { SSL_DEBUG_LOCATION(); } }
#else
    #define SSL_ASSERT(s)
#endif

#define SSL_ERR(err, go, ...) { SSL_DEBUG_LOCATION(); SSL_PRINT(__VA_ARGS__); ret = err; goto go; }

#define SSL_RET(go, ...) {  SSL_DEBUG_LOCATION(); SSL_PRINT(__VA_ARGS__); goto go; }

#define SSL_DEBUG(level, ...) { if (level > SSL_DEBUG_LEVEL) {SSL_PRINT(__VA_ARGS__);} }

#ifdef __cplusplus
}
#endif

#endif
