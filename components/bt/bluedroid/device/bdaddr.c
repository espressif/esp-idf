/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "common/bt_trace.h"
#include "device/bdaddr.h"

static inline bool ets_isxdigit(char c)
{
    if ((c >= '0') && (c <= '9')) {
        return true;
    }
    if ((c >= 'a') && (c <= 'f')) {
        return true;
    }
    return ((c >= 'A') && (c <= 'F'));
}

bool bdaddr_is_empty(const bt_bdaddr_t *addr)
{
    assert(addr != NULL);

    uint8_t zero[sizeof(bt_bdaddr_t)] = { 0 };
    return memcmp(addr, &zero, sizeof(bt_bdaddr_t)) == 0;
}

bool bdaddr_equals(const bt_bdaddr_t *first, const bt_bdaddr_t *second)
{
    assert(first != NULL);
    assert(second != NULL);

    return memcmp(first, second, sizeof(bt_bdaddr_t)) == 0;
}

bt_bdaddr_t *bdaddr_copy(bt_bdaddr_t *dest, const bt_bdaddr_t *src)
{
    assert(dest != NULL);
    assert(src != NULL);

    return (bt_bdaddr_t *)memcpy(dest, src, sizeof(bt_bdaddr_t));
}

const char *bdaddr_to_string(const bt_bdaddr_t *addr, char *string, size_t size)
{
    assert(addr != NULL);
    assert(string != NULL);

    if (size < 18) {
        return NULL;
    }

    const uint8_t *ptr = addr->address;
    sprintf(string, "%02x:%02x:%02x:%02x:%02x:%02x",
            ptr[0], ptr[1], ptr[2],
            ptr[3], ptr[4], ptr[5]);
    return string;
}

bool string_is_bdaddr(const char *string)
{
    assert(string != NULL);

    size_t len = strlen(string);
    if (len != 17) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        // Every 3rd char must be ':'.
        if (((i + 1) % 3) == 0 && string[i] != ':') {
            return false;
        }

        // All other chars must be a hex digit.
        if (((i + 1) % 3) != 0 && !ets_isxdigit(string[i])) {
            return false;
        }
    }
    return true;
}

bool string_to_bdaddr(const char *string, bt_bdaddr_t *addr)
{
    assert(string != NULL);
    assert(addr != NULL);

    bt_bdaddr_t new_addr;
    uint8_t *ptr = new_addr.address;
    uint32_t ptr_32[6];
    bool ret  = sscanf(string, "%02x:%02x:%02x:%02x:%02x:%02x",
                      &ptr_32[0], &ptr_32[1], &ptr_32[2], &ptr_32[3], &ptr_32[4], &ptr_32[5]) == 6;
    if (ret) {
        for (uint8_t i = 0; i < 6; i++){
            ptr[i] = (uint8_t) ptr_32[i];
        }
        memcpy(addr, &new_addr, sizeof(bt_bdaddr_t));
    }

    return ret;
}

hash_index_t hash_function_bdaddr(const void *key)
{
    hash_index_t hash = 5381;
    const char *bytes = (const char *)key;
    for (size_t i = 0; i < sizeof(bt_bdaddr_t); ++i) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    return hash;
}
