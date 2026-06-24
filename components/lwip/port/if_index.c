/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip/if_api.h"

unsigned int if_nametoindex(const char *ifname)
{
    return lwip_if_nametoindex(ifname);
}

char *if_indextoname(unsigned int ifindex, char *ifname)
{
    return lwip_if_indextoname(ifindex, ifname);
}
