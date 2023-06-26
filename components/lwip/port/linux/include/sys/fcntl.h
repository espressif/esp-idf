/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "sdkconfig.h"

#ifdef LWIP_HDR_LINUX_SYS_SOCKETS_H
// only if we prefer linux system sockets, include from system paths
#include_next <fcntl.h>
#elif CONFIG_IDF_TARGET_LINUX
// need to declare, as on linux we bypass IDF vfs and wrap posix io functions
extern int fcntl(int s, int cmd, ...);
#endif
// ignore otherwise (typically included from lwipopts.h) since lwip provides all necessary definitions
