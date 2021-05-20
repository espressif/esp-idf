/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

# include "export.h"
# include "randombytes.h"

SODIUM_EXPORT
extern const struct randombytes_implementation randombytes_esp32_implementation;

/* Defining RANDOMBYTES_DEFAULT_IMPLEMENTATION here allows us to compile with the ESP32 hardware
   implementation as the default. No need to call randombytes_set_implementation().

   Doing it in the header like this is easier than passing it via a -D argument to gcc.
*/
#undef RANDOMBYTES_DEFAULT_IMPLEMENTATION
#define RANDOMBYTES_DEFAULT_IMPLEMENTATION &randombytes_esp32_implementation
