// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
