// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SUPPLICANT_OPT_H
#define _SUPPLICANT_OPT_H

#include "sdkconfig.h"

#if CONFIG_WPA_MBEDTLS_CRYPTO
#define USE_MBEDTLS_CRYPTO 1
#else
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_CRYPTO_INTERNAL
#define CONFIG_TLSV12
#endif

#if CONFIG_WPA_DEBUG_PRINT
#define DEBUG_PRINT
#endif


#if CONFIG_WPA_SCAN_CACHE
#define SCAN_CACHE_SUPPORTED
#endif

#endif /* _SUPPLICANT_OPT_H */
