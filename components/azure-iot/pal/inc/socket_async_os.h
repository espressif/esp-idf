// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//This file pulls in OS-specific header files to allow compilation of socket_async.c under
// most OS's except for Windows.

// For lwIP systems
// Tested with:
// ESP32

#ifndef SOCKET_ASYNC_OS_H
#define SOCKET_ASYNC_OS_H

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#endif // SOCKET_ASYNC_OS_H