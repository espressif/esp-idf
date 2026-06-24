/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <netdb.h>


int esp_getaddrinfo(const char *nodename, const char *servname,
                               const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo hints_copy = *hints;
    struct addrinfo *res4 = NULL, *res6 = NULL;
    int ret4, ret6;

    if (hints->ai_family == AF_UNSPEC) {
        // Attempt to get IPv4 addresses
        hints_copy.ai_family = AF_INET;
        ret4 = lwip_getaddrinfo(nodename, servname, &hints_copy, &res4);

        // Attempt to get IPv6 addresses
        hints_copy.ai_family = AF_INET6;
        ret6 = lwip_getaddrinfo(nodename, servname, &hints_copy, &res6);

        // Handle results
        if (ret4 != 0 && ret6 != 0) {
            return ret4;  // Both calls failed, return the first error code
        }

        *res = res4 ? res4 : res6;  // Start with the non-NULL result

        if (res4 && res6) {
            // Append IPv6 list to the end of IPv4 list
            struct addrinfo *last = res4;
            while (last->ai_next) {
                last = last->ai_next;
            }
            last->ai_next = res6;
        }

        return ERR_OK;  // Success
    } else if ((hints->ai_family == AF_INET) || (hints->ai_family == AF_INET6)) {
        return lwip_getaddrinfo(nodename, servname, hints, res);
    }

    // Handle unsupported cases
    return EAI_FAMILY;
}
