// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//This file pulls in OS-specific header files to allow compilation of socket_async.c under
// most OS's except for Windows.

// For ESP32 lwIP systems which use the ESP-IDF's non-standard lwIP include structure
// Tested with:
// ESP32

#ifndef LWIP_SNTP_OS_H
#define LWIP_SNTP_OS_H

#include "apps/sntp/sntp.h"

#endif // LWIP_SNTP_OS_H