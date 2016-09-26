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

#ifndef _BDADDR_H_
#define _BDADDR_H_

#include <stdbool.h>
#include <stddef.h>

#include "bt_defs.h"
#include "hash_map.h"

// Note: the string representation of a bdaddr is expected to have the format
// xx:xx:xx:xx:xx:xx
// where each 'x' is a hex digit. The API presented in this header will accept
// both uppercase and lowercase digits but will only ever produce lowercase
// digits.

// Returns true if |addr| is the empty address (00:00:00:00:00:00).
// |addr| may not be NULL.
bool bdaddr_is_empty(const bt_bdaddr_t *addr);

// Returns true if |first| and |second| refer to the same address. Neither
// may be NULL.
bool bdaddr_equals(const bt_bdaddr_t *first, const bt_bdaddr_t *second);

// Returns destination bdaddr |dest| after copying |src| to |dest|.
// |dest| and |src| must not be NULL.
bt_bdaddr_t *bdaddr_copy(bt_bdaddr_t *dest, const bt_bdaddr_t *src);

// Makes a string representation of |addr| and places it into |string|. |size|
// refers to the size of |string|'s buffer and must be >= 18. On success, this
// function returns |string|, otherwise it returns NULL. Neither |addr| nor |string|
// may be NULL.
const char *bdaddr_to_string(const bt_bdaddr_t *addr, char *string, size_t size);

// Returns true if |string| represents a Bluetooth address. |string| may not be NULL.
bool string_is_bdaddr(const char *string);

// Converts |string| to bt_bdaddr_t and places it in |addr|. If |string| does not
// represent a Bluetooth address, |addr| is not modified and this function returns
// false. Otherwise, it returns true. Neither |string| nor |addr| may be NULL.
bool string_to_bdaddr(const char *string, bt_bdaddr_t *addr);

// A hash function tailored for bdaddrs.
hash_index_t hash_function_bdaddr(const void *key);

#endif
