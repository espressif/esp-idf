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
#include "randombytes_internal.h"
#include "esp_system.h"

static const char *randombytes_esp32_implementation_name(void)
{
    return "esp32";
}

/*
  Plug the ESP32 hardware RNG into libsodium's custom RNG support, as per
  https://download.libsodium.org/doc/advanced/custom_rng.html

  Note that this RNG is selected by default (see randombytes_default.h), so there
  is no need to call randombytes_set_implementation().
*/
const struct randombytes_implementation randombytes_esp32_implementation = {
    .implementation_name = randombytes_esp32_implementation_name,
    .random = esp_random,
    .stir = NULL,
    .uniform = NULL,
    .buf = esp_fill_random,
    .close = NULL,
};

